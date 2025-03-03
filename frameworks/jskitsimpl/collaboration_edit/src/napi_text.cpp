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

#define LOG_TAG "Text"

#include "napi_text.h"

#include "napi_error_utils.h"
#include "napi_parser.h"

namespace OHOS::CollaborationEdit {
static __thread napi_ref g_text_cons_ref = nullptr;

Text::Text() : AbstractType()
{}

Text::~Text()
{}

void Text::Init(napi_env env, napi_value exports)
{
    napi_value cons = Text::Constructor(env);
    NAPI_CALL_RETURN_VOID(env, napi_create_reference(env, cons, 1, &g_text_cons_ref));
    NAPI_CALL_RETURN_VOID(env, napi_set_named_property(env, exports, "Text", cons));
    LOG_DEBUG("Text::Init end.");
}

napi_value Text::Constructor(napi_env env)
{
    napi_property_descriptor descriptors[] = {
        DECLARE_NAPI_FUNCTION("getId", GetUniqueId),
        DECLARE_NAPI_FUNCTION("insert", Insert),
        DECLARE_NAPI_FUNCTION("delete", Delete),
        DECLARE_NAPI_FUNCTION("format", Format),
        DECLARE_NAPI_FUNCTION("getPlainText", GetPlainText),
        DECLARE_NAPI_FUNCTION("getJsonResult", GetJsonResult),
    };
    napi_value cons = nullptr;
    NAPI_CALL(env, napi_define_class(env, "Text", NAPI_AUTO_LENGTH, New, nullptr,
        sizeof(descriptors) / sizeof(napi_property_descriptor), descriptors, &cons));
    return cons;
}

napi_value Text::New(napi_env env, napi_callback_info info)
{
    napi_value newTarget = nullptr;
    NAPI_CALL(env, napi_get_new_target(env, info, &newTarget));

    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));

    // create instance by 'new Node(name)'
    if (newTarget != nullptr) {
        Text *text = new (std::nothrow) Text();
        ASSERT_THROW_BASE(env, text != nullptr, Status::INTERNAL_ERROR, "text new: new text go wrong", self);
        auto finalize = [](napi_env env, void *data, void *hint) {
            Text *text = reinterpret_cast<Text *>(data);
            delete text;
        };
        napi_wrap(env, self, text, finalize, nullptr, nullptr);
        return self;
    }

    // create instance by 'Node(name)'
    napi_value cons = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, g_text_cons_ref, &cons));
    napi_value output = nullptr;
    NAPI_CALL(env, napi_new_instance(env, cons, 0, nullptr, &output));
    return output;
}

napi_value Text::GetUniqueId(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    Text *text = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&text)));
    ASSERT(text != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, text->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    if (!text->GetID().has_value()) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "empty id");
        return nullptr;
    }
    ID id = text->GetID().value();
    napi_value result;
    NAPI_CALL(env, napi_create_object(env, &result));
    napi_value jsDeviceId;
    NapiUtils::SetValue(env, id.deviceId, jsDeviceId);
    NAPI_CALL(env, napi_set_named_property(env, result, "deviceId", jsDeviceId));
    napi_value jsClock;
    NapiUtils::SetValue(env, static_cast<int64_t>(id.clock), jsClock);
    NAPI_CALL(env, napi_set_named_property(env, result, "clock", jsClock));
    return result;
}

napi_value Text::Insert(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[3] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    Text *text = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&text)));
    ASSERT(text != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, text->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    int64_t index;
    napi_status status = NapiUtils::GetValue(env, argv[0], index);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: read index go wrong");
    ASSERT_THROW(env, index >= 0, Status::INVALID_ARGUMENT, "Param Error: Invalid index");
    std::string content;
    status = NapiUtils::GetValue(env, argv[1], content);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: read content go wrong");
    napi_valuetype valueType;
    // argv[2] represents the third parameter
    NAPI_CALL(env, napi_typeof(env, argv[2], &valueType));
    std::string formatStr;
    if (valueType != napi_undefined) {
        int ret = Parser::ParseJsFormatToStr(env, argv[2], formatStr);
        ASSERT_THROW(env, ret == OK, Status::INVALID_ARGUMENT, "convert format go wrong");
    }
    int32_t retCode = text->GetAdapter()->TextInsert(index, content, formatStr);
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "TextInsert go wrong.");
    }
    return nullptr;
}

napi_value Text::Delete(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    Text *text = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&text)));
    ASSERT(text != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, text->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    int64_t index;
    napi_status status = NapiUtils::GetValue(env, argv[0], index);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: read index go wrong");
    ASSERT_THROW(env, index >= 0, Status::INVALID_ARGUMENT, "Param Error: Invalid index");
    int64_t length = 0;
    status = NapiUtils::GetValue(env, argv[1], length);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: read length go wrong");
    ASSERT_THROW(env, length > 0, Status::INVALID_ARGUMENT, "Param Error: Invalid length");
    int32_t retCode = text->GetAdapter()->TextDelete(index, length);
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "TextDelete go wrong.");
    }
    return nullptr;
}

napi_value Text::Format(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[3] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    Text *text = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&text)));
    ASSERT(text != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, text->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    int64_t index;
    napi_status status = NapiUtils::GetValue(env, argv[0], index);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: read index go wrong");
    ASSERT_THROW(env, index >= 0, Status::INVALID_ARGUMENT, "Param Error: Invalid index");
    int64_t length = 0;
    status = NapiUtils::GetValue(env, argv[1], length);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "Param Error: read length go wrong");
    ASSERT_THROW(env, length > 0, Status::INVALID_ARGUMENT, "Param Error: Invalid length");
    napi_valuetype valueType;
    // argv[2] represents the third parameter
    NAPI_CALL(env, napi_typeof(env, argv[2], &valueType));
    std::string formatStr;
    if (valueType != napi_undefined) {
        // argv[2] represents the third parameter
        (void)Parser::ParseJsFormatToStr(env, argv[2], formatStr);
    }
    int32_t retCode = text->GetAdapter()->TextFormat(index, length, formatStr);
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "TextFormat go wrong.");
    }
    return nullptr;
}

napi_value Text::GetPlainText(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    Text *text = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&text)));
    ASSERT(text != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, text->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    auto [retCode, result] = text->GetAdapter()->ReadStringText();
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "ReadStringText go wrong.");
        return nullptr;
    }
    napi_value output = nullptr;
    NapiUtils::SetValue(env, result, output);
    return output;
}

napi_value Text::GetJsonResult(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    Text *text = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&text)));
    ASSERT(text != nullptr, "unwrap self go wrong.", nullptr);
    ASSERT_THROW(env, text->GetID().has_value(), Status::UNSUPPORTED_OPERATION, "empty id");
    auto [retCode, result] = text->GetAdapter()->ReadDeltaText("", "");
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "ReadDeltaText go wrong.");
        return nullptr;
    }
    napi_value output = nullptr;
    NapiUtils::SetValue(env, result, output);
    return output;
}
} // namespace OHOS::CollaborationEdit
