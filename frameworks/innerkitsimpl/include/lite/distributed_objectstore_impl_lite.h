/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#ifndef DISTRIBUTED_OBJECTSTORE_IMPL_LITE_H
#define DISTRIBUTED_OBJECTSTORE_IMPL_LITE_H

#include "distributed_objectstore.h"
namespace OHOS::ObjectStore {
class DistributedObjectStoreImpl : public DistributedObjectStore {
public:
    DistributedObjectStoreImpl();
    ~DistributedObjectStoreImpl() override;
    uint32_t Get(const std::string &sessionId, DistributedObject **object) override;
    DistributedObject *CreateObject(const std::string &sessionId) override;
    DistributedObject *CreateObject(const std::string &sessionId, uint32_t &status) override;
    uint32_t DeleteObject(const std::string &sessionId) override;
    uint32_t Watch(DistributedObject *object, std::shared_ptr<ObjectWatcher> watcher) override;
    uint32_t UnWatch(DistributedObject *object) override;
    uint32_t SetStatusNotifier(std::shared_ptr<StatusNotifier> notifier) override;
    uint32_t SetProgressNotifier(std::shared_ptr<ProgressNotifier> notifier) override;
    void NotifyCachedStatus(const std::string &sessionId) override;
    void NotifyProgressStatus(const std::string &sessionId) override;
};
} // namespace OHOS::ObjectStore
#endif // DISTRIBUTED_OBJECTSTORE_IMPL_LITE_H