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

#include "distributed_objectstore_impl_lite.h"
#include "objectstore_errors.h"
namespace OHOS::ObjectStore {
DistributedObjectStoreImpl::DistributedObjectStoreImpl()
{
}

DistributedObjectStoreImpl::~DistributedObjectStoreImpl()
{
}

uint32_t DistributedObjectStoreImpl::Get(const std::string &sessionId, DistributedObject **object)
{
    return ERR_NULL_OBJECT;
}

DistributedObject *DistributedObjectStoreImpl::CreateObject(const std::string &sessionId)
{
    return nullptr;
}

DistributedObject *DistributedObjectStoreImpl::CreateObject(const std::string &sessionId, uint32_t &status)
{
    return nullptr;
}

uint32_t DistributedObjectStoreImpl::DeleteObject(const std::string &sessionId)
{
    return ERR_NULL_OBJECT;
}

uint32_t DistributedObjectStoreImpl::Watch(DistributedObject *object, std::shared_ptr<ObjectWatcher> watcher)
{
    return ERR_NULL_OBJECT;
}

uint32_t DistributedObjectStoreImpl::UnWatch(DistributedObject *object)
{
    return ERR_NULL_OBJECT;
}

uint32_t DistributedObjectStoreImpl::SetStatusNotifier(std::shared_ptr<StatusNotifier> notifier)
{
    return ERR_NULL_OBJECT;
}

uint32_t DistributedObjectStoreImpl::SetProgressNotifier(std::shared_ptr<ProgressNotifier> notifier)
{
    return ERR_NULL_OBJECT;
}

DistributedObjectStore *DistributedObjectStore::GetInstance(const std::string &bundleName)
{
    return nullptr;
}

void DistributedObjectStoreImpl::NotifyCachedStatus(const std::string &sessionId)
{
    return;
}

void DistributedObjectStoreImpl::NotifyProgressStatus(const std::string &sessionId)
{
    return;
}
} // namespace OHOS::ObjectStore