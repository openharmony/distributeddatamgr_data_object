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

#ifndef NAPI_COLLABORATION_UNDO_MANAGER_H
#define NAPI_COLLABORATION_UNDO_MANAGER_H

#include "db_store.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "napi/native_node_api.h"
#include "napi_utils.h"
#include "rd_adapter.h"

namespace OHOS::CollaborationEdit {

class UndoManager {
public:
    UndoManager(std::string tableName, int64_t captureTimeout);
    ~UndoManager();
    static napi_value NewInstance(napi_env env, napi_callback_info info);

    std::string GetTableName();
    int64_t GetCaptureTimeout();
    std::shared_ptr<RdAdapter> GetAdapter();
    void SetDBStore(std::shared_ptr<DBStore> dbStore);

private:
    static napi_value Constructor(napi_env env);
    static napi_value Initialize(napi_env env, napi_callback_info info);

    static napi_value Undo(napi_env env, napi_callback_info info);
    static napi_value Redo(napi_env env, napi_callback_info info);

    std::shared_ptr<RdAdapter> adapter_;
    int64_t captureTimeout_;
};
} // namespace OHOS::CollaborationEdit
#endif // NAPI_COLLABORATION_UNDO_MANAGER_H
