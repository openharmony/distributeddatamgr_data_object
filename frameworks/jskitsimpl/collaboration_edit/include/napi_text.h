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

#ifndef COLLABORATION_EDIT_TEXT_H
#define COLLABORATION_EDIT_TEXT_H

#include "napi_abstract_type.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi_utils.h"

namespace OHOS::CollaborationEdit {
class Text : public AbstractType {
public:
    Text();
    ~Text();
    static void Init(napi_env env, napi_value exports);
    static napi_value Constructor(napi_env env);

private:
    static napi_value New(napi_env env, napi_callback_info info);

    static napi_value GetUniqueId(napi_env env, napi_callback_info info);
    static napi_value Insert(napi_env env, napi_callback_info info);
    static napi_value Delete(napi_env env, napi_callback_info info);
    static napi_value Format(napi_env env, napi_callback_info info);
    static napi_value GetPlainText(napi_env env, napi_callback_info info);
    static napi_value GetJsonResult(napi_env env, napi_callback_info info);
};
}
#endif // COLLABORATION_EDIT_TEXT_H
