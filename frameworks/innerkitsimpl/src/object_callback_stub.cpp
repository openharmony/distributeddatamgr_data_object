/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#define LOG_TAG "ObjectCallbackStub"
#include "object_callback_stub.h"
#include <ipc_skeleton.h>
#include <logger.h>
#include "itypes_util.h"
#include "log_print.h"

namespace OHOS {
namespace DistributedObject {
int ObjectSaveCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    ZLOGI("code:%{public}u, callingPid:%{public}d", code, IPCSkeleton::GetCallingPid());
    auto localDescriptor = GetDescriptor();
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (remoteDescriptor != localDescriptor) {
        ZLOGE("interface token is not equal");
        return -1;
    }
    if (code == COMPLETED) {
        std::map<std::string, int32_t> results;
        if (!ITypesUtil::Unmarshal(data, results)) {
            ZLOGE("Unmarshalling failed");
            return -1;
        }
        Completed(results);
        return 0;
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int ObjectRevokeSaveCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    ZLOGI("code:%{public}u, callingPid:%{public}d", code, IPCSkeleton::GetCallingPid());
    auto localDescriptor = GetDescriptor();
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (remoteDescriptor != localDescriptor) {
        ZLOGE("interface token is not equal");
        return -1;
    }
    if (code == COMPLETED) {
        int32_t status;
        if (!ITypesUtil::Unmarshal(data, status)) {
            ZLOGE("write descriptor failed");
            return -1;
        }
        Completed(status);
        return 0;
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int ObjectRetrieveCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    ZLOGI("code:%{public}u, callingPid:%{public}d", code, IPCSkeleton::GetCallingPid());
    auto localDescriptor = GetDescriptor();
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (remoteDescriptor != localDescriptor) {
        ZLOGE("interface token is not equal");
        return -1;
    }
    if (code == COMPLETED) {
        std::map<std::string, std::vector<uint8_t>> results;
        if (!ITypesUtil::Unmarshal(data, results)) {
            ZLOGE("write descriptor failed");
            return -1;
        }
        Completed(results);
        return 0;
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}

int ObjectChangeCallbackStub::OnRemoteRequest(
    uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option)
{
    ZLOGI("code:%{public}u, callingPid:%{public}d", code, IPCSkeleton::GetCallingPid());
    auto localDescriptor = GetDescriptor();
    auto remoteDescriptor = data.ReadInterfaceToken();
    if (remoteDescriptor != localDescriptor) {
        ZLOGE("interface token is not equal");
        return -1;
    }
    if (code == COMPLETED) {
        std::map<std::string, std::vector<uint8_t>> results;
        if (!ITypesUtil::Unmarshal(data, results)) {
            ZLOGE("write descriptor failed");
            return -1;
        }
        Completed(results);
        return 0;
    }
    return IPCObjectStub::OnRemoteRequest(code, data, reply, option);
}
} // namespace DistributedObject
} // namespace OHOS
