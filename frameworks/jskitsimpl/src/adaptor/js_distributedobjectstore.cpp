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

#include "js_distributedobjectstore.h"
#include <cstring>
#include "ability_context.h"
#include "accesstoken_kit.h"
#include "distributed_objectstore.h"
#include "js_common.h"
#include "js_distributedobject.h"
#include "js_object_wrapper.h"

#include "logger.h"
#include "objectstore_errors.h"

namespace OHOS::ObjectStore {
constexpr size_t TYPE_SIZE = 10;
const int MIN_NUMERIC = 999999;
const std::string DISTRIBUTED_DATASYNC = "ohos.permission.DISTRIBUTED_DATASYNC";
static ConcurrentMap<std::string, std::list<napi_ref>> g_statusCallBacks;
static ConcurrentMap<std::string, std::list<napi_ref>> g_changeCallBacks;
std::atomic<uint32_t> JSDistributedObjectStore::sequenceNum_ { MIN_NUMERIC };
bool JSDistributedObjectStore::AddCallback(napi_env env, ConcurrentMap<std::string, std::list<napi_ref>> &callbacks,
    const std::string &objectId, napi_value callback)
{
    LOG_INFO("add callback %{public}s", objectId.c_str());
    napi_ref ref = nullptr;
    napi_status status = napi_create_reference(env, callback, 1, &ref);
    if (status != napi_ok) {
        return false;
    }
    return callbacks.Compute(objectId, [&](const std::string &key, std::list <napi_ref> &lists) {
        lists.push_back(ref);
        return true;
    });
}

bool JSDistributedObjectStore::DelCallback(napi_env env, ConcurrentMap<std::string, std::list<napi_ref>> &callbacks,
    const std::string &sessionId, napi_value callback)
{
    LOG_INFO("del callback %{public}s", sessionId.c_str());
    napi_status status;
    bool result = true;
    if (callback == nullptr) {
        result = callbacks.ComputeIfPresent(sessionId, [&](const std::string &key, std::list <napi_ref> &lists) {
            for (auto ref : lists) {
                status = napi_delete_reference(env, ref);
                if (status != napi_ok) {
                    LOG_ERROR("error! %{public}d %{public}d", status, napi_ok);
                    return false;
                }
            }
            return true;
        });
    }
    napi_value callbackTmp;
    result = callbacks.ComputeIfPresent(sessionId, [&](const std::string &key, std::list <napi_ref> &lists) {
        for (auto iter = lists.begin(); iter != lists.end();) {
            status = napi_get_reference_value(env, *iter, &callbackTmp);
            if (status != napi_ok) {
                LOG_ERROR("error! %{public}d %{public}d", status, napi_ok);
                return false;
            }
            bool isEquals = false;
            napi_strict_equals(env, callbackTmp, callback, &isEquals);
            if (isEquals) {
                napi_delete_reference(env, *iter);
                iter = lists.erase(iter);
            } else {
                iter++;
            }
        }
        if (lists.empty()) {
            callbacks.Erase(sessionId);
        }
        return true;
    });
    return result;
}

napi_value JSDistributedObjectStore::NewDistributedObject(
    napi_env env, DistributedObjectStore *objectStore, DistributedObject *object, const std::string &objectId)
{
    napi_value result;
    napi_status status = napi_new_instance(env, JSDistributedObject::GetCons(env), 0, nullptr, &result);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    JSObjectWrapper *objectWrapper = new JSObjectWrapper(objectStore, object);
    objectWrapper->SetObjectId(objectId);
    status = napi_wrap(
        env, result, objectWrapper,
        [](napi_env env, void *data, void *hint) {
            if (data == nullptr) {
                LOG_WARN("objectWrapper is nullptr.");
                return;
            }
            auto objectWrapper = (JSObjectWrapper *)data;
            if (objectWrapper->GetObject() == nullptr) {
                delete objectWrapper;
                return;
            }

            g_changeCallBacks.Erase(objectWrapper->GetObjectId());
            g_statusCallBacks.Erase(objectWrapper->GetObjectId());
            LOG_INFO("start delete object");
            DistributedObjectStore::GetInstance(JSDistributedObjectStore::GetBundleName(env))
                ->DeleteObject(objectWrapper->GetObject()->GetSessionId());
            delete objectWrapper;
        },
        nullptr, nullptr);
    RestoreWatchers(env, objectWrapper, objectId);

    objectStore->NotifyCachedStatus(object->GetSessionId());
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    return result;
}

// function createObjectSync(version: number, sessionId: string, objectId:string): DistributedObject;
// function createObjectSync(version: number, sessionId: string, objectId:string, context: Context): DistributedObject;
napi_value JSDistributedObjectStore::JSCreateObjectSync(napi_env env, napi_callback_info info)
{
    size_t requireArgc = 3;
    size_t argc = 4;
    napi_value argv[4] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    double version = 0;
    std::string sessionId;
    std::string objectId;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    napi_valuetype valueType = napi_undefined;
    status = napi_typeof(env, argv[0], &valueType);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    status = JSUtil::GetValue(env, argv[0], version);

    SETERR_RETURN(argc >= requireArgc, version,
                  JSUtil::SetError(env, INVALID_PARAMS, "at least 1 parameters!"));
    ASSERT_STATUS_ELSE_RETURN(!IsSandBox(), env, version);
    LOG_INFO("start JSCreateObjectSync");
    SETERR_RETURN(JSDistributedObjectStore::CheckSyncPermission(), version,
                  JSUtil::SetError(env, NO_PERMISSION, "no permission ohos.permission.DISTRIBUTED_DATASYNC"));

    status = napi_typeof(env, argv[1], &valueType);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);

    SETERR_RETURN(valueType == napi_string, version, JSUtil::SetError(env, INVALID_PARAMS, "sessionId", "string"));
    status = JSUtil::GetValue(env, argv[1], sessionId);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    status = napi_typeof(env, argv[2], &valueType);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    CHECK_EQUAL_WITH_RETURN_NULL(valueType, napi_string);
    status = JSUtil::GetValue(env, argv[2], objectId);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    if (argc > requireArgc) {
        napi_valuetype objectType = napi_undefined;
        status = napi_typeof(env, argv[3], &objectType);
        CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
        SETERR_RETURN(valueType == objectType, version, JSUtil::SetError(env, INVALID_PARAMS, "context", "Context"));
    }
    DistributedObjectStore *objectInfo =
            DistributedObjectStore::GetInstance(JSDistributedObjectStore::GetBundleName(env));
    ASSERT_STATUS_ELSE_RETURN(objectInfo != nullptr, env, version);

    uint32_t result = 0;
    DistributedObject *object = objectInfo->CreateObject(sessionId, result);
    ASSERT_DB_ERROR_RETURN(result != ERR_EXIST, env, version);
    ASSERT_STATUS_ELSE_RETURN(result == SUCCESS, env, version);
    ASSERT_STATUS_ELSE_RETURN(object != nullptr, env, version);
    return NewDistributedObject(env, objectInfo, object, objectId);
}

// function destroyObjectSync(version: number, object: DistributedObject): number;
napi_value JSDistributedObjectStore::JSDestroyObjectSync(napi_env env, napi_callback_info info)
{
    double version = 0;
    LOG_INFO("start");
    size_t requireArgc = 2;
    size_t argc = 2;
    napi_value argv[2] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    std::string sessionId;
    std::string bundleName;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    status = JSUtil::GetValue(env, argv[0], version);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    ASSERT_MATCH_ELSE_RETURN_NULL(argc >= requireArgc);
    JSObjectWrapper *objectWrapper = nullptr;
    status = napi_unwrap(env, argv[1], (void **) &objectWrapper);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    ASSERT_STATUS_ELSE_RETURN(objectWrapper != nullptr, env, version);
    DistributedObjectStore *objectInfo =
            DistributedObjectStore::GetInstance(JSDistributedObjectStore::GetBundleName(env));
    ASSERT_STATUS_ELSE_RETURN(objectInfo != nullptr && objectWrapper->GetObject() != nullptr, env, version);
    objectWrapper->DeleteWatch(env, CHANGE);
    objectWrapper->DeleteWatch(env, STATUS);
    objectInfo->DeleteObject(objectWrapper->GetObject()->GetSessionId());
    objectWrapper->DestroyObject();
    return nullptr;
}

// function on(version: number, type: 'change', object: DistributedObject, callback: Callback<ChangedDataObserver>): void;
// function on(version: number, type: 'status', object: DistributedObject, callback: Callback<ObjectStatusObserver>): void;
napi_value JSDistributedObjectStore::JSOn(napi_env env, napi_callback_info info)
{
    double version = 0;
    LOG_INFO("start");
    size_t requireArgc = 4;
    size_t argc = 4;
    napi_value argv[4] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    status = JSUtil::GetValue(env, argv[0], version);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    SETERR_RETURN(argc >= requireArgc, version, JSUtil::SetError(env, INVALID_PARAMS, "at least 2 parameters!"));
    char type[TYPE_SIZE] = {0};
    size_t eventTypeLen = 0;
    napi_valuetype valueType = napi_undefined;
    status = napi_typeof(env, argv[1], &valueType);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    SETERR_RETURN(valueType == napi_string, version, JSUtil::SetError(env, INVALID_PARAMS, "type", "string")); 
    status = napi_get_value_string_utf8(env, argv[1], type, TYPE_SIZE, &eventTypeLen);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);

    napi_valuetype objectType = napi_undefined;
    status = napi_typeof(env, argv[2], &objectType);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    ASSERT_MATCH_ELSE_RETURN_NULL(objectType == napi_object);

    JSObjectWrapper *wrapper = nullptr;
    status = napi_unwrap(env, argv[2], (void **) &wrapper);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    ASSERT_STATUS_ELSE_RETURN(wrapper != nullptr, env, version);
    napi_valuetype callbackType = napi_undefined;
    status = napi_typeof(env, argv[3], &callbackType);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    SETERR_RETURN(valueType == napi_function, version, JSUtil::SetError(env, INVALID_PARAMS, "callback", "function")); 
    wrapper->AddWatch(env, type, argv[3]);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

// function off(version: number, type: 'change', object: DistributedObject, callback?: Callback<ChangedDataObserver>): void;
// function off(version: number, type: 'status', object: DistributedObject, callback?: Callback<ObjectStatusObserver>): void;
napi_value JSDistributedObjectStore::JSOff(napi_env env, napi_callback_info info)
{
    double version = 0;
    LOG_INFO("start");
    size_t requireArgc = 3;
    size_t argc = 4;
    napi_value argv[4] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    char type[TYPE_SIZE] = {0};
    size_t typeLen = 0;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    status = JSUtil::GetValue(env, argv[0], version);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    SETERR_RETURN(argc >= requireArgc, version, JSUtil::SetError(env, INVALID_PARAMS, "at least 1 parameters!"));
    napi_valuetype valueType = napi_undefined;
    status = napi_typeof(env, argv[1], &valueType);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    SETERR_RETURN(valueType == napi_string, version, JSUtil::SetError(env, INVALID_PARAMS, "type", "string"));
    status = napi_get_value_string_utf8(env, argv[1], type, TYPE_SIZE, &typeLen);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);

    status = napi_typeof(env, argv[2], &valueType);
    ASSERT_MATCH_ELSE_RETURN_NULL(valueType == napi_object);
    JSObjectWrapper *wrapper = nullptr;
    status = napi_unwrap(env, argv[2], (void **) &wrapper);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    ASSERT_STATUS_ELSE_RETURN(wrapper != nullptr, env, version);

    if (argc == requireArgc) {
        LOG_INFO("delete all");
        wrapper->DeleteWatch(env, type);
    } else {
        LOG_INFO("delete");
        status = napi_typeof(env, argv[3], &valueType);
        SETERR_RETURN(valueType == napi_function, version, JSUtil::SetError(env, INVALID_PARAMS, "callback", "function"));       
        wrapper->DeleteWatch(env, type, argv[3]);
    }
    napi_value result = nullptr;
    status = napi_get_undefined(env, &result);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    return result;
}

std::string JSDistributedObjectStore::GetBundleName(napi_env env)
{
    static std::string bundleName;
    if (bundleName.empty()) {
        bundleName = AbilityRuntime::Context::GetApplicationContext()->GetBundleName();
    }
    return bundleName;
}

void JSDistributedObjectStore::RestoreWatchers(napi_env env, JSObjectWrapper *wrapper, const std::string &objectId)
{
    napi_status status;
    napi_value callbackValue;
    bool watchResult = true;
    LOG_DEBUG("start restore %{public}s", objectId.c_str());
    watchResult = g_changeCallBacks.ComputeIfPresent(objectId,
                                                     [&](const std::string &key, std::list <napi_ref> &lists) {
                                                         for (auto callback : lists) {
                                                             status = napi_get_reference_value(env, callback,
                                                                                               &callbackValue);
                                                             if (status != napi_ok) {
                                                                 LOG_ERROR("error! %{public}d", status);
                                                                 continue;
                                                             }
                                                             wrapper->AddWatch(env, CHANGE, callbackValue);
                                                         }
                                                         return true;
                                                     });
    if (!watchResult) {
        LOG_INFO("no callback %{public}s", objectId.c_str());
    }
    watchResult = g_statusCallBacks.ComputeIfPresent(objectId,
                                                     [&](const std::string &key, std::list <napi_ref> &lists) {
                                                         for (auto callback : lists) {
                                                             status = napi_get_reference_value(env, callback,
                                                                                               &callbackValue);
                                                             if (status != napi_ok) {
                                                                 LOG_ERROR("error! %{public}d", status);
                                                                 continue;
                                                             }
                                                             wrapper->AddWatch(env, STATUS, callbackValue);
                                                         }
                                                         return true;
                                                     });
    if (!watchResult) {
        LOG_INFO("no status %{public}s", objectId.c_str());
    }
}

// function recordCallback(version: number, type: 'change', objectId: string, callback: Callback<ChangedDataObserver>): void;
// function recordCallback(version: number, type: 'status', objectId: string, callback: Callback<ObjectStatusObserver>): void;
napi_value JSDistributedObjectStore::JSRecordCallback(napi_env env, napi_callback_info info)
{
    double version = 0;
    LOG_INFO("start");
    size_t requireArgc = 4;
    size_t argc = 4;
    napi_value argv[4] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    status = JSUtil::GetValue(env, argv[0], version);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    SETERR_RETURN(argc >= requireArgc, version, JSUtil::SetError(env, INVALID_PARAMS, "at least 2 parameters!"));
    char type[TYPE_SIZE] = {0};
    size_t eventTypeLen = 0;
    napi_valuetype valueType = napi_undefined;
    status = napi_typeof(env, argv[1], &valueType);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    SETERR_RETURN(valueType == napi_string, version, JSUtil::SetError(env, INVALID_PARAMS, "type", "string"));
    status = napi_get_value_string_utf8(env, argv[1], type, TYPE_SIZE, &eventTypeLen);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);

    std::string objectId;
    status = napi_typeof(env, argv[2], &valueType);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    CHECK_EQUAL_WITH_RETURN_NULL(valueType, napi_string);
    status = JSUtil::GetValue(env, argv[2], objectId);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);

    napi_valuetype callbackType = napi_undefined;
    status = napi_typeof(env, argv[3], &callbackType);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    SETERR_RETURN(callbackType == napi_function, version, JSUtil::SetError(env, INVALID_PARAMS, "callback", "function"));
    bool addResult = true;
    if (!strcmp(CHANGE, type)) {
        addResult = AddCallback(env, g_changeCallBacks, objectId, argv[3]);
    } else if (!strcmp(STATUS, type)) {
        addResult = AddCallback(env, g_statusCallBacks, objectId, argv[3]);
    }
    ASSERT_STATUS_ELSE_RETURN(addResult, env, version);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

// function deleteCallback(version: number, type: 'change', objectId: string, callback?: Callback<ChangedDataObserver>): void;
// function deleteCallback(version: number, type: 'status', objectId: string, callback?: Callback<ObjectStatusObserver>): void;
napi_value JSDistributedObjectStore::JSDeleteCallback(napi_env env, napi_callback_info info)
{
    double version = 0;
    LOG_INFO("start");
    size_t requireArgc = 3;
    size_t argc = 4;
    napi_value argv[4] = {0};
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    status = JSUtil::GetValue(env, argv[0], version);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    SETERR_RETURN(argc >= requireArgc, version, JSUtil::SetError(env, INVALID_PARAMS, "at least 1 parameters!"));
    char type[TYPE_SIZE] = {0};
    size_t eventTypeLen = 0;
    napi_valuetype valueType = napi_undefined;
    status = napi_typeof(env, argv[1], &valueType);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    SETERR_RETURN(valueType == napi_string, version, JSUtil::SetError(env, INVALID_PARAMS, "type", "string"));
    status = napi_get_value_string_utf8(env, argv[1], type, TYPE_SIZE, &eventTypeLen);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);

    std::string objectId;
    status = napi_typeof(env, argv[2], &valueType);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    CHECK_EQUAL_WITH_RETURN_NULL(valueType, napi_string);
    status = JSUtil::GetValue(env, argv[2], objectId);
    CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
    bool delResult = true;
    if (argc == 3) {
        if (!strcmp(CHANGE, type)) {
            delResult = DelCallback(env, g_changeCallBacks, objectId);
        } else if (!strcmp(STATUS, type)) {
            delResult = DelCallback(env, g_statusCallBacks, objectId);
        }
    } else {
        napi_valuetype callbackType = napi_undefined;
        status = napi_typeof(env, argv[3], &callbackType);
        CHECK_STATUS_WITH_RETURN(status, napi_ok, env, version);
        SETERR_RETURN(valueType == napi_function, version, JSUtil::SetError(env, INVALID_PARAMS, "callback", "founction"));
        if (!strcmp(CHANGE, type)) {
            delResult = DelCallback(env, g_changeCallBacks, objectId, argv[3]);
        } else if (!strcmp(STATUS, type)) {
            delResult = DelCallback(env, g_statusCallBacks, objectId, argv[3]);
        }
    }
    // ASSERT_STATUS_ELSE_RETURN(delResult, env, version);
    napi_value result = nullptr;
    napi_get_undefined(env, &result);
    return result;
}

napi_value JSDistributedObjectStore::JSEquenceNum(napi_env env, napi_callback_info info)
{
    std::string str = std::to_string(sequenceNum_++);
    napi_value result = nullptr;
    napi_status status = napi_create_string_utf8(env, str.c_str(), str.size(), &result);
    CHECK_EQUAL_WITH_RETURN_NULL(status, napi_ok);
    return result;
}

bool JSDistributedObjectStore::CheckSyncPermission()
{
    int32_t ret = Security::AccessToken::AccessTokenKit::VerifyAccessToken(
        AbilityRuntime::Context::GetApplicationContext()->GetApplicationInfo()->accessTokenId, DISTRIBUTED_DATASYNC);
    if (ret == Security::AccessToken::PermissionState::PERMISSION_DENIED) {
        LOG_ERROR("VerifyPermission %{public}d: PERMISSION_DENIED",
            AbilityRuntime::Context::GetApplicationContext()->GetApplicationInfo()->accessTokenId);
        return false;
    }
    return true;
}

// don't create distributed data object while this application is sandbox
bool JSDistributedObjectStore::IsSandBox()
{
    int32_t dlpFlag = Security::AccessToken::AccessTokenKit::GetHapDlpFlag(
        AbilityRuntime::Context::GetApplicationContext()->GetApplicationInfo()->accessTokenId);
    if (dlpFlag != 0) {
        return true;
    }
    return false;
}
} // namespace OHOS::ObjectStore
