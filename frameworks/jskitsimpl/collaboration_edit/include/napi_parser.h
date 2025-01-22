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
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"

namespace OHOS::CollaborationEdit {
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
};
} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_PARSER_H
