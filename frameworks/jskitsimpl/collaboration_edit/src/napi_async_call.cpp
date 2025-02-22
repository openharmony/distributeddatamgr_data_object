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
#define LOG_TAG "AsyncCall"

#include "napi_async_call.h"

#include "log_print.h"
#include "napi_error_utils.h"

namespace OHOS::CollaborationEdit {

int SyncContext::SetAll(
    napi_env env, napi_callback_info info, InputAction input, ExecuteAction exec, OutputAction output)
{
    env_ = env;
    size_t argc = 2;
    napi_value self = nullptr;
    napi_value argv[2] = { nullptr };
    napi_status status = napi_get_cb_info(env, info, &argc, argv, &self, nullptr);
    if (status != napi_ok) {
        LOG_ERROR("get callback info go wrong, status: %{public}" PRIi32, static_cast<int32_t>(status));
        return ERR;
    }

    napi_create_reference(env, self, 1, &self_);
    if (status != napi_ok) {
        LOG_ERROR("create self reference go wrong, status: %{public}" PRIi32, static_cast<int32_t>(status));
        return ERR;
    }

    int ret = input(env, argc, argv, self);
    ASSERT(ret == OK, "get input function go wrong", ret);

    output_ = std::move(output);
    exec_ = std::move(exec);
    return OK;
}

void SyncContext::ReleaseInnerReference()
{
    if (keep_ != nullptr) {
        keep_.reset();
        keep_ = nullptr;
    }
}

SyncContext::~SyncContext()
{
    LOG_DEBUG("SyncContext freed.");
    if (env_ == nullptr) {
        return;
    }
    if (work_ != nullptr) {
        napi_delete_async_work(env_, work_);
        work_ = nullptr;
    }
    if (self_ != nullptr) {
        napi_delete_reference(env_, self_);
        self_ = nullptr;
    }
    env_ = nullptr;
}

int AsyncCall::Call(napi_env env, std::shared_ptr<SyncContext> context, const char *fun)
{
    if (context->callback_ == nullptr) {
        LOG_ERROR("create async task wrong, callback is null");
        return ERR;
    }
    context->fun = (fun == nullptr || fun[0] == '\0') ? SYNC_FUNCTION_NAME : fun;
    context->keep_ = context;
    napi_value resource = nullptr;
    napi_status status = napi_create_string_utf8(env, context->fun, NAPI_AUTO_LENGTH, &resource);
    if (status != napi_ok) {
        LOG_ERROR("napi create string wrong, status: %{public}" PRIi32, static_cast<int32_t>(status));
        return ERR;
    }
    // create async work, execute function is OnExecute, complete function is OnComplete
    status = napi_create_async_work(env, nullptr, resource, AsyncCall::OnExecute, AsyncCall::OnComplete,
        reinterpret_cast<void *>(context.get()), &context->work_);
    if (status != napi_ok) {
        LOG_ERROR("create async work go wrong, status: %{public}" PRIi32, static_cast<int32_t>(status));
        return ERR;
    }
    // add async work to execute queue
    status = napi_queue_async_work_with_qos(env, context->work_, napi_qos_user_initiated);
    if (status != napi_ok) {
        LOG_ERROR("add async work go wrong, status: %{public}" PRIi32, static_cast<int32_t>(status));
        return ERR;
    }
    return OK;
}

void AsyncCall::OnExecute(napi_env env, void *data)
{
    SyncContext *context = reinterpret_cast<SyncContext *>(data);
    if (context->exec_ != nullptr) {
        context->execCode_ = context->exec_();
    }
    context->exec_ = nullptr;
}

void AsyncCall::OnComplete(napi_env env, void *data)
{
    SyncContext *context = reinterpret_cast<SyncContext *>(data);
    if (context->output_) {
        context->output_(env, context->result_);
    }
    context->output_ = nullptr;
}

void AsyncCall::OnComplete(napi_env env, napi_status status, void *data)
{
    OnComplete(env, data);
    if (status != napi_ok) {
        LOG_ERROR("task execute go wrong, status: %{public}" PRIi32, static_cast<int32_t>(status));
        SyncContext *context = reinterpret_cast<SyncContext *>(data);
        context->ReleaseInnerReference();
    }
}

void AsyncCall::CloudSyncCallback(napi_env env, napi_value js_cb, void *context, void *data)
{
    SyncCallbackParamT *callbackParam = reinterpret_cast<SyncCallbackParamT *>(data);
    ASSERT_VOID(callbackParam != nullptr, "callbackParam is null");

    napi_value param = nullptr;
    napi_status status = napi_create_object(env, &param);
    if (status != napi_ok) {
        LOG_ERROR("create param object wrong, status: %{public}" PRIi32, static_cast<int32_t>(status));
        callbackParam->promise.set_value(0);
        return;
    }
    
    napi_value pCode = nullptr;
    NapiUtils::SetValue(env, callbackParam->detail.code, pCode);
    status = napi_set_named_property(env, param, "code", pCode);
    if (status != napi_ok) {
        LOG_ERROR("set named property wrong, status: %{public}" PRIi32, static_cast<int32_t>(status));
        callbackParam->promise.set_value(0);
        return;
    }

    auto start = std::chrono::steady_clock::now();
    status = napi_call_function(env, nullptr, js_cb, 1, &param, nullptr);
    auto finish = std::chrono::steady_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(finish - start);
    if (duration > TIME_THRESHOLD) {
        int64_t ms = duration.count();
        LOG_WARN("callback overtime, time: %{public}" PRIi64 " ms", ms);
    }
    if (status != napi_ok) {
        LOG_ERROR("napi call js function wrong, status: %{public}" PRIi32, static_cast<int32_t>(status));
    }
    callbackParam->promise.set_value(0);
}
} // namespace CollaborationEdit
