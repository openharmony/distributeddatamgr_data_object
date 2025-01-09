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

#ifndef NAPI_COLLABORATION_EDIT_OBJECT_H
#define NAPI_COLLABORATION_EDIT_OBJECT_H

#include "db_store.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi_undo_manager.h"
#include "napi_utils.h"

namespace OHOS::CollaborationEdit {

class CollaborationEditObject {
public:
    CollaborationEditObject(std::string docName, ContextParam param);
    ~CollaborationEditObject();
    static napi_value NewInstance(napi_env env, napi_callback_info info);
    static napi_value Delete(napi_env env, napi_callback_info info);
    void SetDBStore(std::shared_ptr<DBStore> dbStore);
    std::shared_ptr<DBStore> GetDBStore();

private:
    static napi_value Constructor(napi_env env);
    static napi_value Initialize(napi_env env, napi_callback_info info);

    static napi_value GetEditUnit(napi_env env, napi_callback_info info);
    static napi_value GetUndoRedoManager(napi_env env, napi_callback_info info);
    static napi_value DeleteUndoRedoManager(napi_env env, napi_callback_info info);
    static napi_value GetName(napi_env env, napi_callback_info info);

    std::string docName_;
    std::shared_ptr<ContextParam> param_;
    std::shared_ptr<DBStore> dbStore_;
};
} // namespace OHOS::CollaborationEdit
#endif // NAPI_COLLABORATION_EDIT_OBJECT_H
