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
#ifndef OHOS_ANI_DATAOBJECT_SESSION_H
#define OHOS_ANI_DATAOBJECT_SESSION_H
#include "ohos.data.distributedDataObject.proj.hpp"
#include "ohos.data.distributedDataObject.impl.hpp"

#include <memory>
#include <map>
#include <string>
#include <vector>
#include <mutex>
#include <shared_mutex>
#include "ani_watcher.h"
#include "object_types.h"

namespace OHOS::ObjectStore {
using NativeObjectValueType = std::variant<std::monostate, bool, double,
    std::string, std::vector<uint8_t>, OHOS::CommonType::AssetValue, std::vector<OHOS::CommonType::AssetValue>>;

const int32_t SDK_VERSION_9 = 9;
constexpr const char* SESSION_ID_REGEX = "^\\w+$";
constexpr const char* COMPLEX_TYPE = "[COMPLEX]";
constexpr const char* STRING_TYPE = "[STRING]";
constexpr const char* NULL_TYPE = "[NULL]";

class AniDataobjectSession {
public:
    AniDataobjectSession(OHOS::ObjectStore::DistributedObject* obj, const std::string &sessionId);
    ~AniDataobjectSession();

    void SetDistributedDir(const std::string &dir)
    {
        distributedDir_ = dir;
    }
    std::string GetSessionId() { return sessionId_; }
    void LeaveSession(OHOS::ObjectStore::DistributedObjectStore *objectInfo);

    ::ohos::data::distributedDataObject::SaveSuccessResponse Save(const std::string &deviceId, int32_t version);
    ::ohos::data::distributedDataObject::RevokeSaveSuccessResponse RevokeSave();

    bool AddWatch(OHOS::ObjectStore::DistributedObjectStore *objectStore,
        const std::string &type, VarCallbackType taiheCallback);
    void DeleteWatch(const std::string &type, VarCallbackType taiheCallback);

    uint32_t BindAssetStore(const std::string &key, OHOS::ObjectStore::AssetBindInfo &nativeBindInfo);
    uint32_t SyncDataToStore(const std::string &key, const NativeObjectValueType &objValue, bool withPrefix);
    bool SyncAssetToStore(const std::string &key, const OHOS::CommonType::AssetValue &asset);
    bool SyncAssetPropertyToStore(const std::string &key, const std::string &property, uint32_t value);
    bool SyncAssetPropertyToStore(const std::string &key, const std::string &property, const std::string &value);
    bool SyncAssetsToStore(const std::string &key, const std::vector<OHOS::CommonType::AssetValue> &assets);
    uint32_t FlushCachedData(const std::map<std::string, NativeObjectValueType> &dataMap);

    NativeObjectValueType GetValueFromStore(const char *key);
    NativeObjectValueType GetAssetValueFromStore(const char *key);
    NativeObjectValueType GetAssetsValueFromStore(const char *key, size_t size);
    void RemoveTypePrefixForAsset(OHOS::CommonType::AssetValue &asset);

private:
    NativeObjectValueType HandleStringType(const char* key);
    NativeObjectValueType HandleDoubleType(const char* key);
    NativeObjectValueType HandleBooleanType(const char* key);
    NativeObjectValueType HandleComplexType(const char* key);

private:
    std::shared_mutex watchMutex_{};
    std::shared_ptr<OHOS::ObjectStore::AniWatcher> watcher_ = nullptr;
    std::string distributedDir_;
    OHOS::ObjectStore::DistributedObject* distributedObj_ = nullptr;
    std::string sessionId_;
};

} //namespace OHOS::ObjectStore
#endif

