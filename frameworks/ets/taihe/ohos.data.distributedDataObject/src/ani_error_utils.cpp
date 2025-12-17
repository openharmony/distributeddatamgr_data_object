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
#include <algorithm>

#include "taihe/runtime.hpp"
#include "ani_error_utils.h"
#include "logger.h"

namespace AniErrorUtils {
using namespace OHOS::ObjectStore;

constexpr char CLASS_NAME_BUSINESSERROR[] = "@ohos.base.BusinessError";

void ThrowError(const char* message)
{
    if (message == nullptr) {
        return;
    }
    std::string errMsg(message);
    taihe::set_error(errMsg);
}

void ThrowError(int32_t code, const char* message)
{
    if (message == nullptr) {
        return;
    }
    std::string errMsg(message);
    taihe::set_business_error(code, errMsg);
}

void ThrowError(int32_t code, const std::string &message)
{
    taihe::set_business_error(code, message);
}

ani_object ToBusinessError(ani_env *env, int32_t code, const std::string &message)
{
    if (env == nullptr) {
        return {};
    }
    ani_class cls {};
    ASSERT_WITH_RET_LOG(env->FindClass(CLASS_NAME_BUSINESSERROR, &cls) == ANI_OK, nullptr,
        "find class %{public}s failed", CLASS_NAME_BUSINESSERROR);
    ani_method ctor {};
    ASSERT_WITH_RET_LOG(env->Class_FindMethod(cls, "<ctor>", ":", &ctor) == ANI_OK, nullptr,
        "find method BusinessError constructor failed");
    ani_object error {};
    ASSERT_WITH_RET_LOG(env->Object_New(cls, ctor, &error) == ANI_OK, nullptr,
        "new object %{public}s failed", CLASS_NAME_BUSINESSERROR);
    ASSERT_WITH_RET_LOG(
        env->Object_SetPropertyByName_Int(error, "code", static_cast<ani_int>(code)) == ANI_OK, nullptr,
        "set property BusinessError.code failed");
    if (message.size() > 0) {
        ani_string messageRef {};
        ASSERT_WITH_RET_LOG(env->String_NewUTF8(message.c_str(), message.size(), &messageRef) == ANI_OK, nullptr,
            "new message string failed");
        ASSERT_WITH_RET_LOG(
            env->Object_SetPropertyByName_Ref(error, "message", static_cast<ani_ref>(messageRef)) == ANI_OK, nullptr,
            "set property BusinessError.message failed");
    }
    return error;
}

} // namespace AniErrorUtils