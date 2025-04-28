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

#include "object_error.h"

namespace OHOS {
namespace ObjectStore {
static constexpr int EXCEPTION_DEVICE_NOT_SUPPORT = 801;
static constexpr int EXCEPTION_PARAMETER_CHECK = 401;
static constexpr int EXCEPTION_NO_PERMISSION = 201;
static constexpr int EXCEPTION_DB_EXIST = 15400001;

std::string ParametersType::GetMessage()
{
    return "Parameter error. The type of '" + name + "' must be '" + wantType + "'.";
}

int ParametersType::GetCode()
{
    return EXCEPTION_PARAMETER_CHECK;
}

std::string ParametersNum::GetMessage()
{
    return "Parameter error. Need " + wantNum + " parameters!";
}

int ParametersNum::GetCode()
{
    return EXCEPTION_PARAMETER_CHECK;
}

std::string PermissionError::GetMessage()
{
    return "Permission verification failed. An attempt was made to join session forbidden by permission: "
           "ohos.permission.DISTRIBUTED_DATASYNC.";
}

int DatabaseError::GetCode()
{
    return EXCEPTION_DB_EXIST;
}

std::string DatabaseError::GetMessage()
{
    return "Failed to create the in-memory database. One possible reason is that the distributed object has "
        "already been created";
}

int PermissionError::GetCode()
{
    return EXCEPTION_NO_PERMISSION;
}

std::string InnerError::GetMessage()
{
    return "";
}

int InnerError::GetCode()
{
    return EXCEPTION_INNER;
}

std::string DeviceNotSupportedError::GetMessage()
{
    return "Capability not supported.";
}

int DeviceNotSupportedError::GetCode()
{
    return EXCEPTION_DEVICE_NOT_SUPPORT;
}
} // namespace ObjectStore
} // namespace OHOS