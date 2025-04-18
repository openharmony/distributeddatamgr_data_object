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
#ifndef NAPI_QUEUE_H
#define NAPI_QUEUE_H

#include "napi/native_node_api.h"
#include "object_error.h"

namespace OHOS::ObjectStore {
using NapiCbInfoParser = std::function<void(size_t argc, napi_value *argv)>;
using NapiAsyncExecute = std::function<void(void)>;
using NapiAsyncComplete = std::function<void(napi_value &)>;

struct ContextBase {
    virtual ~ContextBase();
    void GetCbInfo(
        napi_env env, napi_callback_info info, NapiCbInfoParser parse = NapiCbInfoParser(), bool sync = false);

    inline void GetCbInfoSync(napi_env env, napi_callback_info info, NapiCbInfoParser parse = NapiCbInfoParser())
    {
        /* sync = true, means no callback, not AsyncWork. */
        GetCbInfo(env, info, parse, true);
    }

    void SetError(std::shared_ptr<Error> err)
    {
        error = err;
    }

    napi_env env = nullptr;
    napi_value output = nullptr;
    napi_status status = napi_invalid_arg;
    std::string message;
    std::shared_ptr<Error> error;
    napi_value self = nullptr;
    void *native = nullptr;

private:
    napi_deferred deferred = nullptr;
    napi_ref callbackRef = nullptr;
    napi_ref selfRef = nullptr;

    NapiAsyncExecute execute = nullptr;
    NapiAsyncComplete complete = nullptr;
    std::shared_ptr<ContextBase> hold; /* cross thread data */

    friend class NapiQueue;
};

/* check condition related to argc/argv, return and logging. */
#define INVALID_ARGS_RETURN_ERROR(ctxt, condition, message, err)    \
    do {                                                         \
        if (!(condition)) {                                      \
            (ctxt)->status = napi_invalid_arg;                   \
            (ctxt)->error = err;                                 \
            LOG_ERROR("test (" #condition ") failed: " message); \
            return;                                              \
        }                                                        \
    } while (0)

#define INVALID_STATUS_RETURN_ERROR(ctxt, error)                           \
    do {                                                                \
        if ((ctxt)->status != napi_ok) {                                \
            (ctxt)->message = error;                                    \
            LOG_ERROR("test (ctxt->status == napi_ok) failed: " error); \
            return;                                                     \
        }                                                               \
    } while (0)

class NapiQueue {
public:
    static napi_value AsyncWork(napi_env env, std::shared_ptr<ContextBase> contextBase, const std::string &name,
        NapiAsyncExecute execute = NapiAsyncExecute(), NapiAsyncComplete complete = NapiAsyncComplete());
    static void SetBusinessError(napi_env env, napi_value *businessError, std::shared_ptr<Error> error);

private:
    enum {
        /* AsyncCallback / Promise output result index  */
        RESULT_ERROR = 0,
        RESULT_DATA = 1,
        RESULT_ALL = 2
    };
    struct AsyncContext {
        std::shared_ptr<ContextBase> ctxt;
        NapiAsyncExecute execute = nullptr;
        NapiAsyncComplete complete = nullptr;
        napi_deferred deferred = nullptr;
        napi_async_work work = nullptr;
    };
    static void GenerateOutput(ContextBase *ctxt);
};
} // namespace OHOS::ObjectStore
#endif // OHOS_NAPI_QUEUE_H
