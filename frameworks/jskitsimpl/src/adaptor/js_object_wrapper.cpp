/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "js_object_wrapper.h"

#include "logger.h"
namespace OHOS::ObjectStore {
JSObjectWrapper::JSObjectWrapper(DistributedObjectStore *objectStore, DistributedObject *object)
    : objectStore_(objectStore), object_(object)
{
}

JSObjectWrapper::~JSObjectWrapper()
{
    LOG_INFO("JSObjectWrapper::~JSObjectWrapper");
    std::unique_lock<std::shared_mutex> cacheLock(watchMutex_);
    if (watcher_ != nullptr) {
        watcher_ = nullptr;
    }
    LOG_INFO("JSObjectWrapper::~JSObjectWrapper end");
}

DistributedObject *JSObjectWrapper::GetObject()
{
    return object_;
}

bool JSObjectWrapper::AddWatch(napi_env env, const char *type, napi_value handler)
{
    std::unique_lock<std::shared_mutex> cacheLock(watchMutex_);
    if (watcher_ == nullptr) {
        watcher_ = std::make_shared<JSWatcher>(env, objectStore_, object_);
        std::weak_ptr<JSWatcher> watcher = watcher_;
        auto changeEventListener = new ChangeEventListener(watcher, objectStore_, object_);
        auto statusEventListener = new StatusEventListener(watcher, object_->GetSessionId());
        auto progressEventListener = new ProgressEventListener(watcher, object_->GetSessionId());
        watcher_->SetListener(changeEventListener, statusEventListener, progressEventListener);
    }
    return watcher_->On(type, handler);
}

void JSObjectWrapper::DeleteWatch(napi_env env, const char *type, napi_value handler)
{
    std::unique_lock<std::shared_mutex> cacheLock(watchMutex_);
    if (watcher_ != nullptr) {
        watcher_->Off(type, handler);
        LOG_INFO("JSObjectWrapper::DeleteWatch %{public}s", type);
        if (watcher_->IsEmpty()) {
            LOG_DEBUG("delete JSWatcher");
            watcher_ = nullptr;
        }
    } else {
        LOG_ERROR("JSObjectWrapper::DeleteWatch watcher_ is null");
    }
}

bool JSObjectWrapper::IsUndefined(const char *value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string tmpStr = value;
    auto it = std::find(undefinedProperties_.begin(), undefinedProperties_.end(), tmpStr);
    if (it == undefinedProperties_.end()) {
        return false;
    }
    return true;
}

void JSObjectWrapper::AddUndefined(const char *value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string tmpStr = value;
    if (std::find(undefinedProperties_.begin(), undefinedProperties_.end(), tmpStr) == undefinedProperties_.end()) {
        undefinedProperties_.push_back(tmpStr);
    }
}

void JSObjectWrapper::DeleteUndefined(const char *value)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string tmpStr = value;
    auto it = std::find(undefinedProperties_.begin(), undefinedProperties_.end(), tmpStr);
    if (it != undefinedProperties_.end()) {
        undefinedProperties_.erase(it);
    }
}

void JSObjectWrapper::DestroyObject()
{
    object_ = nullptr;
}

void JSObjectWrapper::SetObjectId(const std::string &objectId)
{
    objectId_ = objectId;
}

std::string JSObjectWrapper::GetObjectId()
{
    return objectId_;
}

} // namespace OHOS::ObjectStore