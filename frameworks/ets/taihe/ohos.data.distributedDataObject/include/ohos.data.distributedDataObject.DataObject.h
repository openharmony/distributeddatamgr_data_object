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

#pragma once
#include "ohos.data.distributedDataObject.proj.hpp"
#include "ohos.data.distributedDataObject.impl.hpp"

#include <variant>
#include <map>
#include <optional>
#include <ani.h>
#include "taihe/runtime.hpp"
#include "ani_watcher.h"
#include "ani_dataobject_session.h"
#include "common_types.h"

namespace OHOS::ObjectStore {

class DataObjectImpl {
public:
    explicit DataObjectImpl(ani_object context, ani_object sourceObj, ::taihe::array_view<::taihe::string> keys);
    ~DataObjectImpl();
    static ani_vm* GetVm() { return vm_; }
    static std::string JsonStringify(ani_env* aniEnv, ani_ref sourceObj);
    static ani_ref JsonParseToJsonElement(ani_env* aniEnv, std::string const& str);
    NativeObjectValueType ParseObjectValue(ani_env* aniEnv, ani_ref valueRef);
    bool ParseRecord(ani_env* env, ani_ref recordRef, std::map<std::string, NativeObjectValueType> &resultMap);
    void ParseObject(ani_object sourceObj, ::taihe::array_view<::taihe::string> const& keys);

    static std::string GenerateRandomNum();
    void UpdateBundleInfo();
    void UpdateBundleInfoWithContext(ani_object context);
    OHOS::ObjectStore::DistributedObjectStore* GetObjectStore();
    bool IsSandBox();
    static bool IsSameTaiheCallback(VarCallbackType p1, VarCallbackType p2);
    bool AddChangeCallback(AniChangeCallbackType taiheCallback);
    bool AddStatusCallback(AniStatusCallbackType taiheCallback);
    bool AddProgressCallback(AniProgressCallbackType taiheCallback);
    bool DeleteChangeCallback(VarCallbackType taiheCallback);
    bool DeleteStatusCallback(VarCallbackType taiheCallback);
    bool DeleteProgressCallback(VarCallbackType taiheCallback);
    void RestoreWatchers(const std::string &objectId);

    bool JoinSession(std::string sessionId);
    void LeaveSession();
    void SetSessionIdSync(::taihe::optional_view<::taihe::string> sessionId);
    ::ohos::data::distributedDataObject::SaveSuccessResponse SaveSync(::taihe::string_view deviceId);
    ::ohos::data::distributedDataObject::RevokeSaveSuccessResponse RevokeSaveSync();
    bool GetFileAttribute(const std::string &pathName, size_t &size, std::string &ctime, std::string &mtime);
    OHOS::CommonType::AssetValue GetDefaultAsset(OHOS::CommonType::AssetValue initValue, const std::string &uri);
    void SetAssetSync(::taihe::string_view assetKey, ::taihe::string_view uri);
    void SetAssetsSync(::taihe::string_view assetsKey, ::taihe::array_view<::taihe::string> uris);

    void OnChange(::taihe::callback_view<
        void(::taihe::string_view, ::taihe::array_view<::taihe::string>)> callback);
    void OffChange(::taihe::optional_view<
        ::taihe::callback<void(::taihe::string_view, ::taihe::array_view<::taihe::string>)>> callback);
    void OnStatus(::taihe::callback_view<
        void(::taihe::string_view, ::taihe::string_view, ::taihe::string_view)> callback);
    void OffStatus(::taihe::optional_view<
        ::taihe::callback<void(::taihe::string_view, ::taihe::string_view, ::taihe::string_view)>> callback);
    void OnProgressChanged(::taihe::callback_view<
        void(::taihe::string_view, int32_t)> callback);
    void OffProgressChanged(::taihe::optional_view<
        ::taihe::callback<void(::taihe::string_view, int32_t)>> callback);
    void BindAssetStoreSync(::taihe::string_view assetKey,
        ::ohos::data::distributedDataObject::BindInfo const& bindInfo);

    ::ohos::data::distributedDataObject::ObjectValueType GetValueImpl(::taihe::string_view key);
    void SetValueImpl(::taihe::string_view key, uintptr_t valueRef);
    void SetAssetItemValue(::taihe::string_view key, ::taihe::string_view attr,
        ::ohos::data::distributedDataObject::AssetValueType const& value);
    void UpdateAssetAttr(OHOS::CommonType::AssetValue &asset, std::string const& externalKey,
        std::string const& attr, ::ohos::data::distributedDataObject::AssetValueType const& value);

    ani_ref GetAssetsRefFromStore(ani_env* aniEnv, std::string assetsKey);
    ::ohos::data::distributedDataObject::ObjectValueType ObjectValueTypeToTaihe(
        ani_env* aniEnv, std::string const& externalKey, NativeObjectValueType const& valueObj);

protected:
    static ani_vm* vm_;
    ani_ref globalSourceObj_ = nullptr;
    std::string bundleName_;
    std::string distributedDir_;
    std::map<std::string, NativeObjectValueType> sourceDataMap_;
    std::string objectId_;
    unsigned long version_ = 0;
    int sdkVersion_ = SDK_VERSION_9;

    std::shared_ptr<OHOS::ObjectStore::AniDataobjectSession> session_;
    std::mutex sessionInfoMutex_;
    std::list<VarCallbackType> statusCallBacks_;
    std::mutex statusCallBacksMutex_;
    std::list<VarCallbackType> changeCallBacks_;
    std::mutex changeCallBacksMutex_;
    std::list<VarCallbackType> progressCallBacks_;
    std::mutex progressCallBacksMutex_;
};

} //namespace OHOS::ObjectStore