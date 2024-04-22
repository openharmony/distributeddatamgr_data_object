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
constexpr int32_t SOFTBUS_ERR = 1;
constexpr int32_t INVALID_SESSION_ID = -1;
constexpr int32_t SESSION_NAME_SIZE_MAX = 65;
constexpr int32_t DEVICE_ID_SIZE_MAX = 65;
constexpr size_t TASK_CAPACITY_MAX = 15;
constexpr int32_t SELF_SIDE = 1;

using namespace std;
SoftBusAdapter *AppDataListenerWrap::softBusAdapter_;
std::shared_ptr<SoftBusAdapter> SoftBusAdapter::instance_;

SoftBusAdapter::SoftBusAdapter()
{
    LOG_INFO("begin");
    taskQueue_ = std::make_shared<TaskScheduler>(TASK_CAPACITY_MAX, "data_object");
    AppDataListenerWrap::SetDataHandler(this);

    sessionListener_.OnSessionOpened = AppDataListenerWrap::OnSessionOpened;
    sessionListener_.OnSessionClosed = AppDataListenerWrap::OnSessionClosed;
    sessionListener_.OnBytesReceived = AppDataListenerWrap::OnBytesReceived;
    sessionListener_.OnMessageReceived = AppDataListenerWrap::OnMessageReceived;
}

SoftBusAdapter::~SoftBusAdapter()
{
    if (taskQueue_ != nullptr) {
        taskQueue_->Clean();
        taskQueue_ = nullptr;
    }
    dataCaches_.clear();
    sessionIds_.clear();
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
    DistributedSchedule::ContinueInfo continueInfo;
    int32_t result = DistributedSchedule::DmsHandler::GetInstance().GetContinueInfo(continueInfo);
    if (result != 0) {
        LOG_ERROR("GetContinueInfo failed, error code: %{public}d", result);
    }
    if (!continueInfo.srcNetworkId_.empty() && !continueInfo.dstNetworkId_.empty()) {
        DevManager::DetailInfo localDevice = DevManager::GetInstance()->GetLocalDevice();
        LOG_DEBUG("LocalNetworkId: %{public}s. srcNetworkId: %{public}s. dstNetworkId: %{public}s.",
            ToBeAnonymous(localDevice.networkId).c_str(), ToBeAnonymous(continueInfo.srcNetworkId_).c_str(),
            ToBeAnonymous(continueInfo.dstNetworkId_).c_str());
        if (localDevice.networkId == continueInfo.srcNetworkId_
            || localDevice.networkId == continueInfo.dstNetworkId_) {
            LOG_INFO("Local device is continuing.");
            return {};
        }
    }
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
        std::string uuid = DevManager::GetInstance()->GetUuidByNodeId(std::string(info[i].networkId));
        DeviceInfo deviceInfo = { uuid, std::string(info[i].deviceName), std::to_string(info[i].deviceTypeId) };
        dis.push_back(deviceInfo);
    }
    if (info != nullptr) {
        FreeNodeInfo(info);
    }
    return dis;
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

void SoftBusAdapter::UpdateRelationship(const std::string &networkid, const DeviceChangeType &type)
{
    auto uuid = DevManager::GetInstance()->GetUuidByNodeId(networkid);
    lock_guard<mutex> lock(networkMutex_);
    switch (type) {
        case DeviceChangeType::DEVICE_OFFLINE: {
            auto size = this->networkId2Uuid_.erase(networkid);
            if (size == 0) {
                LOG_WARN("not found id:%{public}s.", networkid.c_str());
            }
            break;
        }
        case DeviceChangeType::DEVICE_ONLINE: {
            std::pair<std::string, std::string> value = { networkid, uuid };
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

    LOG_WARN("get the network id from devices.");
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

RecOperate SoftBusAdapter::CalcRecOperate(uint32_t dataSize, RouteType currentRouteType, bool &isP2P) const
{
    if (currentRouteType == RouteType::WIFI_STA) {
        return KEEP;
    }
    if (currentRouteType == RouteType::INVALID_ROUTE_TYPE || currentRouteType == BT_BR || currentRouteType == BT_BLE) {
        if (dataSize > P2P_SIZE_THRESHOLD) {
            isP2P = true;
            return CHANGE_TO_P2P;
        }
    }
    if (currentRouteType == WIFI_P2P) {
        if (dataSize < P2P_SIZE_THRESHOLD * SWITCH_DELAY_FACTOR) {
            return CHANGE_TO_BLUETOOTH;
        }
        isP2P = true;
    }
    return KEEP;
}

SessionAttribute SoftBusAdapter::GetSessionAttribute(bool isP2P)
{
    SessionAttribute attr;
    attr.dataType = TYPE_BYTES;
    // If the dataType is BYTES, the default strategy is wifi_5G > wifi_2.4G > BR, without P2P;
    if (!isP2P) {
        return attr;
    }

    int index = 0;
    attr.linkType[index++] = LINK_TYPE_WIFI_WLAN_5G;
    attr.linkType[index++] = LINK_TYPE_WIFI_WLAN_2G;
    attr.linkType[index++] = LINK_TYPE_WIFI_P2P;
    attr.linkType[index++] = LINK_TYPE_BR;
    attr.linkTypeNum = index;
    return attr;
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

uint32_t SoftBusAdapter::GetSize(const std::string &deviceId)
{
    uint32_t dataSize = 0;
    lock_guard<mutex> lock(deviceDataLock_);
    auto it = dataCaches_.find(deviceId);
    if (it == dataCaches_.end() || it->second.empty()) {
        return dataSize;
    }
    for (const auto &msg : it->second) {
        dataSize += msg.size;
    }
    return dataSize;
}

RouteType SoftBusAdapter::GetRouteType(int sessionId)
{
    RouteType routeType = RouteType::INVALID_ROUTE_TYPE;
    int ret = GetSessionOption(sessionId, SESSION_OPTION_LINK_TYPE, &routeType, sizeof(routeType));
    if (ret != SOFTBUS_OK) {
        LOG_ERROR("getSessionOption failed, ret is %{public}d.", ret);
    }
    return routeType;
}

Status SoftBusAdapter::SendData(const PipeInfo &pipeInfo, const DeviceId &deviceId, const DataInfo &dataInfo,
    uint32_t totalLength, const MessageInfo &info)
{
    lock_guard<mutex> lock(sendDataMutex_);
    auto result = CacheData(deviceId.deviceId, dataInfo);
    if (result != Status::SUCCESS) {
        return result;
    }
    uint32_t dataSize = GetSize(deviceId.deviceId) + totalLength;
    int sessionId = GetSessionId(deviceId.deviceId);
    bool isP2P = false;
    RouteType currentRouteType = GetRouteType(sessionId);
    RecOperate recOperate = CalcRecOperate(dataSize, currentRouteType, isP2P);
    if (recOperate != KEEP && sessionId != INVALID_SESSION_ID) {
        LOG_INFO("close session : %{public}d, currentRouteType : %{public}d, recOperate : %{public}d", sessionId,
            currentRouteType, recOperate);
        CloseSession(sessionId);
    }
    SessionAttribute attr = GetSessionAttribute(isP2P);
    OpenSession(pipeInfo.pipeId.c_str(), pipeInfo.pipeId.c_str(), ToNodeID(deviceId.deviceId).c_str(),
        "GROUP_ID", &attr);
    return Status::SUCCESS;
}

void SoftBusAdapter::CacheSession(int32_t sessionId)
{
    std::string deviceId;
    if (GetDeviceId(sessionId, deviceId) == SOFTBUS_OK) {
        lock_guard<mutex> lock(deviceSessionLock_);
        sessionIds_[deviceId] = sessionId;
    }
}

void SoftBusAdapter::DoSend()
{
    auto task([this]() {
        lock_guard<mutex> lockSession(deviceSessionLock_);
        for (auto &it : sessionIds_) {
            if (it.second == INVALID_SESSION_ID) {
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

int SoftBusAdapter::GetDeviceId(int sessionId, std::string &deviceId)
{
    char peerDevId[DEVICE_ID_SIZE_MAX] = "";
    int ret = GetPeerDeviceId(sessionId, peerDevId, sizeof(peerDevId));
    if (ret != SOFTBUS_OK) {
        LOG_ERROR("get peerDeviceId failed, sessionId :%{public}d", sessionId);
        return ret;
    }
    lock_guard<mutex> lock(networkMutex_);
    auto it = networkId2Uuid_.find(std::string(peerDevId));
    if (it != networkId2Uuid_.end()) {
        deviceId = it->second;
    }
    return ret;
}

int SoftBusAdapter::GetSessionId(const std::string &deviceId)
{
    lock_guard<mutex> lock(deviceSessionLock_);
    auto it = sessionIds_.find(deviceId);
    if (it != sessionIds_.end()) {
        return it->second;
    }
    return INVALID_SESSION_ID;
}

void SoftBusAdapter::OnSessionOpen(int32_t sessionId, int32_t status)
{
    if (status != SOFTBUS_OK) {
        LOG_DEBUG("[OnSessionOpen] OpenSession failed");
        return;
    }
    if (GetSessionSide(sessionId) != SELF_SIDE) {
        return;
    }
    CacheSession(sessionId);
    DoSend();
}

void SoftBusAdapter::OnSessionClose(int32_t sessionId)
{
    std::string deviceId;
    if (GetSessionSide(sessionId) != SELF_SIDE) {
        return;
    }
    if (GetDeviceId(sessionId, deviceId) == SOFTBUS_OK) {
        lock_guard<mutex> lock(deviceSessionLock_);
        if (sessionIds_.find(deviceId) != sessionIds_.end()) {
            sessionIds_.erase(deviceId);
        }
    }
    DoSend();
}

bool SoftBusAdapter::IsSameStartedOnPeer(
    const struct PipeInfo &pipeInfo, __attribute__((unused)) const struct DeviceId &peer)
{
    LOG_INFO(
        "pipeInfo:%{public}s peer.deviceId:%{public}s", pipeInfo.pipeId.c_str(), ToBeAnonymous(peer.deviceId).c_str());
    {
        lock_guard<mutex> lock(busSessionMutex_);
        if (busSessionMap_.find(pipeInfo.pipeId + peer.deviceId) != busSessionMap_.end()) {
            LOG_INFO("Found session in map. Return true.");
            return true;
        }
    }
    SessionAttribute attr;
    attr.dataType = TYPE_BYTES;
    int sessionId = OpenSession(
        pipeInfo.pipeId.c_str(), pipeInfo.pipeId.c_str(), ToNodeID(peer.deviceId).c_str(), "GROUP_ID", &attr);
    LOG_INFO("[IsSameStartedOnPeer] sessionId=%{public}d", sessionId);
    if (sessionId == INVALID_SESSION_ID) {
        LOG_ERROR("OpenSession return null, pipeInfo:%{public}s. Return false.", pipeInfo.pipeId.c_str());
        return false;
    }
    LOG_INFO("session started, pipeInfo:%{public}s. sessionId:%{public}d Return "
             "true. ",
        pipeInfo.pipeId.c_str(), sessionId);
    return true;
}

void SoftBusAdapter::SetMessageTransFlag(const PipeInfo &pipeInfo, bool flag)
{
    LOG_INFO("pipeInfo: %{public}s flag: %{public}d", pipeInfo.pipeId.c_str(), static_cast<bool>(flag));
    flag_ = flag;
}

int SoftBusAdapter::CreateSessionServerAdapter(const std::string &sessionName)
{
    LOG_DEBUG("begin");
    return CreateSessionServer("ohos.objectstore", sessionName.c_str(), &sessionListener_);
}

int SoftBusAdapter::RemoveSessionServerAdapter(const std::string &sessionName) const
{
    LOG_DEBUG("begin");
    return RemoveSessionServer("ohos.objectstore", sessionName.c_str());
}

void SoftBusAdapter::InsertSession(const std::string &sessionName)
{
    lock_guard<mutex> lock(busSessionMutex_);
    busSessionMap_.insert({ sessionName, true });
}

void SoftBusAdapter::DeleteSession(const std::string &sessionName)
{
    lock_guard<mutex> lock(busSessionMutex_);
    busSessionMap_.erase(sessionName);
}

void SoftBusAdapter::NotifyDataListeners(
    const uint8_t *ptr, const int size, const std::string &deviceId, const PipeInfo &pipeInfo)
{
    LOG_DEBUG("begin");
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

void AppDataListenerWrap::SetDataHandler(SoftBusAdapter *handler)
{
    LOG_INFO("begin");
    softBusAdapter_ = handler;
}

int AppDataListenerWrap::OnSessionOpened(int sessionId, int result)
{
    LOG_DEBUG("[SessionOpen] sessionId:%{public}d, result:%{public}d", sessionId, result);
    char mySessionName[SESSION_NAME_SIZE_MAX] = "";
    char peerSessionName[SESSION_NAME_SIZE_MAX] = "";
    char peerDevId[DEVICE_ID_SIZE_MAX] = "";
    softBusAdapter_->OnSessionOpen(sessionId, result);
    if (result != SOFTBUS_OK) {
        LOG_WARN("session %{public}d open failed, result:%{public}d.", sessionId, result);
        return result;
    }
    int ret = GetMySessionName(sessionId, mySessionName, sizeof(mySessionName));
    if (ret != SOFTBUS_OK) {
        LOG_WARN("get my session name failed, session id is %{public}d.", sessionId);
        return SOFTBUS_ERR;
    }
    ret = GetPeerSessionName(sessionId, peerSessionName, sizeof(peerSessionName));
    if (ret != SOFTBUS_OK) {
        LOG_WARN("get my peer session name failed, session id is %{public}d.", sessionId);
        return SOFTBUS_ERR;
    }
    ret = GetPeerDeviceId(sessionId, peerDevId, sizeof(peerDevId));
    if (ret != SOFTBUS_OK) {
        LOG_WARN("get my peer device id failed, session id is %{public}d.", sessionId);
        return SOFTBUS_ERR;
    }
    std::string peerUuid = DevManager::GetInstance()->GetUuidByNodeId(std::string(peerDevId));
    LOG_DEBUG("[SessionOpen] mySessionName:%{public}s, "
              "peerSessionName:%{public}s, peerDevId:%{public}s",
        mySessionName, peerSessionName, SoftBusAdapter::ToBeAnonymous(peerUuid).c_str());

    if (strlen(peerSessionName) < 1) {
        softBusAdapter_->InsertSession(std::string(mySessionName) + peerUuid);
    } else {
        softBusAdapter_->InsertSession(std::string(peerSessionName) + peerUuid);
    }
    return 0;
}

void AppDataListenerWrap::OnSessionClosed(int sessionId)
{
    LOG_INFO("[SessionClosed] sessionId:%{public}d", sessionId);
    char mySessionName[SESSION_NAME_SIZE_MAX] = "";
    char peerSessionName[SESSION_NAME_SIZE_MAX] = "";
    char peerDevId[DEVICE_ID_SIZE_MAX] = "";

    softBusAdapter_->OnSessionClose(sessionId);
    int ret = GetMySessionName(sessionId, mySessionName, sizeof(mySessionName));
    if (ret != SOFTBUS_OK) {
        LOG_WARN("get my session name failed, session id is %{public}d.", sessionId);
        return;
    }
    ret = GetPeerSessionName(sessionId, peerSessionName, sizeof(peerSessionName));
    if (ret != SOFTBUS_OK) {
        LOG_WARN("get my peer session name failed, session id is %{public}d.", sessionId);
        return;
    }
    ret = GetPeerDeviceId(sessionId, peerDevId, sizeof(peerDevId));
    if (ret != SOFTBUS_OK) {
        LOG_WARN("get my peer device id failed, session id is %{public}d.", sessionId);
        return;
    }
    std::string peerUuid = DevManager::GetInstance()->GetUuidByNodeId(std::string(peerDevId));
    LOG_DEBUG("[SessionClosed] mySessionName:%{public}s, "
              "peerSessionName:%{public}s, peerDevId:%{public}s",
        mySessionName, peerSessionName, SoftBusAdapter::ToBeAnonymous(peerUuid).c_str());
    if (strlen(peerSessionName) < 1) {
        softBusAdapter_->DeleteSession(std::string(mySessionName) + peerUuid);
    } else {
        softBusAdapter_->DeleteSession(std::string(peerSessionName) + peerUuid);
    }
}

void AppDataListenerWrap::OnMessageReceived(int sessionId, const void *data, unsigned int dataLen)
{
    LOG_INFO("begin");
    if (sessionId == INVALID_SESSION_ID) {
        return;
    }
    char peerSessionName[SESSION_NAME_SIZE_MAX] = "";
    char peerDevId[DEVICE_ID_SIZE_MAX] = "";
    int ret = GetPeerSessionName(sessionId, peerSessionName, sizeof(peerSessionName));
    if (ret != SOFTBUS_OK) {
        LOG_WARN("get my peer session name failed, session id is %{public}d.", sessionId);
        return;
    }
    ret = GetPeerDeviceId(sessionId, peerDevId, sizeof(peerDevId));
    if (ret != SOFTBUS_OK) {
        LOG_WARN("get my peer device id failed, session id is %{public}d.", sessionId);
        return;
    }
    std::string peerUuid = DevManager::GetInstance()->GetUuidByNodeId(std::string(peerDevId));
    LOG_DEBUG("[MessageReceived] session id:%{public}d, "
              "peerSessionName:%{public}s, peerDevId:%{public}s",
        sessionId, peerSessionName, SoftBusAdapter::ToBeAnonymous(peerUuid).c_str());
    NotifyDataListeners(reinterpret_cast<const uint8_t *>(data), dataLen, peerUuid, { std::string(peerSessionName) });
}

void AppDataListenerWrap::OnBytesReceived(int sessionId, const void *data, unsigned int dataLen)
{
    if (sessionId == INVALID_SESSION_ID) {
        return;
    }
    char peerSessionName[SESSION_NAME_SIZE_MAX] = "";
    char peerDevId[DEVICE_ID_SIZE_MAX] = "";
    int ret = GetPeerSessionName(sessionId, peerSessionName, sizeof(peerSessionName));
    if (ret != SOFTBUS_OK) {
        LOG_WARN("get my peer session name failed, session id is %{public}d.", sessionId);
        return;
    }
    ret = GetPeerDeviceId(sessionId, peerDevId, sizeof(peerDevId));
    if (ret != SOFTBUS_OK) {
        LOG_WARN("get my peer device id failed, session id is %{public}d.", sessionId);
        return;
    }
    std::string peerUuid = DevManager::GetInstance()->GetUuidByNodeId(std::string(peerDevId));
    LOG_DEBUG("[BytesReceived] session id:%{public}d, peerSessionName:%{public}s, "
              "peerDevId:%{public}s",
        sessionId, peerSessionName, SoftBusAdapter::ToBeAnonymous(peerUuid).c_str());
    NotifyDataListeners(reinterpret_cast<const uint8_t *>(data), dataLen, peerUuid, { std::string(peerSessionName) });
}

void AppDataListenerWrap::NotifyDataListeners(
    const uint8_t *ptr, const int size, const std::string &deviceId, const PipeInfo &pipeInfo)
{
    return softBusAdapter_->NotifyDataListeners(ptr, size, deviceId, pipeInfo);
}
} // namespace ObjectStore
} // namespace OHOS
