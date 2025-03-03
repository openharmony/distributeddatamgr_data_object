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

#define LOG_TAG "EditUnit"

#include "napi_edit_unit.h"

#include <nlohmann/json.hpp>

#include "napi_collaboration_edit_object.h"
#include "napi_error_utils.h"
#include "napi_parser.h"

namespace OHOS::CollaborationEdit {
using json = nlohmann::json;
EditUnit::EditUnit(std::string name) : AbstractType(),  name_(name)
{}

EditUnit::~EditUnit()
{}

napi_value EditUnit::Initialize(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    std::string name;
    napi_status status = NapiUtils::GetValue(env, argv[0], name);
    ASSERT_THROW_BASE(env, status == napi_ok, Status::INVALID_ARGUMENT, "read name param go wrong", self);
    ASSERT_THROW_BASE(env, !name.empty(), Status::INVALID_ARGUMENT, "Param Error: invalid name", self);
    name = std::to_string(LABEL_FRAGMENT) + "_" + name;
    EditUnit *editUnit = new (std::nothrow) EditUnit(name);
    ASSERT_THROW_BASE(env, editUnit != nullptr, Status::INTERNAL_ERROR, "Initialize: new editunit go wrong", self);
    editUnit->SetTableName(name);
    auto finalize = [](napi_env env, void *data, void *hint) {
        EditUnit *editUnit = reinterpret_cast<EditUnit *>(data);
        delete editUnit;
    };
    napi_wrap(env, self, editUnit, finalize, nullptr, nullptr);
    return self;
}

napi_value EditUnit::Constructor(napi_env env)
{
    auto lambda = []() -> std::vector<napi_property_descriptor> {
        std::vector<napi_property_descriptor> properties = {
            DECLARE_NAPI_FUNCTION("insertNodes", InsertNodes),
            DECLARE_NAPI_FUNCTION("delete", Delete),
            DECLARE_NAPI_FUNCTION("getChildren", GetChildren),
            DECLARE_NAPI_FUNCTION("getJsonResult", GetJsonResult),
            DECLARE_NAPI_FUNCTION("getName", GetName),
            DECLARE_NAPI_FUNCTION("getRelativePos", GetRelativePos),
            DECLARE_NAPI_FUNCTION("getAbsolutePos", GetAbsolutePos),
        };
        return properties;
    };
    return NapiUtils::DefineClass(
        env, "ohos.data.collaborationEditObject", "EditUnit", lambda, Initialize);
}

napi_value EditUnit::NewInstance(napi_env env, napi_callback_info info, napi_value parent)
{
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    napi_value editUnit = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));
    napi_status status = napi_new_instance(env, Constructor(env), argc, argv, &editUnit);
    if (editUnit == nullptr || status != napi_ok) {
        LOG_ERROR("new instance go wrong");
    }
    return editUnit;
}

napi_value EditUnit::GetName(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    EditUnit *editUnit = nullptr;
    napi_status status = napi_unwrap(env, self, reinterpret_cast<void **>(&editUnit));
    if (status != napi_ok || editUnit == nullptr) {
        LOG_ERROR("unwrap editUnit go wrong, status = %{public}d", status);
        return nullptr;
    }
    std::string name = editUnit->name_;
    if (name.compare(0, NUMBER_OF_CHARS_IN_LABEL_PREFIX, std::to_string(LABEL_FRAGMENT) + "_") == 0) {
        name = name.substr(NUMBER_OF_CHARS_IN_LABEL_PREFIX);
    }
    napi_value result;
    NapiUtils::SetValue(env, name, result);
    return result;
}

napi_value EditUnit::InsertNodes(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    EditUnit *editUnit = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&editUnit)));
    ASSERT(editUnit != nullptr, "unwrap self go wrong.", nullptr);
    int64_t index = 0;
    napi_status status = NapiUtils::GetValue(env, argv[0], index);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: Read index go wrong.");
    ASSERT_THROW(env, index >= 0, Status::INVALID_ARGUMENT, "Param Error: Invalid index.");
    bool isArray = false;
    NAPI_CALL(env, napi_is_array(env, argv[1], &isArray));
    ASSERT_THROW(env, isArray, Status::INVALID_ARGUMENT, "Param Error: The nodes must be an array.");
    uint32_t length = 0;
    NAPI_CALL(env, napi_get_array_length(env, argv[1], &length));
    for (uint32_t i = 0; i < length; i++) {
        napi_value jsNode = nullptr;
        NAPI_CALL(env, napi_get_element(env, argv[1], i, &jsNode));
        Node *tempNode = nullptr;
        NAPI_CALL(env, napi_unwrap(env, jsNode, reinterpret_cast<void **>(&tempNode)));
        auto [retCode, id] = editUnit->GetAdapter()->InsertNode(index + i, tempNode->InnerGetName());
        if (retCode != SUCCESS || !id.has_value()) {
            ThrowNapiError(env, retCode, "InsertNodes go wrong.");
            return nullptr;
        }
        tempNode->SetID(id);
        tempNode->SetTableName(editUnit->GetTableName());
        tempNode->SetDBStore(editUnit->GetDBStore());
    }
    LOG_INFO("insert nodes successfully.");
    return nullptr;
}

napi_value EditUnit::Delete(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    EditUnit *editUnit = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&editUnit)));
    ASSERT(editUnit != nullptr, "unwrap self go wrong.", nullptr);
    int64_t index = 0;
    napi_status status = NapiUtils::GetValue(env, argv[0], index);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: Read index go wrong.");
    ASSERT_THROW(env, index >= 0, Status::INVALID_ARGUMENT, "Param Error: Invalid index.");
    int64_t length = 0;
    status = NapiUtils::GetValue(env, argv[1], length);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: Read length go wrong.");
    ASSERT_THROW(env, length > 0, Status::INVALID_ARGUMENT, "Param Error: Invalid length.");

    int32_t retCode = editUnit->GetAdapter()->DeleteChildren(index, length);
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "Delete Nodes go wrong.");
    }
    return nullptr;
}

napi_value EditUnit::GetChildren(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    EditUnit *editUnit = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&editUnit)));
    ASSERT(editUnit != nullptr, "unwrap self go wrong.", nullptr);
    int64_t start = 0;
    napi_status status = NapiUtils::GetValue(env, argv[0], start);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: Read start index go wrong.");
    ASSERT_THROW(env, start >= 0, Status::INVALID_ARGUMENT, "Param Error: Invalid start.");
    int64_t end = 0;
    status = NapiUtils::GetValue(env, argv[1], end);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: Read end index go wrong.");
    ASSERT_THROW(env, end >= 0, Status::INVALID_ARGUMENT, "Param Error: Invalid end.");
    ASSERT_THROW(env, end > start, Status::INVALID_ARGUMENT, "Param Error: end should be greater than start.");

    auto [retCode, result] = editUnit->GetAdapter()->GetChildren(start, end - start);
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "Get Children go wrong.");
        return nullptr;
    }
    // transfer string to node array
    napi_value output = nullptr;
    int ret = Parser::ParseJsonStrToJsChildren(env, result, editUnit, output);
    if (ret != OK) {
        ThrowNapiError(env, Status::INTERNAL_ERROR, "convert result go wrong.");
        return nullptr;
    }
    return output;
}

napi_value EditUnit::GetJsonResult(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    EditUnit *editUnit = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&editUnit)));
    ASSERT(editUnit != nullptr, "unwrap self go wrong.", nullptr);
    auto [retCode, result] = editUnit->GetAdapter()->GetJsonString();
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "toString go wrong.");
        return nullptr;
    }
    napi_value output = nullptr;
    napi_status status = NapiUtils::SetValue(env, result, output);
    if (status != napi_ok || output == nullptr) {
        ThrowNapiError(env, Status::INTERNAL_ERROR, "create output string go wrong.");
        return nullptr;
    }
    return output;
}

napi_value EditUnit::GetRelativePos(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));

    uint32_t pos;
    napi_status status = NapiUtils::GetValue(env, argv[0], pos);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "read pos param go wrong");

    EditUnit *editUnit = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&editUnit)));
    ASSERT(editUnit != nullptr, "unwrap self go wrong.", nullptr);

    auto dbStore = editUnit->GetDBStore();
    std::string relPos;
    int32_t ret = (*dbStore).GetRelativePos(editUnit->name_.c_str(), "{}", pos, relPos);
    ASSERT_THROW(env, ret == OK, Status::INTERNAL_ERROR, "GetRelativePos go wrong");

    return Parser::GetRelativePosFromJsonStr(env, relPos);
}

napi_value EditUnit::GetAbsolutePos(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    EditUnit *editUnit = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&editUnit)));
    ASSERT(editUnit != nullptr, "unwrap self go wrong.", nullptr);

    json root;
    napi_value type;
    napi_status status = NapiUtils::GetNamedProperty(env, argv[0], "parentId", type);
    json typeJson;
    if (status == napi_ok && !NapiUtils::IsNull(env, type)) {
        Parser::GetUniqueIdFromNapiValueToJsonStr(env, type, typeJson);
    } else {
        typeJson = nullptr;
    }
    root["type"] = typeJson;

    std::string typeName;
    NapiUtils::GetNamedProperty(env, argv[0], "parentName", typeName);
    if (!typeName.empty()) {
        root["tname"] = typeName;
    } else {
        root["tname"] = nullptr;
    }

    napi_value item;
    status = NapiUtils::GetNamedProperty(env, argv[0], "id", item);
    json itemJson;
    if (status == napi_ok && !NapiUtils::IsNull(env, item)) {
        Parser::GetUniqueIdFromNapiValueToJsonStr(env, item, itemJson);
    } else {
        itemJson = nullptr;
    }
    root["item"] = itemJson;

    int64_t assoc;
    status = NapiUtils::GetNamedProperty(env, argv[0], "pos", assoc);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "read pos param go wrong");
    root["assoc"] = assoc;

    std::string relPos_str = root.dump();
    auto dbStore = editUnit->GetDBStore();
    uint32_t pos;
    int32_t ret = (*dbStore).GetAbsolutePos(editUnit->name_.c_str(), relPos_str.c_str(), "{}", &pos);
    ASSERT_THROW(env, ret == SUCCESS, ret, "GetAbsolutePos go wrong");

    napi_value output = nullptr;
    status = NapiUtils::SetValue(env, static_cast<int64_t>(pos), output);
    return output;
}
}  // namespace OHOS::CollaborationEdit
