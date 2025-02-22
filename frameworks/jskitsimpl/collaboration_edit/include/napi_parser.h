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

#ifndef COLLABORATION_EDIT_PARSER_H
#define COLLABORATION_EDIT_PARSER_H

#include <string>
#include <variant>
#include <vector>

#include <nlohmann/json.hpp>

#include "napi_abstract_type.h"
#include "napi_cloud_db.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS::CollaborationEdit {
using json = nlohmann::json;
class Parser {
public:
    static void Stringsplit(std::string str, const char split, std::vector<std::string> &res);
    static bool ConvertToUint64(std::string str, uint64_t &out);
    static std::optional<ID> ConvertStrToID(std::string str);
    static int ParseJsonStrToJsChildren(
        napi_env env, const std::string &nodeListStr, AbstractType *parent, napi_value &out);
    static int ParseFromAttrsJsonStr(napi_env env, const std::string &jsonStr, napi_value &out);
    static int ParseJsFormatToStr(napi_env env, napi_value jsFormat, std::string &out);
    static int ParseVariantJsValueToStr(napi_env env, napi_value input, std::string &out);
    static int CheckValueType(napi_env env, napi_value value);
    static int ParseCloudDbFields(napi_env env, napi_value input, std::vector<napi_value> &cloudDbFuncVector);
    static napi_value GetUniqueIdFromJsonStr(napi_env env, json &root);
    static napi_value GetRelativePosFromJsonStr(napi_env env, std::string &relPos);
    static void GetUniqueIdFromNapiValueToJsonStr(napi_env env, napi_value type, json &typeJson);
    static int ParseJsonStrToJsUpdateNode(
        napi_env env, std::string nodeJsonStr, std::shared_ptr<DBStore> dbStore, napi_value &out);
    static napi_value ParseFromAssetOpConfig(napi_env env, const AssetOpConfig &config);

private:
    static int SetRelativePosType(napi_env env, json &root, napi_value &relativePos);
    static int SetRelativePosItem(napi_env env, json &root, napi_value &relativePos);
    static int SetRelativePosTname(napi_env env, json &root, napi_value &relativePos);
    static int SetRelativePosAssoc(napi_env env, json &root, napi_value &relativePos);
};
} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_PARSER_H
