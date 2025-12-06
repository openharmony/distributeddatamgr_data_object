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

#include "ohos.data.distributedDataObject.proj.hpp"
#include "ohos.data.distributedDataObject.impl.hpp"
#include "ohos.data.distributedDataObject.DataObject.h"
#include <random>
#include <regex>
#include <mutex>
#include <sys/stat.h>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "ani_base_context.h"
#include "ani_error_utils.h"
#include "application_context.h"
#include "context.h"
#include "ani_error_utils.h"
#include "ani_utils.h"
#include "logger.h"
#include "object_error.h"
#include "objectstore_errors.h"

namespace OHOS::ObjectStore {
using namespace OHOS;

static const int32_t SESSION_ID_MAX_LENGTH = 128;
static const int32_t ASSETS_MAX_NUMBER = 50;

ani_vm* DataObjectImpl::vm_ = nullptr;

DataObjectImpl::DataObjectImpl(ani_object context, ani_object sourceObj,
    ::taihe::array_view<::taihe::string> keys)
{
    objectId_ = GenerateRandomNum();
    UpdateBundleInfoWithContext(context);
    ParseObject(sourceObj, keys);
    ani_env *aniEnv = taihe::get_env();
    if (aniEnv != nullptr && ANI_OK != aniEnv->GetVM(&vm_)) {
        vm_ = nullptr;
    }
}

DataObjectImpl::~DataObjectImpl()
{
    VarCallbackType empty;
    DeleteChangeCallback(empty);
    DeleteStatusCallback(empty);
    DeleteProgressCallback(empty);
    LeaveSession();
}

std::string DataObjectImpl::JsonStringify(ani_env* aniEnv, ani_ref sourceObj)
{
    if (aniEnv == nullptr || sourceObj == nullptr) {
        LOG_ERROR("aniEnv = nullptr || sourceObj = nullptr");
        return {};
    }
    ani_namespace doSpace = {};
    auto aniRet = aniEnv->FindNamespace("@ohos.data.distributedDataObject.distributedDataObject", &doSpace);
    if (aniRet != ANI_OK) {
        LOG_ERROR("JsonStringify, FindNamespace failed %{public}d", aniRet);
        return {};
    }

    ani_function funcStringify = {};
    aniRet = aniEnv->Namespace_FindFunction(doSpace,
        "jsonStringify", "C{std.core.Object}:C{std.core.String}", &funcStringify);
    if (aniRet != ANI_OK) {
        LOG_ERROR("Namespace_FindFunction jsonStringify failed %{public}d", aniRet);
        return {};
    }

    ani_ref result = nullptr;
    aniRet = aniEnv->Function_Call_Ref(funcStringify, &result, sourceObj);
    if (aniRet != ANI_OK || result == nullptr) {
        LOG_ERROR("Function_Call_Ref jsonStringify failed %{public}d", aniRet);
        return {};
    }
    return ani_utils::AniStringUtils::ToStd(aniEnv, (ani_string)result);
}

ani_ref DataObjectImpl::JsonParseToJsonElement(ani_env* aniEnv, std::string const& str)
{
    if (aniEnv == nullptr) {
        LOG_ERROR("aniEnv = nullptr");
        return {};
    }
    ani_ref nullref = nullptr;
    auto aniRet = aniEnv->GetNull(&nullref);
    
    ani_namespace doSpace = {};
    aniRet = aniEnv->FindNamespace("@ohos.data.distributedDataObject.distributedDataObject", &doSpace);
    if (aniRet != ANI_OK) {
        LOG_ERROR("JsonParseToJsonElement, FindNamespace failed %{public}d", aniRet);
        return nullref;
    }
    ani_function funcToObj = {};
    aniRet = aniEnv->Namespace_FindFunction(doSpace,
        "jsonParseJsonElement", "C{std.core.String}:C{std.core.Object}", &funcToObj);
    if (aniRet != ANI_OK) {
        LOG_ERROR("Namespace_FindFunction jsonParseJsonElement failed %{public}d", aniRet);
        return nullref;
    }
    ani_string aniStr = ani_utils::AniCreateString(aniEnv, str);
    ani_ref result = nullptr;
    aniRet = aniEnv->Function_Call_Ref(funcToObj, &result, aniStr);
    if (aniRet != ANI_OK) {
        LOG_ERROR("Function_Call_Ref jsonParseJsonElement failed %{public}d", aniRet);
        return nullref;
    }
    return result;
}

NativeObjectValueType DataObjectImpl::ParseObjectValue(ani_env *aniEnv, ani_ref valueRef)
{
    NativeObjectValueType objValue;
    if (aniEnv == nullptr || valueRef == nullptr) {
        return objValue;
    }
    ani_object objRef = reinterpret_cast<ani_object>(valueRef);
    ani_utils::UnionAccessor accessor(aniEnv, objRef);
    std::string strValue;
    bool boolValue = false;
    double doubleValue = 0;
    std::vector<uint8_t> uint8Array;
    OHOS::CommonType::Asset asset;
    std::vector<OHOS::CommonType::Asset> assets;

    ani_boolean isNull = ANI_FALSE;
    aniEnv->Reference_IsNull(valueRef, &isNull);
    ani_boolean isUndefined = ANI_FALSE;
    aniEnv->Reference_IsUndefined(valueRef, &isUndefined);

    if (isNull) {
        LOG_INFO("ParseObjectValue, isNull");
        objValue = NULL_TYPE;
    } else if (isUndefined) {
        LOG_INFO("ParseObjectValue, isUndefined");
    } else if (accessor.TryConvert<std::string>(strValue)) {
        LOG_INFO("ParseObjectValue, string");
        objValue = STRING_TYPE + strValue;
    } else if (accessor.TryConvertToNumber(doubleValue)) {
        LOG_INFO("ParseObjectValue, number");
        objValue = doubleValue;
    } else if (accessor.TryConvert<bool>(boolValue)) {
        LOG_INFO("ParseObjectValue, bool");
        objValue = boolValue;
    } else if (accessor.TryConvert<std::vector<uint8_t>>(uint8Array)) {
        LOG_INFO("ParseObjectValue, uint8Array");
        objValue = uint8Array;
    } else if (accessor.TryConvert<OHOS::CommonType::Asset>(asset)) {
        LOG_INFO("ParseObjectValue, Asset");
        objValue = asset;
    } else if (accessor.TryConvert<std::vector<OHOS::CommonType::Asset>>(assets)) {
        LOG_INFO("ParseObjectValue, Assets");
        objValue = assets;
    } else if (accessor.IsInstanceOf("std.core.Object")) {
        std::string strobj = JsonStringify(aniEnv, valueRef);
        LOG_INFO("ParseObjectValue, Object JsonStringify, %{public}s", strobj.c_str());
        objValue = COMPLEX_TYPE + strobj;
    }
    return objValue;
}

bool DataObjectImpl::ParseRecord(ani_env* env, ani_ref recordRef,
    std::map<std::string, NativeObjectValueType> &resultMap)
{
    if (env == nullptr || recordRef == nullptr) {
        return false;
    }

    ani_method entryMethod = ani_utils::AniGetClassMethod(env,
        "escompat.Record", "entries", ":C{std.core.IterableIterator}");
    ani_method iterNextMethod = ani_utils::AniGetClassMethod(env,
        "std.core.Iterator", "next", ":C{std.core.IteratorResult}");
    ani_class iterResultClass = ani_utils::AniGetClass(env, "std.core.IteratorResult");
    if (entryMethod == nullptr || iterNextMethod == nullptr || iterResultClass == nullptr) {
        LOG_ERROR("ParseRecord, get iter failed");
        return false;
    }
    ani_object ani_iter = {};
    env->Object_CallMethod_Ref(static_cast<ani_object>(recordRef), entryMethod, reinterpret_cast<ani_ref*>(&ani_iter));
    ani_boolean iter_done = false;
    while (!iter_done) {
        ani_object temp_ani_next = {};
        env->Object_CallMethod_Ref(ani_iter, iterNextMethod, reinterpret_cast<ani_ref*>(&temp_ani_next));
        ani_field fieldTmp = ani_utils::AniFindClassField(env, iterResultClass, "done");
        env->Object_GetField_Boolean(temp_ani_next, fieldTmp, &iter_done);
        if (iter_done) {
            break;
        }
        ani_tuple_value temp_ani_item = {};
        fieldTmp = ani_utils::AniFindClassField(env, iterResultClass, "value");
        env->Object_GetField_Ref(temp_ani_next, fieldTmp, reinterpret_cast<ani_ref*>(&temp_ani_item));
        ani_ref temp_ani_key = {};
        env->TupleValue_GetItem_Ref(temp_ani_item, 0, &temp_ani_key);
        std::string stdkey;
        ani_object keyobj = reinterpret_cast<ani_object>(temp_ani_key);
        ani_utils::UnionAccessor acessor(env, keyobj);
        if (!acessor.TryConvert(stdkey) || stdkey.size() == 0) {
            break;
        }
        ani_ref temp_ani_val = {};
        env->TupleValue_GetItem_Ref(temp_ani_item, 1, &temp_ani_val);
        NativeObjectValueType parseResult = ParseObjectValue(env, temp_ani_val);
        resultMap[stdkey] = parseResult;
        LOG_INFO("ParseRecord, stdkey %{public}s", stdkey.c_str());
    }
    return true;
}

void DataObjectImpl::ParseObject(ani_object sourceObj, ::taihe::array_view<::taihe::string> const& keys)
{
    ani_env *aniEnv = taihe::get_env();
    if (aniEnv == nullptr || sourceObj == nullptr) {
        return;
    }
    ani_utils::UnionAccessor accessor(aniEnv, sourceObj);
    std::vector<ani_ref> aniRefArray;
    bool result = accessor.TryConvertArray(aniRefArray);
    if (result) {
        LOG_INFO("ParseObject, sourceObj is Array");
        for (size_t k = 0; k < aniRefArray.size(); k++) {
            ani_ref item = aniRefArray[k];
            NativeObjectValueType parseResult = ParseObjectValue(aniEnv, item);
            sourceDataMap_[std::to_string(k)] = parseResult;
        }
        return;
    }
    if (accessor.IsInstanceOf("escompat.Record")) {
        LOG_INFO("ParseObject, sourceObj Record");
        ParseRecord(aniEnv, sourceObj, sourceDataMap_);
        return;
    }
    for (auto itemKey : keys) {
        LOG_INFO("ParseObject, itemKey %{public}s", itemKey.c_str());
        ani_ref aniValue = nullptr;
        ani_status status = aniEnv->Object_GetPropertyByName_Ref(sourceObj, itemKey.c_str(), &aniValue);
        if (status == ANI_OK) {
            NativeObjectValueType parseResult = ParseObjectValue(aniEnv, aniValue);
            sourceDataMap_[std::string(itemKey)] = parseResult;
        }
    }
}

std::string DataObjectImpl::GenerateRandomNum()
{
    std::random_device randomDevice;
    std::uniform_int_distribution<uint32_t> distribution(0, std::numeric_limits<uint32_t>::max());
    std::string randomStr = std::to_string(distribution(randomDevice));
    return randomStr;
}

void DataObjectImpl::UpdateBundleInfo()
{
    if (bundleName_.empty()) {
        bundleName_ = AbilityRuntime::Context::GetApplicationContext()->GetBundleName();
    }
    if (distributedDir_.empty()) {
        distributedDir_ = AbilityRuntime::Context::GetApplicationContext()->GetDistributedFilesDir();
    }
}

void DataObjectImpl::UpdateBundleInfoWithContext(ani_object context)
{
    if (context == nullptr) {
        return UpdateBundleInfo();
    }
    ani_env *aniEnv = taihe::get_env();
    if (aniEnv == nullptr) {
        return UpdateBundleInfo();
    }
    auto abilityContext = OHOS::AbilityRuntime::GetStageModeContext(aniEnv, context);
    if (abilityContext == nullptr) {
        return UpdateBundleInfo();
    }
    bundleName_ = abilityContext->GetBundleName();
    distributedDir_ = abilityContext->GetDistributedFilesDir();
}

DistributedObjectStore* DataObjectImpl::GetObjectStore()
{
    DistributedObjectStore *objectInfo = DistributedObjectStore::GetInstance(bundleName_);
    return objectInfo;
}

bool DataObjectImpl::IsSandBox()
{
    int32_t dlpFlag = Security::AccessToken::AccessTokenKit::GetHapDlpFlag(
        AbilityRuntime::Context::GetApplicationContext()->GetApplicationInfo()->accessTokenId);
    if (dlpFlag != 0) {
        return true;
    }
    return false;
}

bool DataObjectImpl::IsSameTaiheCallback(VarCallbackType p1, VarCallbackType p2)
{
    if (std::holds_alternative<std::monostate>(p1) &&
        std::holds_alternative<std::monostate>(p2)) {
        return true;
    }
    if (std::holds_alternative<AniChangeCallbackType>(p1) &&
        std::holds_alternative<AniChangeCallbackType>(p2)) {
        AniChangeCallbackType* p1val = std::get_if<AniChangeCallbackType>(&p1);
        AniChangeCallbackType* p2val = std::get_if<AniChangeCallbackType>(&p2);
        if (p1val != nullptr && p2val != nullptr) {
            return (p1val == p2val);
        } else {
            return false;
        }
    }
    if (std::holds_alternative<AniStatusCallbackType>(p1) &&
        std::holds_alternative<AniStatusCallbackType>(p2)) {
        AniStatusCallbackType* p1val = std::get_if<AniStatusCallbackType>(&p1);
        AniStatusCallbackType* p2val = std::get_if<AniStatusCallbackType>(&p2);
        if (p1val != nullptr && p2val != nullptr) {
            return (p1val == p2val);
        } else {
            return false;
        }
    }
    return false;
}

bool DataObjectImpl::AddChangeCallback(AniChangeCallbackType taiheCallback)
{
    std::lock_guard<std::mutex> guard(changeCallBacksMutex_);
    for (auto &item : changeCallBacks_) {
        AniChangeCallbackType* pval = std::get_if<AniChangeCallbackType>(&item);
        if (pval == nullptr) {
            continue;
        }
        if (IsSameTaiheCallback(*pval, taiheCallback)) {
            LOG_WARN("AddChangeCallback, callback duplicated");
            return true;
        }
    }
    changeCallBacks_.push_back(taiheCallback);
    return true;
}

bool DataObjectImpl::AddStatusCallback(AniStatusCallbackType taiheCallback)
{
    std::lock_guard<std::mutex> guard(statusCallBacksMutex_);
    for (auto &item : statusCallBacks_) {
        AniStatusCallbackType* pval = std::get_if<AniStatusCallbackType>(&item);
        if (pval == nullptr) {
            continue;
        }
        if (IsSameTaiheCallback(*pval, taiheCallback)) {
            LOG_WARN("AddStatusCallback, callback duplicated");
            return true;
        }
    }
    statusCallBacks_.push_back(taiheCallback);
    return true;
}

bool DataObjectImpl::AddProgressCallback(AniProgressCallbackType taiheCallback)
{
    std::lock_guard<std::mutex> guard(progressCallBacksMutex_);
    for (auto &item : progressCallBacks_) {
        AniProgressCallbackType* pval = std::get_if<AniProgressCallbackType>(&item);
        if (pval == nullptr) {
            continue;
        }
        if (IsSameTaiheCallback(*pval, taiheCallback)) {
            LOG_WARN("AddProgressCallback, callback duplicated");
            return true;
        }
    }
    progressCallBacks_.push_back(taiheCallback);
    return true;
}

bool DataObjectImpl::DeleteChangeCallback(VarCallbackType taiheCallback)
{
    std::lock_guard<std::mutex> guard(changeCallBacksMutex_);
    if (std::holds_alternative<std::monostate>(taiheCallback)) {
        LOG_INFO("DataObjectImpl::DeleteChangeCallback, erase all");
        changeCallBacks_.clear();
        return true;
    }

    for (auto iter = changeCallBacks_.begin(); iter != changeCallBacks_.end(); ++iter) {
        AniChangeCallbackType* pval = std::get_if<AniChangeCallbackType>(&(*iter));
        if (pval == nullptr) {
            continue;
        }
        if (IsSameTaiheCallback(*pval, taiheCallback)) {
            LOG_INFO("callback found, erased");
            changeCallBacks_.erase(iter);
            return true;
        }
    }
    return true;
}

bool DataObjectImpl::DeleteStatusCallback(VarCallbackType taiheCallback)
{
    std::lock_guard<std::mutex> guard(statusCallBacksMutex_);
    if (std::holds_alternative<std::monostate>(taiheCallback)) {
        LOG_INFO("DataObjectImpl::DeleteStatusCallback, erase all");
        statusCallBacks_.clear();
        return true;
    }

    for (auto iter = statusCallBacks_.begin(); iter != statusCallBacks_.end(); ++iter) {
        AniStatusCallbackType* pval = std::get_if<AniStatusCallbackType>(&(*iter));
        if (pval == nullptr) {
            continue;
        }
        if (IsSameTaiheCallback(*pval, taiheCallback)) {
            LOG_INFO("callback found, erased");
            statusCallBacks_.erase(iter);
            return true;
        }
    }
    return true;
}

bool DataObjectImpl::DeleteProgressCallback(VarCallbackType taiheCallback)
{
    std::lock_guard<std::mutex> guard(progressCallBacksMutex_);
    if (std::holds_alternative<std::monostate>(taiheCallback)) {
        LOG_INFO("DataObjectImpl::DeleteStatusCallback, erase all");
        progressCallBacks_.clear();
        return true;
    }

    for (auto iter = progressCallBacks_.begin(); iter != progressCallBacks_.end(); ++iter) {
        AniProgressCallbackType* pval = std::get_if<AniProgressCallbackType>(&(*iter));
        if (pval == nullptr) {
            continue;
        }
        if (IsSameTaiheCallback(*pval, taiheCallback)) {
            LOG_INFO("callback found, erased");
            progressCallBacks_.erase(iter);
            return true;
        }
    }
    return true;
}

void DataObjectImpl::RestoreWatchers(const std::string &objectId)
{
    LOG_INFO("DataObjectImpl::RestoreWatchers %{public}s", objectId.c_str());
    std::lock_guard<std::mutex> guard(sessionInfoMutex_);
    if (session_ == nullptr) {
        return;
    }
    if (objectId.size() == 0) {
        LOG_ERROR("DataObjectImpl::RestoreWatchers, objectId empty");
        return;
    }
    {
        std::lock_guard<std::mutex> guard(changeCallBacksMutex_);
        for (auto &item : changeCallBacks_) {
            AniChangeCallbackType* pval = std::get_if<AniChangeCallbackType>(&item);
            if (pval == nullptr) {
                continue;
            }
            session_->AddWatch(GetObjectStore(), EVENT_CHANGE, *pval);
        }
    }
    {
        std::lock_guard<std::mutex> guard(statusCallBacksMutex_);
        for (auto &item : statusCallBacks_) {
            AniStatusCallbackType* pval = std::get_if<AniStatusCallbackType>(&item);
            if (pval == nullptr) {
                continue;
            }
            session_->AddWatch(GetObjectStore(), EVENT_STATUS, *pval);
        }
    }
    {
        std::lock_guard<std::mutex> guard(progressCallBacksMutex_);
        for (auto &item : progressCallBacks_) {
            AniProgressCallbackType* pval = std::get_if<AniProgressCallbackType>(&item);
            if (pval == nullptr) {
                continue;
            }
            session_->AddWatch(GetObjectStore(), EVENT_PROGRESS, *pval);
        }
    }
}

bool DataObjectImpl::JoinSession(std::string sessionId)
{
    LOG_INFO("DataObjectImpl::JoinSession, called");
    if (sourceDataMap_.size() == 0) {
        LOG_INFO("DataObjectImpl::JoinSession, sourceObj_ nullptr");
        return false;
    }
    if (IsSandBox()) {
        LOG_INFO("DataObjectImpl::JoinSession, IsSandBox");
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return false;
    }
    OHOS::ObjectStore::DistributedObjectStore *objectStore = GetObjectStore();
    if (objectStore == nullptr) {
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return false;
    }
    uint32_t result = 0;
    OHOS::ObjectStore::DistributedObject *object = objectStore->CreateObject(sessionId, result);
    if (result == ERR_EXIST) {
        auto err = std::make_shared<DatabaseError>();
        ani_errorutils::ThrowError(err->GetCode(), err->GetMessage());
        return false;
    } else if (result == ERR_NO_PERMISSION) {
        auto err = std::make_shared<PermissionError>();
        ani_errorutils::ThrowError(err->GetCode(), err->GetMessage());
        return false;
    } else if (result != SUCCESS || object == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), err->GetMessage());
        return false;
    }
    {
        std::lock_guard<std::mutex> guard(sessionInfoMutex_);
        session_ = std::make_shared<AniDataobjectSession>(object, sessionId);
    }
    RestoreWatchers(objectId_);
    objectStore->NotifyCachedStatus(object->GetSessionId());
    objectStore->NotifyProgressStatus(object->GetSessionId());
    {
        std::lock_guard<std::mutex> guard(sessionInfoMutex_);
        session_->FlushCachedData(sourceDataMap_);
    }
    return true;
}

void DataObjectImpl::LeaveSession()
{
    LOG_INFO("DataObjectImpl::LeaveSession, called");
    std::lock_guard<std::mutex> guard(sessionInfoMutex_);
    if (session_ == nullptr) {
        return;
    }
    OHOS::ObjectStore::DistributedObjectStore *objectInfo = GetObjectStore();
    session_->LeaveSession(objectInfo);
    session_ = nullptr;
}

void DataObjectImpl::SetSessionIdSync(::taihe::optional_view<::taihe::string> sessionId)
{
    if (!sessionId.has_value() || sessionId.value().size() == 0) {
        LOG_INFO("DataObjectImpl::SetSessionIdSync, sessionId empty");
        LeaveSession();
        return;
    }
    std::string paraSessionId = std::string(sessionId.value());
    LOG_INFO("DataObjectImpl::SetSessionIdSync, paraSessionId %{public}s", paraSessionId.c_str());

    {
        if (session_ != nullptr) {
            LOG_INFO("DataObjectImpl::SetSessionIdSync, session_ %{public}p, %{public}s",
                session_.get(), session_->GetSessionId().c_str());
        }

        std::lock_guard<std::mutex> guard(sessionInfoMutex_);
        if (session_ != nullptr && paraSessionId == session_->GetSessionId()) {
            LOG_INFO("DataObjectImpl::SetSessionIdSync, same sessionid, skip");
            return;
        }
    }

    LeaveSession();
    std::regex pattern(SESSION_ID_REGEX);
    if (paraSessionId.size() > SESSION_ID_MAX_LENGTH || !std::regex_match(paraSessionId, pattern)) {
        LOG_INFO("DataObjectImpl::SetSessionIdSync, invalid sessionid");
        ani_errorutils::ThrowError(ani_errorutils::AniError_ParameterCheck,
            "The sessionId allows only letters, digits, and underscores(_), and cannot exceed 128 in length.");
        return;
    }
    JoinSession(paraSessionId);
}

::ohos::data::distributedDataObject::SaveSuccessResponse DataObjectImpl::SaveSync(::taihe::string_view deviceId)
{
    LOG_INFO("DataObjectImpl::SaveSync");
    std::lock_guard<std::mutex> guard(sessionInfoMutex_);
    if (session_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "object is null");
        return {};
    }
    auto result = session_->Save(std::string(deviceId), version_);
    return result;
}

::ohos::data::distributedDataObject::RevokeSaveSuccessResponse DataObjectImpl::RevokeSaveSync()
{
    LOG_INFO("DataObjectImpl::RevokeSaveSync");
    std::lock_guard<std::mutex> guard(sessionInfoMutex_);
    if (session_ == nullptr) {
        auto err = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(err->GetCode(), "object is null");
        return {};
    }
    auto result = session_->RevokeSave();
    return result;
}

bool DataObjectImpl::GetFileAttribute(const std::string &pathName, size_t &size, std::string &ctime, std::string &mtime)
{
    LOG_INFO("DataObjectImpl::GetFileAttribute");
    if (pathName.empty()) {
        LOG_ERROR("GetFileAttribute, input parameter exception.");
        return false;
    }
    struct stat statbuf;
    int ret = stat(pathName.c_str(), &statbuf);
    if (ret != 0) {
        LOG_ERROR("GetFileSize, GetFileAttribute failed, ret:%{public}d, errno:%{public}d.", ret, errno);
        return false;
    }
    size = statbuf.st_size;
    ctime = std::to_string(statbuf.st_ctim.tv_sec);
    mtime = std::to_string(statbuf.st_mtim.tv_sec);
    return true;
}

OHOS::CommonType::AssetValue DataObjectImpl::GetDefaultAsset(OHOS::CommonType::AssetValue initValue,
    const std::string &uri)
{
    LOG_INFO("DataObjectImpl::GetDefaultAsset, distributedDir_ %{public}s", distributedDir_.c_str());
    OHOS::CommonType::AssetValue assetRet = initValue;

    std::size_t pos = uri.rfind("/");
    std::string fileName = uri;
    if (pos != std::string::npos) {
        fileName = uri.substr(pos + 1);
    }
    std::string filePath = distributedDir_ + '/' + fileName;

    size_t fileSize = 0;
    std::string createTime;
    std::string modifyTime;
    bool result = GetFileAttribute(filePath, fileSize, createTime, modifyTime);
    if (result == false) {
        LOG_ERROR("GetFileAttribute, failed");
        return assetRet;
    }
    assetRet.name = fileName;
    assetRet.uri = uri;
    assetRet.path = filePath;
    assetRet.createTime = createTime;
    assetRet.modifyTime = modifyTime;
    assetRet.size = std::to_string(fileSize);
    LOG_INFO("DataObjectImpl::GetDefaultAsset, %{public}zu", fileSize);
    return assetRet;
}

void DataObjectImpl::SetAssetSync(::taihe::string_view assetKey, ::taihe::string_view uri)
{
    LOG_INFO("DataObjectImpl::SetAssetSync");
    std::lock_guard<std::mutex> guard(sessionInfoMutex_);
    if (session_ != nullptr) {
        ani_errorutils::ThrowError(ani_errorutils::AniError_SessionJoined,
            "SessionId has been set, and asset cannot be set.");
        return;
    }
    if (uri.size() == 0) {
        ani_errorutils::ThrowError(ani_errorutils::AniError_ParameterError,
            "The asset uri to be set is null.");
        return;
    }
    if (assetKey.size() == 0 || uri.size() == 0) {
        ani_errorutils::ThrowError(ani_errorutils::AniError_ParameterError,
            "The property or uri of the asset is invalid.");
        return;
    }
    OHOS::CommonType::AssetValue initValue;
    if (sourceDataMap_.find(std::string(assetKey)) != sourceDataMap_.end()) {
        auto tmp = sourceDataMap_[std::string(assetKey)];
        auto pvalaAsset = std::get_if<OHOS::CommonType::AssetValue>(&tmp);
        if (pvalaAsset != nullptr) {
            initValue = *pvalaAsset;
        }
    }
    auto asset = GetDefaultAsset(initValue, std::string(uri));
    std::vector<OHOS::CommonType::AssetValue> assets = { asset };
    NativeObjectValueType objValue = assets;
    sourceDataMap_[std::string(assetKey)] = objValue;
    sourceDataMap_[std::string(assetKey) + "0"] = asset;
}

void DataObjectImpl::SetAssetsSync(::taihe::string_view assetsKey, ::taihe::array_view<::taihe::string> uris)
{
    LOG_INFO("DataObjectImpl::SetAssetsSync");
    std::lock_guard<std::mutex> guard(sessionInfoMutex_);
    if (session_ != nullptr) {
        ani_errorutils::ThrowError(ani_errorutils::AniError_SessionJoined,
            "SessionId has been set, and asset cannot be set.");
        return;
    }
    if (uris.size() == 0 || uris.size() > ASSETS_MAX_NUMBER) {
        ani_errorutils::ThrowError(ani_errorutils::AniError_ParameterError,
            "The uri array of the set assets is not an array or the length is invalid.");
        return;
    }
    for (auto uri : uris) {
        if (uri.size() == 0) {
            ani_errorutils::ThrowError(ani_errorutils::AniError_ParameterError, "Uri in assets array is invalid.");
            return;
        }
    }
    std::vector<OHOS::CommonType::AssetValue> assets;
    int32_t assetIndex = 0;
    for (auto uri : uris) {
        auto asset = GetDefaultAsset({}, std::string(uri));
        assets.emplace_back(asset);
        sourceDataMap_[std::string(assetsKey) + std::to_string(assetIndex)] = asset;
        assetIndex ++;
    }
    NativeObjectValueType objValue = assets;
    sourceDataMap_[std::string(assetsKey)] = objValue;
}

void DataObjectImpl::OnChange(::taihe::callback_view<void(::taihe::string_view sessionId,
    ::taihe::array_view<::taihe::string> fields)> callback)
{
    VarCallbackType varCallback = callback;
    {
        std::lock_guard<std::mutex> guard(sessionInfoMutex_);
        if (session_ != nullptr && session_->GetSessionId().size() > 0) {
            session_->AddWatch(GetObjectStore(), EVENT_CHANGE, varCallback);
        } else {
            LOG_WARN("DataObjectImpl::OnChange, session not joined");
        }
    }
    if (objectId_.size() <= 0) {
        LOG_ERROR("DataObjectImpl::OnChange, objectId invalid");
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return;
    }
    bool addResult = AddChangeCallback(callback);
    if (!addResult) {
        LOG_ERROR("DataObjectImpl::OnChange, AddCallback failed");
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return;
    }
}

void DataObjectImpl::OffChange(::taihe::optional_view<
    ::taihe::callback<void(::taihe::string_view, ::taihe::array_view<::taihe::string>)>> callback)
{
    VarCallbackType varCallback;
    if (callback.has_value()) {
        varCallback = callback.value();
    }
    {
        std::lock_guard<std::mutex> guard(sessionInfoMutex_);
        if (session_ != nullptr && session_->GetSessionId().size() > 0) {
            session_->DeleteWatch(EVENT_CHANGE, varCallback);
        } else {
            LOG_WARN("DataObjectImpl::OffChange, session not joined");
        }
    }
    bool delResult = DeleteChangeCallback(varCallback);
    if (!delResult) {
        LOG_ERROR("DataObjectImpl::OffChange, DeleteChangeCallback failed");
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return;
    }
}

void DataObjectImpl::OnStatus(::taihe::callback_view<
    void(::taihe::string_view, ::taihe::string_view, ::taihe::string_view)> callback)
{
    VarCallbackType varCallback = callback;
    {
        std::lock_guard<std::mutex> guard(sessionInfoMutex_);
        if (session_ != nullptr && session_->GetSessionId().size() > 0) {
            session_->AddWatch(GetObjectStore(), EVENT_STATUS, varCallback);
        } else {
            LOG_WARN("DataObjectImpl::OnStatus, session not joined");
        }
    }
    if (objectId_.size() <= 0) {
        LOG_ERROR("DataObjectImpl::OnStatus, objectId invalid");
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return;
    }
    bool addResult = AddStatusCallback(callback);
    if (!addResult) {
        LOG_ERROR("DataObjectImpl::OnStatus, AddCallback failed");
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return;
    }
}

void DataObjectImpl::OffStatus(::taihe::optional_view<
    ::taihe::callback<void(::taihe::string_view, ::taihe::string_view, ::taihe::string_view)>> callback)
{
    VarCallbackType varCallback;
    if (callback.has_value()) {
        varCallback = callback.value();
    }
    {
        std::lock_guard<std::mutex> guard(sessionInfoMutex_);
        if (session_ != nullptr && session_->GetSessionId().size() > 0) {
            session_->DeleteWatch(EVENT_STATUS, varCallback);
        } else {
            LOG_WARN("DataObjectImpl::OffStatus, session not joined");
        }
    }
    bool delResult = DeleteStatusCallback(varCallback);
    if (!delResult) {
        LOG_ERROR("DataObjectImpl::OffStatus, DeleteStatusCallback failed");
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return;
    }
}

void DataObjectImpl::OnProgressChanged(::taihe::callback_view<
    void(::taihe::string_view sessionId, int32_t progress)> callback)
{
    VarCallbackType varCallback = callback;
    {
        std::lock_guard<std::mutex> guard(sessionInfoMutex_);
        if (session_ != nullptr && session_->GetSessionId().size() > 0) {
            session_->AddWatch(GetObjectStore(), EVENT_PROGRESS, varCallback);
        } else {
            LOG_WARN("DataObjectImpl::OnProgressChanged, session not joined");
        }
    }
    if (objectId_.size() <= 0) {
        LOG_ERROR("DataObjectImpl::OnProgressChanged, objectId invalid");
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return;
    }
    bool addResult = AddProgressCallback(callback);
    if (!addResult) {
        LOG_ERROR("DataObjectImpl::OnProgressChanged, AddCallback failed");
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return;
    }
}

void DataObjectImpl::OffProgressChanged(::taihe::optional_view<
    ::taihe::callback<void(::taihe::string_view sessionId, int32_t progress)>> callback)
{
    VarCallbackType varCallback;
    if (callback.has_value()) {
        varCallback = callback.value();
    }
    {
        std::lock_guard<std::mutex> guard(sessionInfoMutex_);
        if (session_ != nullptr && session_->GetSessionId().size() > 0) {
            session_->DeleteWatch(EVENT_PROGRESS, varCallback);
        } else {
            LOG_WARN("DataObjectImpl::OffProgressChanged, session not joined");
        }
    }
    bool delResult = DeleteProgressCallback(varCallback);
    if (!delResult) {
        LOG_ERROR("DataObjectImpl::OffProgressChanged, DeleteProgressCallback failed");
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), innerError->GetMessage());
        return;
    }
}

void DataObjectImpl::BindAssetStoreSync(::taihe::string_view assetKey,
    ::ohos::data::distributedDataObject::BindInfo const& bindInfo)
{
    LOG_INFO("DataObjectImpl::BindAssetStoreSync");
    std::lock_guard<std::mutex> guard(sessionInfoMutex_);
    if (session_ == nullptr || session_->GetSessionId().empty()) {
        auto innerError = std::make_shared<InnerError>();
        ani_errorutils::ThrowError(innerError->GetCode(), "not join a session, can not do bindAssetStore");
        return;
    }
    OHOS::ObjectStore::AssetBindInfo nativeBindInfo = ani_utils::BindInfoToNative(bindInfo);
    session_->BindAssetStore(std::string(assetKey), nativeBindInfo);
}

::ohos::data::distributedDataObject::ObjectValueType DataObjectImpl::GetValueImpl(::taihe::string_view key)
{
    using namespace ::ohos::data;
    LOG_INFO("DataObjectImpl::GetValueImpl");
    ani_env *aniEnv = taihe::get_env();
    if (aniEnv == nullptr) {
        LOG_INFO("DataObjectImpl::GetValueImpl, aniEnv null");
        return distributedDataObject::ObjectValueType::make_OPAQUE(0);
    }
    NativeObjectValueType result;
    auto iter = sourceDataMap_.find(std::string(key));
    if (iter == sourceDataMap_.end()) {
        LOG_ERROR("DataObjectImpl::GetValueImpl, no key matched");
        return distributedDataObject::ObjectValueType::make_UNDEFINED();
    }
    result = iter->second;

    std::lock_guard<std::mutex> guard(sessionInfoMutex_);
    if (session_ != nullptr && !session_->GetSessionId().empty()) {
        auto pvalAsset = std::get_if<OHOS::CommonType::AssetValue>(&result);
        auto pvalAssets = std::get_if<std::vector<OHOS::CommonType::AssetValue>>(&result);
        if (pvalAsset != nullptr) {
            std::string keytmp = std::string(key);
            result = session_->GetAssetValueFromStore(keytmp.c_str());
        } else if (pvalAssets != nullptr) {
            LOG_INFO("DataObjectImpl::GetValueImpl, is assets");
            ani_ref aniRef = GetAssetsRefFromStore(aniEnv, std::string(key));
            if (aniRef == nullptr) {
                return distributedDataObject::ObjectValueType::make_UNDEFINED();
            }
            return distributedDataObject::ObjectValueType::make_OPAQUE(reinterpret_cast<uintptr_t>(aniRef));
        } else {
            result = session_->GetValueFromStore(std::string(key).c_str());
        }
    }
    return ObjectValueTypeToTaihe(aniEnv, std::string(key), result);
}

void DataObjectImpl::SetValueImpl(::taihe::string_view key, uintptr_t valueRef)
{
    LOG_INFO("DataObjectImpl::SetValueImpl, key %{public}s", key.c_str());
    ani_env *aniEnv = taihe::get_env();
    if (aniEnv == nullptr) {
        LOG_INFO("DataObjectImpl::SetValueImpl, aniEnv null");
        return;
    }
    auto iter = sourceDataMap_.find(std::string(key));
    if (iter == sourceDataMap_.end()) {
        LOG_ERROR("DataObjectImpl::SetValueImpl, no key matched");
        ani_errorutils::ThrowError(ani_errorutils::AniError_ParameterCheck, "Parameter error. Incorrect key to set.");
        return;
    }
    ani_object aniObj = reinterpret_cast<ani_object>(valueRef);
    auto parseResult = ParseObjectValue(aniEnv, aniObj);
    if (std::holds_alternative<std::monostate>(parseResult)) {
        LOG_INFO("ParseObjectValue ret, monostate");
        ani_errorutils::ThrowError(ani_errorutils::AniError_ParameterCheck, "Parameter error. Incorrect value to set.");
        return;
    }

    NativeObjectValueType oldValue = iter->second;
    bool oldIsAsset = std::holds_alternative<OHOS::CommonType::AssetValue>(oldValue);
    bool oldIsAssets = std::holds_alternative<std::vector<OHOS::CommonType::AssetValue>>(oldValue);
    bool newIsAsset = std::holds_alternative<OHOS::CommonType::AssetValue>(parseResult);
    bool newIsAssets = std::holds_alternative<std::vector<OHOS::CommonType::AssetValue>>(parseResult);
    if (oldIsAsset != newIsAsset) {
        LOG_ERROR("DataObjectImpl::SetValueImpl, asset value not matched");
    }
    if (oldIsAssets != newIsAssets) {
        LOG_ERROR("DataObjectImpl::SetValueImpl, asset[] value not matched");
    }
    std::lock_guard<std::mutex> guard(sessionInfoMutex_);
    sourceDataMap_[std::string(key)] = parseResult;
    if (session_ != nullptr && !session_->GetSessionId().empty()) {
        session_->SyncDataToStore(std::string(key), parseResult, true);
    }
    version_ ++;
}

void DataObjectImpl::SetAssetItemValue(::taihe::string_view key, ::taihe::string_view attr,
    ::ohos::data::distributedDataObject::AssetValueType const& value)
{
    LOG_INFO("DataObjectImpl::SetAssetItemValue, key %{public}s, attr %{public}s", key.c_str(), attr.c_str());
    auto iter = sourceDataMap_.find(std::string(key));
    if (iter == sourceDataMap_.end()) {
        LOG_ERROR("DataObjectImpl::SetAssetItemValue, no key matched");
        ani_errorutils::ThrowError(ani_errorutils::AniError_ParameterCheck, "Parameter error. Incorrect key to set.");
        return;
    }
    auto existedValue = iter->second;
    auto pvalAsset = std::get_if<OHOS::CommonType::AssetValue>(&existedValue);
    if (pvalAsset == nullptr) {
        LOG_ERROR("DataObjectImpl::SetAssetItemValue, target is not Asset");
        ani_errorutils::ThrowError(ani_errorutils::AniError_ParameterCheck, "Parameter error. target is not Asset.");
        return;
    }
    auto &asset = *pvalAsset;
    UpdateAssetAttr(asset, std::string(key), std::string(attr), value);
    version_ ++;
}

void DataObjectImpl::UpdateAssetAttr(OHOS::CommonType::AssetValue &asset, std::string const& externalKey,
    std::string const& attr, ::ohos::data::distributedDataObject::AssetValueType const& value)
{
    std::string paraStr;
    uint32_t paraStatus = OHOS::CommonType::AssetValue::Status::STATUS_UNKNOWN;
    if (value.get_tag() == ::ohos::data::distributedDataObject::AssetValueType::tag_t::STR) {
        paraStr = value.get_STR_ref();
    } else {
        auto taiheStatus = value.get_STATUS_ref();
        paraStatus = ani_utils::TaiheStatusToNative(taiheStatus);
    }
    LOG_INFO("DataObjectImpl::UpdateAssetAttr, attr %{public}s, paraStr %{public}s", attr.c_str(), paraStr.c_str());
    if (attr == "id") {
        asset.id = paraStr;
    } else if (attr == "name") {
        asset.name = paraStr;
    } else if (attr == "uri") {
        asset.uri = paraStr;
    } else if (attr == "createTime") {
        asset.createTime = paraStr;
    } else if (attr == "modifyTime") {
        asset.modifyTime = paraStr;
    } else if (attr == "size") {
        asset.size = paraStr;
    } else if (attr == "hash") {
        asset.hash = paraStr;
    } else if (attr == "path") {
        asset.path = paraStr;
    } else if (attr == "status") {
        asset.status = paraStatus;
    } else {
        return;
    }
    if (session_ != nullptr && !session_->GetSessionId().empty()) {
        if (attr == "status") {
            session_->SyncAssetAttrToStore(externalKey, attr, paraStatus);
        } else {
            session_->SyncAssetAttrToStore(externalKey, attr, paraStr);
        }
    }
}

ani_ref DataObjectImpl::GetAssetsRefFromStore(ani_env* aniEnv, std::string assetsKey)
{
    LOG_INFO("DataObjectImpl::GetAssetsRefFromStore");
    if (session_ == nullptr) {
        return nullptr;
    }
    NativeObjectValueType result = session_->GetValueFromStore(assetsKey.c_str());
    auto pvalString = std::get_if<std::string>(&result);
    if (pvalString == nullptr) {
        return nullptr;
    }
    std::string strtemp = *pvalString;
    LOG_INFO("DataObjectImpl::GetAssetsRefFromStore, strtemp %{public}s", strtemp.c_str());
    if (strtemp.find(COMPLEX_TYPE) == 0) {
        strtemp = strtemp.substr(COMPLEX_TYPE.length());
        ani_ref aniRef = DataObjectImpl::JsonParseToJsonElement(aniEnv, strtemp);
        LOG_INFO("DataObjectImpl::GetAssetsRefFromStore, JsonParse %{public}p", aniRef);
        return aniRef;
    }
    return nullptr;
}

::ohos::data::distributedDataObject::ObjectValueType DataObjectImpl::ObjectValueTypeToTaihe(
    ani_env* aniEnv, std::string const& externalKey, NativeObjectValueType const &valueObj)
{
    using namespace ::ohos::data;
    if (aniEnv == nullptr) {
        return distributedDataObject::ObjectValueType::make_OPAQUE(0);
    }
    if (std::get_if<std::monostate>(&valueObj)) {
        return distributedDataObject::ObjectValueType::make_EMPTY();
    }
    auto pvalBool = std::get_if<bool>(&valueObj);
    if (pvalBool != nullptr) {
        return distributedDataObject::ObjectValueType::make_BOOL(*pvalBool);
    }
    auto pvalDouble = std::get_if<double>(&valueObj);
    if (pvalDouble != nullptr) {
        return distributedDataObject::ObjectValueType::make_F64(*pvalDouble);
    }
    auto pvalUint8Array = std::get_if<std::vector<uint8_t>>(&valueObj);
    if (pvalUint8Array != nullptr) {
        auto stdArray =  *pvalUint8Array;
        auto taiheArray = ::taihe::array<uint8_t>(::taihe::copy_data_t{}, stdArray.data(), stdArray.size());
        return distributedDataObject::ObjectValueType::make_Uint8Array(taiheArray);
    }
    auto pvalAsset = std::get_if<OHOS::CommonType::AssetValue>(&valueObj);
    if (pvalAsset != nullptr) {
        LOG_INFO("DataObjectImpl::ObjectValueTypeToTaihe, create DdoAssetProxy");
        ani_object aniAssetProxy = ani_utils::AniCreateProxyAsset(aniEnv, externalKey, *pvalAsset);
        return distributedDataObject::ObjectValueType::make_OPAQUE(reinterpret_cast<uintptr_t>(aniAssetProxy));
    }
    auto pvalAssets = std::get_if<std::vector<OHOS::CommonType::AssetValue>>(&valueObj);
    if (pvalAssets != nullptr) {
        auto taiheAssets = ani_utils::AssetsToTaihe(*pvalAssets);
        return distributedDataObject::ObjectValueType::make_ASSETS(taiheAssets);
    }
    auto pvalString = std::get_if<std::string>(&valueObj);
    if (pvalString != nullptr) {
        std::string strtemp = *pvalString;
        if (strtemp.find(STRING_TYPE) == 0) {
            strtemp = strtemp.substr(STRING_TYPE.length());
            return distributedDataObject::ObjectValueType::make_STRING(strtemp);
        } else if (strtemp.find(NULL_TYPE) == 0) {
            return distributedDataObject::ObjectValueType::make_EMPTY();
        } else if (strtemp.find(COMPLEX_TYPE) == 0) {
            strtemp = strtemp.substr(COMPLEX_TYPE.length());
            ani_ref aniRef = JsonParseToJsonElement(aniEnv, strtemp);
            return distributedDataObject::ObjectValueType::make_OPAQUE(reinterpret_cast<uintptr_t>(aniRef));
        }
    }
    return distributedDataObject::ObjectValueType::make_UNDEFINED();
}

} //namespace OHOS::ObjectStore