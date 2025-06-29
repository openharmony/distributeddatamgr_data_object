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

#include "js_distributedobject.h"

#include "js_common.h"
#include "js_util.h"
#include "logger.h"
#include "napi_queue.h"
#include "objectstore_errors.h"

namespace OHOS::ObjectStore {
constexpr size_t KEY_SIZE = 64;

napi_value JSDistributedObject::JSConstructor(napi_env env, napi_callback_info info)
{
    LOG_INFO("start");
    napi_value thisVar = nullptr;
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, 0, &thisVar, &data);
    NOT_MATCH_RETURN_NULL(status == napi_ok);
    return thisVar;
}

// get(key: string): ValueType;
napi_value JSDistributedObject::JSGet(napi_env env, napi_callback_info info)
{
    size_t requireArgc = 1;
    size_t argc = 1;
    napi_value argv[1] = { 0 };
    napi_value thisVar = nullptr;
    void *data = nullptr;
    char key[KEY_SIZE] = { 0 };
    size_t keyLen = 0;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);
    NOT_MATCH_RETURN_NULL(status == napi_ok && argc >= requireArgc);

    status = napi_get_value_string_utf8(env, argv[0], key, KEY_SIZE, &keyLen);
    NOT_MATCH_RETURN_NULL(status == napi_ok);

    JSObjectWrapper *wrapper = nullptr;
    status = napi_unwrap(env, thisVar, (void **)&wrapper);
    NOT_MATCH_RETURN_NULL(status == napi_ok && wrapper != nullptr && wrapper->GetObject() != nullptr);
    napi_value result = nullptr;
    if (wrapper->IsUndefined(key)) {
        napi_get_undefined(env, &result);
        return result;
    }
    DoGet(env, wrapper, key, result);
    return result;
}

// put(key: string, value: ValueType): void;
napi_value JSDistributedObject::JSPut(napi_env env, napi_callback_info info)
{
    size_t requireArgc = 2;
    size_t argc = 2;
    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;
    char key[KEY_SIZE] = { 0 };
    size_t keyLen = 0;
    napi_valuetype valueType;
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &thisVar, nullptr);
    NOT_MATCH_RETURN_NULL(status == napi_ok && argc >= requireArgc);

    status = napi_get_value_string_utf8(env, argv[0], key, KEY_SIZE, &keyLen);
    NOT_MATCH_RETURN_NULL(status == napi_ok);

    JSObjectWrapper *wrapper = nullptr;
    status = napi_unwrap(env, thisVar, (void **)&wrapper);
    NOT_MATCH_RETURN_NULL(status == napi_ok && wrapper != nullptr && wrapper->GetObject() != nullptr);

    status = napi_typeof(env, argv[1], &valueType);
    NOT_MATCH_RETURN_NULL(status == napi_ok);
    if (valueType == napi_undefined) {
        wrapper->AddUndefined(key);
        return nullptr;
    }
    wrapper->DeleteUndefined(key);
    DoPut(env, wrapper, key, valueType, argv[1]);
    LOG_INFO("put %{public}s success", key);
    return nullptr;
}

napi_value JSDistributedObject::GetCons(napi_env env)
{
    static thread_local napi_ref g_instance = nullptr;
    napi_value distributedObjectClass = nullptr;
    if (g_instance != nullptr) {
        napi_status status = napi_get_reference_value(env, g_instance, &distributedObjectClass);
        NOT_MATCH_RETURN_NULL(status == napi_ok);
        return distributedObjectClass;
    }
    const char *distributedObjectName = "DistributedObject";
    napi_property_descriptor distributedObjectDesc[] = {
        DECLARE_NAPI_FUNCTION("put", JSDistributedObject::JSPut),
        DECLARE_NAPI_FUNCTION("get", JSDistributedObject::JSGet),
        DECLARE_NAPI_FUNCTION("save", JSDistributedObject::JSSave),
        DECLARE_NAPI_FUNCTION("revokeSave", JSDistributedObject::JSRevokeSave),
        DECLARE_NAPI_FUNCTION("bindAssetStore", JSDistributedObject::JSBindAssetStore),
    };

    napi_status status = napi_define_class(env, distributedObjectName, strlen(distributedObjectName),
        JSDistributedObject::JSConstructor, nullptr, sizeof(distributedObjectDesc) / sizeof(distributedObjectDesc[0]),
        distributedObjectDesc, &distributedObjectClass);
    NOT_MATCH_RETURN_NULL(status == napi_ok);
    if (g_instance == nullptr) {
        status = napi_create_reference(env, distributedObjectClass, 1, &g_instance);
        NOT_MATCH_RETURN_NULL(status == napi_ok);
    }
    return distributedObjectClass;
}

void JSDistributedObject::DoPut(
    napi_env env, JSObjectWrapper *wrapper, char *key, napi_valuetype type, napi_value value)
{
    std::string keyString = key;
    switch (type) {
        case napi_boolean: {
            bool putValue = false;
            napi_status status = JSUtil::GetValue(env, value, putValue);
            NOT_MATCH_RETURN_VOID(status == napi_ok);
            wrapper->GetObject()->PutBoolean(keyString, putValue);
            break;
        }
        case napi_number: {
            double putValue = 0;
            napi_status status = JSUtil::GetValue(env, value, putValue);
            NOT_MATCH_RETURN_VOID(status == napi_ok);
            wrapper->GetObject()->PutDouble(keyString, putValue);
            break;
        }
        case napi_string: {
            std::string putValue;
            napi_status status = JSUtil::GetValue(env, value, putValue);
            NOT_MATCH_RETURN_VOID(status == napi_ok);
            wrapper->GetObject()->PutString(keyString, putValue);
            break;
        }
        case napi_object: {
            std::vector<uint8_t> putValue;
            napi_status status = JSUtil::GetValue(env, value, putValue);
            NOT_MATCH_RETURN_VOID(status == napi_ok);
            wrapper->GetObject()->PutComplex(keyString, putValue);
            break;
        }
        default: {
            LOG_ERROR("error type! %{public}d", type);
            break;
        }
    }
}

void JSDistributedObject::DoGet(napi_env env, JSObjectWrapper *wrapper, char *key, napi_value &value)
{
    std::string keyString = key;
    Type type = TYPE_STRING;
    wrapper->GetObject()->GetType(keyString, type);
    LOG_DEBUG("get type %{public}s %{public}d", key, type);
    switch (type) {
        case TYPE_STRING: {
            std::string result;
            uint32_t ret = wrapper->GetObject()->GetString(keyString, result);
            NOT_MATCH_RETURN_VOID(ret == SUCCESS);
            napi_status status = JSUtil::SetValue(env, result, value);
            NOT_MATCH_RETURN_VOID(status == napi_ok);
            break;
        }
        case TYPE_DOUBLE: {
            double result;
            uint32_t ret = wrapper->GetObject()->GetDouble(keyString, result);
            LOG_DEBUG("%{public}f", result);
            NOT_MATCH_RETURN_VOID(ret == SUCCESS);
            napi_status status = JSUtil::SetValue(env, result, value);
            NOT_MATCH_RETURN_VOID(status == napi_ok);
            break;
        }
        case TYPE_BOOLEAN: {
            bool result;
            uint32_t ret = wrapper->GetObject()->GetBoolean(keyString, result);
            LOG_DEBUG("%{public}d", result);
            NOT_MATCH_RETURN_VOID(ret == SUCCESS);
            napi_status status = JSUtil::SetValue(env, result, value);
            NOT_MATCH_RETURN_VOID(status == napi_ok);
            break;
        }
        case TYPE_COMPLEX: {
            std::vector<uint8_t> result;
            uint32_t ret = wrapper->GetObject()->GetComplex(keyString, result);
            NOT_MATCH_RETURN_VOID(ret == SUCCESS);
            napi_status status = JSUtil::SetValue(env, result, value);
            NOT_MATCH_RETURN_VOID(status == napi_ok);
            break;
        }
        default: {
            LOG_ERROR("error type! %{public}d", type);
            break;
        }
    }
}

// save(deviceId: string, version: number, callback?:AsyncCallback<SaveSuccessResponse>): void;
// save(deviceId: string, version: number): Promise<SaveSuccessResponse>;
napi_value JSDistributedObject::JSSave(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("JSSave()");
    struct SaveContext : public ContextBase {
        double version;
        std::string deviceId;
        JSObjectWrapper *wrapper;
    };
    auto ctxt = std::make_shared<SaveContext>();
    std::function<void(size_t argc, napi_value * argv)> getCbOpe = [env, ctxt](size_t argc, napi_value *argv) {
        INVALID_ARGS_RETURN_ERROR(ctxt, argc >= 2, "arguments error", std::make_shared<ParametersNum>("1 or 2"));
        ctxt->status = JSUtil::GetValue(env, argv[0], ctxt->deviceId);
        INVALID_ARGS_RETURN_ERROR(ctxt, ctxt->status == napi_ok, "arguments error",
            std::make_shared<ParametersType>("deviceId", "string"));

        ctxt->status = JSUtil::GetValue(env, argv[1], ctxt->version);
        INVALID_STATUS_RETURN_ERROR(ctxt, "invalid arg[1], i.e. invalid version!");
        JSObjectWrapper *wrapper = nullptr;
        napi_status status = napi_unwrap(env, ctxt->self, (void **)&wrapper);
        NOT_MATCH_RETURN_VOID(status == napi_ok && wrapper != nullptr && wrapper->GetObject() != nullptr);
        ctxt->wrapper = wrapper;
    };
    ctxt->GetCbInfo(env, info, getCbOpe);
    NAPI_ASSERT_ERRCODE(env, ctxt->status != napi_invalid_arg, ctxt->error);
    auto execute = [ctxt]() {
        LOG_INFO("start");
        CHECH_STATUS_RETURN_VOID(env, ctxt->wrapper != nullptr, ctxt, "wrapper is null");
        CHECH_STATUS_RETURN_VOID(env, ctxt->wrapper->GetObject() != nullptr, ctxt, "object is null");
        uint32_t status = ctxt->wrapper->GetObject()->Save(ctxt->deviceId);
        INVALID_API_THROW_ERROR(status != ERR_PROCESSING);
        INVALID_STATUS_THROW_ERROR(status == SUCCESS, "operation failed");
        ctxt->status = napi_ok;
        LOG_INFO("end");
    };
    auto output = [env, ctxt](napi_value &result) {
        if (ctxt->status == napi_ok) {
            CHECH_STATUS_RETURN_VOID(env, ctxt->wrapper != nullptr, ctxt, "wrapper is null");
            CHECH_STATUS_RETURN_VOID(env, ctxt->wrapper->GetObject() != nullptr, ctxt, "object is null");
            std::string &sessionId = ctxt->wrapper->GetObject()->GetSessionId();
            ctxt->status = napi_new_instance(
                env, GetSaveResultCons(env, sessionId, ctxt->version, ctxt->deviceId), 0, nullptr, &result);
            INVALID_STATUS_RETURN_ERROR(ctxt, "output failed!");
        }
    };
    return NapiQueue::AsyncWork(env, ctxt, std::string(__FUNCTION__), execute, output);
}

// revokeSave(callback?:AsyncCallback<RevokeSaveSuccessResponse>): void;
// revokeSave(): Promise<RevokeSaveSuccessResponse>;
napi_value JSDistributedObject::JSRevokeSave(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("JSRevokeSave()");
    struct RevokeSaveContext : public ContextBase {
        JSObjectWrapper *wrapper;
    };
    auto ctxt = std::make_shared<RevokeSaveContext>();
    std::function<void(size_t argc, napi_value * argv)> getCbOpe = [env, ctxt](size_t argc, napi_value *argv) {
        // required 1 arguments :: <key>
        JSObjectWrapper *wrapper = nullptr;
        napi_status status = napi_unwrap(env, ctxt->self, (void **)&wrapper);
        NOT_MATCH_RETURN_VOID(status == napi_ok && wrapper != nullptr && wrapper->GetObject() != nullptr);
        ctxt->wrapper = wrapper;
    };
    ctxt->GetCbInfo(env, info, getCbOpe);
    if (ctxt->status != napi_ok) {
        napi_throw_error((env), std::to_string(ctxt->error->GetCode()).c_str(), ctxt->error->GetMessage().c_str());
        return nullptr;
    }
    auto execute = [ctxt]() {
        CHECH_STATUS_RETURN_VOID(env, ctxt->wrapper != nullptr, ctxt, "wrapper is null");
        CHECH_STATUS_RETURN_VOID(env, ctxt->wrapper->GetObject() != nullptr, ctxt, "object is null");
        uint32_t status = ctxt->wrapper->GetObject()->RevokeSave();
        INVALID_API_THROW_ERROR(status != ERR_PROCESSING);
        INVALID_STATUS_THROW_ERROR(status == SUCCESS, "operation failed");
        ctxt->status = napi_ok;
        LOG_INFO("end");
    };
    auto output = [env, ctxt](napi_value &result) {
        if (ctxt->status == napi_ok) {
            CHECH_STATUS_RETURN_VOID(env, ctxt->wrapper != nullptr, ctxt, "wrapper is null");
            CHECH_STATUS_RETURN_VOID(env, ctxt->wrapper->GetObject() != nullptr, ctxt, "object is null");
            ctxt->status = napi_new_instance(env,
                JSDistributedObject::GetRevokeSaveResultCons(env, ctxt->wrapper->GetObject()->GetSessionId()), 0,
                nullptr, &result);
            INVALID_STATUS_RETURN_ERROR(ctxt, "output failed!");
        }
    };
    return NapiQueue::AsyncWork(env, ctxt, std::string(__FUNCTION__), execute, output);
}

napi_value JSDistributedObject::GetSaveResultCons(
    napi_env env, std::string &sessionId, double version, std::string deviceId)
{
    const char *objectName = "SaveResult";
    napi_value napiSessionId;
    napi_value napiVersion;
    napi_value napiDeviceId;
    napi_value result;

    napi_status status = JSUtil::SetValue(env, sessionId, napiSessionId);
    NOT_MATCH_RETURN_NULL(status == napi_ok);
    status = JSUtil::SetValue(env, version, napiVersion);
    NOT_MATCH_RETURN_NULL(status == napi_ok);
    status = JSUtil::SetValue(env, deviceId, napiDeviceId);
    NOT_MATCH_RETURN_NULL(status == napi_ok);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY("sessionId", napiSessionId),
        DECLARE_NAPI_PROPERTY("version", napiVersion),
        DECLARE_NAPI_PROPERTY("deviceId", napiDeviceId)
    };

    status = napi_define_class(env, objectName, strlen(objectName), JSDistributedObject::JSConstructor, nullptr,
        sizeof(desc) / sizeof(desc[0]), desc, &result);
    NOT_MATCH_RETURN_NULL(status == napi_ok);
    return result;
}

napi_value JSDistributedObject::GetRevokeSaveResultCons(napi_env env, std::string &sessionId)
{
    const char *objectName = "RevokeSaveResult";
    napi_value napiSessionId;
    napi_value result;

    napi_status status = JSUtil::SetValue(env, sessionId, napiSessionId);
    NOT_MATCH_RETURN_NULL(status == napi_ok);
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_PROPERTY("sessionId", napiSessionId)
    };

    status = napi_define_class(env, objectName, strlen(objectName), JSDistributedObject::JSConstructor, nullptr,
        sizeof(desc) / sizeof(desc[0]), desc, &result);
    NOT_MATCH_RETURN_NULL(status == napi_ok);
    return result;
}

napi_value JSDistributedObject::JSBindAssetStore(napi_env env, napi_callback_info info)
{
    struct BindAssetStoreContext : public ContextBase {
        std::string assetKey;
        AssetBindInfo bindInfo;
        JSObjectWrapper *wrapper;
    };
    auto ctxt = std::make_shared<BindAssetStoreContext>();
    auto input = [env, ctxt](size_t argc, napi_value *argv) {
        INVALID_ARGS_RETURN_ERROR(ctxt, argc >= 2, "arguments error", std::make_shared<ParametersNum>("2"));
        ctxt->status = JSUtil::GetValue(env, argv[0], ctxt->assetKey);
        INVALID_ARGS_RETURN_ERROR(ctxt, ctxt->status == napi_ok, "arguments error",
            std::make_shared<ParametersType>("assetKey", "string"));

        ctxt->status = JSUtil::GetValue(env, argv[1], ctxt->bindInfo);
        INVALID_ARGS_RETURN_ERROR(ctxt, ctxt->status == napi_ok, "arguments error",
            std::make_shared<ParametersType>("bindInfo", "BindInfo"));
        JSObjectWrapper *wrapper = nullptr;
        napi_status status = napi_unwrap(env, ctxt->self, (void **)&wrapper);
        NOT_MATCH_RETURN_VOID(status == napi_ok && wrapper != nullptr && wrapper->GetObject() != nullptr);
        ctxt->wrapper = wrapper;
    };
    ctxt->GetCbInfo(env, info, input);
    NAPI_ASSERT_ERRCODE(env, ctxt->status == napi_ok, ctxt->error);
    auto execute = [ctxt]() {
        CHECH_STATUS_RETURN_VOID(env, ctxt->wrapper != nullptr, ctxt, "wrapper is null");
        CHECH_STATUS_RETURN_VOID(env, ctxt->wrapper->GetObject() != nullptr, ctxt, "object is null");
        uint32_t status = ctxt->wrapper->GetObject()->BindAssetStore(ctxt->assetKey, ctxt->bindInfo);
        LOG_INFO("BindAssetStore return: %{public}d", status);
        INVALID_API_THROW_ERROR(status != ERR_PROCESSING);
        INVALID_STATUS_THROW_ERROR(status == SUCCESS, "operation failed");
        ctxt->status = napi_ok;
    };
    return NapiQueue::AsyncWork(env, ctxt, std::string(__FUNCTION__), execute);
}
} // namespace OHOS::ObjectStore