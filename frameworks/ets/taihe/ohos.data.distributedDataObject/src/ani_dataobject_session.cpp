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
#define LOG_TAG "AniDataObjectSession"
#include "ani_dataobject_session.h"
#include "ohos.data.distributedDataObject.DataObject.hpp"
#include "logger.h"
#include "ani_utils.h"
#include "ani_error_utils.h"
#include "object_error.h"
#include "objectstore_errors.h"
#include <thread>
#include <chrono>

namespace OHOS::ObjectStore {

const std::string ASSET_KEY_SEPARATOR = ".";

AniDataobjectSession::AniDataobjectSession(OHOS::ObjectStore::DistributedObject* obj,
    std::string sessionId)
{
    distributedObj_ = obj;
    sessionId_ = sessionId;
}

AniDataobjectSession::~AniDataobjectSession()
{
}

void AniDataobjectSession::LeaveSession(OHOS::ObjectStore::DistributedObjectStore *objectInfo)
{
    LOG_INFO("AniDataobjectSession::LeaveSession, called");
    if (objectInfo == nullptr || distributedObj_ == nullptr) {
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return;
    }
    VarCallbackType emptyCallback;
    DeleteWatch(EVENT_CHANGE, emptyCallback);
    DeleteWatch(EVENT_STATUS, emptyCallback);
    DeleteWatch(EVENT_PROGRESS, emptyCallback);
    objectInfo->DeleteObject(distributedObj_->GetSessionId());
    distributedObj_ = nullptr;
    sessionId_.clear();
}

bool AniDataobjectSession::AddWatch(OHOS::ObjectStore::DistributedObjectStore *objectStore, std::string type, VarCallbackType taiheCallback)
{
    LOG_INFO("AniDataobjectSession::AddWatch, called");
    if (type.size() == 0) {
        return false;
    }
    std::unique_lock<std::shared_mutex> cacheLock(watchMutex_);
    if (watcher_ == nullptr) {
        watcher_ = std::make_shared<AniWatcher>(objectStore, distributedObj_);
        std::weak_ptr<AniWatcher> watcher = watcher_;
        auto changeEventListener = new (std::nothrow) ChangeEventListener(watcher, objectStore, distributedObj_);
        auto statusEventListener = new (std::nothrow) StatusEventListener(watcher, distributedObj_->GetSessionId());
        auto progressEventListener = new (std::nothrow) ProgressEventListener(watcher, distributedObj_->GetSessionId());
        LOG_INFO("AniDataobjectSession::AddWatch, SetListener");
        watcher_->SetListener(changeEventListener, statusEventListener, progressEventListener);
    }
    LOG_INFO("AniDataobjectSession::AddWatch, watcher_->On");
    return watcher_->On(type, taiheCallback);
}

void AniDataobjectSession::DeleteWatch(std::string type, VarCallbackType taiheCallback)
{
    LOG_INFO("AniDataobjectSession::DeleteWatch, called");
    if (type.size() == 0) {
        return;
    }
    std::unique_lock<std::shared_mutex> cacheLock(watchMutex_);
    if (watcher_ != nullptr) {
        watcher_->Off(type, taiheCallback);
        LOG_INFO("DataObjectImpl::DeleteWatch %{public}s", type.c_str());
        if (watcher_->IsEmpty()) {
            LOG_DEBUG("delete AniWatcher");
            watcher_ = nullptr;
        }
    } else {
        LOG_ERROR("DataObjectImpl::DeleteWatch watcher_ is null");
    }
}

::ohos::data::distributedDataObject::SaveSuccessResponse AniDataobjectSession::Save(
    std::string deviceId, unsigned long version)
{
    LOG_INFO("AniDataobjectSession::Save, called");
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "object is null");
        return {};
    }

    uint32_t status = distributedObj_->Save(std::string(deviceId));
    if (status == ERR_PROCESSING) {
        auto err = std::make_shared<DeviceNotSupportedError>();
        ani_errorutils::ThrowError(err->GetCode(), err->GetMessage());
        return {};
    } else if (status != SUCCESS) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "operation failed");
        return {};
    }
    return { ::taihe::string(sessionId_), version, deviceId};
}

::ohos::data::distributedDataObject::RevokeSaveSuccessResponse AniDataobjectSession::RevokeSave()
{
    LOG_INFO("AniDataobjectSession::RevokeSave, called");
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "object is null");
        return {};
    }
    uint32_t status = distributedObj_->RevokeSave();
    if (status == ERR_PROCESSING) {
        auto err = std::make_shared<DeviceNotSupportedError>();
        ani_errorutils::ThrowError(err->GetCode(), err->GetMessage());
        return {};
    } else if (status != SUCCESS) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "operation failed");
        return {};
    }
    return { ::taihe::string(sessionId_) };
}

uint32_t AniDataobjectSession::BindAssetStore(std::string key, OHOS::ObjectStore::AssetBindInfo &nativeBindInfo)
{
    LOG_INFO("AniDataobjectSession::BindAssetStore, called");
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "object is null");
        return ERR_NULL_OBJECT;
    }
    uint32_t status = distributedObj_->BindAssetStore(key, nativeBindInfo);
    if (status == ERR_PROCESSING) {
        auto err = std::make_shared<DeviceNotSupportedError>();
        ani_errorutils::ThrowError(err->GetCode(), err->GetMessage());
        return {};
    } else if (status != SUCCESS) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "operation failed");
        return {};
    }
    return status;
}

uint32_t AniDataobjectSession::SyncDataToStore(std::string const& key, NativeObjectValueType const& objValue,
    bool withPrefix)
{
    if (distributedObj_ == nullptr) {
        LOG_ERROR("distributedObj_ nullptr");
        return 0;
    }

    auto pvalBool = std::get_if<bool>(&objValue);
    if (pvalBool != nullptr) {
        LOG_INFO("AniDataobjectSession::SyncDataToStore, PutBoolean %{public}d", *pvalBool);
        distributedObj_->PutBoolean(key, *pvalBool);
        return 0;
    }
    auto pvalDouble = std::get_if<double>(&objValue);
    if (pvalDouble != nullptr) {
        LOG_INFO("AniDataobjectSession::SyncDataToStore, PutDouble %{public}f", *pvalDouble);
        distributedObj_->PutDouble(key, *pvalDouble);
        return 0;
    }
    auto pvalStr = std::get_if<std::string>(&objValue);
    if (pvalStr != nullptr) {
        LOG_INFO("AniDataobjectSession::SyncDataToStore, PutString %{public}s", pvalStr->c_str());
        distributedObj_->PutString(key, *pvalStr);
        return 0;
    }
    auto pvalArray = std::get_if<std::vector<uint8_t>>(&objValue);
    if (pvalArray != nullptr) {
        LOG_INFO("AniDataobjectSession::SyncDataToStore, PutComplex %{public}zu", pvalArray->size());
        distributedObj_->PutComplex(key, *pvalArray);
        return 0;
    }
    auto pvalAsset = std::get_if<OHOS::CommonType::AssetValue>(&objValue);
    if (pvalAsset != nullptr) {
        SyncAssetToStore(key, *pvalAsset);
        return 0;
    }
    auto pvalAssets = std::get_if<std::vector<OHOS::CommonType::AssetValue>>(&objValue);
    if (pvalAssets != nullptr) {
        SyncAssetsToStore(key, *pvalAssets);
        return 0;
    }
    return 0;
}

uint32_t AniDataobjectSession::SyncAssetToStore(std::string const& key, OHOS::CommonType::AssetValue const& asset)
{
    if (distributedObj_ == nullptr) {
        LOG_ERROR("distributedObj_ nullptr");
        return 0;
    }
    LOG_INFO("AniDataobjectSession::SyncAssetToStore, PutString id %{public}s", asset.id.c_str());
    LOG_INFO("AniDataobjectSession::SyncAssetToStore, PutString name %{public}s", asset.name.c_str());
    LOG_INFO("AniDataobjectSession::SyncAssetToStore, PutString uri %{public}s", asset.uri.c_str());
    LOG_INFO("AniDataobjectSession::SyncAssetToStore, PutString createTime %{public}s", asset.createTime.c_str());
    LOG_INFO("AniDataobjectSession::SyncAssetToStore, PutString modifyTime %{public}s", asset.modifyTime.c_str());
    LOG_INFO("AniDataobjectSession::SyncAssetToStore, PutString size %{public}s", asset.size.c_str());
    LOG_INFO("AniDataobjectSession::SyncAssetToStore, PutString hash %{public}s", asset.hash.c_str());
    LOG_INFO("AniDataobjectSession::SyncAssetToStore, PutString path %{public}s", asset.path.c_str());
    LOG_INFO("AniDataobjectSession::SyncAssetToStore, PutDouble status %{public}d", asset.status);

    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "id", STRING_TYPE + asset.id);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "name", STRING_TYPE + asset.name);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "uri", STRING_TYPE + asset.uri);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "createTime", STRING_TYPE + asset.createTime);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "modifyTime", STRING_TYPE + asset.modifyTime);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "size", STRING_TYPE + asset.size);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "hash", STRING_TYPE + asset.hash);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "path", STRING_TYPE + asset.path);
    distributedObj_->PutDouble(key + ASSET_KEY_SEPARATOR + "status", asset.status);
    return 0;
}

uint32_t AniDataobjectSession::SyncAssetsToStore(std::string const& key, std::vector<OHOS::CommonType::AssetValue> const& assets)
{
    LOG_INFO("AniDataobjectSession::SyncAssetsToStore, called");
    ani_vm* vm = DataObjectImpl::GetVm();
    if (vm == nullptr) {
        return 0;
    }
    ani_utils::AniExecuteFunc(vm, [&] (ani_env* newEnv) {
        ani_object aniobj = ani_utils::AniCreateAssets(newEnv, assets);
        std::string str = DataObjectImpl::JsonStringify(newEnv, aniobj);
        LOG_INFO("AniDataobjectSession::SyncAssetsToStore, Json %{public}s", str.c_str());
        distributedObj_->PutString(key, COMPLEX_TYPE + str);
    });
    return 0;
}

uint32_t AniDataobjectSession::FlushCachedData(std::map<std::string, NativeObjectValueType> const& dataMap)
{
    LOG_INFO("AniDataobjectSession::FLushCachedData, called");
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "object is null");
        return ERR_NULL_OBJECT;
    }
    for (const auto &[key, value] : dataMap) {
        SyncDataToStore(key, value, true);
    }
    return 0;
}

NativeObjectValueType AniDataobjectSession::GetValueFromStore(const char *key)
{
    LOG_INFO("AniDataobjectSession::GetValueFromStore, called");
    NativeObjectValueType resultValueType;
    ani_env *aniEnv = taihe::get_env();
    if (aniEnv == nullptr) {
        LOG_INFO("DataObjectImpl::GetValueFromStore, aniEnv null");
        return resultValueType;
    }
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "object is null");
        return resultValueType;
    }

    std::string keyString = key;
    Type type = TYPE_STRING;
    distributedObj_->GetType(keyString, type);
    switch (type) {
        case TYPE_STRING: {
            std::string result;
            uint32_t ret = distributedObj_->GetString(keyString, result);
            if (ret == SUCCESS) {
                resultValueType = result;
                LOG_INFO("DataObjectImpl::GetValueFromStore, result %{public}s", result.c_str());
            }
            break;
        }
        case TYPE_DOUBLE: {
            double result = 0;
            uint32_t ret = distributedObj_->GetDouble(keyString, result);
            if (ret == SUCCESS) {
                resultValueType = result;
                LOG_INFO("DataObjectImpl::GetValueFromStore, result %{public}f", result);
            }
            break;
        }
        case TYPE_BOOLEAN: {
            bool result;
            uint32_t ret = distributedObj_->GetBoolean(keyString, result);
            if (ret == SUCCESS) {
                resultValueType = result;
                LOG_INFO("DataObjectImpl::GetValueFromStore, result %{public}d", result);
            }
            break;
        }
        case TYPE_COMPLEX: {
            std::vector<uint8_t> result;
            uint32_t ret = distributedObj_->GetComplex(keyString, result);
            if (ret == SUCCESS) {
                resultValueType = result;
                LOG_INFO("DataObjectImpl::GetValueFromStore, TYPE_COMPLEX, result %{public}zu", result.size());
            }
            break;
        }
        default: {
            LOG_ERROR("error type! %{public}d", type);
            break;
        }
    }
    return resultValueType;
}

NativeObjectValueType AniDataobjectSession::GetAssetValueFromStore(const char *key)
{
    LOG_INFO("AniDataobjectSession::GetAssetValueFromStore, called");
    NativeObjectValueType resultEmpty;
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "object is null");
        return resultEmpty;
    }
    if (key == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "key is null");
        return resultEmpty;
    }
    std::string stdkey(key);
    OHOS::CommonType::AssetValue assetResult;
    if (SUCCESS != distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "id", assetResult.id)) {
        return resultEmpty;
    }
    if (SUCCESS != distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "name", assetResult.name)) {
        return resultEmpty;
    }
    if (SUCCESS != distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "uri", assetResult.uri)) {
        return resultEmpty;
    }
    if (SUCCESS != distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "createTime", assetResult.createTime)) {
        return resultEmpty;
    }
    if (SUCCESS != distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "modifyTime", assetResult.modifyTime)) {
        return resultEmpty;
    }
    if (SUCCESS != distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "size", assetResult.size)) {
        return resultEmpty;
    }
    if (SUCCESS != distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "hash", assetResult.hash)) {
        return resultEmpty;
    }
    if (SUCCESS != distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "path", assetResult.path)) {
        return resultEmpty;
    }
    double status = 0;
    if (SUCCESS != distributedObj_->GetDouble(stdkey + ASSET_KEY_SEPARATOR + "status", status)) {
        return resultEmpty;
    }
    assetResult.status = (uint32_t)(status);
    RemoveTypePrefixForAsset(assetResult);
    return assetResult;
}

NativeObjectValueType AniDataobjectSession::GetAssetsValueFromStore(const char *key, size_t size)
{
    LOG_INFO("AniDataobjectSession::GetAssetsValueFromStore, called");
    NativeObjectValueType temp;
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "object is null");
        return temp;
    }
    if (key == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "key is null");
        return temp;
    }
    std::vector<OHOS::CommonType::AssetValue> assets;
    for (size_t k = 0; k < size; k++) {
        std::string tempKey = std::string(key) + std::to_string(k);
        NativeObjectValueType temp = GetAssetValueFromStore(tempKey.c_str());
        auto pvalAsset = std::get_if<OHOS::CommonType::AssetValue>(&temp);
        if (pvalAsset != nullptr) {
            assets.push_back(*pvalAsset);
        }
    }
    temp = assets;
    return temp;
}

void AniDataobjectSession::RemoveTypePrefixForAsset(OHOS::CommonType::AssetValue &asset)
{
    LOG_INFO("AniDataobjectSession::RemoveTypePrefixForAsset, called");
    if (asset.id.find(STRING_TYPE) == 0) {
        asset.id = asset.id.substr(STRING_TYPE.length());
    }
    if (asset.name.find(STRING_TYPE) == 0) {
        asset.name = asset.name.substr(STRING_TYPE.length());
    }
    if (asset.uri.find(STRING_TYPE) == 0) {
        asset.uri = asset.uri.substr(STRING_TYPE.length());
    }
    if (asset.createTime.find(STRING_TYPE) == 0) {
        asset.createTime = asset.createTime.substr(STRING_TYPE.length());
    }
    if (asset.modifyTime.find(STRING_TYPE) == 0) {
        asset.modifyTime = asset.modifyTime.substr(STRING_TYPE.length());
    }
    if (asset.size.find(STRING_TYPE) == 0) {
        asset.size = asset.size.substr(STRING_TYPE.length());
    }
    if (asset.hash.find(STRING_TYPE) == 0) {
        asset.hash = asset.hash.substr(STRING_TYPE.length());
    }
    if (asset.path.find(STRING_TYPE) == 0) {
        asset.path = asset.path.substr(STRING_TYPE.length());
    }
}

}