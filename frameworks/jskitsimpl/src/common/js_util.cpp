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
#include "js_util.h"

#include <securec.h>

#include "logger.h"

namespace OHOS::ObjectStore {
constexpr int32_t STR_MAX_LENGTH = 4096;
constexpr size_t STR_TAIL_LENGTH = 1;

/* napi_value <-> bool */
napi_status JSUtil::GetValue(napi_env env, napi_value in, bool &out)
{
    LOG_DEBUG("napi_value <- bool");
    return napi_get_value_bool(env, in, &out);
}

napi_status JSUtil::SetValue(napi_env env, const bool &in, napi_value &out)
{
    LOG_DEBUG("napi_value -> bool");
    return napi_get_boolean(env, in, &out);
}

/* napi_value <-> double */
napi_status JSUtil::GetValue(napi_env env, napi_value in, double &out)
{
    LOG_DEBUG("napi_value -> double");
    return napi_get_value_double(env, in, &out);
}

napi_status JSUtil::SetValue(napi_env env, const double &in, napi_value &out)
{
    LOG_DEBUG("napi_value <- double");
    return napi_create_double(env, in, &out);
}

/* napi_value <-> int64_t */
napi_status JSUtil::GetValue(napi_env env, napi_value in, int64_t& out)
{
    return napi_get_value_int64(env, in, &out);
}

napi_status JSUtil::SetValue(napi_env env, const int64_t& in, napi_value& out)
{
    return napi_create_int64(env, in, &out);
}

/* napi_value <-> uint32_t */
napi_status JSUtil::GetValue(napi_env env, napi_value in, uint32_t& out)
{
    return napi_get_value_uint32(env, in, &out);
}

napi_status JSUtil::SetValue(napi_env env, const uint32_t& in, napi_value& out)
{
    return napi_create_uint32(env, in, &out);
}

/* napi_value <-> std::string */
napi_status JSUtil::GetValue(napi_env env, napi_value in, std::string &out)
{
    size_t maxLen = STR_MAX_LENGTH;
    napi_status status = napi_get_value_string_utf8(env, in, NULL, 0, &maxLen);
    if (status != napi_ok || maxLen <= 0) {
        return napi_generic_failure;
    }
    char *buf = new (std::nothrow) char[maxLen + STR_TAIL_LENGTH];
    if (buf != nullptr) {
        size_t len = 0;
        status = napi_get_value_string_utf8(env, in, buf, maxLen + STR_TAIL_LENGTH, &len);
        if (status == napi_ok) {
            out = std::string(buf);
        }
        delete[] buf;
    } else {
        status = napi_generic_failure;
    }
    return status;
}

napi_status JSUtil::SetValue(napi_env env, const std::string &in, napi_value &out)
{
    LOG_DEBUG("napi_value <- std::string %{public}d", (int)in.length());
    return napi_create_string_utf8(env, in.c_str(), in.size(), &out);
}

/* napi_value <-> std::vector<std::string> */
napi_status JSUtil::GetValue(napi_env env, napi_value in, std::vector<std::string> &out)
{
    LOG_DEBUG("napi_value -> std::vector<std::string>");
    bool isArray = false;
    napi_is_array(env, in, &isArray);
    LOG_ERROR_RETURN(isArray, "not an array", napi_invalid_arg);

    uint32_t length = 0;
    napi_status status = napi_get_array_length(env, in, &length);
    LOG_ERROR_RETURN((status == napi_ok) && (length > 0), "get_array failed!", napi_invalid_arg);
    for (uint32_t i = 0; i < length; ++i) {
        napi_value item = nullptr;
        status = napi_get_element(env, in, i, &item);
        LOG_ERROR_RETURN((item != nullptr) && (status == napi_ok), "no element", napi_invalid_arg);
        std::string value;
        status = GetValue(env, item, value);
        LOG_ERROR_RETURN(status == napi_ok, "not a string", napi_invalid_arg);
        out.push_back(value);
    }
    return status;
}

napi_status JSUtil::SetValue(napi_env env, const std::vector<std::string> &in, napi_value &out)
{
    LOG_DEBUG("napi_value <- std::vector<std::string>");
    napi_status status = napi_create_array_with_length(env, in.size(), &out);
    LOG_ERROR_RETURN(status == napi_ok, "create array failed!", status);
    int index = 0;
    for (auto &item : in) {
        napi_value element = nullptr;
        SetValue(env, item, element);
        status = napi_set_element(env, out, index++, element);
        LOG_ERROR_RETURN((status == napi_ok), "napi_set_element failed!", status);
    }
    return status;
}

/* napi_value <-> std::vector<uint8_t> */
napi_status JSUtil::GetValue(napi_env env, napi_value in, std::vector<uint8_t> &out)
{
    out.clear();
    LOG_DEBUG("napi_value -> std::vector<uint8_t> ");
    napi_typedarray_type type = napi_biguint64_array;
    size_t length = 0;
    napi_value buffer = nullptr;
    size_t offset = 0;
    void *data = nullptr;
    napi_status status = napi_get_typedarray_info(env, in, &type, &length, &data, &buffer, &offset);
    LOG_DEBUG("array type=%{public}d length=%{public}d offset=%{public}d  status=%{public}d", (int)type, (int)length,
        (int)offset, status);
    LOG_ERROR_RETURN(status == napi_ok, "napi_get_typedarray_info failed!", napi_invalid_arg);
    LOG_ERROR_RETURN(type == napi_uint8_array, "is not Uint8Array!", napi_invalid_arg);
    LOG_ERROR_RETURN((length > 0) && (data != nullptr), "invalid data!", napi_invalid_arg);
    out.assign(static_cast<uint8_t *>(data), static_cast<uint8_t *>(data) + length);
    return status;
}

napi_status JSUtil::SetValue(napi_env env, const std::vector<uint8_t> &in, napi_value &out)
{
    LOG_DEBUG("napi_value <- std::vector<uint8_t> ");
    LOG_ERROR_RETURN(in.size() > 0, "invalid std::vector<uint8_t>", napi_invalid_arg);
    void *data = nullptr;
    napi_value buffer = nullptr;
    napi_status status = napi_create_arraybuffer(env, in.size(), &data, &buffer);
    LOG_ERROR_RETURN((status == napi_ok), "create array buffer failed!", status);

    if (memcpy_s(data, in.size(), in.data(), in.size()) != EOK) {
        LOG_ERROR("memcpy_s not EOK");
        return napi_invalid_arg;
    }
    status = napi_create_typedarray(env, napi_uint8_array, in.size(), buffer, 0, &out);
    LOG_ERROR_RETURN((status == napi_ok), "napi_value <- std::vector<uint8_t> invalid value", status);
    return status;
}

napi_status JSUtil::GetValue(napi_env env, napi_value in, AssetBindInfo &bindInfo)
{
    napi_status status = napi_invalid_arg;
    status = GetNamedProperty(env, in, "storeName", bindInfo.storeName);
    LOG_ERROR_RETURN(status == napi_ok, "get store param failed", status);
    status = GetNamedProperty(env, in, "tableName", bindInfo.tableName);
    LOG_ERROR_RETURN(status == napi_ok, "get table param failed", status);
    status = GetNamedProperty(env, in, "primaryKey", bindInfo.primaryKey);

    LOG_ERROR_RETURN(status == napi_ok, "get primary param failed", status);
    status = GetNamedProperty(env, in, "field", bindInfo.field);
    LOG_ERROR_RETURN(status == napi_ok, "get field param failed", status);
    status = GetNamedProperty(env, in, "assetName", bindInfo.assetName);
    LOG_ERROR_RETURN(status == napi_ok, "get assetName param failed", status);
    return status;
}

napi_status JSUtil::GetValue(napi_env env, napi_value in, Asset &asset)
{
    napi_valuetype type;
    napi_status status = napi_typeof(env, in, &type);
    LOG_ERROR_RETURN((status == napi_ok) && (type == napi_object), "Asset type invalid", napi_invalid_arg);
    status = GetNamedProperty(env, in, "name", asset.name);
    LOG_ERROR_RETURN(status == napi_ok, "get name param failed", status);
    status = GetNamedProperty(env, in, "uri", asset.uri);
    LOG_ERROR_RETURN(status == napi_ok, "get uri param failed", status);
    status = GetNamedProperty(env, in, "path", asset.path);
    LOG_ERROR_RETURN(status == napi_ok, "get path param failed", status);
    status = GetNamedProperty(env, in, "createTime", asset.createTime);
    LOG_ERROR_RETURN(status == napi_ok, "get createTime param failed", status);
    status = GetNamedProperty(env, in, "modifyTime", asset.modifyTime);
    LOG_ERROR_RETURN(status == napi_ok, "get modifyTime param failed", status);
    status = GetNamedProperty(env, in, "size", asset.size);
    LOG_ERROR_RETURN(status == napi_ok, "get size param failed", status);
    status = GetNamedProperty(env, in, "status", asset.status, true);
    LOG_ERROR_RETURN(status == napi_ok, "get status param failed", status);
    return status;
}

napi_status JSUtil::GetValue(napi_env env, napi_value in, Assets &assets)
{
    assets.clear();
    bool isArray = false;
    napi_is_array(env, in, &isArray);
    LOG_ERROR_RETURN(isArray, "not an array", napi_generic_failure);

    uint32_t arrLen = 0;
    napi_status status = napi_get_array_length(env, in, &arrLen);
    LOG_ERROR_RETURN((status == napi_ok) && (arrLen > 0), "get_array failed!", status);
    for (uint32_t i = 0; i < arrLen; ++i) {
        napi_value item = nullptr;
        status = napi_get_element(env, in, i, &item);
        LOG_ERROR_RETURN((item != nullptr) && (status == napi_ok), "no element", status);
        Asset asset;
        status = GetValue(env, item, asset);
        LOG_ERROR_RETURN(status == napi_ok, "not a asset object", status);
        assets.push_back(asset);
    }
    return status;
}

napi_status JSUtil::GetValue(napi_env env, napi_value in, ValuesBucket &out)
{
    out.clear();
    napi_value values = nullptr;
    uint32_t count = 0;
    napi_get_all_property_names(env, in, napi_key_own_only,
        static_cast<napi_key_filter>(napi_key_enumerable | napi_key_skip_symbols),
        napi_key_numbers_to_strings, &values);
    napi_status status = napi_get_array_length(env, values, &count);
    LOG_ERROR_RETURN(status == napi_ok && count > 0, "get ValuesBucket failed", napi_invalid_arg);
    napi_value key = nullptr;
    napi_value val = nullptr;
    for (uint32_t index = 0; index < count; index++) {
        status = napi_get_element(env, values, index, &key);
        LOG_ERROR_RETURN(status == napi_ok && key != nullptr, "no element", napi_invalid_arg);
        std::string keyStr;
        status = GetValue(env, key, keyStr);
        LOG_ERROR_RETURN(status == napi_ok, "get key failed", status);
        status = napi_get_property(env, in, key, &val);
        LOG_ERROR_RETURN(status == napi_ok && val != nullptr, "no element", napi_invalid_arg);
        ValueObject value;
        status = GetValue(env, val, value);
        if (status == napi_ok) {
            out.insert(std::pair<std::string, ValueObject>(keyStr, value));
        } else if (status != napi_generic_failure) {
            LOG_ERROR("The value type %{public}s is invalid: ", keyStr.c_str());
            return status;
        }
    }
    return napi_ok;
}

napi_status JSUtil::GetValue(napi_env env, napi_value jsValue, std::monostate &out)
{
    napi_value tempValue;
    napi_get_null(env, &tempValue);
    bool equal = false;
    napi_strict_equals(env, jsValue, tempValue, &equal);
    if (equal) {
        out = std::monostate();
        return napi_ok;
    }
    LOG_DEBUG("jsValue is not null");
    return napi_invalid_arg;
}

bool JSUtil::IsNull(napi_env env, napi_value value)
{
    napi_valuetype type = napi_undefined;
    napi_status status = napi_typeof(env, value, &type);
    if (status == napi_ok && (type == napi_undefined || type == napi_null)) {
        return true;
    }
    return false;
}
} // namespace OHOS::ObjectStore
