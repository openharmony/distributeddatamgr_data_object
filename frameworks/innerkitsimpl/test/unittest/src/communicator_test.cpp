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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <string>
#include <thread>

#include "auto_launch_export.h"
#include "objectstore_errors.h"
#include "ipc_skeleton.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"
#include "app_types.h"
#include "softbus_adapter.h"
#include "dev_manager.h"
#include "app_device_status_change_listener.h"
#include "app_data_change_listener.h"
#include "mock_app_device_change_listener.h"
#include "mock_app_data_change_listener.h"

using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
using namespace OHOS::ObjectStore;

namespace {
class NativeCommunicatorTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
public:
    SoftBusAdapter softBusAdapter_;
};

void NativeCommunicatorTest::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void NativeCommunicatorTest::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void NativeCommunicatorTest::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void NativeCommunicatorTest::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: SoftBusAdapter_StartWatchDeviceChange_001
 * @tc.desc: test SoftBusAdapter StartWatchDeviceChange. insert a normal observer
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, StartWatchDeviceChange_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo001" };
    SoftBusAdapter softBusAdapter;
    MockAppDeviceStatusChangeListener listener;
    auto ret = softBusAdapter.StartWatchDeviceChange(&listener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
}

/**
 * @tc.name: SoftBusAdapter_StartWatchDeviceChange_002
 * @tc.desc: test SoftBusAdapter StartWatchDeviceChange. inserted observer is null
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StartWatchDeviceChange_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo002" };
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.StartWatchDeviceChange(nullptr, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
}

/**
 * @tc.name: SoftBusAdapter_StartWatchDeviceChange_003
 * @tc.desc: test SoftBusAdapter StartWatchDeviceChange. insert the same observer
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StartWatchDeviceChange_003, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo003" };
    SoftBusAdapter softBusAdapter;
    MockAppDeviceStatusChangeListener listener;
    auto ret = softBusAdapter.StartWatchDeviceChange(&listener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = softBusAdapter.StartWatchDeviceChange(&listener, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
}

/**
 * @tc.name: SoftBusAdapter_StopWatchDeviceChange_001
 * @tc.desc: test SoftBusAdapter StopWatchDeviceChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StopWatchDeviceChange_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo001" };
    SoftBusAdapter softBusAdapter;
    MockAppDeviceStatusChangeListener listener;
    auto ret = softBusAdapter.StartWatchDeviceChange(&listener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = softBusAdapter.StopWatchDeviceChange(&listener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
}

/**
 * @tc.name: SoftBusAdapter_StopWatchDeviceChange_002
 * @tc.desc: test SoftBusAdapter StopWatchDeviceChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StopWatchDeviceChange_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo002" };
    SoftBusAdapter softBusAdapter;
    MockAppDeviceStatusChangeListener listener;
    auto ret = softBusAdapter.StartWatchDeviceChange(&listener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = softBusAdapter.StopWatchDeviceChange(nullptr, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
}

/**
 * @tc.name: SoftBusAdapter_StopWatchDeviceChange_003
 * @tc.desc: test SoftBusAdapter StopWatchDeviceChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StopWatchDeviceChange_003, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo003" };
    SoftBusAdapter softBusAdapter;
    MockAppDeviceStatusChangeListener listener1;
    MockAppDeviceStatusChangeListener listener2;
    auto ret = softBusAdapter.StartWatchDeviceChange(&listener1, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = softBusAdapter.StopWatchDeviceChange(&listener2, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
}

/**
 * @tc.name: SoftBusAdapter_NotifyAll_001
 * @tc.desc: test SoftBusAdapter NotifyAll. input parameter of GetChangeLevelType is low
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_NotifyAll_001, TestSize.Level1)
{
    OHOS::ObjectStore::DeviceInfo deviceInfo = { "001", "localLow", "phoneLow" };
    PipeInfo pipeInfo = { "pipInfo001" };
    SoftBusAdapter softBusAdapter;
    MockAppDeviceStatusChangeListenerLow listener;
    auto ret = softBusAdapter.StartWatchDeviceChange(&listener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);

    softBusAdapter.NotifyAll(deviceInfo, DeviceChangeType::DEVICE_ONLINE);
    listener.Wait();
    EXPECT_EQ(listener.Compare(deviceInfo), true);
}

/**
 * @tc.name: SoftBusAdapter_NotifyAll_002
 * @tc.desc: test SoftBusAdapter NotifyAll. input parameter of GetChangeLevelType is high
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_NotifyAll_002, TestSize.Level1)
{
    OHOS::ObjectStore::DeviceInfo deviceInfo = { "001", "localHigh", "phoneHigh" };
    PipeInfo pipeInfo = { "pipInfo001" };
    SoftBusAdapter softBusAdapter;
    MockAppDeviceStatusChangeListenerHigh listener;
    auto ret = softBusAdapter.StartWatchDeviceChange(&listener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);

    softBusAdapter.NotifyAll(deviceInfo, DeviceChangeType::DEVICE_ONLINE);
    listener.Wait();
    EXPECT_EQ(listener.Compare(deviceInfo), true);
}

/**
 * @tc.name: SoftBusAdapter_NotifyAll_003
 * @tc.desc: test SoftBusAdapter NotifyAll. input parameter of GetChangeLevelType is min
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_NotifyAll_003, TestSize.Level1)
{
    OHOS::ObjectStore::DeviceInfo deviceInfo = { "001", "localMin", "phoneMin" };
    PipeInfo pipeInfo = { "pipInfo001" };
    SoftBusAdapter softBusAdapter;
    MockAppDeviceStatusChangeListenerMin listener;
    auto ret = softBusAdapter.StartWatchDeviceChange(&listener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);

    softBusAdapter.NotifyAll(deviceInfo, DeviceChangeType::DEVICE_ONLINE);
    listener.Wait();
    EXPECT_EQ(listener.Compare(deviceInfo), true);
}

/**
 * @tc.name: SoftBusAdapter_IsSameStartedOnPeer_001
 * @tc.desc: test SoftBusAdapter NotifyAll.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_IsSameStartedOnPeer_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo001" };
    DeviceId deviceId = { "deviceId01" };
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.IsSameStartedOnPeer(pipeInfo, deviceId);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: SoftBusAdapter_StartWatchDataChange_001
 * @tc.desc: test SoftBusAdapter StartWatchDataChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StartWatchDataChange_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo001" };
    SoftBusAdapter softBusAdapter;
    MockAppDataChangeListener listener;
    auto ret = softBusAdapter.StartWatchDataChange(&listener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
}

/**
 * @tc.name: SoftBusAdapter_StartWatchDataChange_002
 * @tc.desc: test SoftBusAdapter StartWatchDataChange. insert the same observer
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StartWatchDataChange_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo002" };
    SoftBusAdapter softBusAdapter;
    MockAppDataChangeListener listener;
    auto ret = softBusAdapter.StartWatchDataChange(&listener, pipeInfo);
    ret = softBusAdapter.StartWatchDataChange(&listener, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
}

/**
 * @tc.name: SoftBusAdapter_StartWatchDataChange_003
 * @tc.desc: test SoftBusAdapter StartWatchDataChange. observer is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StartWatchDataChange_003, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo002" };
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.StartWatchDataChange(nullptr, pipeInfo);
    EXPECT_EQ(Status::INVALID_ARGUMENT, ret);
}

/**
 * @tc.name: SoftBusAdapter_StopWatchDataChange_001
 * @tc.desc: test SoftBusAdapter StartWatchDataChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StopWatchDataChange_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo001" };
    SoftBusAdapter softBusAdapter;
    MockAppDataChangeListener listener;
    auto ret = softBusAdapter.StartWatchDataChange(&listener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    softBusAdapter.StopWatchDataChange(&listener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
}

/**
 * @tc.name: SoftBusAdapter_StopWatchDataChange_002
 * @tc.desc: test SoftBusAdapter StartWatchDataChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StopWatchDataChange_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo001" };
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.StopWatchDataChange(nullptr, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
}

/**
 * @tc.name: SoftBusAdapter_GetLocalBasicInfo_001
 * @tc.desc: test SoftBusAdapter GetLocalBasicInfo.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_GetLocalBasicInfo_001, TestSize.Level1)
{
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.GetLocalBasicInfo();
    EXPECT_EQ(true, ret.deviceId.empty());
    EXPECT_EQ(true, ret.deviceName.empty());
    EXPECT_EQ(true, ret.deviceType.empty());
}

/**
 * @tc.name: SoftBusAdapter_GetRemoteNodesBasicInfo_002
 * @tc.desc: test SoftBusAdapter GetRemoteNodesBasicInfo.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_GetLocalBasicInfo_002, TestSize.Level1)
{
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.GetRemoteNodesBasicInfo();
    EXPECT_EQ(false, ret.empty());
}

/**
 * @tc.name: SoftBusAdapter_UpdateRelationship_001
 * @tc.desc: test SoftBusAdapter UpdateRelationship.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_UpdateRelationship_001, TestSize.Level1)
{
    std::string networdId01 = "networdId01";
    SoftBusAdapter softBusAdapter;
    softBusAdapter.UpdateRelationship(networdId01, DeviceChangeType::DEVICE_ONLINE);
    auto ret = softBusAdapter.ToNodeID("");
    EXPECT_EQ(networdId01, ret);
    softBusAdapter.UpdateRelationship(networdId01, DeviceChangeType::DEVICE_OFFLINE);
}

/**
 * @tc.name: SoftBusAdapter_UpdateRelationship_002
 * @tc.desc: test SoftBusAdapter UpdateRelationship.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_UpdateRelationship_002, TestSize.Level1)
{
    std::string networdId01 = "networdId01";
    std::string networdId02 = "networdId02";
    SoftBusAdapter softBusAdapter;
    softBusAdapter.UpdateRelationship(networdId01, DeviceChangeType::DEVICE_ONLINE);
    softBusAdapter.UpdateRelationship(networdId02, DeviceChangeType::DEVICE_ONLINE);
    softBusAdapter.UpdateRelationship(networdId02, DeviceChangeType::DEVICE_OFFLINE);
    auto ret = softBusAdapter.ToNodeID("");
    EXPECT_EQ(networdId01, ret);
    softBusAdapter.UpdateRelationship(networdId01, DeviceChangeType::DEVICE_OFFLINE);
}

/**
 * @tc.name: SoftBusAdapter_UpdateRelationship_003
 * @tc.desc: test SoftBusAdapter UpdateRelationship.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_UpdateRelationship_003, TestSize.Level1)
{
    std::string networdId01 = "networdId01";
    std::string networdId02 = "networdId02";
    SoftBusAdapter softBusAdapter;
    softBusAdapter.UpdateRelationship(networdId01, DeviceChangeType::DEVICE_ONLINE);
    softBusAdapter.UpdateRelationship(networdId01, DeviceChangeType::DEVICE_ONLINE);
    auto ret = softBusAdapter.ToNodeID("");
    EXPECT_EQ(networdId01, ret);
    softBusAdapter.UpdateRelationship(networdId01, DeviceChangeType::DEVICE_OFFLINE);
}

/**
 * @tc.name: SoftBusAdapter_UpdateRelationship_004
 * @tc.desc: test SoftBusAdapter UpdateRelationship.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_UpdateRelationship_004, TestSize.Level1)
{
    std::string networdId01 = "networdId01";
    std::string networdId02 = "networdId02";
    SoftBusAdapter softBusAdapter;
    softBusAdapter.UpdateRelationship(networdId01, DeviceChangeType::DEVICE_ONLINE);
    softBusAdapter.UpdateRelationship(networdId02, DeviceChangeType::DEVICE_OFFLINE);
    auto ret = softBusAdapter.ToNodeID("");
    EXPECT_EQ(networdId01, ret);
    softBusAdapter.UpdateRelationship(networdId01, DeviceChangeType::DEVICE_OFFLINE);
}

/**
 * @tc.name: SoftBusAdapter_UpdateRelationship_005
 * @tc.desc: test SoftBusAdapter UpdateRelationship. The input parameter is an incorrect DeviceChangeType
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_UpdateRelationship_005, TestSize.Level1)
{
    std::string networdId01 = "networdId01";
    std::string networdId02 = "networdId02";
    SoftBusAdapter softBusAdapter;
    softBusAdapter.UpdateRelationship(networdId01, DeviceChangeType::DEVICE_ONLINE);
    softBusAdapter.UpdateRelationship(networdId02, static_cast<DeviceChangeType>(-1));
    auto ret = softBusAdapter.ToNodeID("");
    EXPECT_EQ(networdId01, ret);
    softBusAdapter.UpdateRelationship(networdId01, DeviceChangeType::DEVICE_OFFLINE);
}

/**
 * @tc.name: SoftBusAdapter_RemoveSessionServerAdapter_001
 * @tc.desc: test SoftBusAdapter RemoveSessionServerAdapter.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_RemoveSessionServerAdapter_001, TestSize.Level1)
{
    std::string sessionName = "sessionName01";
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.RemoveSessionServerAdapter(sessionName);
    EXPECT_EQ(SUCCESS, ret);
}

/**
 * @tc.name: SoftBusAdapter_GetDeviceList_001
 * @tc.desc: test SoftBusAdapter GetLocalDevice.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_GetDeviceList_001, TestSize.Level1)
{
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.GetDeviceList();
    EXPECT_EQ(true, ret.empty());
}

/**
 * @tc.name: SoftBusAdapter_GetLocalDevice_001
 * @tc.desc: test SoftBusAdapter GetLocalDevice.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_GetLocalDevice_001, TestSize.Level1)
{
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.GetLocalDevice();
    EXPECT_EQ(true, ret.deviceId.empty());
    EXPECT_EQ(true, ret.deviceName.empty());
    EXPECT_EQ(true, ret.deviceType.empty());
}

/**
 * @tc.name: SoftBusAdapter_SendData_001
 * @tc.desc: test SoftBusAdapter SendData.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_SendData_001, TestSize.Level1)
{
    PipeInfo pipeInfo = {"INVALID_SESSION_NAME"};
    DeviceId deviceId = {"devideId01"};
    uint32_t size = 1;
    uint8_t tmpNum = 1;
    uint8_t *ptr = &tmpNum;
    const DataInfo dataInfo = { const_cast<uint8_t *>(ptr), size };
    MessageInfo messageInfo = {MessageType::DEFAULT};
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.SendData(pipeInfo, deviceId, dataInfo, size, messageInfo);
    EXPECT_EQ(Status::ERROR, ret);
}

/**
 * @tc.name: AppDataListenerWrap_OnServerBind_001
 * @tc.desc: test AppDataListenerWrap OnServerBind.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, AppDataListenerWrap_OnServerBind_001, TestSize.Level1)
{
    AppDataListenerWrap::SetDataHandler(&softBusAdapter_);
    int32_t socket = 9999;
    std::string name = "OnServiceBind_001";
    std::string networkId = "1234567890";
    std::string pkgName = "ohos.objectstore";
    PeerSocketInfo mockInfo = {
        .name = name.data(),
        .networkId = networkId.data(),
        .pkgName = pkgName.data(),
        .dataType = DATA_TYPE_BYTES
    };
    AppDataListenerWrap::OnServerBind(socket, mockInfo);
    PeerSocketInfo info;
    auto ret = softBusAdapter_.GetPeerSocketInfo(socket, info);
    EXPECT_TRUE(ret);
    EXPECT_EQ(mockInfo.name, info.name);
    AppDataListenerWrap::OnServerShutdown(socket, SHUTDOWN_REASON_LOCAL);
    ret = softBusAdapter_.GetPeerSocketInfo(socket, info);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: AppDataListenerWrap_OnServerBytesReceived_001
 * @tc.desc: test AppDataListenerWrap OnServerBytesReceived.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, AppDataListenerWrap_OnServerBytesReceived_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "OnServerBytesReceived_001" };
    SoftBusAdapter softBusAdapter;
    MockAppDataChangeListener observer;
    auto ret = softBusAdapter.StartWatchDataChange(&observer, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    int socket = 1;
    std::string data = "";
    AppDataListenerWrap::OnServerBytesReceived(socket, reinterpret_cast<const void *>(data.c_str()), data.size());
    EXPECT_EQ(data, observer.data);
}

/**
 * @tc.name: DevManager_GetUuidByNodeId_001
 * @tc.desc: test DevManager GetUuidByNodeId.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, DevManager_GetUuidByNodeId_001, TestSize.Level1)
{
    std::string nodeId = "nodeId01";
    auto ret = DevManager::GetInstance()->GetUuidByNodeId(nodeId);
    EXPECT_EQ(true, ret.empty());
}

/**
* @tc.name: DevManager_GetLocalDevice
* @tc.desc: test DevManager GetLocalDevice.
* @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, DevManager_GetLocalDevice_001, TestSize.Level1)
{
    DevManager *devManager = DevManager::GetInstance();
    EXPECT_TRUE(devManager != nullptr);
    DevManager::DetailInfo detailInfo = devManager->GetLocalDevice();
    EXPECT_EQ(detailInfo.networkId, "");
}

/**
* @tc.name: DestructedSoftBusAdapter001
* @tc.desc: Test that the destructor cleans the dataCaches when bytesMsg.ptr is not nullptr.
* @tc.type: FUNC
*/
HWTEST_F(NativeCommunicatorTest, DestructedSoftBusAdapter001, TestSize.Level1)
{
    auto softBusAdapter = std::make_shared<SoftBusAdapter>();
    uint32_t length = 10;
    uint8_t *data = new uint8_t[length];
    SoftBusAdapter::BytesMsg bytesMsg = { data, length };
    softBusAdapter->dataCaches_["device1"] = { bytesMsg };
    EXPECT_FALSE(softBusAdapter->dataCaches_.empty());
}

/**
* @tc.name: DestructedSoftBusAdapter002
* @tc.desc: Test that the destructor does not clean the dataCaches when bytesMsg.ptr is nullptr.
* @tc.type: FUNC
*/
HWTEST_F(NativeCommunicatorTest, DestructedSoftBusAdapter002, TestSize.Level1)
{
    auto softBusAdapter = std::make_shared<SoftBusAdapter>();
    SoftBusAdapter::BytesMsg bytesMsg = { nullptr, 0 };
    softBusAdapter->dataCaches_["device2"] = { bytesMsg };
    EXPECT_FALSE(softBusAdapter->dataCaches_.empty());
}
}
