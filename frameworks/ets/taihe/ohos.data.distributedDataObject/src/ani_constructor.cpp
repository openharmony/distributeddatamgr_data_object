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

#include "ohos.data.commonType.ani.hpp"
#include "ohos.data.distributedDataObject.ani.hpp"
#if __has_include(<ani.h>)
#include <ani.h>
#elif __has_include(<ani/ani.h>)
#include <ani/ani.h>
#else
#error "ani.h not found. Please ensure the Ani SDK is correctly installed."
#endif
#include "logger.h"
using namespace OHOS::ObjectStore;

ANI_EXPORT ani_status ANI_Constructor(ani_vm *vm, uint32_t *result)
{
    if (vm == nullptr) {
        LOG_ERROR("vm is null");
        return ANI_ERROR;
    }
    ani_env *env;
    ani_status status = vm->GetEnv(ANI_VERSION_1, &env);
    if (status != ANI_OK) {
        LOG_ERROR("Error from GetEnv, status=%{public}d", static_cast<int>(status));
        return ANI_ERROR;
    }
    status = ohos::data::distributedDataObject::ANIRegister(env);
    if (status != ANI_OK) {
        LOG_ERROR("Error from ohos::data::distributedDataObject::ANIRegister, status=%{public}d",
            static_cast<int>(status));
        status = ANI_ERROR;
    }
    *result = ANI_VERSION_1;
    return status;
}
