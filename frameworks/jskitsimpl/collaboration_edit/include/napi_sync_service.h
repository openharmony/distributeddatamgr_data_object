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

#ifndef GRD_SYNC_SERVICE_H
#define GRD_SYNC_SERVICE_H

#include <atomic>
#include <memory>
#include <mutex>

#include "db_store.h"
#include "napi_async_call.h"

namespace OHOS::CollaborationEdit {
class SyncService {
public:
    SyncService& operator=(SyncService const&) = delete;

    static std::shared_ptr<SyncService> GetInstance();
    uint64_t GetSyncId();
    std::shared_ptr<SyncContext> GetSyncContext(uint64_t syncId);
    void AddSyncContext(uint64_t syncId, std::shared_ptr<SyncContext> syncContext);
    void RemoveSyncContext(uint64_t syncId);
private:
    std::atomic<uint64_t> syncId_{};
    std::mutex contextMutex_;
    std::map<uint64_t, std::shared_ptr<SyncContext>> syncContextMap_;
};

} // namespace OHOS::CollaborationEdit
#endif // GRD_SYNC_SERVICE_H