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

#ifndef DISTRIBUTEDDATAFWK_SRC_SOFTBUS_ADAPTER_H
#define DISTRIBUTEDDATAFWK_SRC_SOFTBUS_ADAPTER_H
#include <map>
#include <memory>
#include <set>
#include <tuple>
#include <vector>

#include "app_data_change_listener.h"
#include "app_device_status_change_listener.h"
#include "app_types.h"
#include "concurrent_map.h"
#include "session.h"
#include "socket.h"
#include "softbus_bus_center.h"
#include "task_scheduler.h"

namespace OHOS {
namespace ObjectStore {
class SoftBusAdapter {
public:
    SoftBusAdapter();
    ~SoftBusAdapter();
    static std::shared_ptr<SoftBusAdapter> GetInstance();

    // add DeviceChangeListener to watch device change;
    Status StartWatchDeviceChange(const AppDeviceStatusChangeListener *observer, const PipeInfo &pipeInfo);
    // stop DeviceChangeListener to watch device change;
    Status StopWatchDeviceChange(const AppDeviceStatusChangeListener *observer, const PipeInfo &pipeInfo);
    void NotifyAll(const DeviceInfo &deviceInfo, const DeviceChangeType &type);
    DeviceInfo GetLocalDevice();
    std::vector<DeviceInfo> GetDeviceList() const;
    // get local device node information;
    DeviceInfo GetLocalBasicInfo() const;
    // get all remote connected device's node information;
    std::vector<DeviceInfo> GetRemoteNodesBasicInfo() const;
    static std::string ToBeAnonymous(const std::string &name);

    // add DataChangeListener to watch data change;
    Status StartWatchDataChange(const AppDataChangeListener *observer, const PipeInfo &pipeInfo);

    // stop DataChangeListener to watch data change;
    Status StopWatchDataChange(const AppDataChangeListener *observer, const PipeInfo &pipeInfo);

    // Send data to other device, function will be called back after sent to notify send result.
    Status SendData(const PipeInfo &pipeInfo, const DeviceId &deviceId, const DataInfo &dataInfo, uint32_t totalLength,
        const MessageInfo &info);

    bool IsSameStartedOnPeer(const struct PipeInfo &pipeInfo, const struct DeviceId &peer);

    int CreateSessionServerAdapter(const std::string &sessionName);

    int RemoveSessionServerAdapter(const std::string &sessionName) const;

    void UpdateRelationship(const std::string &networkid, const DeviceChangeType &type);

    void NotifyDataListeners(const uint8_t *ptr, const int size, const std::string &deviceId, const PipeInfo &pipeInfo);

    void OnClientShutdown(int32_t socket);

    void OnBind(int32_t socket, PeerSocketInfo info);

    void OnServerShutdown(int32_t socket);

    bool GetPeerSocketInfo(int32_t socket, PeerSocketInfo &info);

    std::string ToNodeID(const std::string &nodeId) const;

private:
    struct BytesMsg {
        uint8_t *ptr;
        uint32_t size;
    };
    static constexpr uint32_t VECTOR_SIZE_THRESHOLD = 100;
    static constexpr uint32_t QOS_COUNT = 3;
    static constexpr QosTV Qos[QOS_COUNT] = {
        { .qos = QOS_TYPE_MIN_BW, .value = 90 * 1024 * 1024 },
        { .qos = QOS_TYPE_MAX_LATENCY, .value = 10000 },
        { .qos = QOS_TYPE_MIN_LATENCY, .value = 2000 } };
    Status CacheData(const std::string &deviceId, const DataInfo &dataInfo);
    void DoSend();
    int GetSocket(const PipeInfo &pipeInfo, const DeviceId &deviceId);
    int CreateClientSocket(const PipeInfo &pipeInfo, const DeviceId &deviceId);
    mutable std::mutex networkMutex_{};
    mutable std::map<std::string, std::string> networkId2Uuid_{};
    DeviceInfo localInfo_{};
    static std::shared_ptr<SoftBusAdapter> instance_;
    std::mutex deviceChangeMutex_;
    std::set<const AppDeviceStatusChangeListener *> listeners_{};
    std::mutex dataChangeMutex_{};
    std::map<std::string, const AppDataChangeListener *> dataChangeListeners_{};
    std::mutex sendDataMutex_{};
    std::mutex socketLock_;
    std::mutex deviceDataLock_;
    std::map<std::string, std::vector<BytesMsg>> dataCaches_;
    std::shared_ptr<TaskScheduler> taskQueue_;
    std::mutex localDeviceLock_{};
    std::map<std::string, int> sockets_;
    int32_t socketServer_{0};
    ConcurrentMap<int32_t, PeerSocketInfo> peerSocketInfos_;
    ISocketListener clientListener_{};
    ISocketListener serverListener_{};
};

class AppDataListenerWrap {
public:
    static void SetDataHandler(SoftBusAdapter *handler);
    static void OnClientShutdown(int32_t socket, ShutdownReason reason);
    static void OnClientBytesReceived(int32_t socket, const void *data, uint32_t dataLen);

    static void OnServerBind(int32_t socket, PeerSocketInfo info);
    static void OnServerShutdown(int32_t socket, ShutdownReason reason);
    static void OnServerBytesReceived(int32_t socket, const void *data, uint32_t dataLen);

public:
    // notifiy all listeners when received message
    static void NotifyDataListeners(
        const uint8_t *ptr, const int size, const std::string &deviceId, const PipeInfo &pipeInfo);
    static SoftBusAdapter *softBusAdapter_;
};
} // namespace ObjectStore
} // namespace OHOS
#endif /* DISTRIBUTEDDATAFWK_SRC_SOFTBUS_ADAPTER_H */
