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

#define LOG_TAG "Node"

#include "napi_node.h"

#include "napi_error_utils.h"
#include "napi_parser.h"

namespace OHOS::CollaborationEdit {
static __thread napi_ref g_node_cons_ref = nullptr;

Node::Node(std::string name) : AbstractType(), name_(name)
{}

Node::~Node()
{}

void Node::Init(napi_env env, napi_value exports)
{
    napi_value cons = Node::Constructor(env);
    NAPI_CALL_RETURN_VOID(env, napi_create_reference(env, cons, 1, &g_node_cons_ref));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, exports, "Node", cons));
    LOG_DEBUG("Node::Init end.");
}

napi_value Node::Constructor(napi_env env)
{
    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_FUNCTION("getId", GetUniqueId),
        DECLARE_NAPI_FUNCTION("getName", GetName),
        DECLARE_NAPI_FUNCTION("insertNodes", InsertNodes),
        DECLARE_NAPI_FUNCTION("delete", Delete),
        DECLARE_NAPI_FUNCTION("getChildren", GetChildren),
        DECLARE_NAPI_FUNCTION("getJsonResult", GetJsonResult),
        DECLARE_NAPI_FUNCTION("insertTexts", InsertTexts),
        DECLARE_NAPI_FUNCTION("setAttributes", SetAttributes),
        DECLARE_NAPI_FUNCTION("removeAttributes", RemoveAttributes),
        DECLARE_NAPI_FUNCTION("getAttributes", GetAttributes),
    };
    napi_value cons = nullptr;
    NAPI_CALL(env, napi_define_class(env, "Node", NAPI_AUTO_LENGTH, New, nullptr,
        sizeof(descriptors) / sizeof(napi_property_descriptor), descriptors, &cons));
    return cons;
}

napi_value Node::New(napi_env env, napi_callback_info info)
{
    napi_value newTarget = nullptr;
    NAPI_CALL(env, napi_get_new_target(env, info, &newTarget));

    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));

    // create instance by 'new Node(name)'
    if (newTarget != nullptr) {
        napi_valuetype valueType = napi_undefined;
        NAPI_CALL(env, napi_typeof(env, argv[0], &valueType));
        if (valueType != napi_string) {
            ThrowNapiError(env, Status::INVALID_ARGUMENT, "Param error. The name must be a string");
            return nullptr;
        }
        std::string name = "";
        NapiUtils::GetValue(env, argv[0], name);
        Node *node = new (std::nothrow) Node(name);
        auto finalize = [](napi_env env, void *data, void *hint) {
            Node *node = reinterpret_cast<Node *>(data);
            delete node;
        };
        napi_wrap(env, self, node, finalize, nullptr, nullptr);
        return self;
    }

    // create instance by 'Node(name)'
    napi_value cons = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, g_node_cons_ref, &cons));
    napi_value output = nullptr;
    NAPI_CALL(env, napi_new_instance(env, cons, argc, argv, &output));
    return output;
}

std::string Node::InnerGetName()
{
    return this->name_;
}

napi_value Node::GetName(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    Node *thisNode = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&thisNode)));
    ASSERT_THROW(env, thisNode != nullptr, Status::INTERNAL_ERROR, "unwrap self is null");
    napi_value result;
    NapiUtils::SetValue(env, thisNode->name_, result);
    return result;
}

napi_value Node::GetUniqueId(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    Node *thisNode = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&thisNode)));
    ASSERT(thisNode != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, thisNode->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    ID id = thisNode->GetID().value();
    napi_value result;
    NAPI_CALL(env, napi_create_object(env, &result));
    napi_value jsDeviceId;
    NapiUtils::SetValue(env, id.deviceId, jsDeviceId);
    NAPI_CALL(env, napi_set_named_property(env, result, "id", jsDeviceId));
    napi_value jsClock;
    NapiUtils::SetValue(env, static_cast<int64_t>(id.clock), jsClock);
    NAPI_CALL(env, napi_set_named_property(env, result, "clock", jsClock));
    return result;
}

napi_value Node::InsertNodes(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    Node *thisNode = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&thisNode)));
    ASSERT(thisNode != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, thisNode->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    int64_t index = 0;
    napi_status status = NapiUtils::GetValue(env, argv[0], index);
    if (status != napi_ok) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "Param Error: Unpack index go wrong.");
        return nullptr;
    }
    bool isArray;
    NAPI_CALL(env, napi_is_array(env, argv[1], &isArray));
    if (!isArray) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "Param Error: The nodes must be an array.");
        return nullptr;
    }
    uint32_t length = 0;
    NAPI_CALL(env, napi_get_array_length(env, argv[1], &length));
    LOG_INFO("length = %{public}u", length);
    for (uint32_t i = 0; i < length; i++) {
        napi_value jsNode = nullptr;
        NAPI_CALL(env, napi_get_element(env, argv[1], i, &jsNode));
        Node *tempNode = nullptr;
        NAPI_CALL(env, napi_unwrap(env, jsNode, reinterpret_cast<void **>(&tempNode)));
        auto [retCode, id] = thisNode->GetAdapter()->InsertNode(index + i, tempNode->InnerGetName());
        if (retCode != SUCCESS || !id.has_value()) {
            ThrowNapiError(env, retCode, "InsertNodes go wrong.");
            return nullptr;
        }
        tempNode->SetID(id);
        tempNode->SetTableName(thisNode->GetTableName());
        tempNode->SetDBStore(thisNode->GetDBStore());
    }
    return nullptr;
}

napi_value Node::Delete(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    Node *thisNode = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&thisNode)));
    ASSERT(thisNode != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, thisNode->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    int64_t index = 0;
    napi_status status = NapiUtils::GetValue(env, argv[0], index);
    if (status != napi_ok) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "Param Error: Unpack index go wrong.");
        return nullptr;
    }
    int64_t length = 0;
    status = NapiUtils::GetValue(env, argv[1], length);
    if (status != napi_ok) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "Param Error: Unpack length go wrong.");
        return nullptr;
    }

    int32_t retCode = thisNode->GetAdapter()->DeleteChildren(index, length);
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "Delete Nodes go wrong.");
    }
    return nullptr;
}

napi_value Node::GetChildren(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    Node *thisNode = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&thisNode)));
    ASSERT(thisNode != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, thisNode->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    int64_t start = 0;
    napi_status status = NapiUtils::GetValue(env, argv[0], start);
    if (status != napi_ok) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "Param Error: Unpack start go wrong.");
        return nullptr;
    }
    int64_t end = 0;
    status = NapiUtils::GetValue(env, argv[1], end);
    if (status != napi_ok) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "Param Error: Unpack end go wrong.");
        return nullptr;
    }

    auto [retCode, result] = thisNode->GetAdapter()->GetChildren(start, end - start);
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "Get Children go wrong.");
        return nullptr;
    }
    // transfer string to node array
    napi_value output = nullptr;
    int ret = Parser::ParseJsonStrToJsChildren(env, result, thisNode, output);
    if (ret != OK) {
        ThrowNapiError(env, Status::INTERNAL_ERROR, "convert result go wrong.");
        return nullptr;
    }
    return output;
}

napi_value Node::GetJsonResult(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    Node *thisNode = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&thisNode)));
    ASSERT(thisNode != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, thisNode->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    auto [retCode, result] = thisNode->GetAdapter()->GetJsonString();
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "toString go wrong.");
        return nullptr;
    }
    napi_value output = nullptr;
    napi_status status = NapiUtils::SetValue(env, result, output);
    if (status != napi_ok || output == nullptr) {
        ThrowNapiError(env, Status::INTERNAL_ERROR, "convert result go wrong.");
        return nullptr;
    }
    return output;
}

napi_value Node::InsertTexts(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    Node *thisNode = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&thisNode)));
    ASSERT(thisNode != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, thisNode->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    int64_t index = 0;
    napi_status status = NapiUtils::GetValue(env, argv[0], index);
    if (status != napi_ok) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "Param Error: Unpack index go wrong.");
        return nullptr;
    }
    bool isArray;
    NAPI_CALL(env, napi_is_array(env, argv[1], &isArray));
    if (!isArray) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "Param Error: The nodes must be an array.");
        return nullptr;
    }
    uint32_t length = 0;
    NAPI_CALL(env, napi_get_array_length(env, argv[1], &length));
    for (uint32_t i = 0; i < length; i++) {
        napi_value jsText = nullptr;
        NAPI_CALL(env, napi_get_element(env, argv[1], i, &jsText));
        Text *tempText = nullptr;
        NAPI_CALL(env, napi_unwrap(env, jsText, reinterpret_cast<void **>(&tempText)));
        auto [retCode, id] = thisNode->GetAdapter()->InsertText(index + i);
        if (retCode != SUCCESS || !id.has_value()) {
            ThrowNapiError(env, retCode, "InsertText go wrong.");
            return nullptr;
        }
        tempText->SetID(id);
        tempText->SetTableName(thisNode->GetTableName());
        tempText->SetDBStore(thisNode->GetDBStore());
    }
    return nullptr;
}

napi_value Node::SetAttributes(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    Node *thisNode = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&thisNode)));
    ASSERT(thisNode != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, thisNode->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");

    // convert input argument
    napi_value keys = nullptr;
    napi_get_all_property_names(env, argv[0], napi_key_own_only,
        static_cast<napi_key_filter>(napi_key_enumerable | napi_key_skip_symbols), napi_key_numbers_to_strings, &keys);
    uint32_t arrLen = 0;
    napi_status status = napi_get_array_length(env, keys, &arrLen);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: Parse argument go wrong.");
    for (size_t i = 0; i < arrLen; i++) {
        napi_value key = nullptr;
        status = napi_get_element(env, keys, i, &key);
        ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: get element go wrong.");
        std::string keyStr;
        NapiUtils::GetValue(env, key, keyStr);
        napi_value value = nullptr;
        napi_get_property(env, argv[0], key, &value);
        std::string valueStr;
        int retCode = Parser::ParseVariantJsValueToStr(env, value, valueStr);
        ASSERT(retCode == OK, "parse value go wrong.", nullptr);
        retCode = thisNode->GetAdapter()->SetAttribute(keyStr, valueStr, 0);
        ASSERT_THROW(env, retCode == SUCCESS, retCode, "Set attribute go wrong.");
    }
    return nullptr;
}

napi_value Node::RemoveAttributes(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    Node *thisNode = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&thisNode)));
    ASSERT(thisNode != nullptr, "unwrap self go wrong.", nullptr);
    bool isArray;
    NAPI_CALL(env, napi_is_array(env, argv[0], &isArray));
    if (!isArray) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "Param Error: The argument must be an array.");
        return nullptr;
    }
    uint32_t length = 0;
    NAPI_CALL(env, napi_get_array_length(env, argv[0], &length));
    for (uint32_t i = 0; i < length; i++) {
        napi_value jsAttributeName = nullptr;
        NAPI_CALL(env, napi_get_element(env, argv[0], i, &jsAttributeName));
        std::string attributeName;
        NapiUtils::GetValue(env, jsAttributeName, attributeName);
        int32_t retCode = thisNode->GetAdapter()->RemoveAttrribute(attributeName);
        if (retCode != SUCCESS) {
            ThrowNapiError(env, retCode, "RemoveAttrribute go wrong.");
            return nullptr;
        }
    }
    return nullptr;
}

napi_value Node::GetAttributes(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    Node *thisNode = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&thisNode)));
    ASSERT(thisNode != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, thisNode->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");

    auto [retCode, result] = thisNode->GetAdapter()->GetAttributes();
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "GetAttributes go wrong.");
        return nullptr;
    }
    // convert result string to record
    napi_value output = nullptr;
    int ret = Parser::ParseFromAttrsJsonStr(env, result, output);
    if (ret != OK) {
        ThrowNapiError(env, Status::INTERNAL_ERROR, "parse result go wrong.");
        return nullptr;
    }
    return output;
}
} // namespace OHOS::CollaborationEdit
