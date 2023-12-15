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
constexpr int32_t HEAD_SIZE = 3;
constexpr const char *REPLACE_CHAIN = "***";
constexpr const char *DEFAULT_ANONYMOUS = "******";

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
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: SoftBusAdapter_IsSameStartedOnPeer_002
 * @tc.desc: test SoftBusAdapter IsSameStartedOnPeer.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_IsSameStartedOnPeer_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo002" };
    DeviceId deviceId = { "deviceId02" };
    SoftBusAdapter softBusAdapter;
    std::string sessionName = "pipInfo002deviceId02";
    softBusAdapter.InsertSession(sessionName);
    auto ret = softBusAdapter.IsSameStartedOnPeer(pipeInfo, deviceId);
    EXPECT_EQ(true, ret);
    softBusAdapter.DeleteSession(sessionName);
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
 * @tc.name: SoftBusAdapter_ToBeAnonymous_001
 * @tc.desc: test SoftBusAdapter ToBeAnonymous.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_ToBeAnonymous_001, TestSize.Level1)
{
    std::string name = "na";
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.ToBeAnonymous(name);
    EXPECT_EQ(DEFAULT_ANONYMOUS, ret);
}

/**
 * @tc.name: SoftBusAdapter_ToBeAnonymous_002
 * @tc.desc: test SoftBusAdapter ToBeAnonymous.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_ToBeAnonymous_002, TestSize.Level1)
{
    std::string name = "name";
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.ToBeAnonymous(name);
    EXPECT_EQ(name.substr(0, HEAD_SIZE) + REPLACE_CHAIN, ret);
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
    EXPECT_EQ(false, ret.empty());
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
    PipeInfo pipeInfo = {};
    DeviceId deviceId = {"devideId01"};
    uint32_t size = 1;
    uint8_t tmpNum = 1;
    uint8_t *ptr = &tmpNum;
    const DataInfo dataInfo = { const_cast<uint8_t *>(ptr), size };
    MessageInfo messageInfo = {MessageType::DEFAULT};
    SoftBusAdapter softBusAdapter;
    auto ret = softBusAdapter.SendData(pipeInfo, deviceId, dataInfo, size, messageInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
}

/**
 * @tc.name: AppDataListenerWrap_OnSessionOpened_001
 * @tc.desc: test AppDataListenerWrap OnSessionOpened.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, AppDataListenerWrap_OnSessionOpened_001, TestSize.Level1)
{
    int sessionId = 1;
    int result = 0;
    AppDataListenerWrap::SetDataHandler(&softBusAdapter_);
    auto ret = AppDataListenerWrap::OnSessionOpened(sessionId, result);
    EXPECT_EQ(0, ret);
    AppDataListenerWrap::OnSessionClosed(sessionId);
}

/**
 * @tc.name: AppDataListenerWrap_OnSessionOpened_002
 * @tc.desc: test AppDataListenerWrap OnSessionOpened.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, AppDataListenerWrap_OnSessionOpened_002, TestSize.Level1)
{
    int sessionId = 1;
    int result = 1;
    AppDataListenerWrap::SetDataHandler(&softBusAdapter_);
    auto ret = AppDataListenerWrap::OnSessionOpened(sessionId, result);
    EXPECT_EQ(1, ret);
    AppDataListenerWrap::OnSessionClosed(sessionId);
}

/**
 * @tc.name: AppDataListenerWrap_OnMessageReceived_001
 * @tc.desc: test AppDataListenerWrap OnMessageReceived.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, AppDataListenerWrap_OnMessageReceived_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "" };
    SoftBusAdapter softBusAdapter;
    MockAppDataChangeListener observer;
    auto ret = softBusAdapter.StartWatchDataChange(&observer, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    int sessionId = 1;
    AppDataListenerWrap::SetDataHandler(&softBusAdapter);
    std::string data = "OnMessageReceived";
    AppDataListenerWrap::OnMessageReceived(sessionId, reinterpret_cast<const void *>(data.c_str()), data.size());
    EXPECT_EQ(data, observer.data);
}

/**
 * @tc.name: AppDataListenerWrap_OnBytesReceived_001
 * @tc.desc: test AppDataListenerWrap OnBytesReceived.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, AppDataListenerWrap_OnBytesReceived_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "" };
    SoftBusAdapter softBusAdapter;
    MockAppDataChangeListener observer;
    auto ret = softBusAdapter.StartWatchDataChange(&observer, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    int sessionId = 1;
    AppDataListenerWrap::SetDataHandler(&softBusAdapter);
    std::string data = "OnBytesReceived";
    AppDataListenerWrap::OnBytesReceived(sessionId, reinterpret_cast<const void *>(data.c_str()), data.size());
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
}