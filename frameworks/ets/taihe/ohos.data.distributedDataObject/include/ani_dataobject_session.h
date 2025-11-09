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
using ObjectValueType = std::variant<std::monostate, bool, double,
    std::string, std::vector<uint8_t>, OHOS::CommonType::AssetValue, std::vector<OHOS::CommonType::AssetValue>>; 

const int32_t SDK_VERSION_9 = 9;
const std::string SESSION_ID_REGEX = "^\\w+$";
const std::string COMPLEX_TYPE = "[COMPLEX]";
const std::string STRING_TYPE = "[STRING]";
const std::string NULL_TYPE = "[NULL]";

class AniDataobjectSession
{
public:
    AniDataobjectSession(OHOS::ObjectStore::DistributedObject* obj, std::string sessionId);
    ~AniDataobjectSession();

    void SetDistributedDir(const std::string &dir)
    {
        distributedDir_ = dir;
    }
    std::string GetSessionId() { return sessionId_; }
    void LeaveSession(OHOS::ObjectStore::DistributedObjectStore *objectInfo);

    ::ohos::data::distributedDataObject::SaveSuccessResponse Save(std::string deviceId, unsigned long version);
    ::ohos::data::distributedDataObject::RevokeSaveSuccessResponse RevokeSave();

    bool AddWatch(OHOS::ObjectStore::DistributedObjectStore *objectStore, std::string type, VarCallbackType taiheCallback);
    void DeleteWatch(std::string type, VarCallbackType taiheCallback);
    
    uint32_t BindAssetStore(std::string key, OHOS::ObjectStore::AssetBindInfo &nativeBindInfo);
    uint32_t SyncDataToStore(std::string const& key, ObjectValueType const& objValue, bool withPrefix);
    uint32_t SyncAssetToStore(std::string const& key, OHOS::CommonType::AssetValue const& asset);
    uint32_t SyncAssetsToStore(std::string const& key, std::vector<OHOS::CommonType::AssetValue> const& assets);
    uint32_t FlushCachedData(std::map<std::string, ObjectValueType> const& dataMap);

    ObjectValueType GetValueFromStore(const char *key);
    ObjectValueType GetAssetValueFromStore(const char *key);
    ObjectValueType GetAssetsValueFromStore(const char *key);
    void RemoveTypePrefixForAsset(OHOS::CommonType::AssetValue &asset);
protected:
    std::shared_mutex watchMutex_{};
    std::shared_ptr<OHOS::ObjectStore::AniWatcher> watcher_ = nullptr;
    std::string distributedDir_;
    OHOS::ObjectStore::DistributedObject* distributedObj_ = nullptr;
    std::string sessionId_;
};

}
#endif

