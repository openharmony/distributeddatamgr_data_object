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

#include <logger.h>
#include <mutex>
#include <thread>

#include "dev_manager.h"
#include "dms_handler.h"
#include "anonymous.h"
#include "kv_store_delegate_manager.h"
#include "process_communicator_impl.h"
#include "securec.h"
#include "session.h"
#include "softbus_adapter.h"
#include "softbus_bus_center.h"

namespace OHOS {
namespace ObjectStore {
constexpr int32_t HEAD_SIZE = 3;
constexpr int32_t END_SIZE = 3;
constexpr int32_t MIN_SIZE = HEAD_SIZE + END_SIZE + 3;
constexpr const char *REPLACE_CHAIN = "***";
constexpr const char *DEFAULT_ANONYMOUS = "******";
constexpr int32_t SOFTBUS_OK = 0;
constexpr int32_t INVALID_SOCKET_ID = 0;
constexpr size_t TASK_CAPACITY_MAX = 15;
using namespace std;
SoftBusAdapter *AppDataListenerWrap::softBusAdapter_;
std::shared_ptr<SoftBusAdapter> SoftBusAdapter::instance_;

SoftBusAdapter::SoftBusAdapter()
{
    LOG_INFO("begin");
    taskQueue_ = std::make_shared<TaskScheduler>(TASK_CAPACITY_MAX, "data_object");
    AppDataListenerWrap::SetDataHandler(this);

    clientListener_.OnShutdown = AppDataListenerWrap::OnClientShutdown;
    clientListener_.OnBytes = AppDataListenerWrap::OnClientBytesReceived;
    clientListener_.OnMessage = AppDataListenerWrap::OnClientBytesReceived;

    serverListener_.OnBind = AppDataListenerWrap::OnServerBind;
    serverListener_.OnShutdown = AppDataListenerWrap::OnServerShutdown;
    serverListener_.OnBytes = AppDataListenerWrap::OnServerBytesReceived;
    serverListener_.OnMessage = AppDataListenerWrap::OnServerBytesReceived;
}

SoftBusAdapter::~SoftBusAdapter()
{
    if (taskQueue_ != nullptr) {
        taskQueue_->Clean();
        taskQueue_ = nullptr;
    }
    dataCaches_.clear();
    sockets_.clear();
}

Status SoftBusAdapter::StartWatchDeviceChange(
    const AppDeviceStatusChangeListener *observer, __attribute__((unused)) const PipeInfo &pipeInfo)
{
    LOG_INFO("begin");
    if (observer == nullptr) {
        LOG_WARN("observer is null.");
        return Status::ERROR;
    }
    std::lock_guard<std::mutex> lock(deviceChangeMutex_);
    auto result = listeners_.insert(observer);
    if (!result.second) {
        LOG_WARN("Add listener error.");
        return Status::ERROR;
    }
    LOG_INFO("end");
    return Status::SUCCESS;
}

Status SoftBusAdapter::StopWatchDeviceChange(
    const AppDeviceStatusChangeListener *observer, __attribute__((unused)) const PipeInfo &pipeInfo)
{
    LOG_INFO("begin");
    if (observer == nullptr) {
        LOG_WARN("observer is null.");
        return Status::ERROR;
    }
    std::lock_guard<std::mutex> lock(deviceChangeMutex_);
    auto result = listeners_.erase(observer);
    if (result <= 0) {
        return Status::ERROR;
    }
    LOG_INFO("end");
    return Status::SUCCESS;
}

void SoftBusAdapter::NotifyAll(const DeviceInfo &deviceInfo, const DeviceChangeType &type)
{
    std::thread th = std::thread([this, deviceInfo, type]() {
        pthread_setname_np(pthread_self(), "Data_Object_NotifyAll");
        std::vector<const AppDeviceStatusChangeListener *> listeners;
        {
            std::lock_guard<std::mutex> lock(deviceChangeMutex_);
            for (const auto &listener : listeners_) {
                listeners.push_back(listener);
            }
        }
        LOG_DEBUG("high");
        std::string uuid = DevManager::GetInstance()->GetUuidByNodeId(deviceInfo.deviceId);
        LOG_DEBUG("[Notify] to DB from: %{public}s, type:%{public}d", ToBeAnonymous(uuid).c_str(), type);
        UpdateRelationship(deviceInfo.deviceId, type);
        for (const auto &device : listeners) {
            if (device == nullptr) {
                continue;
            }
            if (device->GetChangeLevelType() == ChangeLevelType::HIGH) {
                DeviceInfo di = { uuid, deviceInfo.deviceName, deviceInfo.deviceType };
                device->OnDeviceChanged(di, type);
                break;
            }
        }
        LOG_DEBUG("low");
        for (const auto &device : listeners) {
            if (device == nullptr) {
                continue;
            }
            if (device->GetChangeLevelType() == ChangeLevelType::LOW) {
                DeviceInfo di = { uuid, deviceInfo.deviceName, deviceInfo.deviceType };
                device->OnDeviceChanged(di, DeviceChangeType::DEVICE_OFFLINE);
                device->OnDeviceChanged(di, type);
            }
        }
        LOG_DEBUG("min");
        for (const auto &device : listeners) {
            if (device == nullptr) {
                continue;
            }
            if (device->GetChangeLevelType() == ChangeLevelType::MIN) {
                DeviceInfo di = { uuid, deviceInfo.deviceName, deviceInfo.deviceType };
                device->OnDeviceChanged(di, type);
            }
        }
    });
    th.detach();
}

std::vector<DeviceInfo> SoftBusAdapter::GetDeviceList() const
{
    std::vector<DistributedSchedule::EventNotify> events;
    int32_t res = DistributedSchedule::DmsHandler::GetInstance().GetDSchedEventInfo(
        DistributedSchedule::DMS_COLLABORATION, events);
    if (res != ERR_OK) {
        LOG_ERROR("Get collaboration events failed, error code = %{public}d", res);
        return {};
    }
    DevManager::DetailInfo localDevice = DevManager::GetInstance()->GetLocalDevice();
    std::set<std::string> remoteDevices;
    for (const auto &event : events) {
        if (localDevice.networkId == event.srcNetworkId_) {
            std::string uuid = DevManager::GetInstance()->GetUuidByNodeId(event.dstNetworkId_);
            remoteDevices.insert(uuid);
        } else if (localDevice.networkId == event.dstNetworkId_) {
            std::string uuid = DevManager::GetInstance()->GetUuidByNodeId(event.srcNetworkId_);
            remoteDevices.insert(uuid);
        }
        LOG_DEBUG("Collaboration evnet, srcNetworkId: %{public}s, dstNetworkId: %{public}s",
            Anonymous::Change(event.srcNetworkId_).c_str(), Anonymous::Change(event.dstNetworkId_).c_str());
    }
    std::vector<DeviceInfo> deviceInfos;
    for (const auto &deviceId : remoteDevices) {
        DeviceInfo deviceInfo{ deviceId };
        deviceInfos.push_back(deviceInfo);
    }
    LOG_INFO("Collaboration deivces size:%{public}zu", deviceInfos.size());
    return deviceInfos;
}

bool SoftBusAdapter::IsContinue()
{
    std::vector<DistributedSchedule::EventNotify> events;
    int32_t res =
        DistributedSchedule::DmsHandler::GetInstance().GetDSchedEventInfo(DistributedSchedule::DMS_CONTINUE, events);
    if (res != ERR_OK) {
        LOG_ERROR("Get continue events failed, error code: %{public}d", res);
        return false;
    }
    DevManager::DetailInfo localDevice = DevManager::GetInstance()->GetLocalDevice();
    for (const auto &event : events) {
        if (localDevice.networkId == event.srcNetworkId_ || localDevice.networkId == event.dstNetworkId_) {
            LOG_INFO("Local is continue");
            return true;
        }
    }
    return false;
}

DeviceInfo SoftBusAdapter::GetLocalDevice()
{
    std::lock_guard<std::mutex> lock(localDeviceLock_);
    if (!localInfo_.deviceId.empty()) {
        return localInfo_;
    }

    NodeBasicInfo info;
    int32_t ret = GetLocalNodeDeviceInfo("ohos.objectstore", &info);
    if (ret != SOFTBUS_OK) {
        LOG_ERROR("GetLocalNodeDeviceInfo error");
        return DeviceInfo();
    }
    std::string uuid = DevManager::GetInstance()->GetUuidByNodeId(std::string(info.networkId));
    LOG_DEBUG("[LocalDevice] id:%{private}s, name:%{private}s, type:%{private}d", ToBeAnonymous(uuid).c_str(),
        info.deviceName, info.deviceTypeId);
    localInfo_ = { uuid, std::string(info.deviceName), std::to_string(info.deviceTypeId) };
    return localInfo_;
}

DeviceInfo SoftBusAdapter::GetLocalBasicInfo() const
{
    LOG_DEBUG("begin");
    NodeBasicInfo info;
    int32_t ret = GetLocalNodeDeviceInfo("ohos.objectstore", &info);
    if (ret != SOFTBUS_OK) {
        LOG_ERROR("GetLocalNodeDeviceInfo error");
        return DeviceInfo();
    }
    LOG_DEBUG("[LocalBasicInfo] networkId:%{private}s, name:%{private}s, "
              "type:%{private}d",
        ToBeAnonymous(std::string(info.networkId)).c_str(), info.deviceName, info.deviceTypeId);
    DeviceInfo localInfo = { std::string(info.networkId), std::string(info.deviceName),
        std::to_string(info.deviceTypeId) };
    return localInfo;
}

std::vector<DeviceInfo> SoftBusAdapter::GetRemoteNodesBasicInfo() const
{
    LOG_DEBUG("begin");
    std::vector<DeviceInfo> dis;
    NodeBasicInfo *info = nullptr;
    int32_t infoNum = 0;
    dis.clear();

    int32_t ret = GetAllNodeDeviceInfo("ohos.objectstore", &info, &infoNum);
    if (ret != SOFTBUS_OK) {
        LOG_ERROR("GetAllNodeDeviceInfo error");
        return dis;
    }
    LOG_DEBUG("GetAllNodeDeviceInfo success infoNum=%{public}d", infoNum);

    for (int i = 0; i < infoNum; i++) {
        dis.push_back(
            { std::string(info[i].networkId), std::string(info[i].deviceName), std::to_string(info[i].deviceTypeId) });
    }
    if (info != nullptr) {
        FreeNodeInfo(info);
    }
    return dis;
}

void SoftBusAdapter::UpdateRelationship(const std::string &networkId, const DeviceChangeType &type)
{
    auto uuid = DevManager::GetInstance()->GetUuidByNodeId(networkId);
    lock_guard<mutex> lock(networkMutex_);
    switch (type) {
        case DeviceChangeType::DEVICE_OFFLINE: {
            auto size = this->networkId2Uuid_.erase(networkId);
            if (size == 0) {
                LOG_WARN("not found id:%{public}s.", Anonymous::Change(networkId).c_str());
            }
            break;
        }
        case DeviceChangeType::DEVICE_ONLINE: {
            std::pair<std::string, std::string> value = { networkId, uuid };
            auto res = this->networkId2Uuid_.insert(std::move(value));
            if (!res.second) {
                LOG_WARN("insert failed.");
            }
            break;
        }
        default: {
            LOG_WARN("unknown type.");
            break;
        }
    }
}
std::string SoftBusAdapter::ToNodeID(const std::string &nodeId) const
{
    {
        lock_guard<mutex> lock(networkMutex_);
        for (auto const &e : networkId2Uuid_) {
            if (nodeId == e.second) { // id is uuid
                return e.first;
            }
        }
    }
    std::vector<DeviceInfo> devices;
    NodeBasicInfo *info = nullptr;
    int32_t infoNum = 0;
    std::string networkId;
    int32_t ret = GetAllNodeDeviceInfo("ohos.objectstore", &info, &infoNum);
    if (ret == SOFTBUS_OK) {
        lock_guard<mutex> lock(networkMutex_);
        for (int i = 0; i < infoNum; i++) {
            if (networkId2Uuid_.find(info[i].networkId) != networkId2Uuid_.end()) {
                continue;
            }
            auto uuid = DevManager::GetInstance()->GetUuidByNodeId(std::string(info[i].networkId));
            networkId2Uuid_.insert({ info[i].networkId, uuid });
            if (uuid == nodeId) {
                networkId = info[i].networkId;
            }
        }
    }
    if (info != nullptr) {
        FreeNodeInfo(info);
    }
    return networkId;
}

std::string SoftBusAdapter::ToBeAnonymous(const std::string &name)
{
    if (name.length() <= HEAD_SIZE) {
        return DEFAULT_ANONYMOUS;
    }

    if (name.length() < MIN_SIZE) {
        return (name.substr(0, HEAD_SIZE) + REPLACE_CHAIN);
    }

    return (name.substr(0, HEAD_SIZE) + REPLACE_CHAIN + name.substr(name.length() - END_SIZE, END_SIZE));
}

std::shared_ptr<SoftBusAdapter> SoftBusAdapter::GetInstance()
{
    static std::once_flag onceFlag;
    std::call_once(onceFlag, [&] { instance_ = std::make_shared<SoftBusAdapter>(); });
    return instance_;
}

Status SoftBusAdapter::StartWatchDataChange(const AppDataChangeListener *observer, const PipeInfo &pipeInfo)
{
    LOG_DEBUG("begin");
    if (observer == nullptr) {
        return Status::INVALID_ARGUMENT;
    }
    lock_guard<mutex> lock(dataChangeMutex_);
    auto it = dataChangeListeners_.find(pipeInfo.pipeId);
    if (it != dataChangeListeners_.end()) {
        LOG_WARN("Add listener error or repeated adding.");
        return Status::ERROR;
    }
    LOG_DEBUG("current appid %{public}s", pipeInfo.pipeId.c_str());
    dataChangeListeners_.insert({ pipeInfo.pipeId, observer });
    return Status::SUCCESS;
}

Status SoftBusAdapter::StopWatchDataChange(
    __attribute__((unused)) const AppDataChangeListener *observer, const PipeInfo &pipeInfo)
{
    LOG_DEBUG("begin");
    lock_guard<mutex> lock(dataChangeMutex_);
    if (dataChangeListeners_.erase(pipeInfo.pipeId)) {
        return Status::SUCCESS;
    }
    LOG_WARN("stop data observer error, pipeInfo:%{public}s", pipeInfo.pipeId.c_str());
    return Status::ERROR;
}

Status SoftBusAdapter::SendData(const PipeInfo &pipeInfo, const DeviceId &deviceId, const DataInfo &dataInfo,
    uint32_t totalLength, const MessageInfo &info)
{
    lock_guard<mutex> lock(sendDataMutex_);
    auto result = CacheData(deviceId.deviceId, dataInfo);
    if (result != Status::SUCCESS) {
        return result;
    }
    int clientSocket = GetSocket(pipeInfo, deviceId);
    if (clientSocket == INVALID_SOCKET_ID) {
        return Status::ERROR;
    }
    DoSend();
    return Status::SUCCESS;
}

Status SoftBusAdapter::CacheData(const std::string &deviceId, const DataInfo &dataInfo)
{
    uint8_t *data = new uint8_t[dataInfo.length];
    if (data == nullptr) {
        LOG_ERROR("[SendData] create buffer fail.");
        return Status::INVALID_ARGUMENT;
    }
    if (memcpy_s(data, dataInfo.length, dataInfo.data, dataInfo.length) != EOK) {
        LOG_ERROR("[SendData] memcpy_s fail.");
        delete[] data;
        return Status::INVALID_ARGUMENT;
    }
    BytesMsg bytesMsg = { data, dataInfo.length };
    lock_guard<mutex> lock(deviceDataLock_);
    auto deviceIdData = dataCaches_.find(deviceId);
    if (deviceIdData == dataCaches_.end()) {
        dataCaches_[deviceId] = { bytesMsg };
    } else {
        deviceIdData->second.push_back(bytesMsg);
        if (deviceIdData->second.size() > VECTOR_SIZE_THRESHOLD) {
            deviceIdData->second.erase(deviceIdData->second.begin());
        }
    }
    return Status::SUCCESS;
}

int SoftBusAdapter::GetSocket(const PipeInfo &pipeInfo, const DeviceId &deviceId)
{
    lock_guard<mutex> lock(socketLock_);
    auto it = sockets_.find(deviceId.deviceId);
    if (it != sockets_.end()) {
        return it->second;
    }
    int socketId = CreateClientSocket(pipeInfo, deviceId);
    if (socketId == INVALID_SOCKET_ID) {
        return INVALID_SOCKET_ID;
    }
    sockets_[deviceId.deviceId] = socketId;
    return socketId;
}

int SoftBusAdapter::CreateClientSocket(const PipeInfo &pipeInfo, const DeviceId &deviceId)
{
        SocketInfo socketInfo;
        socketInfo.name = const_cast<char *>(pipeInfo.pipeId.c_str());
        socketInfo.peerName = const_cast<char *>(pipeInfo.pipeId.c_str());
        std::string networkId = ToNodeID(deviceId.deviceId);
        socketInfo.peerNetworkId = const_cast<char *>(networkId.c_str());
        
        std::string pkgName = "ohos.objectstore";
        socketInfo.pkgName = pkgName.data();
        socketInfo.dataType = DATA_TYPE_BYTES;
        int socketId = Socket(socketInfo);
        if (socketId <= 0) {
            LOG_ERROR("Create socket failed, error code: %{public}d, name:%{public}s", socketId, socketInfo.name);
            return INVALID_SOCKET_ID;
        }
        int32_t res = Bind(socketId, Qos, QOS_COUNT, &clientListener_);
        if (res != 0) {
            LOG_ERROR("Bind failed, error code: %{public}d, peerName: %{public}s, peerNetworkId: %{public}s",
                res, socketInfo.peerName, Anonymous::Change(networkId).c_str());
            Shutdown(socketId);
            return INVALID_SOCKET_ID;
        }
        return socketId;
}

void SoftBusAdapter::DoSend()
{
    auto task([this]() {
        lock_guard<mutex> lock(socketLock_);
        for (auto &it : sockets_) {
            if (it.second <= INVALID_SOCKET_ID) {
                continue;
            }
            lock_guard<mutex> lock(deviceDataLock_);
            auto dataCache = dataCaches_.find(it.first);
            if (dataCache == dataCaches_.end()) {
                continue;
            }
            for (auto msg = dataCache->second.begin(); msg != dataCache->second.end();) {
                auto &byteMsg = *msg;
                int32_t ret = SendBytes(it.second, byteMsg.ptr, byteMsg.size);
                if (ret != SOFTBUS_OK) {
                    LOG_ERROR("[SendBytes] to %{public}d failed, ret:%{public}d.", it.second, ret);
                }
                LOG_DEBUG("[SendBytes] delete msgPtr.");
                delete[] byteMsg.ptr;
                msg = dataCache->second.erase(msg);
            }
        }
    });
    taskQueue_->Execute(std::move(task));
}

void SoftBusAdapter::OnClientShutdown(int32_t socket)
{
    lock_guard<mutex> lock(socketLock_);
    for (auto iter = sockets_.begin(); iter != sockets_.end();) {
        if (iter->second == socket) {
            iter = sockets_.erase(iter);
        } else {
            iter++;
        }
    }
}

bool SoftBusAdapter::IsSameStartedOnPeer(
    const struct PipeInfo &pipeInfo, __attribute__((unused)) const struct DeviceId &peer)
{
    int socket = GetSocket(pipeInfo, peer);
    return socket != INVALID_SOCKET_ID;
}

int SoftBusAdapter::CreateSessionServerAdapter(const std::string &sessionName)
{
    SocketInfo socketInfo;
    socketInfo.name = const_cast<char *>(sessionName.c_str());
    std::string pkgName = "ohos.objectstore";
    socketInfo.pkgName = pkgName.data();
    socketServer_ = Socket(socketInfo);
    if (socketServer_ <= 0) {
        LOG_ERROR("Create socket failed, error code: %{public}d, name:%{public}s", socketServer_, socketInfo.name);
        return static_cast<int>(Status::ERROR);
    }
    int res = Listen(socketServer_, Qos, QOS_COUNT, &serverListener_);
    if (res != SOFTBUS_OK) {
        LOG_ERROR("Listen socket failed, error code: %{public}d, socket:%{public}d", res, socketServer_);
        return static_cast<int>(Status::ERROR);
    }
    return SOFTBUS_OK;
}

int SoftBusAdapter::RemoveSessionServerAdapter(const std::string &sessionName) const
{
    Shutdown(socketServer_);
    LOG_INFO("Shutdown server socket: %{public}d", socketServer_);
    return 0;
}

void SoftBusAdapter::NotifyDataListeners(
    const uint8_t *ptr, const int size, const std::string &deviceId, const PipeInfo &pipeInfo)
{
    lock_guard<mutex> lock(dataChangeMutex_);
    auto it = dataChangeListeners_.find(pipeInfo.pipeId);
    if (it != dataChangeListeners_.end()) {
        LOG_DEBUG("ready to notify, pipeName:%{public}s, deviceId:%{public}s.", pipeInfo.pipeId.c_str(),
            ToBeAnonymous(deviceId).c_str());
        DeviceInfo deviceInfo = { deviceId, "", "" };
        it->second->OnMessage(deviceInfo, ptr, size, pipeInfo);
        return;
    }
    LOG_WARN("no listener %{public}s.", pipeInfo.pipeId.c_str());
}

bool SoftBusAdapter::GetPeerSocketInfo(int32_t socket, PeerSocketInfo &info)
{
    auto it = peerSocketInfos_.Find(socket);
    if (it.first) {
        info = it.second;
        return true;
    }
    return false;
}

void SoftBusAdapter::OnBind(int32_t socket, PeerSocketInfo info)
{
    peerSocketInfos_.Insert(socket, info);
}

void SoftBusAdapter::OnServerShutdown(int32_t socket)
{
    peerSocketInfos_.Erase(socket);
}

void AppDataListenerWrap::SetDataHandler(SoftBusAdapter *handler)
{
    softBusAdapter_ = handler;
}

void AppDataListenerWrap::OnClientShutdown(int32_t socket, ShutdownReason reason)
{
    softBusAdapter_->OnClientShutdown(socket);
    LOG_INFO("Client socket shutdown, socket: %{public}d, reason: %{public}d", socket, reason);
}

void AppDataListenerWrap::OnClientBytesReceived(int32_t socket, const void *data, uint32_t dataLen) {}

void AppDataListenerWrap::OnServerBind(int32_t socket, PeerSocketInfo info)
{
    softBusAdapter_->OnBind(socket, info);
    LOG_INFO("Server on bind, socket: %{public}d, peer networkId: %{public}s", socket,
        SoftBusAdapter::ToBeAnonymous(info.networkId).c_str());
}

void AppDataListenerWrap::OnServerShutdown(int32_t socket, ShutdownReason reason)
{
    softBusAdapter_->OnServerShutdown(socket);
    LOG_INFO("Server socket shutdown, socket: %{public}d, reason: %{public}d", socket, reason);
}

void AppDataListenerWrap::OnServerBytesReceived(int32_t socket, const void *data, uint32_t dataLen)
{
    PeerSocketInfo info;
    if (!softBusAdapter_->GetPeerSocketInfo(socket, info)) {
        LOG_ERROR("Get peer socket info failed, socket: %{public}d", socket);
        return;
    };
    LOG_DEBUG("Server receive bytes, socket: %{public}d, networkId: %{public}s, dataLen: %{public}u", socket,
        Anonymous::Change(info.networkId).c_str(), dataLen);
    std::string peerDevUuid = DevManager::GetInstance()->GetUuidByNodeId(std::string(info.networkId));
    NotifyDataListeners(reinterpret_cast<const uint8_t *>(data), dataLen, peerDevUuid, { info.name });
}

void AppDataListenerWrap::NotifyDataListeners(const uint8_t *ptr, const int size, const std::string &deviceId,
    const PipeInfo &pipeInfo)
{
    softBusAdapter_->NotifyDataListeners(ptr, size, deviceId, pipeInfo);
}
} // namespace ObjectStore
} // namespace OHOS
