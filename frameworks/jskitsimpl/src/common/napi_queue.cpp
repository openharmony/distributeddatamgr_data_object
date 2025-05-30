/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "napi_queue.h"

#include "js_common.h"
#include "logger.h"

namespace OHOS::ObjectStore {
static constexpr size_t ARGC_MAX = 6;

ContextBase::~ContextBase()
{
    LOG_DEBUG("no memory leak after callback or promise[resolved/rejected]");
    if (env != nullptr) {
        napi_delete_reference(env, selfRef);
        env = nullptr;
    }
}

void ContextBase::GetCbInfo(napi_env envi, napi_callback_info info, NapiCbInfoParser parse, bool sync)
{
    env = envi;
    size_t argc = ARGC_MAX;
    napi_value argv[ARGC_MAX] = { nullptr };
    status = napi_get_cb_info(env, info, &argc, argv, &self, nullptr);
    INVALID_STATUS_RETURN_ERROR(this, "napi_get_cb_info failed!");
    INVALID_ARGS_RETURN_ERROR(this, argc <= ARGC_MAX, "too many arguments!", std::make_shared<InnerError>());
    INVALID_ARGS_RETURN_ERROR(this, self != nullptr, "no JavaScript this argument!", std::make_shared<InnerError>());
    napi_create_reference(env, self, 1, &selfRef);
    status = napi_unwrap(env, self, &native);
    INVALID_STATUS_RETURN_ERROR(this, "self unwrap failed!");

    if (!sync && (argc > 0)) {
        // get the last arguments :: <callback>
        size_t index = argc - 1;
        napi_valuetype type = napi_undefined;
        napi_status tyst = napi_typeof(env, argv[index], &type);
        if ((tyst == napi_ok) && (type == napi_function)) {
            status = napi_create_reference(env, argv[index], 1, &callbackRef);
            INVALID_STATUS_RETURN_ERROR(this, "ref callback failed!");
            argc = index;
            LOG_DEBUG("async callback, no promise");
        } else {
            INVALID_ARGS_RETURN_ERROR(this, type == napi_undefined, "arguments error!",
                std::make_shared<ParametersType>("callback", "function"));
            LOG_DEBUG("no callback, async promise");
        }
    }

    if (parse) {
        parse(argc, argv);
    } else {
        INVALID_ARGS_RETURN_ERROR(this, argc == 0, "required no arguments!", std::make_shared<InnerError>());
    }
}

napi_value NapiQueue::AsyncWork(napi_env env, std::shared_ptr<ContextBase> contextBase, const std::string &name,
    NapiAsyncExecute execute, NapiAsyncComplete complete)
{
    AsyncContext *aCtx = new (std::nothrow) AsyncContext;
    if (aCtx == nullptr) {
        LOG_ERROR("create aysnc context failed");
        return nullptr;
    }
    aCtx->ctxt = std::move(contextBase);
    aCtx->execute = std::move(execute);
    aCtx->complete = std::move(complete);
    auto ctxt = aCtx->ctxt;
    napi_value promise = nullptr;
    if (ctxt->callbackRef == nullptr) {
        napi_create_promise(ctxt->env, &ctxt->deferred, &promise);
    } else {
        napi_get_undefined(ctxt->env, &promise);
    }
    napi_value resource = nullptr;
    napi_create_string_utf8(ctxt->env, name.c_str(), NAPI_AUTO_LENGTH, &resource);
    napi_create_async_work(ctxt->env, nullptr, resource,
        [](napi_env env, void *data) {
            NOT_MATCH_RETURN_VOID(data != nullptr);
            auto actxt = reinterpret_cast<AsyncContext *>(data);
            if (actxt->execute && actxt->ctxt->status == napi_ok) {
                actxt->execute();
            }
        },
        [](napi_env env, napi_status status, void *data) {
            NOT_MATCH_RETURN_VOID(data != nullptr);
            auto actxt = reinterpret_cast<AsyncContext *>(data);
            if ((status != napi_ok) && (actxt->ctxt->status == napi_ok)) {
                actxt->ctxt->status = status;
            }
            if ((actxt->complete) && (status == napi_ok) && (actxt->ctxt->status == napi_ok)) {
                actxt->complete(actxt->ctxt->output);
            }
            GenerateOutput(actxt->ctxt.get());
            napi_delete_reference(env, actxt->ctxt->callbackRef);
            napi_delete_async_work(env, actxt->work);
            delete actxt;
        }, reinterpret_cast<void *>(aCtx), &aCtx->work);
    auto status = napi_queue_async_work(ctxt->env, aCtx->work);
    if (status != napi_ok) {
        napi_get_undefined(env, &promise);
        delete aCtx;
    }
    return promise;
}

void NapiQueue::SetBusinessError(napi_env env, napi_value *businessError, std::shared_ptr<Error> error)
{
    napi_create_object(env, businessError);
    if (error != nullptr && error->GetCode() != EXCEPTION_INNER) {
        napi_value code = nullptr;
        napi_value msg = nullptr;
        napi_create_int32(env, error->GetCode(), &code);
        napi_create_string_utf8(env, error->GetMessage().c_str(), NAPI_AUTO_LENGTH, &msg);
        napi_set_named_property(env, *businessError, "code", code);
        napi_set_named_property(env, *businessError, "message", msg);
    }
}

void NapiQueue::GenerateOutput(ContextBase *ctxt)
{
    napi_value result[RESULT_ALL] = { nullptr };
    if (ctxt->status == napi_ok) {
        napi_get_undefined(ctxt->env, &result[RESULT_ERROR]);
        if (ctxt->output == nullptr) {
            napi_get_undefined(ctxt->env, &ctxt->output);
        }
        result[RESULT_DATA] = ctxt->output;
    } else {
        napi_value businessError = nullptr;
        SetBusinessError(ctxt->env, &businessError, ctxt->error);
        result[RESULT_ERROR] = businessError;
        napi_get_undefined(ctxt->env, &result[RESULT_DATA]);
    }
    if (ctxt->deferred != nullptr) {
        if (ctxt->status == napi_ok) {
            LOG_DEBUG("deferred promise resolved");
            napi_resolve_deferred(ctxt->env, ctxt->deferred, result[RESULT_DATA]);
        } else {
            LOG_DEBUG("deferred promise rejected");
            napi_reject_deferred(ctxt->env, ctxt->deferred, result[RESULT_ERROR]);
        }
    } else {
        napi_value callback = nullptr;
        napi_get_reference_value(ctxt->env, ctxt->callbackRef, &callback);
        napi_value callbackResult = nullptr;
        LOG_DEBUG("call callback function");
        napi_call_function(ctxt->env, nullptr, callback, RESULT_ALL, result, &callbackResult);
    }
    ctxt->execute = nullptr;
    ctxt->complete = nullptr;
    ctxt->hold.reset(); // release ctxt.
}
} // namespace OHOS::DistributedData
