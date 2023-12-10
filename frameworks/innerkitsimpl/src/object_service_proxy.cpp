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

#define LOG_TAG "ObjectServiceProxy"
#include "object_service_proxy.h"

#include <logger.h>
#include "log_print.h"
#include "objectstore_errors.h"
#include "object_types_util.h"

namespace OHOS::DistributedObject {
using namespace ObjectStore;

using ObjectCode = ObjectStoreService::ObjectServiceInterfaceCode;

ObjectServiceProxy::ObjectServiceProxy(const sptr<IRemoteObject> &impl) : IRemoteProxy<IObjectService>(impl)
{
    ZLOGI("init service proxy.");
}

int32_t ObjectServiceProxy::ObjectStoreSave(const std::string &bundleName, const std::string &sessionId,
    const std::string &deviceId, const std::map<std::string, std::vector<uint8_t>> &objectData,
    sptr<IRemoteObject> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(ObjectServiceProxy::GetDescriptor())) {
        ZLOGE("write descriptor failed");
        return ERR_IPC;
    }
    if (!ITypesUtil::Marshal(data, bundleName, sessionId, deviceId, objectData, callback)) {
        ZLOGE("Marshalling failed, bundleName = %{public}s", bundleName.c_str());
        return ERR_IPC;
    }
    MessageParcel reply;
    MessageOption mo { MessageOption::TF_SYNC };
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        LOG_ERROR("ObjectStoreSave remoteObject is nullptr.");
        return ERR_IPC;
    }
    int32_t error = remoteObject->SendRequest(static_cast<uint32_t>(ObjectCode::OBJECTSTORE_SAVE), data, reply, mo);
    if (error != 0) {
        ZLOGE("SendRequest returned %d", error);
        return ERR_IPC;
    }
    return reply.ReadInt32();
}

int32_t ObjectServiceProxy::OnAssetChanged(const std::string &bundleName, const std::string &sessionId,
    const std::string &deviceId, const Asset &assetValue)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(ObjectServiceProxy::GetDescriptor())) {
        ZLOGE("write descriptor failed");
        return ERR_IPC;
    }
    if (!ITypesUtil::Marshal(data, bundleName, sessionId, deviceId)) {
        ZLOGE("Marshalling failed, bundleName = %{public}s", bundleName.c_str());
        return ERR_IPC;
    }
    if (!ITypesUtil::Marshalling(assetValue, data)) {
        ZLOGE("Marshalling failed assetValue");
        return ERR_IPC;
    }
    MessageParcel reply;
    MessageOption mo { MessageOption::TF_SYNC };
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        LOG_ERROR("OnAssetChanged remoteObject is nullptr.");
        return ERR_IPC;
    }
    int32_t error = remoteObject->SendRequest(
        static_cast<uint32_t>(ObjectCode::OBJECTSTORE_ON_ASSET_CHANGED), data, reply, mo);
    if (error != 0) {
        ZLOGE("SendRequest returned %d", error);
        return ERR_IPC;
    }
    return reply.ReadInt32();
}

int32_t ObjectServiceProxy::ObjectStoreRevokeSave(
    const std::string &bundleName, const std::string &sessionId, sptr<IRemoteObject> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(ObjectServiceProxy::GetDescriptor())) {
        ZLOGE("write descriptor failed");
        return ERR_IPC;
    }

    if (!ITypesUtil::Marshal(data, bundleName, sessionId, callback)) {
        ZLOGE("Marshalling failed, bundleName = %{public}s", bundleName.c_str());
        return ERR_IPC;
    }

    MessageParcel reply;
    MessageOption mo { MessageOption::TF_SYNC };
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        LOG_ERROR("ObjectStoreRevokeSave remoteObject is nullptr.");
        return ERR_IPC;
    }
    int32_t error =
        remoteObject->SendRequest(static_cast<uint32_t>(ObjectCode::OBJECTSTORE_REVOKE_SAVE), data, reply, mo);
    if (error != 0) {
        ZLOGE("SendRequest returned %d", error);
        return ERR_IPC;
    }
    return reply.ReadInt32();
}

int32_t ObjectServiceProxy::ObjectStoreRetrieve(
    const std::string &bundleName, const std::string &sessionId, sptr<IRemoteObject> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(ObjectServiceProxy::GetDescriptor())) {
        ZLOGE("write descriptor failed");
        return ERR_IPC;
    }

    if (!ITypesUtil::Marshal(data, bundleName, sessionId, callback)) {
        ZLOGE("Marshalling failed, bundleName = %{public}s", bundleName.c_str());
        return ERR_IPC;
    }

    MessageParcel reply;
    MessageOption mo { MessageOption::TF_SYNC };
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        LOG_ERROR("ObjectStoreRetrieve remoteObject is nullptr.");
        return ERR_IPC;
    }
    int32_t error =
        remoteObject->SendRequest(static_cast<uint32_t>(ObjectCode::OBJECTSTORE_RETRIEVE), data, reply, mo);
    if (error != 0) {
        ZLOGE("SendRequest returned %d", error);
        return ERR_IPC;
    }
    return reply.ReadInt32();
}

int32_t ObjectServiceProxy::RegisterDataObserver(const std::string &bundleName,
                                                 const std::string &sessionId, sptr<IRemoteObject> callback)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(ObjectServiceProxy::GetDescriptor())) {
        ZLOGE("write descriptor failed");
        return ERR_IPC;
    }
    
    if (!ITypesUtil::Marshal(data, bundleName, sessionId, callback)) {
        ZLOGE("Marshalling failed, bundleName = %{public}s", bundleName.c_str());
        return ERR_IPC;
    }
    
    MessageParcel reply;
    MessageOption mo { MessageOption::TF_SYNC };
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        LOG_ERROR("RegisterDataObserver remoteObject is nullptr.");
        return ERR_IPC;
    }
    int32_t error =
        remoteObject->SendRequest(static_cast<uint32_t>(ObjectCode::OBJECTSTORE_REGISTER_OBSERVER), data, reply, mo);
    if (error != 0) {
        ZLOGE("SendRequest returned %d", error);
        return ERR_IPC;
    }
    return reply.ReadInt32();
}

int32_t ObjectServiceProxy::UnregisterDataChangeObserver(const std::string &bundleName, const std::string &sessionId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(ObjectServiceProxy::GetDescriptor())) {
        ZLOGE("write descriptor failed");
        return ERR_IPC;
    }
    
    if (!ITypesUtil::Marshal(data, bundleName, sessionId)) {
        ZLOGE("Marshalling failed, bundleName = %{public}s", bundleName.c_str());
        return ERR_IPC;
    }
    
    MessageParcel reply;
    MessageOption mo { MessageOption::TF_SYNC };
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        LOG_ERROR("UnregisterDataChangeObserver remoteObject is nullptr.");
        return ERR_IPC;
    }
    int32_t error =
        remoteObject->SendRequest(static_cast<uint32_t>(ObjectCode::OBJECTSTORE_UNREGISTER_OBSERVER), data, reply, mo);
    if (error != 0) {
        ZLOGE("SendRequest returned %d", error);
        return ERR_IPC;
    }
    return reply.ReadInt32();
}

int32_t ObjectServiceProxy::BindAssetStore(const std::string &bundleName, const std::string &sessionId,
    Asset &asset, AssetBindInfo &bindInfo)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(ObjectServiceProxy::GetDescriptor())) {
        ZLOGE("write descriptor failed");
        return ERR_IPC;
    }
    
    if (!ITypesUtil::Marshal(data, bundleName, sessionId, asset, bindInfo)) {
        ZLOGE("Marshalling failed, bundleName = %{public}s", bundleName.c_str());
        return ERR_IPC;
    }

    MessageParcel reply;
    MessageOption mo { MessageOption::TF_SYNC };
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        LOG_ERROR("BindAssetStore remoteObject is nullptr.");
        return ERR_IPC;
    }
    int32_t error =
        remoteObject->SendRequest(static_cast<uint32_t>(ObjectCode::OBJECTSTORE_BIND_ASSET_STORE), data, reply, mo);
    if (error != 0) {
        ZLOGE("SendRequest returned %d", error);
        return ERR_IPC;
    }
    return reply.ReadInt32();
}

int32_t ObjectServiceProxy::DeleteSnapshot(const std::string &bundleName, const std::string &sessionId)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(ObjectServiceProxy::GetDescriptor())) {
        ZLOGE("write descriptor failed");
        return ERR_IPC;
    }

    if (!ITypesUtil::Marshal(data, bundleName, sessionId)) {
        ZLOGE("Marshalling failed, bundleName = %{public}s", bundleName.c_str());
        return ERR_IPC;
    }

    MessageParcel reply;
    MessageOption mo { MessageOption::TF_SYNC };
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        LOG_ERROR("DeleteSnapshot remoteObject is nullptr.");
        return ERR_IPC;
    }
    int32_t error =
        remoteObject->SendRequest(static_cast<uint32_t>(ObjectCode::OBJECTSTORE_DELETE_SNAPSHOT), data, reply, mo);
    if (error != 0) {
        ZLOGE("SendRequest returned %d", error);
        return ERR_IPC;
    }
    return reply.ReadInt32();
}
} // namespace OHOS::DistributedObject
