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

#include "ani_notifier_impl.h"

#include "anonymous.h"
#include "logger.h"
#include "objectstore_errors.h"

namespace OHOS::ObjectStore {

std::shared_ptr<AniNotifierImpl> AniNotifierImpl::GetInstance()
{
    static std::shared_ptr<AniNotifierImpl> instance;
    static std::mutex instanceLock;
    std::lock_guard<std::mutex> lockGuard(instanceLock);
    if (instance == nullptr) {
        if (instance == nullptr) {
            instance = std::make_shared<AniNotifierImpl>();
            DistributedObjectStore *storeInstance = DistributedObjectStore::GetInstance();
            if (storeInstance == nullptr) {
                LOG_ERROR("Get store instance nullptr");
                return instance;
            }
            auto ret = storeInstance->SetStatusNotifier(instance);
            if (ret != SUCCESS) {
                LOG_ERROR("SetStatusNotifier %{public}d error", ret);
            } else {
                LOG_INFO("SetStatusNotifier success");
            }
        }
    }
    return instance;
}

void AniNotifierImpl::AddWatcher(const std::string &sessionId, std::weak_ptr<AniWatcher> watcher)
{
    std::lock_guard<std::mutex> lock(mutex_);
    watchers_.insert_or_assign(sessionId, watcher);
}

void AniNotifierImpl::DelWatcher(const std::string &sessionId)
{
    std::lock_guard<std::mutex> lock(mutex_);
    watchers_.erase(sessionId);
}

void AniNotifierImpl::OnChanged(
    const std::string &sessionId, const std::string &networkId, const std::string &onlineStatus)
{
    LOG_INFO("status changed %{public}s %{public}s %{public}s", sessionId.c_str(), Anonymous::Change(networkId).c_str(),
        onlineStatus.c_str());
    std::lock_guard<std::mutex> lock(mutex_);
    if (watchers_.count(sessionId) != 0) {
        std::shared_ptr<AniWatcher> lockedWatcher = watchers_.at(sessionId).lock();
        if (lockedWatcher) {
            lockedWatcher->Emit(EVENT_STATUS, sessionId, networkId, onlineStatus);
            LOG_INFO("end emit %{public}s %{public}s %{public}s", sessionId.c_str(),
                Anonymous::Change(networkId).c_str(), onlineStatus.c_str());
        } else {
            LOG_ERROR("watcher expired");
        }
    }
}

AniNotifierImpl::~AniNotifierImpl()
{
}
} // namespace OHOS::ObjectStore
