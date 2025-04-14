/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#ifndef DATA_OBJ_NAPI_ASYNC_CALL_H
#define DATA_OBJ_NAPI_ASYNC_CALL_H

#include <future>

#include "db_store.h"
#include "napi_errno.h"
#include "napi_utils.h"
#include "napi/native_api.h"
#include "napi/native_common.h"
#include "rd_type.h"

namespace OHOS::CollaborationEdit {
using InputAction = std::function<int(napi_env, size_t, napi_value *, napi_value)>;
using OutputAction = std::function<void(napi_env, napi_value &)>;
using ExecuteAction = std::function<int()>;

constexpr const char *SYNC_FUNCTION_NAME = "CollaborationAsyncCall";
constexpr std::chrono::seconds TIME_THRESHOLD = std::chrono::seconds(30);

class SyncContext {
public:
    int SetAll(napi_env env, napi_callback_info info, InputAction input, ExecuteAction exec, OutputAction output);
    void ReleaseInnerReference();
    void Release();
    virtual ~SyncContext();

    napi_env env_ = nullptr;
    napi_ref self_ = nullptr;
    void *boundObj = nullptr;   // unwrapped collaborationEdit object
    const char *fun = nullptr;  // function name in napi queue

    napi_async_work work_ = nullptr;
    napi_value result_ = nullptr;

    int execCode_ = OK;
    OutputAction output_ = nullptr;
    ExecuteAction exec_ = nullptr;
    std::shared_ptr<SyncContext> keep_; // keep self alive

    uint64_t syncId = 0u;
    std::mutex callbackMutex_;
    napi_threadsafe_function callback_ = nullptr;

    int32_t syncMode_;
    std::shared_ptr<DBStore> dbStore_ = nullptr;
};

struct ProgressDetailT {
    ProgressCode code;
};

struct SyncCallbackParamT {
    std::promise<int> promise;
    ProgressDetailT detail;
    std::shared_ptr<SyncContext> syncContext;
};

class AsyncCall final {
public:
    static int Call(napi_env env, std::shared_ptr<SyncContext> context, const char *fun);
    static void CloudSyncCallback(napi_env env, napi_value js_cb, void *context, void *data);

private:
    static void OnExecute(napi_env env, void *data);
    static void OnComplete(napi_env env, void *data);
    static void OnReturn(napi_env env, napi_status status, void *data);
    static void OnComplete(napi_env env, napi_status status, void *data);
    static void ReleaseSyncContext(std::shared_ptr<SyncContext> syncContext);
};
} // namespace OHOS::CollaborationEdit
#endif
