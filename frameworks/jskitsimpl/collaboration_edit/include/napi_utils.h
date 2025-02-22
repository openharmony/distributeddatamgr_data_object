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

#ifndef COLLABORATION_EDIT_NAPI_UTILS_H
#define COLLABORATION_EDIT_NAPI_UTILS_H

#include <map>
#include <vector>

#include "ability.h"
#include "napi_base_context.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS::CollaborationEdit {
struct ContextParam {
    std::string bundleName;
    std::string moduleName;
    std::string baseDir;
    int32_t area;
    bool isSystemApp = false;
    bool isStageMode = true;
};

class NapiUtils final {
public:
    using Descriptor = std::function<std::vector<napi_property_descriptor>()>;
    struct JsFeatureSpace {
        const char *spaceName;
        const char *nameBase64;
        bool isComponent;
    };

    static napi_status GetValue(napi_env env, napi_value input, napi_value &out);
    static napi_status SetValue(napi_env env, napi_value input, napi_value &out);
    /* napi_value <-> bool */
    static napi_status GetValue(napi_env env, napi_value input, bool &out);

    static napi_status SetValue(napi_env env, const bool &input, napi_value &out);

    /* napi_value <-> int32_t */
    static napi_status GetValue(napi_env env, napi_value input, int32_t &out);

    static napi_status SetValue(napi_env env, const int32_t &input, napi_value &out);

    /* napi_value <-> uint32_t */
    static napi_status GetValue(napi_env env, napi_value input, uint32_t &out);

    static napi_status SetValue(napi_env env, const uint32_t &input, napi_value &out);

    /* napi_value <-> int64_t */
    static napi_status GetValue(napi_env env, napi_value input, int64_t &out);

    static napi_status SetValue(napi_env env, const int64_t &input, napi_value &out);

    /* napi_value <-> double */
    static napi_status GetValue(napi_env env, napi_value input, double &out);

    static napi_status SetValue(napi_env env, const double &input, napi_value &out);

    /* napi_value <-> std::string */
    static napi_status GetValue(napi_env env, napi_value input, std::string &out);

    static napi_status SetValue(napi_env env, const std::string &input, napi_value &out);

    /* napi_value <-> std::vector<uint8_t> */
    static napi_status GetValue(napi_env env, napi_value input, std::vector<uint8_t> &out);

    static napi_status SetValue(napi_env env, const std::vector<uint8_t> &input, napi_value &out);

    /* napi_value <-> std::vector<int32_t> */
    static napi_status GetValue(napi_env env, napi_value input, std::vector<int32_t> &out);

    static napi_status SetValue(napi_env env, const std::vector<int32_t> &input, napi_value &out);

    /* napi_value <-> std::vector<uint32_t> */
    static napi_status GetValue(napi_env env, napi_value input, std::vector<uint32_t> &out);

    static napi_status SetValue(napi_env env, const std::vector<uint32_t> &input, napi_value &out);

    /* napi_value <-> std::vector<int64_t> */
    static napi_status GetValue(napi_env env, napi_value input, std::vector<int64_t> &out);

    static napi_status SetValue(napi_env env, const std::vector<int64_t> &input, napi_value &out);

    /* napi_value <-> std::vector<double> */
    static napi_status GetValue(napi_env env, napi_value input, std::vector<double> &out);

    static napi_status SetValue(napi_env env, const std::vector<double> &input, napi_value &out);

    /* napi_value <-> std::vector<std::string> */
    static napi_status GetValue(napi_env env, napi_value input, std::vector<std::string> &out);

    static napi_status SetValue(napi_env env, const std::vector<std::string> &input, napi_value &out);

    /* napi_value <-> std::map<std::string, int32_t> */
    static napi_status GetValue(napi_env env, napi_value input, std::map<std::string, int32_t> &out);

    static napi_status SetValue(napi_env env, const std::map<std::string, int32_t> &input, napi_value &out);

    static napi_status GetCurrentAbilityParam(napi_env env, ContextParam &param);

    static napi_status GetValue(napi_env env, napi_value input, ContextParam &param);

    static bool IsNull(napi_env env, napi_value value);

    static const std::optional<JsFeatureSpace> GetJsFeatureSpace(const std::string &name);

    /* napi_define_class  wrapper */
    static napi_value DefineClass(napi_env env, const std::string &spaceName, const std::string &className,
        const Descriptor &descriptor, napi_callback ctor);

    static napi_value GetClass(napi_env env, const std::string &spaceName, const std::string &className);

    template <typename T>
    static inline napi_status GetNamedProperty(
        napi_env env, napi_value input, const std::string &prop, T &value, bool optional = false)
    {
        auto [status, jsValue] = GetInnerValue(env, input, prop, optional);
        return (jsValue == nullptr) ? status : GetValue(env, jsValue, value);
    };

    static std::string RemovePrefix(std::string str, std::string prefix);

private:
    enum {
        /* std::map<key, value> to js::tuple<key, value> */
        TUPLE_KEY = 0,
        TUPLE_VALUE,
        TUPLE_SIZE
    };
    static std::pair<napi_status, napi_value> GetInnerValue(
        napi_env env, napi_value input, const std::string &prop, bool optional);
};
} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_NAPI_UTILS_H
