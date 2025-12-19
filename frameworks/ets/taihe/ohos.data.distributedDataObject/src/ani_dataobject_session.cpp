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

#include <chrono>
#include <thread>

#include "ani_error_utils.h"
#include "ani_utils.h"
#include "logger.h"
#include "object_error.h"
#include "objectstore_errors.h"
#include "ohos.data.distributedDataObject.DataObject.h"

namespace OHOS::ObjectStore {

constexpr const char* ASSET_KEY_SEPARATOR = ".";

AniDataobjectSession::AniDataobjectSession(OHOS::ObjectStore::DistributedObject *obj, const std::string &sessionId)
{
    distributedObj_ = obj;
    sessionId_ = sessionId;
}

AniDataobjectSession::~AniDataobjectSession()
{
}

void AniDataobjectSession::LeaveSession(OHOS::ObjectStore::DistributedObjectStore *objectInfo)
{
    LOG_INFO("LeaveSession, called");
    if (objectInfo == nullptr || distributedObj_ == nullptr) {
        LOG_ERROR("leave seession err");
        auto innerError = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(innerError->GetCode(), innerError->GetMessage());
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

bool AniDataobjectSession::AddWatch(
    OHOS::ObjectStore::DistributedObjectStore *objectStore, const std::string &type, VarCallbackType taiheCallback)
{
    LOG_INFO("AddWatch, called");
    if (type.size() == 0 || objectStore == nullptr) {
        LOG_ERROR("AddWatch err,type = %{public}s", type.c_str());
        return false;
    }
    std::unique_lock<std::shared_mutex> cacheLock(watchMutex_);
    if (watcher_ == nullptr) {
        watcher_ = std::make_shared<AniWatcher>(objectStore, distributedObj_);
        std::weak_ptr<AniWatcher> watcher = watcher_;
        auto changeEventListener = new (std::nothrow) ChangeEventListener(watcher, objectStore, distributedObj_);
        auto statusEventListener = new (std::nothrow) StatusEventListener(watcher, distributedObj_->GetSessionId());
        auto progressEventListener = new (std::nothrow) ProgressEventListener(watcher, distributedObj_->GetSessionId());
        LOG_DEBUG("AniDataobjectSession::AddWatch, SetListener");
        watcher_->SetListener(changeEventListener, statusEventListener, progressEventListener);
    }
    return watcher_->On(type, taiheCallback);
}

void AniDataobjectSession::DeleteWatch(const std::string &type, VarCallbackType taiheCallback)
{
    LOG_INFO("DeleteWatch, called type %{public}s", type.c_str());
    if (type.empty()) {
        LOG_ERROR("delete watch err");
        return;
    }
    std::unique_lock<std::shared_mutex> cacheLock(watchMutex_);
    if (watcher_ != nullptr) {
        watcher_->Off(type, taiheCallback);
        LOG_INFO("DeleteWatch %{public}s", type.c_str());
        if (watcher_->IsEmpty()) {
            LOG_ERROR("delete AniWatcher");
            watcher_ = nullptr;
        }
    } else {
        LOG_ERROR("DataObjectImpl::DeleteWatch watcher_ is null");
    }
}

::ohos::data::distributedDataObject::SaveSuccessResponse AniDataobjectSession::Save(
    const std::string &deviceId, int32_t version)
{
    LOG_INFO("Save, called");
    if (distributedObj_ == nullptr) {
        LOG_ERROR("AniDataobjectSession::Save, distributedObj_ == nullptr");
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "object is null");
        return {};
    }

    uint32_t status = distributedObj_->Save(deviceId);
    if (status == ERR_PROCESSING) {
        auto err = std::make_shared<DeviceNotSupportedError>();
        AniErrorUtils::ThrowError(err->GetCode(), err->GetMessage());
        return {};
    } else if (status != SUCCESS) {
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "operation failed");
        return {};
    }
    return { ::taihe::string(sessionId_), version, deviceId };
}

::ohos::data::distributedDataObject::RevokeSaveSuccessResponse AniDataobjectSession::RevokeSave()
{
    LOG_INFO("RevokeSave, called");
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "object is null");
        return {};
    }
    uint32_t status = distributedObj_->RevokeSave();
    if (status == ERR_PROCESSING) {
        auto err = std::make_shared<DeviceNotSupportedError>();
        AniErrorUtils::ThrowError(err->GetCode(), err->GetMessage());
        return {};
    } else if (status != SUCCESS) {
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "operation failed");
        return {};
    }
    return { ::taihe::string(sessionId_) };
}

uint32_t AniDataobjectSession::BindAssetStore(const std::string &key, OHOS::ObjectStore::AssetBindInfo &nativeBindInfo)
{
    LOG_INFO("BindAssetStore, called");
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "object is null");
        return ERR_NULL_OBJECT;
    }
    uint32_t status = distributedObj_->BindAssetStore(key, nativeBindInfo);
    LOG_INFO("BindAssetStore, ret %{public}d", status);
    if (status == ERR_PROCESSING) {
        auto err = std::make_shared<DeviceNotSupportedError>();
        AniErrorUtils::ThrowError(err->GetCode(), err->GetMessage());
        return {};
    } else if (status != SUCCESS) {
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "operation failed");
        return {};
    }
    return status;
}

uint32_t AniDataobjectSession::SyncDataToStore(
    const std::string &key, const NativeObjectValueType &objValue, bool withPrefix)
{
    if (distributedObj_ == nullptr) {
        LOG_ERROR("distributedObj_ nullptr");
        return 0;
    }

    auto pvalBool = std::get_if<bool>(&objValue);
    if (pvalBool != nullptr) {
        LOG_INFO("SyncDataToStore, PutBoolean %{public}d", *pvalBool);
        distributedObj_->PutBoolean(key, *pvalBool);
        return 0;
    }
    auto pvalDouble = std::get_if<double>(&objValue);
    if (pvalDouble != nullptr) {
        LOG_INFO("SyncDataToStore, PutDouble %{public}f", *pvalDouble);
        distributedObj_->PutDouble(key, *pvalDouble);
        return 0;
    }
    auto pvalStr = std::get_if<std::string>(&objValue);
    if (pvalStr != nullptr) {
        LOG_INFO("SyncDataToStore, PutString %{public}s", pvalStr->c_str());
        distributedObj_->PutString(key, *pvalStr);
        return 0;
    }
    auto pvalArray = std::get_if<std::vector<uint8_t>>(&objValue);
    if (pvalArray != nullptr) {
        LOG_INFO("SyncDataToStore, PutComplex %{public}zu", pvalArray->size());
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

bool AniDataobjectSession::SyncAssetToStore(const std::string &key, const OHOS::CommonType::AssetValue &asset)
{
    if (distributedObj_ == nullptr) {
        LOG_ERROR("distributedObj_ nullptr");
        return false;
    }
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "id", std::string(STRING_TYPE) + asset.id);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "name", std::string(STRING_TYPE) + asset.name);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "uri", std::string(STRING_TYPE) + asset.uri);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "createTime", std::string(STRING_TYPE) + asset.createTime);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "modifyTime", std::string(STRING_TYPE) + asset.modifyTime);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "size", std::string(STRING_TYPE) + asset.size);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "hash", std::string(STRING_TYPE) + asset.hash);
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + "path", std::string(STRING_TYPE) + asset.path);
    distributedObj_->PutDouble(key + ASSET_KEY_SEPARATOR + "status", asset.status);
    return true;
}

bool AniDataobjectSession::SyncAssetPropertyToStore(
    const std::string &key, const std::string &property, uint32_t value)
{
    if (distributedObj_ == nullptr) {
        LOG_ERROR("distributedObj_ nullptr");
        return false;
    }
    distributedObj_->PutDouble(key + ASSET_KEY_SEPARATOR + property, value);
    return true;
}

bool AniDataobjectSession::SyncAssetPropertyToStore(
    const std::string &key, const std::string &property, const std::string &value)
{
    if (distributedObj_ == nullptr) {
        LOG_ERROR("distributedObj_ nullptr");
        return false;
    }
    distributedObj_->PutString(key + ASSET_KEY_SEPARATOR + property, std::string(STRING_TYPE) + value);
    return true;
}

bool AniDataobjectSession::SyncAssetsToStore(
    const std::string &key, const std::vector<OHOS::CommonType::AssetValue> &assets)
{
    LOG_INFO("SyncAssetsToStore, called");
    ani_vm *vm = DataObjectImpl::GetVm();
    if (vm == nullptr || distributedObj_ == nullptr) {
        return false;
    }
    AniUtils::AniExecuteFunc(vm, [&assets, &key, distributedObj = distributedObj_](ani_env *newEnv) {
        ani_object aniobj = AniUtils::AniCreateAssets(newEnv, assets);
        std::string str = DataObjectImpl::JsonStringify(newEnv, aniobj);
        distributedObj->PutString(key, std::string(COMPLEX_TYPE) + str);
    });
    return true;
}

uint32_t AniDataobjectSession::FlushCachedData(const std::map<std::string, NativeObjectValueType> &dataMap)
{
    LOG_INFO("FLushCachedData, called");
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "object is null");
        return ERR_NULL_OBJECT;
    }
    for (const auto &[key, value] : dataMap) {
        SyncDataToStore(key, value, true);
    }
    return 0;
}

NativeObjectValueType AniDataobjectSession::HandleStringType(const char* key)
{
    NativeObjectValueType resultValueType;
    std::string result;
    uint32_t ret = distributedObj_->GetString(key, result);
    if (ret == SUCCESS) {
        resultValueType = result;
        LOG_INFO("GetValueFromStore, result %{public}s", result.c_str());
    }
    return resultValueType;
}

NativeObjectValueType AniDataobjectSession::HandleDoubleType(const char* key)
{
    NativeObjectValueType resultValueType;
    double result = 0;
    uint32_t ret = distributedObj_->GetDouble(key, result);
    if (ret == SUCCESS) {
        resultValueType = result;
        LOG_INFO("GetValueFromStore, result %{public}f", result);
    }
    return resultValueType;
}

NativeObjectValueType AniDataobjectSession::HandleBooleanType(const char* key)
{
    NativeObjectValueType resultValueType;
    bool result;
    uint32_t ret = distributedObj_->GetBoolean(key, result);
    if (ret == SUCCESS) {
        resultValueType = result;
        LOG_INFO("GetValueFromStore, result %{public}d", result);
    }
    return resultValueType;
}

NativeObjectValueType AniDataobjectSession::HandleComplexType(const char* key)
{
    NativeObjectValueType resultValueType;
    std::vector<uint8_t> result;
    uint32_t ret = distributedObj_->GetComplex(key, result);
    if (ret == SUCCESS) {
        resultValueType = result;
        LOG_INFO("GetValueFromStore, TYPE_COMPLEX, result %{public}zu", result.size());
    }
    return resultValueType;
}

NativeObjectValueType AniDataobjectSession::GetValueFromStore(const char *key)
{
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "object is null");
        return std::monostate();
    }

    Type type = TYPE_STRING;
    distributedObj_->GetType(std::string(key), type);
    
    switch (type) {
        case TYPE_STRING:
            return HandleStringType(key);
        case TYPE_DOUBLE:
            return HandleDoubleType(key);
        case TYPE_BOOLEAN:
            return HandleBooleanType(key);
        case TYPE_COMPLEX:
            return HandleComplexType(key);
        default:
            LOG_ERROR("error type! %{public}d", type);
            break;
    }

    return std::monostate();
}

NativeObjectValueType AniDataobjectSession::GetAssetValueFromStore(const char *key)
{
    NativeObjectValueType resultEmpty;
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "object is null");
        return resultEmpty;
    }
    if (key == nullptr) {
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "key is null");
        return resultEmpty;
    }
    std::string stdkey(key);
    OHOS::CommonType::AssetValue assetResult;
    if (distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "id", assetResult.id) != SUCCESS) {
        return resultEmpty;
    }
    if (distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "name", assetResult.name) != SUCCESS) {
        return resultEmpty;
    }
    if (distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "uri", assetResult.uri) != SUCCESS) {
        return resultEmpty;
    }
    if (distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "createTime", assetResult.createTime) != SUCCESS) {
        return resultEmpty;
    }
    if (distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "modifyTime", assetResult.modifyTime) != SUCCESS) {
        return resultEmpty;
    }
    if (distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "size", assetResult.size) != SUCCESS) {
        return resultEmpty;
    }
    if (distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "hash", assetResult.hash) != SUCCESS) {
        return resultEmpty;
    }
    if (distributedObj_->GetString(stdkey + ASSET_KEY_SEPARATOR + "path", assetResult.path) != SUCCESS) {
        return resultEmpty;
    }
    double status = 0;
    if (distributedObj_->GetDouble(stdkey + ASSET_KEY_SEPARATOR + "status", status) != SUCCESS) {
        return resultEmpty;
    }
    assetResult.status = static_cast<uint32_t>(status);
    RemoveTypePrefixForAsset(assetResult);
    return assetResult;
}

NativeObjectValueType AniDataobjectSession::GetAssetsValueFromStore(const char *key, size_t size)
{
    if (distributedObj_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "object is null");
        return std::monostate{};
    }
    if (key == nullptr) {
        auto err = std::make_shared<InnerError>();
        AniErrorUtils::ThrowError(err->GetCode(), "key is null");
        return std::monostate{};
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
    return assets;
}

void AniDataobjectSession::RemoveTypePrefixForAsset(OHOS::CommonType::AssetValue &asset)
{
    auto removePrefixIfNeeded = [](std::string &str) {
        if (str.find(STRING_TYPE) == 0) {
            str = str.substr(std::string(STRING_TYPE).length());
        }
    };

    removePrefixIfNeeded(asset.id);
    removePrefixIfNeeded(asset.name);
    removePrefixIfNeeded(asset.uri);
    removePrefixIfNeeded(asset.createTime);
    removePrefixIfNeeded(asset.modifyTime);
    removePrefixIfNeeded(asset.size);
    removePrefixIfNeeded(asset.hash);
    removePrefixIfNeeded(asset.path);
}
}