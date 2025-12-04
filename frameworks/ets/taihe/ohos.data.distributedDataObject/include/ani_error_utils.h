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
#ifndef OHOS_ANI_ERROR_UTILS_H
#define OHOS_ANI_ERROR_UTILS_H

#include <string>
#include <optional>
#include "taihe/runtime.hpp"

namespace ani_errorutils {

enum AniError : int32_t {
    AniError_ParameterCheck = 401,
    AniError_ParameterError = 15400002,
    AniError_SessionJoined = 15400003,
};

void ThrowError(const char* message);
void ThrowError(int32_t code, const std::string &message);
ani_object ToBusinessError(ani_env *env, int32_t code, const std::string &message);

#define CHECK_AND_RETURN_RET_LOG(cond, ret, fmt, ...) \
    do {                                              \
        if (!(cond)) {                                \
            LOG_INFO(fmt, ##__VA_ARGS__);           \
            return ret;                               \
        }                                             \
    } while (0)

}  // namespace ani_errorutils
#endif  // OHOS_ANI_ERROR_UTILS_H
