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

#define LOG_TAG "EntryPoint"

#include "log_print.h"
#include "napi_collaboration_edit_object.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi_node.h"
#include "napi_text.h"

using namespace OHOS::CollaborationEdit;

static napi_value Init(napi_env env, napi_value exports)
{
    napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("getCollaborationEditObject", CollaborationEditObject::NewInstance),
        DECLARE_NAPI_FUNCTION("deleteCollaborationEditObject", CollaborationEditObject::Delete),
    };
    NAPI_CALL(env, napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc));
    Node::Init(env, exports);
    Text::Init(env, exports);
    return exports;
}

static __attribute__((constructor)) void RegisterModule()
{
    static napi_module module = {
        .nm_version = 1,
        .nm_flags = 0,
        .nm_filename = nullptr,
        .nm_register_func = Init,
        .nm_modname = "data.collaborationEditObject",
        .nm_priv = ((void *)0),
        .reserved = {0}
    };
    napi_module_register(&module);
    LOG_INFO("module register data.collaborationEditObject");
}
