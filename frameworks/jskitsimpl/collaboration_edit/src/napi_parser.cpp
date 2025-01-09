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

#define LOG_TAG "NapiParser"
#include "napi_parser.h"

#include <charconv>
#include <iostream>
#include <sstream>

#include <nlohmann/json.hpp>

#include "napi_errno.h"
#include "napi_error_utils.h"
#include "napi_node.h"
#include "napi_utils.h"

namespace OHOS::CollaborationEdit {
using json = nlohmann::json;

static constexpr const uint8_t NUMBER_OF_FIELDS_IN_ID = 2;

void Parser::Stringsplit(std::string str, const char split, std::vector<std::string> &res)
{
    std::istringstream iss(str);
    std::string token;
    while (std::getline(iss, token, split)) {
        res.push_back(token);
    }
}

bool Parser::ConvertToUint64(std::string str, uint64_t &out)
{
    auto [ptr, errCode] = std::from_chars(str.data(), str.data() + str.size(), out);
    return errCode == std::errc{} && ptr == str.data() + str.size();
}

std::optional<ID> Parser::ConvertStrToID(std::string idStr)
{
    if (idStr.empty()) {
        return std::nullopt;
    }
    std::vector<std::string> strList;
    Stringsplit(idStr, '_', strList);
    if (strList.size() != NUMBER_OF_FIELDS_IN_ID) {  // id returned from kernel is a string like 'id_clock'
        return std::nullopt;
    }
    uint64_t clock = 0;
    if (!ConvertToUint64(strList[1], clock)) {
        return std::nullopt;
    }
    ID id(strList[0], clock);
    return id;
}

int ParseJsonToJsNode(napi_env env, json jsonObj, AbstractType *parent, napi_value &out)
{
    napi_value constructor = Node::Constructor(env);
    ASSERT(constructor != nullptr, "node constructor is null", ERR);
    std::string nodeName = jsonObj["nodeName"];
    std::string idStr = jsonObj["elementId"];
    std::optional<ID> id = Parser::ConvertStrToID(idStr);
    ASSERT(id.has_value(), "incorrect id.", ERR); // ID should not be null
    napi_value jsNodeName = nullptr;
    napi_status status = NapiUtils::SetValue(env, nodeName, jsNodeName);
    ASSERT(status == napi_ok, "wrap nodeName go wrong.", ERR);
    size_t argc = 1;
    napi_value argv[1] = {jsNodeName};
    napi_value jsNode;
    status = napi_new_instance(env, constructor, argc, argv, &jsNode);
    ASSERT(status == napi_ok, "new node instance go wrong.", ERR);
    Node *tempNode = nullptr;
    status = napi_unwrap(env, jsNode, reinterpret_cast<void **>(&tempNode));
    ASSERT(status == napi_ok, "unwrap jsNode go wrong.", ERR);
    tempNode->SetDBStore(parent->GetDBStore());
    tempNode->SetTableName(parent->GetTableName());
    tempNode->SetID(id);
    out = jsNode;
    return OK;
}

int ParseJsonToJsText(napi_env env, json jsonObj, AbstractType *parent, napi_value &out)
{
    napi_value constructor = Text::Constructor(env);
    ASSERT(constructor != nullptr, "text constructor is null", ERR);
    std::string idStr = jsonObj["elementId"];
    LOG_DEBUG("id is %{public}s", idStr.c_str());
    std::optional<ID> id = Parser::ConvertStrToID(idStr);
    ASSERT(id.has_value(), "incorrect id.", ERR); // ID should not be null
    napi_value jsText;
    napi_status status = napi_new_instance(env, constructor, 0, nullptr, &jsText);
    ASSERT(status == napi_ok, "new text instance go wrong.", ERR);
    Node *tempText = nullptr;
    status = napi_unwrap(env, jsText, reinterpret_cast<void **>(&tempText));
    ASSERT(status == napi_ok, "unwrap jsText go wrong.", ERR);
    tempText->SetDBStore(parent->GetDBStore());
    tempText->SetTableName(parent->GetTableName());
    tempText->SetID(id);
    out = jsText;
    return OK;
}

int Parser::ParseJsonStrToJsChildren(napi_env env, std::string nodeJsonStr, AbstractType *parent, napi_value &out)
{
    napi_status status = napi_create_array(env, &out);
    ASSERT(status == napi_ok, "create array go wrong!", ERR);
    json jsonArray = json::parse(nodeJsonStr);
    ASSERT(jsonArray.is_array(), "result is not json array.", ERR);
    int i = 0;
    for (const auto &jsonObj : jsonArray) {
        if (!jsonObj.contains("type")) {
            continue;
        }
        std::string type = jsonObj["type"];
        napi_value jsNode;
        if (type.compare("XML_ELEMENT") == 0) {
            int ret = ParseJsonToJsNode(env, jsonObj, parent, jsNode);
            ASSERT(ret == OK, "Parse json to node go wrong.", ERR);
        } else if (type.compare("XML_TEXT") == 0) {
            int ret = ParseJsonToJsText(env, jsonObj, parent, jsNode);
            ASSERT(ret == OK, "Parse json to text go wrong.", ERR);
        } else {
            LOG_ERROR("Unsupported type. type = %{public}s", type.c_str());
            continue;
        }
        status = napi_set_element(env, out, i, jsNode);
        ASSERT(status == napi_ok, "set element go wrong.", ERR);
        i++;
    }
    return OK;
}

int Parser::ParseFromAttrsJsonStr(napi_env env, std::string jsonStr, napi_value &out)
{
    napi_status status = napi_create_object(env, &out);
    ASSERT(status == napi_ok, "create object go wrong!", ERR);
    json jsonObject = json::parse(jsonStr);
    napi_value jsAttr = nullptr;
    for (json::iterator iter = jsonObject.begin(); iter != jsonObject.end(); ++iter) {
        std::string key = iter.key();
        json value = iter.value();
        if (value.is_string()) {
            NapiUtils::SetValue(env, value.get<std::string>(), jsAttr);
        } else if (value.is_number_integer()) {
            NapiUtils::SetValue(env, value.get<int>(), jsAttr);
        } else if (value.is_boolean()) {
            NapiUtils::SetValue(env, value.get<bool>(), jsAttr);
        } else {
            LOG_ERROR("Unsupported value type");
            continue;
        }
        status = napi_set_named_property(env, out, key.c_str(), jsAttr);
    }
    return OK;
}

int Parser::ParseJsFormatToStr(napi_env env, napi_value jsFormat, std::string &out)
{
    napi_value keys = nullptr;
    napi_get_all_property_names(env, jsFormat, napi_key_own_only,
        static_cast<napi_key_filter>(napi_key_enumerable | napi_key_skip_symbols), napi_key_numbers_to_strings, &keys);
    uint32_t arrLen = 0;
    napi_status status = napi_get_array_length(env, keys, &arrLen);
    ASSERT(status == napi_ok, "get keys array length go wrong.", ERR);
    json resJson;
    for (size_t i = 0; i < arrLen; i++) {
        napi_value key = nullptr;
        status = napi_get_element(env, keys, i, &key);
        ASSERT(status == napi_ok, "get key element go wrong.", ERR);
        std::string keyStr;
        NapiUtils::GetValue(env, key, keyStr);
        napi_value jsValue = nullptr;
        napi_get_property(env, jsFormat, key, &jsValue);
        std::string value;
        int ret = ParseVariantJsValueToStr(env, jsValue, value);
        ASSERT(ret == OK, "parse value go wrong.", ERR);
        resJson[keyStr] = value;
    }
    out = resJson.dump();
    return OK;
}

int Parser::ParseVariantJsValueToStr(napi_env env, napi_value input, std::string &out)
{
    napi_valuetype valueType;
    napi_status status = napi_typeof(env, input, &valueType);
    ASSERT(status == napi_ok, "get type of input go wrong.", ERR);
    ASSERT(valueType != napi_undefined, "cannot be undefined.", ERR);
    switch (valueType) {
        case napi_number: {
            int64_t intValue;
            status = NapiUtils::GetValue(env, input, intValue);
            if (status == napi_ok) {
                out = std::to_string(intValue);
                break;
            }
            double doubleValue;
            status = NapiUtils::GetValue(env, input, doubleValue);
            ASSERT(status == napi_ok, "Neither int or double", ERR);
            out = std::to_string(doubleValue);
            break;
        }
        case napi_string: {
            std::string strValue;
            status = NapiUtils::GetValue(env, input, strValue);
            ASSERT(status == napi_ok, "convert to str go wrong.", ERR);
            out = strValue;
            break;
        }
        case napi_boolean: {
            bool bValue;
            status = NapiUtils::GetValue(env, input, bValue);
            ASSERT(status == napi_ok, "convert to bool go wrong.", ERR);
            out = bValue ? "true" : "false";
            break;
        }
        default: {
            LOG_ERROR("unsupported format type: %{public}d", valueType);
            return ERR;
        }
    }
    return OK;
}

} // namespace OHOS::CollaborationEdit
