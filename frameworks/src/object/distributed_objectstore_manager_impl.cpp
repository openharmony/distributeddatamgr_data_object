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

#include "distributed_objectstore_manager_impl.h"
#include "distributed_objectstore_impl.h"
#include "object_utils.h"
#include "flat_object_store.h"
#include "objectstore_errors.h"
#include "logger.h"

namespace OHOS::ObjectStore {
namespace {
    const std::string OBJECT_ID_SCHEMA = "dataobject://";
    const std::string SEPARATOR = "/";
}  // namespace

DistributedObjectStoreManagerImpl::~DistributedObjectStoreManagerImpl()
{
    for (auto &store : stores_) {
        delete store.second;
    }
}

DistributedObjectStore *DistributedObjectStoreManagerImpl::Create(const std::string &bundleName,
    const std::string &sessionId)
{
    std::unique_lock<std::shared_mutex> lock(storeManagerMutex_);
    if (stores_.count(bundleName + sessionId) != 0) {
        LOG_ERROR("store has been create!");
        return nullptr;
    }
    LOG_INFO("start create %s %s", bundleName.c_str(), sessionId.c_str());
    FlatObjectStore *flatObjectStore = new (std::nothrow) FlatObjectStore(bundleName, sessionId);
    if (flatObjectStore == nullptr) {
        LOG_ERROR("no memory for FlatObjectStore malloc!");
        return nullptr;
    }

    uint32_t errCode = flatObjectStore->Open();
    if (errCode != SUCCESS) {
        LOG_ERROR("open failed %s %s", bundleName.c_str(), sessionId.c_str());
        delete flatObjectStore;
        return nullptr;
    }

    DistributedObjectStore *store = new (std::nothrow) DistributedObjectStoreImpl(flatObjectStore);
    if (store == nullptr) {
        LOG_ERROR("no memory for ObjectStore malloc!");
        delete flatObjectStore;
        return nullptr;
    }
    LOG_INFO("create %s %s successfully", bundleName.c_str(), sessionId.c_str());
    stores_.emplace(bundleName + sessionId, store);
    return store;
}

uint32_t DistributedObjectStoreManagerImpl::Destroy(const std::string &bundleName, const std::string &sessionId)
{
    std::unique_lock<std::shared_mutex> lock(storeManagerMutex_);
    std::map<std::string, DistributedObjectStore *>::iterator it = stores_.find(bundleName + sessionId);
    if (it == stores_.end()) {
        LOG_ERROR("not find %s %s", bundleName.c_str(), sessionId.c_str());
        return ERR_INVAL;
    }
    DistributedObjectStore *deleteStore = it->second;
    deleteStore->Close();
    delete deleteStore;
    stores_.erase(it);
    return SUCCESS;
}
} // namespace ObjectStore
