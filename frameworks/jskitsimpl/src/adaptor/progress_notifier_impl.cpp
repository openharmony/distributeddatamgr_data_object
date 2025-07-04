/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "progress_notifier_impl.h"

#include "anonymous.h"
#include "logger.h"
#include "objectstore_errors.h"

namespace OHOS::ObjectStore {

std::shared_ptr<ProgressNotifierImpl> ProgressNotifierImpl::GetInstance()
{
    static std::shared_ptr<ProgressNotifierImpl> instance;
    static std::mutex instanceLock;
    std::lock_guard<std::mutex> lockGuard(instanceLock);
    if (instance == nullptr) {
        instance = std::make_shared<ProgressNotifierImpl>();
        DistributedObjectStore *storeInstance = DistributedObjectStore::GetInstance();
        if (storeInstance == nullptr) {
            LOG_ERROR("Get store instance nullptr");
            return instance;
        }
        auto ret = storeInstance->SetProgressNotifier(instance);
        if (ret != SUCCESS) {
            LOG_ERROR("SetProgressNotifier %{public}d error", ret);
        } else {
            LOG_INFO("SetProgressNotifier success");
        }
    }
    return instance;
}

void ProgressNotifierImpl::AddWatcher(const std::string &sessionId, std::weak_ptr<JSWatcher> watcher)
{
    std::lock_guard<std::mutex> lock(mutex_);
    watchers_.insert_or_assign(sessionId, watcher);
}

void ProgressNotifierImpl::DelWatcher(const std::string &sessionId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    watchers_.erase(sessionId);
}

void ProgressNotifierImpl::OnChanged(const std::string &sessionId, int32_t progress)
{
    std::lock_guard<std::mutex> lock(mutex_);
    if (watchers_.count(sessionId) != 0) {
        LOG_INFO("start emit %{public}s %{public}d", Anonymous::Change(sessionId).c_str(), progress);
        std::shared_ptr<JSWatcher> lockedWatcher = watchers_.at(sessionId).lock();
        if (lockedWatcher) {
            lockedWatcher->Emit("progress", sessionId, progress);
            LOG_INFO("end emit %{public}s %{public}d", Anonymous::Change(sessionId).c_str(), progress);
        } else {
            LOG_ERROR("watcher expired");
        }
    }
}

ProgressNotifierImpl::~ProgressNotifierImpl()
{
}

} // namespace OHOS::ObjectStore
