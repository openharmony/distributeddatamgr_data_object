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

#ifndef DISTRIBUTED_OBJECTSTORE_IMPL_H
#define DISTRIBUTED_OBJECTSTORE_IMPL_H

#include <shared_mutex>

#include "distributed_objectstore.h"
namespace OHOS::ObjectStore {
class WatcherProxy;
enum SyncStatus : int8_t {
    SYNC_START,
    SYNCING,
    SYNC_SUCCESS,
    SYNC_FAIL,
};
class DistributedObjectStoreImpl : public DistributedObjectStore {
public:
    DistributedObjectStoreImpl(FlatObjectStore *flatObjectStore);
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

private:
    DistributedObject *CacheObject(const std::string &sessionId, FlatObjectStore *flatObjectStore);
    void RemoveCacheObject(const std::string &sessionId);
    FlatObjectStore *flatObjectStore_ = nullptr;
    std::map<DistributedObject *, std::shared_ptr<WatcherProxy>> watchers_;
    std::shared_mutex dataMutex_ {};
    std::vector<DistributedObject *> objects_ {};
    std::mutex watchersLock_;
};
class StatusNotifierProxy : public StatusWatcher {
public:
    virtual ~StatusNotifierProxy();
    StatusNotifierProxy(const std::shared_ptr<StatusNotifier> &notifier);
    void OnChanged(
        const std::string &sessionId, const std::string &networkId, const std::string &onlineStatus) override;

private:
    std::shared_ptr<StatusNotifier> notifier;
};

class ProgressNotifierProxy : public ProgressWatcher {
public:
    virtual ~ProgressNotifierProxy();
    ProgressNotifierProxy(const std::shared_ptr<ProgressNotifier> &notifier);
    void OnChanged(const std::string &sessionId, int32_t progress) override;

private:
    std::shared_ptr<ProgressNotifier> notifier;
};

class WatcherProxy : public FlatObjectWatcher {
public:
    using AssetChangeCallback = std::function<void(const std::string& sessionId,
        const std::string &assetKey, std::shared_ptr<ObjectWatcher> watcher)>;

    WatcherProxy(const std::shared_ptr<ObjectWatcher> objectWatcher, const std::string &sessionId);
    void OnChanged(
        const std::string &sessionId, const std::vector<std::string> &changedData, bool enableTransfer) override;
    void SetAssetChangeCallBack(const AssetChangeCallback &assetChangeCallback);
private:
    bool FindChangedAssetKey(const std::string &changedKey, std::string &assetKey);
    std::shared_ptr<ObjectWatcher> objectWatcher_;
    AssetChangeCallback assetChangeCallback_;
};
} // namespace OHOS::ObjectStore
#endif // DISTRIBUTED_OBJECTSTORE_IMPL_H
