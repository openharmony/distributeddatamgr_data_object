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
#define LOG_TAG "NapiErrorUtils"

#include "napi_error_utils.h"

namespace OHOS::CollaborationEdit {

void ThrowNapiError(napi_env env, int32_t status, const std::string &errMessage)
{
    if (status == Status::SUCCESS) {
        return;
    }
    LOG_ERROR("ThrowNapiError message: %{public}s", errMessage.c_str());
    std::string jsCode = std::to_string(status);
    napi_throw_error(env, jsCode.c_str(), errMessage.c_str());
}

} // namespace OHOS::CollaborationEdit