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
#include "app_device_status_change_listener.h"
#include "process_communicator_impl.h"
#include "app_data_change_listener.h"
#include "mock_app_device_change_listener.h"
#include "mock_app_data_change_listener.h"

using namespace testing::ext;
using namespace OHOS::Security::AccessToken;
using namespace OHOS::ObjectStore;

class NativeCommunicatorTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
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
 * @tc.desc: test SoftBusAdapter NotifyAll.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, StartWatchDeviceChange_001, TestSize.Level1)
{
    PipeInfo pipeInfo = {"pipInfo001"};
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    MockAppDeviceStatusChangeListener *mockAppDeviceStatusChangeListener = new MockAppDeviceStatusChangeListener();
    auto ret = softBusAdapter->StartWatchDeviceChange(mockAppDeviceStatusChangeListener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    delete softBusAdapter;
    delete mockAppDeviceStatusChangeListener;
}

/**
 * @tc.name: SoftBusAdapter_StartWatchDeviceChange_002
 * @tc.desc: test SoftBusAdapter NotifyAll. input parameter of observer is null
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StartWatchDeviceChange_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo002" };
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    MockAppDeviceStatusChangeListener *mockAppDeviceStatusChangeListener = nullptr;
    auto ret = softBusAdapter->StartWatchDeviceChange(mockAppDeviceStatusChangeListener, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
    delete softBusAdapter;
}

/**
 * @tc.name: SoftBusAdapter_StartWatchDeviceChange_003
 * @tc.desc: test SoftBusAdapter StartWatchDeviceChange. insert the same observer
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StartWatchDeviceChange_003, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo003" };
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    MockAppDeviceStatusChangeListener *mockAppDeviceStatusChangeListener = new MockAppDeviceStatusChangeListener();
    auto ret = softBusAdapter->StartWatchDeviceChange(mockAppDeviceStatusChangeListener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = softBusAdapter->StartWatchDeviceChange(mockAppDeviceStatusChangeListener, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
    delete softBusAdapter;
    delete mockAppDeviceStatusChangeListener;
}

/**
 * @tc.name: SoftBusAdapter_StopWatchDeviceChange_001
 * @tc.desc: test SoftBusAdapter StopWatchDeviceChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StopWatchDeviceChange_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo001" };
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    MockAppDeviceStatusChangeListener *mockAppDeviceStatusChangeListener = new MockAppDeviceStatusChangeListener();
    auto ret = softBusAdapter->StartWatchDeviceChange(mockAppDeviceStatusChangeListener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = softBusAdapter->StopWatchDeviceChange(mockAppDeviceStatusChangeListener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    delete softBusAdapter;
    delete mockAppDeviceStatusChangeListener;
}

/**
 * @tc.name: SoftBusAdapter_StopWatchDeviceChange_002
 * @tc.desc: test SoftBusAdapter StopWatchDeviceChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StopWatchDeviceChange_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo002" };
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    MockAppDeviceStatusChangeListener *mockAppDeviceStatusChangeListener = new MockAppDeviceStatusChangeListener();
    auto ret = softBusAdapter->StartWatchDeviceChange(mockAppDeviceStatusChangeListener, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = softBusAdapter->StopWatchDeviceChange(nullptr, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
    delete softBusAdapter;
    delete mockAppDeviceStatusChangeListener;
}

/**
 * @tc.name: SoftBusAdapter_StopWatchDeviceChange_003
 * @tc.desc: test SoftBusAdapter StopWatchDeviceChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StopWatchDeviceChange_003, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo003" };
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    MockAppDeviceStatusChangeListener *mockAppDeviceStatusChangeListener1 = new MockAppDeviceStatusChangeListener();
    MockAppDeviceStatusChangeListener *mockAppDeviceStatusChangeListener2 = new MockAppDeviceStatusChangeListener();
    auto ret = softBusAdapter->StartWatchDeviceChange(mockAppDeviceStatusChangeListener1, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = softBusAdapter->StopWatchDeviceChange(mockAppDeviceStatusChangeListener2, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
    delete softBusAdapter;
    delete mockAppDeviceStatusChangeListener1;
    delete mockAppDeviceStatusChangeListener2;
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
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    std::string sessionName = "session01";
    auto ret = softBusAdapter->IsSameStartedOnPeer(pipeInfo, deviceId);
    EXPECT_EQ(false, ret);
    delete softBusAdapter;
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
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    std::string sessionName = "pipInfo002deviceId02";
    softBusAdapter->InsertSession(sessionName);
    auto ret = softBusAdapter->IsSameStartedOnPeer(pipeInfo, deviceId);
    EXPECT_EQ(true, ret);
    softBusAdapter->DeleteSession(sessionName);
    delete softBusAdapter;
}

/**
 * @tc.name: SoftBusAdapter_SendData_001
 * @tc.desc: test SoftBusAdapter SendData. OpenSession error
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_SendData_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo001" };
    DeviceId deviceId = { "deviceId01" };
    uint8_t ptr = 1;
    int size = 1;
    MessageInfo info = { MessageType::DEFAULT };
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    auto ret = softBusAdapter->SendData(pipeInfo, deviceId, &ptr, size, info);
    EXPECT_EQ(Status::CREATE_SESSION_ERROR, ret);
    delete softBusAdapter;
}

/**
 * @tc.name: SoftBusAdapter_NotifyDataListeners_001
 * @tc.desc: test SoftBusAdapter NotifyDataListeners.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_NotifyDataListeners_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo001" };
    std::string deviceId = "deviceId01";
    uint8_t ptr = 1;
    int size = 1;
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    softBusAdapter->NotifyDataListeners(&ptr, size, deviceId, pipeInfo);
    delete softBusAdapter;
}

/**
 * @tc.name: SoftBusAdapter_StartWatchDataChange_001
 * @tc.desc: test SoftBusAdapter StartWatchDataChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StartWatchDataChange_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo001" };
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    AppDataChangeListener *observer = new MockAppDataChangeListener();
    auto ret = softBusAdapter->StartWatchDataChange(observer, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    delete softBusAdapter;
    delete observer;
}

/**
 * @tc.name: SoftBusAdapter_StartWatchDataChange_002
 * @tc.desc: test SoftBusAdapter StartWatchDataChange. insert the same observer
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StartWatchDataChange_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo002" };
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    AppDataChangeListener *observer = new MockAppDataChangeListener();
    auto ret = softBusAdapter->StartWatchDataChange(observer, pipeInfo);
    ret = softBusAdapter->StartWatchDataChange(observer, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
    delete softBusAdapter;
    delete observer;
}

/**
 * @tc.name: SoftBusAdapter_StopWatchDataChange_001
 * @tc.desc: test SoftBusAdapter StartWatchDataChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeCommunicatorTest, SoftBusAdapter_StopWatchDataChange_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo001" };
    SoftBusAdapter *softBusAdapter = new SoftBusAdapter();
    AppDataChangeListener *observer = new MockAppDataChangeListener();
    auto ret = softBusAdapter->StartWatchDataChange(observer, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    softBusAdapter->StopWatchDataChange(observer, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    delete softBusAdapter;
    delete observer;
}