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

#ifndef FLAT_OBJECT_STORE_H
#define FLAT_OBJECT_STORE_H

#include <memory>
#include <string>

#include "bytes.h"
#include "flat_object_storage_engine.h"
#include "distributed_object.h"
#include "object_types.h"

namespace OHOS::ObjectStore {
class FlatObjectWatcher : public TableWatcher {
public:
    FlatObjectWatcher(const std::string &sessionId) : TableWatcher(sessionId)
    {
    }
    virtual void OnChanged(
        const std::string &sessionId, const std::vector<std::string> &changedData, bool enableTransfer) = 0;
};

class CacheManager {
public:
    CacheManager();
    uint32_t Save(const std::string &bundleName, const std::string &sessionId, const std::string &deviceId,
        const std::map<std::string, std::vector<uint8_t>> &objectData);
    uint32_t RevokeSave(const std::string &bundleName, const std::string &sessionId);
    int32_t ResumeObject(const std::string &bundleName, const std::string &sessionId,
                         std::function<void(const std::map<std::string, std::vector<uint8_t>> &data)> &callback);
    int32_t SubscribeDataChange(const std::string &bundleName, const std::string &sessionId,
                                std::function<void(const std::map<std::string, std::vector<uint8_t>> &data)> &callback);
    int32_t UnregisterDataChange(const std::string &bundleName, const std::string &sessionId);
    int32_t DeleteSnapshot(const std::string &bundleName, const std::string &sessionId);
private:
    int32_t SaveObject(const std::string &bundleName, const std::string &sessionId,
        const std::string &deviceId, const std::map<std::string, std::vector<uint8_t>> &objectData,
        const std::function<void(const std::map<std::string, int32_t> &)> &callback);
    int32_t RevokeSaveObject(
        const std::string &bundleName, const std::string &sessionId, const std::function<void(int32_t)> &callback);
    std::mutex mutex_;
    static constexpr uint32_t WAIT_TIME = 5;
};

class FlatObjectStore {
public:
    explicit FlatObjectStore(const std::string &bundleName);
    ~FlatObjectStore();
    std::string GetBundleName();
    uint32_t CreateObject(const std::string &sessionId);
    uint32_t Delete(const std::string &objectId);
    uint32_t Watch(const std::string &objectId, std::shared_ptr<FlatObjectWatcher> watcher);
    uint32_t UnWatch(const std::string &objectId);
    uint32_t SetStatusNotifier(std::shared_ptr<StatusWatcher> sharedPtr);
    uint32_t SyncAllData(const std::string &sessionId,
        const std::function<void(const std::map<std::string, DistributedDB::DBStatus> &)> &onComplete);
    uint32_t Save(const std::string &sessionId, const std::string &deviceId);
    uint32_t RevokeSave(const std::string &sessionId);
    void CheckRetrieveCache(const std::string &sessionId);
    void FilterData(const std::string &sessionId,
                    std::map<std::string, std::vector<uint8_t>> &data);
    uint32_t PutDouble(const std::string &sessionId, const std::string &key, double value);
    uint32_t PutBoolean(const std::string &sessionId, const std::string &key, bool value);
    uint32_t PutString(const std::string &sessionId, const std::string &key, const std::string &value);
    uint32_t PutComplex(const std::string &sessionId, const std::string &key, const std::vector<uint8_t> &value);
    uint32_t GetDouble(const std::string &sessionId, const std::string &key, double &value);
    uint32_t GetBoolean(const std::string &sessionId, const std::string &key, bool &value);
    uint32_t GetString(const std::string &sessionId, const std::string &key, std::string &value);
    uint32_t GetComplex(const std::string &sessionId, const std::string &key, std::vector<uint8_t> &value);
    uint32_t GetType(const std::string &sessionId, const std::string &key, Type &type);
    uint32_t BindAssetStore(const std::string &sessionId, AssetBindInfo &bindInfo, Asset &assetValue);
private:
    uint32_t Put(const std::string &sessionId, const std::string &key, std::vector<uint8_t> value);
    uint32_t Get(const std::string &sessionId, const std::string &key, Bytes &value);

    std::shared_ptr<FlatObjectStorageEngine> storageEngine_;
    CacheManager *cacheManager_;
    std::mutex mutex_;
    std::vector<std::string> retrievedCache_ {};
    std::string bundleName_;
};
} // namespace OHOS::ObjectStore

#endif // FLAT_OBJECT_STORE_H
