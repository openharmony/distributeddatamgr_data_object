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

#ifndef JS_COMMON_H
#define JS_COMMON_H
#include "hilog/log.h"
namespace OHOS::ObjectStore {
class Constants {
public:
    static constexpr const char *CHANGE = "change";
    static constexpr const char *STATUS = "status";
};
#define OBJECT_REVT_NOTHING

#define NOT_MATCH_RETURN(condition, res)                \
    do {                                                \
        if (!(condition)) {                             \
            LOG_ERROR("error! %{public}s", #condition); \
            return res;                                 \
        }                                               \
    } while (0)

#define NOT_MATCH_RETURN_VOID(condition)        \
    NOT_MATCH_RETURN(condition, OBJECT_REVT_NOTHING)

#define NOT_MATCH_RETURN_NULL(condition)        \
    NOT_MATCH_RETURN(condition, nullptr)

#define NOT_MATCH_RETURN_FALSE(condition)       \
    NOT_MATCH_RETURN(condition, false)

#define NOT_MATCH_GOTO_ERROR(condition)         \
    {                                                   \
        if (!(condition)) {                             \
            LOG_ERROR("error! %{public}s", #condition); \
            goto ERROR;                                 \
        }                                               \
    }

#define INVALID_API_THROW_ERROR(assertion)                                                                   \
    do {                                                                                                     \
        if (!(assertion)) {                                                                                  \
            std::shared_ptr<DeviceNotSupportedError> apiError = std::make_shared<DeviceNotSupportedError>(); \
            ctxt->SetError(apiError);                                                                        \
            ctxt->status = napi_generic_failure;                                                             \
            return;                                                                                          \
        }                                                                                                    \
    } while (0)

#define INVALID_STATUS_THROW_ERROR(assertion, msg)                                   \
    do {                                                                             \
        if (!(assertion)) {                                                          \
            std::shared_ptr<InnerError> innerError = std::make_shared<InnerError>(); \
            ctxt->SetError(innerError);                                              \
            ctxt->status = napi_generic_failure;                                     \
            ctxt->message = msg;                                                     \
            return;                                                                  \
        }                                                                            \
    } while (0)
} // namespace OHOS::ObjectStore
#endif // JS_COMMON_H