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

#include "napi_sync_service.h"

namespace OHOS::CollaborationEdit {
std::shared_ptr<SyncService> SyncService::GetInstance()
{
    static std::shared_ptr<SyncService> serviceObj_ = std::make_shared<SyncService>();
    return serviceObj_;
}

uint64_t SyncService::GetSyncId()
{
    uint64_t value = ++syncId_;
    if (value == 0) {
        value = ++syncId_;
    }
    return value;
}

std::shared_ptr<SyncContext> SyncService::GetSyncContext(uint64_t syncId)
{
    std::lock_guard<std::mutex> lock(contextMutex_);
    auto iter = syncContextMap_.find(syncId);
    if (iter == syncContextMap_.end()) {
        return nullptr;
    }
    return iter->second;
}

void SyncService::AddSyncContext(uint64_t syncId, std::shared_ptr<SyncContext> syncContext)
{
    std::lock_guard<std::mutex> lock(contextMutex_);
    syncContextMap_[syncId] = syncContext;
}

void SyncService::RemoveSyncContext(uint64_t syncId)
{
    std::lock_guard<std::mutex> lock(contextMutex_);
    auto iter = syncContextMap_.find(syncId);
    if (iter != syncContextMap_.end()) {
        syncContextMap_.erase(iter);
    }
}
}
