/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "distributed_objectstore_impl.h"
#include "distributed_object_impl.h"
#include "objectstore_errors.h"
#include "object_store_manager.h"
#include "string_utils.h"
#include "object_utils.h"
#include "securec.h"

namespace OHOS::ObjectStore {
DistributedObjectStoreImpl::DistributedObjectStoreImpl(FlatObjectStore *flatObjectStore)
    : flatObjectStore_(flatObjectStore)
{}

DistributedObjectStoreImpl::~DistributedObjectStoreImpl()
{
    delete flatObjectStore_;
}

namespace {
    const std::string DEFAULT_USER_ID = "0";
    const std::string SEPARATOR = "/";
    const std::string GenerateObjectId(const std::string &prefix, const std::string &classPath, const std::string &key)
    {
        return prefix + SEPARATOR + classPath + SEPARATOR + key;
    }
}  // namespace

DistributedObjectImpl *DistributedObjectStoreImpl::CacheObject(FlatObject *flatObject, FlatObjectStore *flatObjectStore)
{
    DistributedObjectImpl *object = new (std::nothrow) DistributedObjectImpl(flatObject, flatObjectStore);
    if (object == nullptr) {
        return nullptr;
    }
    std::unique_lock<std::shared_mutex> cacheLock(dataMutex_);
    objects_.push_back(object);
    return object;
}

DistributedObject *DistributedObjectStoreImpl::CreateObject(const std::string &classPath, const std::string &key)
{
    if (flatObjectStore_ == nullptr) {
        LOG_ERROR("store for not opened!");
        return nullptr;
    }

    FlatObject *flatObject = new (std::nothrow) FlatObject();
    if (flatObject == nullptr) {
        LOG_ERROR("no memory for FlatObjectStore malloc!");
        return nullptr;
    }
    const std::vector<std::string> remoteIds = flatObjectStore_->GetRemoteStoreIds();
    for (const auto &deviceId : flatObjectStore_->GetRemoteStoreIds()) {
        std::string prefix = ObjectUtils::GenObjectIdPrefix(deviceId, DEFAULT_USER_ID,
            flatObjectStore_->GetBundleName(), flatObjectStore_->GetName());
        std::string oid = GenerateObjectId(prefix, classPath, key);
        uint32_t errCode = flatObjectStore_->Get(StringUtils::StrToBytes(oid), *flatObject);
        LOG_INFO("get %s errCode %d", oid.c_str(), errCode);
        if (errCode == SUCCESS) {
            const std::map<Bytes, Bytes> &cFields = flatObject->GetFields();
            if (cFields.size() == 0) {
                LOG_INFO("field empty %s", oid.c_str());
                continue;
            }
            LOG_INFO("find exist object");
            return CacheObject(flatObject, flatObjectStore_);
        }
    }
    std::string oid = GenerateObjectId(flatObjectStore_->GetPrefix(), classPath, key);
    flatObject->SetId(StringUtils::StrToBytes(oid));
    flatObject->SetField(StringUtils::StrToBytes("stubInfo"), StringUtils::StrToBytes(""));
    flatObjectStore_->Put(*flatObject);
    LOG_INFO("create object for owner");
    return CacheObject(flatObject, flatObjectStore_);
}

uint32_t DistributedObjectStoreImpl::Sync(DistributedObject *object)
{
    if (object == nullptr || flatObjectStore_ == nullptr) {
        return ERR_INVAL;
    }
    // object is abstract, it must be DistributedObjectImpl pointer
    return flatObjectStore_->Put(*static_cast<DistributedObjectImpl *>(object)->GetObject());
}

uint32_t DistributedObjectStoreImpl::Delete(DistributedObject *object)
{
    if (object == nullptr || flatObjectStore_ == nullptr) {
        return ERR_INVAL;
    }
    // object is abstract, it must be DistributedObjectImpl pointer
    return flatObjectStore_->Delete(static_cast<DistributedObjectImpl *>(object)->GetObject()->GetId());
}

uint32_t DistributedObjectStoreImpl::Get(const std::string &objectId, DistributedObject *object)
{
    std::string oid;
    auto iter = objects_.begin();
    while (iter != objects_.end()) {
        uint32_t ret = (*iter)->GetObjectId(oid);
        if (ret != SUCCESS) {
            LOG_ERROR("DistributedObjectStoreImpl::Get oid err, ret %d", ret);
            return ret;
        }
        if (objectId == oid) {
            object = *iter;
            return SUCCESS;
        }
        iter++;
    }
    LOG_ERROR("DistributedObjectStoreImpl::Get object err, no object");
    return ERR_GET_OBJECT;
}

uint32_t DistributedObjectStoreImpl::Watch(DistributedObject *object, std::shared_ptr<ObjectWatcher> watcher)
{
    if (object == nullptr || flatObjectStore_ == nullptr) {
        LOG_ERROR("DistributedObjectStoreImpl::Watch err object ");
        return ERR_INVAL;
    }
    if (watchers_.count(object) != 0) {
        LOG_ERROR("DistributedObjectStoreImpl::Watch already gets object");
        return ERR_EXIST;
    }
    std::shared_ptr<WatcherProxy> watcherProxy = std::make_shared<WatcherProxy>(watcher);
    // object is abstract, it must be DistributedObjectImpl pointer
    uint32_t ret = flatObjectStore_
            ->Watch(static_cast<DistributedObjectImpl *>(object)->GetObject()->GetId(), watcherProxy);
    if (ret != SUCCESS) {
        LOG_ERROR("DistributedObjectStoreImpl::Watch ret:%d", ret);
        return ret;
    }
    watchers_.insert_or_assign(object, watcherProxy);
    LOG_INFO("DistributedObjectStoreImpl:Watch object success.");
    return SUCCESS;
}

uint32_t DistributedObjectStoreImpl::UnWatch(DistributedObject *object)
{
    if (object == nullptr || flatObjectStore_ == nullptr) {
        return ERR_INVAL;
    }
    if (watchers_.count(object) == 0) {
        LOG_INFO("DistributedObjectStoreImpl:UnWatch no watcher.");
        return SUCCESS;
    }
    std::shared_ptr<FlatObjectWatcher> proxy = watchers_.at(object);
    Bytes objectId = static_cast<DistributedObjectImpl *>(object)->GetObject()->GetId();
    flatObjectStore_->Unwatch(objectId, proxy);
    watchers_.erase(object);
    LOG_INFO("DistributedObjectStoreImpl:UnWatch object success.");
    return SUCCESS;
}

void DistributedObjectStoreImpl::Close()
{
    {
        std::unique_lock<std::shared_mutex> cacheLock(dataMutex_);
        for (auto &item : objects_) {
            flatObjectStore_->Delete(item->GetObject()->GetId());
            delete item;
        }
    }
    flatObjectStore_->Close();
}

WatcherProxy::WatcherProxy(const std::shared_ptr<ObjectWatcher> objectWatcher)
    : objectWatcher_(objectWatcher)
{}

void WatcherProxy::OnChanged(const Bytes &id)
{
    std::string str;
    uint32_t ret = StringUtils::BytesToString(id, str);
    if (ret != SUCCESS) {
        LOG_ERROR("WatcherProxy:OnChanged bytesToString err,ret %d", ret);
    }
    objectWatcher_->OnChanged(str);
}

void WatcherProxy::OnDeleted(const Bytes &id)
{
    std::string str;
    uint32_t ret = StringUtils::BytesToString(id, str);
    if (ret != SUCCESS) {
        LOG_ERROR("WatcherProxy:OnChanged bytesToString err,ret %d", ret);
    }
    objectWatcher_->OnDeleted(str);
}
}  // namespace OHOS::ObjectStore
