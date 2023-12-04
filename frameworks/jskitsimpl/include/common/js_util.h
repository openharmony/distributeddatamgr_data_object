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
#ifndef OHOS_JS_UTIL_H
#define OHOS_JS_UTIL_H
#include <cstdint>
#include <map>
#include <string>
#include <variant>
#include <vector>

#include "napi/native_api.h"
#include "napi/native_node_api.h"
#include "object_types.h"

namespace OHOS::ObjectStore {
class JSUtil final {
public:
    /* napi_value <-> bool */
    static napi_status GetValue(napi_env env, napi_value in, bool &out);
    static napi_status SetValue(napi_env env, const bool &in, napi_value &out);

    /* napi_value <-> double */
    static napi_status GetValue(napi_env env, napi_value in, double &out);
    static napi_status SetValue(napi_env env, const double &in, napi_value &out);

    /* napi_value <-> std::string */
    static napi_status GetValue(napi_env env, napi_value in, std::string &out);
    static napi_status SetValue(napi_env env, const std::string &in, napi_value &out);
    
    /* napi_value <-> int32_t */
    static napi_status GetValue(napi_env env, napi_value in, int32_t& out);
    static napi_status SetValue(napi_env env, const int32_t& in, napi_value& out);

    static napi_status GetValue(napi_env env, napi_value in, uint32_t& out);
    static napi_status SetValue(napi_env env, const uint32_t& in, napi_value& out);

    /* napi_value <-> int64_t */
    static napi_status GetValue(napi_env env, napi_value in, int64_t& out);
    static napi_status SetValue(napi_env env, const int64_t& in, napi_value& out);

    /* napi_value <-> std::vector<std::string> */
    static napi_status GetValue(napi_env env, napi_value in, std::vector<std::string> &out);
    static napi_status SetValue(napi_env env, const std::vector<std::string> &in, napi_value &out);

    /* napi_value <-> std::vector<uint8_t> */
    static napi_status GetValue(napi_env env, napi_value in, std::vector<uint8_t> &out);
    static napi_status SetValue(napi_env env, const std::vector<uint8_t> &in, napi_value &out);

    static napi_status GetValue(napi_env env, napi_value in, Assets &assets);
    
    static napi_status GetValue(napi_env env, napi_value in, Asset &asset);
    
    static napi_status GetValue(napi_env env, napi_value in, AssetBindInfo &out);
    
    static napi_status GetValue(napi_env env, napi_value in, ValuesBucket &out);

    static napi_status GetValue(napi_env env, napi_value jsValue, std::monostate &out);

    static void GenerateNapiError(napi_env env, int32_t status, int32_t &errCode, std::string &errMessage);

    static bool IsNull(napi_env env, napi_value value);

    template <typename T>
    static inline napi_status GetNamedProperty(napi_env env, napi_value in, const std::string& prop,
        T& value, bool optional = false)
    {
        bool hasProp = false;
        napi_status status = napi_has_named_property(env, in, prop.c_str(), &hasProp);
        if (!hasProp) {
            status = optional ? napi_ok : napi_generic_failure;
            return status;
        }

        if ((status == napi_ok) && hasProp) {
            napi_value inner = nullptr;
            status = napi_get_named_property(env, in, prop.c_str(), &inner);
            if (!optional && IsNull(env, inner)) {
                return napi_generic_failure;
            }
            if ((status == napi_ok) && (inner != nullptr)) {
                return GetValue(env, inner, value);
            }
        }
        return napi_invalid_arg;
    };

    template<typename T>
    static napi_status GetValues(napi_env env, napi_value jsValue, T &value)
    {
        return napi_invalid_arg;
    }

    template<typename T, typename First, typename... Types>
    static napi_status GetValues(napi_env env, napi_value jsValue, T &value)
    {
        First cValue;
        auto ret = GetValue(env, jsValue, cValue);
        if (ret == napi_ok) {
            value = cValue;
            return ret;
        }
        return GetValues<T, Types...>(env, jsValue, value);
    }

    template<typename... Types>
    static napi_status GetValue(napi_env env, napi_value jsValue, std::variant<Types...> &value)
    {
        napi_valuetype type;
        napi_status status = napi_typeof(env, jsValue, &type);
        if (status != napi_ok) {
            return napi_invalid_arg;
        }
        if (type == napi_undefined) {
            return napi_generic_failure;
        }

        return GetValues<decltype(value), Types...>(env, jsValue, value);
    };
};

#define NAPI_ASSERT_ERRCODE_V9(env, assertion, version, err)                                                       \
    do {                                                                                                        \
        if (!(assertion)) {                                                                                     \
            if ((version) >= 9) {                                                                               \
                napi_throw_error((env), std::to_string((err)->GetCode()).c_str(), (err)->GetMessage().c_str()); \
            }                                                                                                   \
            return nullptr;                                                                                     \
        }                                                                                                       \
    } while (0)

#define NAPI_ASSERT_ERRCODE(env, condition, err)                                                           \
    do {                                                                                                    \
        if (!(condition)) {                                                                                 \
            napi_throw_error((env), std::to_string((err)->GetCode()).c_str(), (err)->GetMessage().c_str()); \
            return nullptr;                                                                                 \
        }                                                                                                   \
    } while (0)

#define CHECH_STATUS_RETURN_VOID(env, condition, ctxt, info) \
    do {                                                     \
        if (!(condition)) {                                  \
            LOG_ERROR(info);                                 \
            (ctxt)->status = napi_generic_failure;           \
            (ctxt)->message = std::string(info);             \
            return;                                          \
        }                                                    \
    } while (0)

#define LOG_ERROR_RETURN(condition, message, retVal)             \
    do {                                                         \
        if (!(condition)) {                                      \
            LOG_ERROR("test (" #condition ") failed: " message); \
            return retVal;                                       \
        }                                                        \
    } while (0)

#define LOG_ERROR_RETURN_VOID(condition, message)                \
    do {                                                         \
        if (!(condition)) {                                      \
            LOG_ERROR("test (" #condition ") failed: " message); \
            return;                                              \
        }                                                        \
    } while (0)
} // namespace OHOS::ObjectStore
#endif // OHOS_JS_UTIL_H
