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

#include "client_adaptor.h"

#include <thread>

#include "iservice_registry.h"
#include "itypes_util.h"
#include "logger.h"
#include "objectstore_errors.h"

namespace OHOS::ObjectStore {
std::shared_ptr<ObjectStoreDataServiceProxy> ClientAdaptor::distributedDataMgr_ = nullptr;
std::mutex ClientAdaptor::mutex_;

using KvStoreCode = OHOS::DistributedObject::ObjectStoreService::KvStoreServiceInterfaceCode;

sptr<OHOS::DistributedObject::IObjectService> ClientAdaptor::GetObjectService()
{
    std::lock_guard<decltype(mutex_)> lockGuard(mutex_);
    if (distributedDataMgr_ == nullptr) {
        distributedDataMgr_ = GetDistributedDataManager();
    }
    if (distributedDataMgr_ == nullptr) {
        LOG_ERROR("get distributed data manager failed");
        return nullptr;
    }

    auto remote = distributedDataMgr_->GetFeatureInterface("data_object");
    if (remote == nullptr) {
        LOG_ERROR("get object service failed");
        return nullptr;
    }
    return iface_cast<DistributedObject::IObjectService>(remote);
}

std::shared_ptr<ObjectStoreDataServiceProxy> ClientAdaptor::GetDistributedDataManager()
{
    int retry = 0;
    while (++retry <= GET_SA_RETRY_TIMES) {
        auto manager = SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (manager == nullptr) {
            LOG_ERROR("get system ability manager failed");
            return nullptr;
        }
        LOG_INFO("get distributed data manager %{public}d", retry);
        auto remoteObject = manager->CheckSystemAbility(DISTRIBUTED_KV_DATA_SERVICE_ABILITY_ID);
        if (remoteObject == nullptr) {
            std::this_thread::sleep_for(std::chrono::seconds(RETRY_INTERVAL));
            continue;
        }
        LOG_INFO("get distributed data manager success");
        sptr<ObjectStoreDataServiceProxy> proxy = new (std::nothrow)ObjectStoreDataServiceProxy(remoteObject);
        if (proxy == nullptr) {
            LOG_ERROR("new ObjectStoreDataServiceProxy fail.");
            return nullptr;
        }
        sptr<ClientAdaptor::ServiceDeathRecipient> deathRecipientPtr = new (std::nothrow)ServiceDeathRecipient();
        if (deathRecipientPtr == nullptr) {
            LOG_ERROR("new deathRecipientPtr fail!");
            return nullptr;
        }
        if (!remoteObject->AddDeathRecipient(deathRecipientPtr)) {
            LOG_ERROR("Add death recipient fail!");
        }
        return std::shared_ptr<ObjectStoreDataServiceProxy>(proxy.GetRefPtr(), [holder = proxy](const auto *) {});
    }

    LOG_ERROR("get distributed data manager failed");
    return nullptr;
}

ClientAdaptor::ServiceDeathRecipient::ServiceDeathRecipient()
{
}

ClientAdaptor::ServiceDeathRecipient::~ServiceDeathRecipient()
{
}

void ClientAdaptor::ServiceDeathRecipient::OnRemoteDied(const wptr<IRemoteObject> &remote)
{
    LOG_WARN("DistributedDataService die!");
    std::lock_guard<decltype(mutex_)> lockGuard(mutex_);
    distributedDataMgr_ = nullptr;
}

uint32_t ClientAdaptor::RegisterClientDeathListener(const std::string &appId, sptr<IRemoteObject> remoteObject)
{
    std::lock_guard<decltype(mutex_)> lockGuard(mutex_);
    if (distributedDataMgr_ == nullptr) {
        distributedDataMgr_ = GetDistributedDataManager();
    }
    if (distributedDataMgr_ == nullptr) {
        LOG_ERROR("get distributed data manager failed");
        return ERR_EXIST;
    }
    
    auto status = distributedDataMgr_->RegisterClientDeathObserver(appId, remoteObject);
    if (status != SUCCESS) {
        LOG_ERROR("RegisterClientDeathObserver failed");
        return ERR_EXIST;
    }
    return SUCCESS;
}

ObjectStoreDataServiceProxy::ObjectStoreDataServiceProxy(const sptr<IRemoteObject> &impl)
    : IRemoteProxy<DistributedObject::IKvStoreDataService>(impl)
{
    LOG_INFO("init data service proxy.");
}

sptr<IRemoteObject> ObjectStoreDataServiceProxy::GetFeatureInterface(const std::string &name)
{
    MessageParcel data;
    if (!data.WriteInterfaceToken(ObjectStoreDataServiceProxy::GetDescriptor())) {
        LOG_ERROR("write descriptor failed");
        return nullptr;
    }

    if (!ITypesUtil::Marshal(data, name)) {
        LOG_ERROR("write name failed");
        return nullptr;
    }

    MessageParcel reply;
    MessageOption mo { MessageOption::TF_SYNC };
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        LOG_ERROR("SendRequest remote is nullptr.");
        return nullptr;
    }
    int32_t error =
        remote->SendRequest(static_cast<uint32_t>(KvStoreCode::GET_FEATURE_INTERFACE), data, reply, mo);
    if (error != 0) {
        LOG_ERROR("SendRequest returned %{public}d", error);
        return nullptr;
    }

    sptr<IRemoteObject> remoteObject;
    if (!ITypesUtil::Unmarshal(reply, remoteObject)) {
        LOG_ERROR("remote object is nullptr");
        return nullptr;
    }
    return remoteObject;
}

uint32_t ObjectStoreDataServiceProxy::RegisterClientDeathObserver(
    const std::string &appId, sptr<IRemoteObject> observer)
{
    MessageParcel data;
    MessageParcel reply;
    if (!data.WriteInterfaceToken(ObjectStoreDataServiceProxy::GetDescriptor())) {
        LOG_ERROR("write descriptor failed");
        return ERR_IPC;
    }
    if (observer == nullptr) {
        return ERR_INVALID_ARGS;
    }
    if (!ITypesUtil::Marshal(data, appId, observer)) {
        LOG_ERROR("remote observer fail");
        return ERR_IPC;
    }

    MessageOption mo { MessageOption::TF_SYNC };
    sptr<IRemoteObject> remoteObject = Remote();
    if (remoteObject == nullptr) {
        LOG_ERROR("SendRequest remoteObject is nullptr.");
        return ERR_IPC;
    }
    int32_t error =
        remoteObject->SendRequest(static_cast<uint32_t>(KvStoreCode::REGISTERCLIENTDEATHOBSERVER), data, reply, mo);
    if (error != 0) {
        LOG_WARN("failed during IPC. errCode %d", error);
        return ERR_IPC;
    }
    return static_cast<uint32_t>(reply.ReadInt32());
}
}