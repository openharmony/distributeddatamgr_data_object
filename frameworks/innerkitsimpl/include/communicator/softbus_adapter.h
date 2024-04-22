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

    void SetMessageTransFlag(const PipeInfo &pipeInfo, bool flag);

    int CreateSessionServerAdapter(const std::string &sessionName);

    int RemoveSessionServerAdapter(const std::string &sessionName) const;

    void UpdateRelationship(const std::string &networkid, const DeviceChangeType &type);

    void InsertSession(const std::string &sessionName);

    void DeleteSession(const std::string &sessionName);

    void NotifyDataListeners(const uint8_t *ptr, const int size, const std::string &deviceId, const PipeInfo &pipeInfo);

    std::string ToNodeID(const std::string &nodeId) const;

    void OnSessionOpen(int32_t sessionId, int32_t status);

    void OnSessionClose(int32_t sessionId);

private:
    struct BytesMsg {
        uint8_t *ptr;
        uint32_t size;
    };
    static constexpr uint32_t P2P_SIZE_THRESHOLD = 0x10000u;    // 64KB
    static constexpr uint32_t VECTOR_SIZE_THRESHOLD = 100;
    static constexpr float SWITCH_DELAY_FACTOR = 0.6f;
    Status CacheData(const std::string &deviceId, const DataInfo &dataInfo);
    uint32_t GetSize(const std::string &deviceId);
    RouteType GetRouteType(int sessionId);
    RecOperate CalcRecOperate(uint32_t dataSize, RouteType currentRouteType, bool &isP2P) const;
    void CacheSession(int32_t sessionId);
    void DoSend();
    int GetDeviceId(int sessionId, std::string &deviceId);
    int GetSessionId(const std::string &deviceId);
    SessionAttribute GetSessionAttribute(bool isP2P);
    mutable std::mutex networkMutex_{};
    mutable std::map<std::string, std::string> networkId2Uuid_{};
    DeviceInfo localInfo_{};
    static std::shared_ptr<SoftBusAdapter> instance_;
    std::mutex deviceChangeMutex_;
    std::set<const AppDeviceStatusChangeListener *> listeners_{};
    std::mutex dataChangeMutex_{};
    std::map<std::string, const AppDataChangeListener *> dataChangeListeners_{};
    std::mutex busSessionMutex_{};
    std::map<std::string, bool> busSessionMap_{};
    bool flag_ = true; // only for br flag
    ISessionListener sessionListener_{};
    std::mutex statusMutex_{};
    std::mutex sendDataMutex_{};
    std::mutex mapLock_;
    std::mutex deviceSessionLock_;
    std::map<std::string, int> sessionIds_;
    std::mutex deviceDataLock_;
    std::map<std::string, std::vector<BytesMsg>> dataCaches_;
    std::shared_ptr<TaskScheduler> taskQueue_;
    std::mutex localDeviceLock_{};
};

class AppDataListenerWrap {
public:
    static void SetDataHandler(SoftBusAdapter *handler);
    static int OnSessionOpened(int sessionId, int result);
    static void OnSessionClosed(int sessionId);
    static void OnMessageReceived(int sessionId, const void *data, unsigned int dataLen);
    static void OnBytesReceived(int sessionId, const void *data, unsigned int dataLen);

public:
    // notifiy all listeners when received message
    static void NotifyDataListeners(
        const uint8_t *ptr, const int size, const std::string &deviceId, const PipeInfo &pipeInfo);
    static SoftBusAdapter *softBusAdapter_;
};
} // namespace ObjectStore
} // namespace OHOS
#endif /* DISTRIBUTEDDATAFWK_SRC_SOFTBUS_ADAPTER_H */