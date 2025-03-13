/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#define LOG_TAG "NapiUtils"

#include "napi_utils.h"

#include "napi_error_utils.h"

namespace OHOS::CollaborationEdit {
constexpr size_t STR_TAIL_LENGTH = 1;

// second param is the base64 code of data.collaborationEditObject
static constexpr NapiUtils::JsFeatureSpace FEATURE_NAME_SPACES[] = {
    {"ohos.data.collaborationEditObject", "ZGF0YS5jb2xsYWJvcmF0aW9uRWRpdE9iamVjdA==", true},
};

const std::optional<NapiUtils::JsFeatureSpace> NapiUtils::GetJsFeatureSpace(const std::string &name)
{
    auto jsFeature = JsFeatureSpace{name.data(), "", false};
    auto iter = std::lower_bound(FEATURE_NAME_SPACES,
        FEATURE_NAME_SPACES + sizeof(FEATURE_NAME_SPACES) / sizeof(FEATURE_NAME_SPACES[0]),
        jsFeature,
        [](const JsFeatureSpace &JsFeatureSpace1, const JsFeatureSpace &JsFeatureSpace2) {
            return strcmp(JsFeatureSpace1.spaceName, JsFeatureSpace2.spaceName) < 0;
        });
    if (iter < FEATURE_NAME_SPACES + sizeof(FEATURE_NAME_SPACES) / sizeof(FEATURE_NAME_SPACES[0]) &&
        strcmp(iter->spaceName, name.data()) == 0) {
        return *iter;
    }
    return std::nullopt;
}

napi_status NapiUtils::GetValue(napi_env env, napi_value input, napi_value &out)
{
    out = input;
    return napi_ok;
}

napi_status NapiUtils::SetValue(napi_env env, napi_value input, napi_value &out)
{
    out = input;
    return napi_ok;
}

/* napi_value <-> bool */
napi_status NapiUtils::GetValue(napi_env env, napi_value input, bool &out)
{
    return napi_get_value_bool(env, input, &out);
}

napi_status NapiUtils::SetValue(napi_env env, const bool &input, napi_value &out)
{
    return napi_get_boolean(env, input, &out);
}

/* napi_value <-> int32_t */
napi_status NapiUtils::GetValue(napi_env env, napi_value input, int32_t &out)
{
    return napi_get_value_int32(env, input, &out);
}

napi_status NapiUtils::SetValue(napi_env env, const int32_t &input, napi_value &out)
{
    return napi_create_int32(env, input, &out);
}

/* napi_value <-> uint32_t */
napi_status NapiUtils::GetValue(napi_env env, napi_value input, uint32_t &out)
{
    return napi_get_value_uint32(env, input, &out);
}

napi_status NapiUtils::SetValue(napi_env env, const uint32_t &input, napi_value &out)
{
    return napi_create_uint32(env, input, &out);
}

/* napi_value <-> int64_t */
napi_status NapiUtils::GetValue(napi_env env, napi_value input, int64_t &out)
{
    return napi_get_value_int64(env, input, &out);
}

napi_status NapiUtils::SetValue(napi_env env, const int64_t &input, napi_value &out)
{
    return napi_create_int64(env, input, &out);
}

/* napi_value <-> double */
napi_status NapiUtils::GetValue(napi_env env, napi_value input, double &out)
{
    return napi_get_value_double(env, input, &out);
}

napi_status NapiUtils::SetValue(napi_env env, const double &input, napi_value &out)
{
    return napi_create_double(env, input, &out);
}

/* napi_value <-> std::string */
napi_status NapiUtils::GetValue(napi_env env, napi_value input, std::string &out)
{
    napi_valuetype type = napi_undefined;
    napi_status ret = napi_typeof(env, input, &type);
    ASSERT((ret == napi_ok) && (type == napi_string), "invalid type", napi_invalid_arg);

    size_t maxLen = 0;
    ret = napi_get_value_string_utf8(env, input, nullptr, 0, &maxLen);
    if (ret != napi_ok) {
        return ret;
    }
    LOG_DEBUG("napi_value -> std::string get length %{public}d", (int)maxLen);
    char *buf = new (std::nothrow) char[maxLen + STR_TAIL_LENGTH];
    ASSERT(buf != nullptr, "buffer is nullptr", napi_generic_failure);
    size_t len = 0;
    ret = napi_get_value_string_utf8(env, input, buf, maxLen + STR_TAIL_LENGTH, &len);
    if (ret == napi_ok) {
        buf[len] = 0;
        out = std::string(buf);
    }
    delete[] buf;
    return ret;
}

napi_status NapiUtils::SetValue(napi_env env, const std::string &input, napi_value &out)
{
    return napi_create_string_utf8(env, input.c_str(), input.size(), &out);
}

/* napi_value <-> std::vector<uint8_t> */
napi_status NapiUtils::GetValue(napi_env env, napi_value input, std::vector<uint8_t> &out)
{
    out.clear();
    LOG_DEBUG("napi_value -> std::vector<uint8_t> ");
    napi_typedarray_type type = napi_biguint64_array;
    napi_value buffer = nullptr;
    void *data = nullptr;
    size_t length = 0;
    size_t offset = 0;
    napi_status statusMsg = napi_get_typedarray_info(env, input, &type, &length, &data, &buffer, &offset);
    LOG_DEBUG("array type=%{public}d length=%{public}d offset=%{public}d", (int)type, (int)length, (int)offset);
    ASSERT(statusMsg == napi_ok, "napi_get_typedarray_info go wrong!", napi_invalid_arg);
    ASSERT(type == napi_uint8_array, "is not Uint8Array!", napi_invalid_arg);
    ASSERT((length > 0) && (data != nullptr), "invalid data!", napi_invalid_arg);
    out.assign(static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + length);
    return statusMsg;
}

napi_status NapiUtils::SetValue(napi_env env, const std::vector<uint8_t> &input, napi_value &out)
{
    LOG_DEBUG("napi_value <- std::vector<uint8_t> ");
    ASSERT(input.size() > 0, "invalid std::vector<uint8_t>", napi_invalid_arg);
    void *data = nullptr;
    napi_value buffer = nullptr;
    napi_status statusMsg = napi_create_arraybuffer(env, input.size(), &data, &buffer);
    ASSERT((statusMsg == napi_ok), "create array buffer go wrong!", statusMsg);

    if (memcpy_s(data, input.size(), input.data(), input.size()) != EOK) {
        LOG_ERROR("napi_value <- std::vector<uint8_t>: memcpy_s go wrong, vector size:%{public}zd", input.size());
        return napi_invalid_arg;
    }
    statusMsg = napi_create_typedarray(env, napi_uint8_array, input.size(), buffer, 0, &out);
    ASSERT((statusMsg == napi_ok), "napi_value <- std::vector<uint8_t> invalid value", statusMsg);
    return statusMsg;
}

napi_status NapiUtils::GetCurrentAbilityParam(napi_env env, ContextParam &param)
{
    auto ability = AbilityRuntime::GetCurrentAbility(env);
    if (ability == nullptr) {
        LOG_ERROR("GetCurrentAbility go wrong");
        return napi_invalid_arg;
    }

    auto context = ability->GetAbilityContext();
    if (context == nullptr) {
        LOG_ERROR("GetAbilityContext go wrong");
        return napi_invalid_arg;
    }
    param.area = context->GetArea();
    param.baseDir = context->GetDatabaseDir();
    param.bundleName = context->GetBundleName();
    auto abilityInfo = context->GetAbilityInfo();
    if (abilityInfo != nullptr) {
        param.moduleName = abilityInfo->moduleName;
    }
    return napi_ok;
}

napi_status NapiUtils::GetValue(napi_env env, napi_value input, ContextParam &param)
{
    if (input == nullptr) {
        LOG_INFO("get ContextParam, input is null");
        param.isStageMode = false;
        return GetCurrentAbilityParam(env, param);
    }
    napi_status status = GetNamedProperty(env, input, "stageMode", param.isStageMode);
    ASSERT(status == napi_ok, "get stageMode param go wrong", napi_invalid_arg);
    if (!param.isStageMode) {
        LOG_WARN("isStageMode false -> fa stage");
        return GetCurrentAbilityParam(env, param);
    }
    LOG_DEBUG("stage mode branch");
    status = GetNamedProperty(env, input, "databaseDir", param.baseDir);
    ASSERT(status == napi_ok, "get databaseDir go wrong", napi_invalid_arg);
    status = GetNamedProperty(env, input, "area", param.area, true);
    ASSERT(status == napi_ok, "get area go wrong", napi_invalid_arg);

    napi_value hapInfo = nullptr;
    GetNamedProperty(env, input, "currentHapModuleInfo", hapInfo);
    if (hapInfo != nullptr) {
        status = GetNamedProperty(env, hapInfo, "name", param.moduleName);
        ASSERT(status == napi_ok, "get currentHapModuleInfo.name go wrong", napi_invalid_arg);
    }

    napi_value appInfo = nullptr;
    GetNamedProperty(env, input, "applicationInfo", appInfo);
    if (appInfo != nullptr) {
        status = GetNamedProperty(env, appInfo, "name", param.bundleName);
        ASSERT(status == napi_ok, "get applicationInfo.name go wrong", napi_invalid_arg);
        status = GetNamedProperty(env, appInfo, "systemApp", param.isSystemApp, true);
        ASSERT(status == napi_ok, "get applicationInfo.systemApp go wrong", napi_invalid_arg);
    }
    return napi_ok;
}

napi_value NapiUtils::DefineClass(napi_env environment, const std::string &spaceName, const std::string &className,
    const Descriptor &descriptor, napi_callback ctor)
{
    auto featureSpace = GetJsFeatureSpace(spaceName);
    if (!featureSpace.has_value() || !featureSpace->isComponent) {
        LOG_INFO("featureSpace has no feature space name");
        return nullptr;
    }
    if (GetClass(environment, spaceName, className)) {
        return GetClass(environment, spaceName, className);
    }
    auto rootPropName = std::string(featureSpace->nameBase64);
    napi_value root = nullptr;
    bool hasRoot = false;
    napi_value global = nullptr;
    napi_get_global(environment, &global);
    napi_has_named_property(environment, global, rootPropName.c_str(), &hasRoot);
    if (hasRoot) {
        napi_get_named_property(environment, global, rootPropName.c_str(), &root);
    } else {
        napi_create_object(environment, &root);
        napi_set_named_property(environment, global, rootPropName.c_str(), root);
    }

    std::string propName = "constructor_of_" + className;
    napi_value constructor = nullptr;
    bool hasProp = false;
    napi_has_named_property(environment, root, propName.c_str(), &hasProp);
    if (hasProp) {
        napi_get_named_property(environment, root, propName.c_str(), &constructor);
        if (constructor != nullptr) {
            LOG_DEBUG("got data.distributedCollab.%{public}s as constructor", propName.c_str());
            return constructor;
        }
        hasProp = false;  // no constructor.
    }

    auto properties = descriptor();
    NAPI_CALL(environment,
        napi_define_class(environment,
            className.c_str(),
            className.size(),
            ctor,
            nullptr,
            properties.size(),
            properties.data(),
            &constructor));
    NAPI_ASSERT(environment, constructor != nullptr, "napi_define_class go wrong!");

    if (!hasProp) {
        napi_set_named_property(environment, root, propName.c_str(), constructor);
        LOG_DEBUG("save constructor to data.distributedCollab.%{public}s", propName.c_str());
    }
    return constructor;
}

napi_value NapiUtils::GetClass(napi_env env, const std::string &spaceName, const std::string &className)
{
    auto featureSpace = GetJsFeatureSpace(spaceName);
    if (!featureSpace.has_value()) {
        LOG_DEBUG("featureSpace has no feature space name");
        return nullptr;
    }
    auto rootPropName = std::string(featureSpace->nameBase64);
    napi_value root = nullptr;
    napi_value global = nullptr;
    napi_get_global(env, &global);
    bool hasRoot;
    napi_has_named_property(env, global, rootPropName.c_str(), &hasRoot);
    if (!hasRoot) {
        LOG_DEBUG("GetClass has no root");
        return nullptr;
    }
    napi_get_named_property(env, global, rootPropName.c_str(), &root);
    std::string propName = "constructor_of_" + className;
    bool hasProperty = false;
    napi_value constructor = nullptr;
    napi_has_named_property(env, root, propName.c_str(), &hasProperty);
    if (!hasProperty) {
        LOG_DEBUG("GetClass has no constructor_of_className %{public}s", className.c_str());
        return nullptr;
    }
    napi_get_named_property(env, root, propName.c_str(), &constructor);
    if (constructor != nullptr) {
        LOG_DEBUG("got data.distributedCollab.%{public}s as constructor", propName.c_str());
        return constructor;
    }
    hasProperty = false;  // no constructor.
    return constructor;
}

bool NapiUtils::IsNull(napi_env env, napi_value value)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, value, &type);
    if (status == napi_ok && (type == napi_undefined || type == napi_null)) {
        return true;
    }
    return false;
}

std::pair<napi_status, napi_value> NapiUtils::GetInnerValue(
    napi_env env, napi_value input, const std::string &prop, bool optional)
{
    bool hasProp = false;
    napi_status status = napi_has_named_property(env, input, prop.c_str(), &hasProp);
    if (status != napi_ok) {
        return std::make_pair(napi_generic_failure, nullptr);
    }
    if (!hasProp) {
        status = optional ? napi_ok : napi_generic_failure;
        return std::make_pair(status, nullptr);
    }
    napi_value inner = nullptr;
    status = napi_get_named_property(env, input, prop.c_str(), &inner);
    if (status != napi_ok || inner == nullptr) {
        return std::make_pair(napi_generic_failure, nullptr);
    }
    if (optional && NapiUtils::IsNull(env, inner)) {
        return std::make_pair(napi_ok, nullptr);
    }
    return std::make_pair(napi_ok, inner);
}

std::string NapiUtils::RemovePrefix(std::string str, std::string prefix)
{
    size_t prefixLength = prefix.length();
    size_t foundPos = str.find_first_of(prefix);
    if (foundPos == std::string::npos) {
        return str;
    }
    return str.erase(foundPos, prefixLength);
}

} // namespace OHOS::CollaborationEdit
