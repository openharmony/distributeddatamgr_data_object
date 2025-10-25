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

#include "app_pipe_mgr.h"

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <string>

#include "app_types.h"
#include "auto_launch_export.h"
#include "ipc_skeleton.h"
#include "mock_app_data_change_listener.h"
#include "objectstore_errors.h"

namespace {
using namespace testing::ext;
using namespace OHOS::ObjectStore;

class NativeAppPipeMgrTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void NativeAppPipeMgrTest::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void NativeAppPipeMgrTest::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void NativeAppPipeMgrTest::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void NativeAppPipeMgrTest::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: NativeAppPipeMgrTest_StartWatchDataChange_001
 * @tc.desc: test NativeAppPipeMgrTest StartWatchDataChange. argument invalid
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_StartWatchDataChange_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "" };
    AppDataChangeListener *observer = new MockAppDataChangeListener();
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->StartWatchDataChange(observer, pipeInfo);
    EXPECT_EQ(Status::INVALID_ARGUMENT, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_StartWatchDataChange_002
 * @tc.desc: test NativeAppPipeMgrTest StartWatchDataChange. pipId not found.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_StartWatchDataChange_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipId01" };
    AppDataChangeListener *observer = new MockAppDataChangeListener();
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->StartWatchDataChange(observer, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_StartWatchDataChange_003
 * @tc.desc: test NativeAppPipeMgrTest StartWatchDataChange. observer is nullptr.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_StartWatchDataChange_003, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipId01" };
    AppPipeMgr appPipeMgr;
    auto ret = appPipeMgr.StartWatchDataChange(nullptr, pipeInfo);
    EXPECT_EQ(Status::INVALID_ARGUMENT, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_StartWatchDataChange_004
 * @tc.desc: test NativeAppPipeMgrTest StartWatchDataChange. the pipId can be found.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_StartWatchDataChange_004, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipId01" };
    AppPipeMgr appPipeMgr;
    auto ret = appPipeMgr.Start(pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    MockAppDataChangeListener observer;
    ret = appPipeMgr.StartWatchDataChange(&observer, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_StopWatchDataChange_001
 * @tc.desc: test NativeAppPipeMgrTest StopWatchDataChange. argument invalid
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_StopWatchDataChange_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipId01" };
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->StopWatchDataChange(nullptr, pipeInfo);
    EXPECT_EQ(Status::INVALID_ARGUMENT, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_StopWatchDataChange_002
 * @tc.desc: test NativeAppPipeMgrTest StopWatchDataChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_StopWatchDataChange_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipId01" };
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    AppDataChangeListener *observer = new MockAppDataChangeListener();
    auto ret = appPipeMgr->StopWatchDataChange(observer, pipeInfo);
    EXPECT_EQ(Status::ERROR, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_StopWatchDataChange_003
 * @tc.desc: test NativeAppPipeMgrTest StopWatchDataChange.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_StopWatchDataChange_003, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipId02" };
    AppPipeMgr appPipeMgr;
    auto ret = appPipeMgr.Start(pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    MockAppDataChangeListener observer;
    ret = appPipeMgr.StartWatchDataChange(&observer, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = appPipeMgr.StopWatchDataChange(&observer, pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_Start_001
 * @tc.desc: test NativeAppPipeMgrTest Start. pipInfo is empty
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_Start_001, TestSize.Level1)
{
    PipeInfo pipeInfo = {};
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->Start(pipeInfo);
    EXPECT_EQ(Status::INVALID_ARGUMENT, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_Start_002
 * @tc.desc: test NativeAppPipeMgrTest Start. invalid pipInfo
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_Start_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "INVALID_SESSION_NAME" };
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->Start(pipeInfo);
    EXPECT_EQ(Status::ILLEGAL_STATE, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_Start_003
 * @tc.desc: test NativeAppPipeMgrTest Start.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_Start_003, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo01" };
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->Start(pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = appPipeMgr->Stop(pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_Start_004
 * @tc.desc: test NativeAppPipeMgrTest Start. repeat start
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_Start_004, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo01" };
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->Start(pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = appPipeMgr->Start(pipeInfo);
    EXPECT_EQ(Status::REPEATED_REGISTER, ret);
    ret = appPipeMgr->Stop(pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_Stop_001
 * @tc.desc: test NativeAppPipeMgrTest Stop. pipInfo not found.
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_Stop_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo01" };
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->Stop(pipeInfo);
    EXPECT_EQ(Status::KEY_NOT_FOUND, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_Stop_002
 * @tc.desc: test NativeAppPipeMgrTest Stop. RemoveSessionServer failed
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_Stop_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "REMOVE_FAILED_SESSION_NAME" };
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->Start(pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = appPipeMgr->Stop(pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_SendData_001
 * @tc.desc: test NativeAppPipeMgrTest SendData. input is invalid
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_SendData_001, TestSize.Level1)
{
    PipeInfo pipeInfo = {};
    DeviceId deviceId = { "devideId01" };
    uint32_t size = 1;
    uint8_t tmpNum = 1;
    uint8_t *ptr = &tmpNum;
    const DataInfo dataInfo = { const_cast<uint8_t *>(ptr), size };
    MessageInfo messageInfo = { MessageType::DEFAULT };
    AppPipeMgr appPipeMgr;
    // pipeInfo is empty
    auto ret = appPipeMgr.SendData(pipeInfo, deviceId, dataInfo, size, messageInfo);
    EXPECT_EQ(Status::ERROR, ret);

    // deviceId is empty
    PipeInfo pipeInfo1 = { "pipeInfo" };
    DeviceId deviceId1 = { "" };
    ret = appPipeMgr.SendData(pipeInfo1, deviceId1, dataInfo, size, messageInfo);
    EXPECT_EQ(Status::ERROR, ret);

    // dataInfo.length is less than or equal to 0
    DeviceId deviceId2 = { "devideId01" };
    const DataInfo dataInfo1 = { const_cast<uint8_t *>(ptr), 0 };
    ret = appPipeMgr.SendData(pipeInfo1, deviceId2, dataInfo1, size, messageInfo);
    EXPECT_EQ(Status::ERROR, ret);

    // dataInfo.length exceeds limit
    static const int MAX_TRANSFER_SIZE = 1024 * 1024 * 5;
    const DataInfo dataInfo2 = { const_cast<uint8_t *>(ptr), MAX_TRANSFER_SIZE + 1 };
    ret = appPipeMgr.SendData(pipeInfo1, deviceId2, dataInfo2, size, messageInfo);
    EXPECT_EQ(Status::ERROR, ret);

    // dataInfo.data is nullptr
    const DataInfo dataInfo3 = { nullptr, size };
    ret = appPipeMgr.SendData(pipeInfo1, deviceId2, dataInfo3, size, messageInfo);
    EXPECT_EQ(Status::ERROR, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_SendData_002
 * @tc.desc: test NativeAppPipeMgrTest SendData. pipInfo not found
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_SendData_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo02" };
    DeviceId deviceId = { "devideId02" };
    uint32_t size = 1;
    uint8_t tmpNum = 1;
    uint8_t *ptr = &tmpNum;
    const DataInfo dataInfo = { const_cast<uint8_t *>(ptr), size };
    MessageInfo messageInfo = { MessageType::DEFAULT };
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->SendData(pipeInfo, deviceId, dataInfo, size, messageInfo);
    EXPECT_EQ(Status::KEY_NOT_FOUND, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_SendData_003
 * @tc.desc: test NativeAppPipeMgrTest SendData. pipInfo not found
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_SendData_003, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo02" };
    DeviceId deviceId = { "devideId02" };
    uint32_t size = 1;
    uint8_t tmpNum = 1;
    uint8_t *ptr = &tmpNum;
    const DataInfo dataInfo = { const_cast<uint8_t *>(ptr), size };
    MessageInfo messageInfo = { MessageType::DEFAULT };
    AppPipeMgr appPipeMgr;
    Status ret = appPipeMgr.Start(pipeInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
    ret = appPipeMgr.SendData(pipeInfo, deviceId, dataInfo, size, messageInfo);
    EXPECT_EQ(Status::SUCCESS, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_IsSameStartedOnPeer_001
 * @tc.desc: test NativeAppPipeMgrTest IsSameStartedOnPeer. pipInfo or deviceId is empty
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_IsSameStartedOnPeer_001, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo01" };
    DeviceId deviceId = {};
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->IsSameStartedOnPeer(pipeInfo, deviceId);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_IsSameStartedOnPeer_002
 * @tc.desc: test NativeAppPipeMgrTest IsSameStartedOnPeer. pipInfo not found
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_IsSameStartedOnPeer_002, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo02" };
    DeviceId deviceId = { "deviceId02" };
    AppPipeMgr *appPipeMgr = new AppPipeMgr();
    auto ret = appPipeMgr->IsSameStartedOnPeer(pipeInfo, deviceId);
    EXPECT_EQ(false, ret);
}

/**
 * @tc.name: NativeAppPipeMgrTest_IsSameStartedOnPeer_003
 * @tc.desc: test NativeAppPipeMgrTest IsSameStartedOnPeer. pipInfo not found
 * @tc.type: FUNC
 */
HWTEST_F(NativeAppPipeMgrTest, NativeAppPipeMgrTest_IsSameStartedOnPeer_003, TestSize.Level1)
{
    PipeInfo pipeInfo = { "pipInfo02" };
    DeviceId deviceId = { "deviceId02" };
    AppPipeMgr appPipeMgr;
    appPipeMgr.Start(pipeInfo);
    auto ret = appPipeMgr.IsSameStartedOnPeer(pipeInfo, deviceId);
    EXPECT_EQ(true, ret);
}

/**
 * @tc.name: AppPipeMgr_Stop_NormalCase_ShouldSuccess
 * @tc.desc: Verify Stop function works correctly in normal case
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(NativeAppPipeMgrTest, Stop_NormalCase_ShouldSuccess, TestSize.Level1)
{
    AppPipeMgr pipeMgr;
    PipeInfo pipeInfo;
    pipeInfo.pipeId = "test_pipe_001";

    Status startStatus = pipeMgr.Start(pipeInfo);
    EXPECT_EQ(startStatus, Status::SUCCESS);

    Status stopStatus = pipeMgr.Stop(pipeInfo);

    EXPECT_EQ(stopStatus, Status::SUCCESS);
}

/**
 * @tc.name: AppPipeMgr_Stop_PipeNotFound_ShouldReturnKeyNotFound
 * @tc.desc: Verify Stop returns KEY_NOT_FOUND when pipeId not exists
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(NativeAppPipeMgrTest, Stop_PipeNotFound_ShouldReturnKeyNotFound, TestSize.Level1)
{
    AppPipeMgr pipeMgr;
    PipeInfo pipeInfo;
    pipeInfo.pipeId = "non_existent_pipe";

    Status status = pipeMgr.Stop(pipeInfo);

    EXPECT_EQ(status, Status::KEY_NOT_FOUND);
}

/**
 * @tc.name: AppPipeMgr_Stop_RemoveSessionServerFailed_ShouldReturnError
 * @tc.desc: Verify Stop returns ERROR when RemoveSessionServer fails
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(NativeAppPipeMgrTest, Stop_RemoveSessionServerFailed_ShouldReturnError, TestSize.Level1)
{
    AppPipeMgr pipeMgr;
    PipeInfo pipeInfo;
    pipeInfo.pipeId = "test_pipe_fail_remove";

    {
        std::lock_guard<std::mutex> lock(pipeMgr.dataBusMapMutex_);
        pipeMgr.dataBusMap_[pipeInfo.pipeId] = nullptr;
    }

    Status status = pipeMgr.Stop(pipeInfo);
    EXPECT_EQ(status, Status::ERROR);
}

/**
 * @tc.name: AppPipeMgr_Stop_ConcurrentAccess_ShouldThreadSafe
 * @tc.desc: Verify Stop function is thread-safe with concurrent access
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(NativeAppPipeMgrTest, Stop_ConcurrentAccess_ShouldThreadSafe, TestSize.Level2)
{
    AppPipeMgr pipeMgr;
    constexpr int THREAD_COUNT = 5;
    std::vector<std::thread> threads;
    std::vector<Status> results(THREAD_COUNT);

    // Setup multiple pipes first
    for (int i = 0; i < THREAD_COUNT; ++i) {
        PipeInfo pipeInfo;
        pipeInfo.pipeId = "concurrent_pipe_" + std::to_string(i);
        pipeMgr.Start(pipeInfo);
    }

    // Concurrent stop operations
    for (int i = 0; i < THREAD_COUNT; ++i) {
        threads.emplace_back([&, i]() {
            PipeInfo pipeInfo;
            pipeInfo.pipeId = "concurrent_pipe_" + std::to_string(i);
            results[i] = pipeMgr.Stop(pipeInfo);
        });
    }

    for (auto &thread : threads) {
        thread.join();
    }

    // All stops should succeed
    for (int i = 0; i < THREAD_COUNT; ++i) {
        EXPECT_EQ(results[i], Status::SUCCESS);
    }
}

/**
 * @tc.name: AppPipeMgr_Stop_EmptyPipeId_ShouldWorkNormally
 * @tc.desc: Verify Stop handles empty pipeId correctly (though Start would reject it)
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(NativeAppPipeMgrTest, Stop_EmptyPipeId_ShouldWorkNormally, TestSize.Level1)
{
    AppPipeMgr pipeMgr;
    PipeInfo pipeInfo;
    pipeInfo.pipeId = ""; // Empty pipe ID

    Status status = pipeMgr.Stop(pipeInfo);

    // Should return KEY_NOT_FOUND since empty ID wouldn't be in the map
    EXPECT_EQ(status, Status::KEY_NOT_FOUND);
}

/**
 * @tc.name: AppPipeMgr_Stop_AfterMultipleStarts_ShouldSuccess
 * @tc.desc: Verify Stop works correctly after pipe has been started multiple times
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(NativeAppPipeMgrTest, Stop_AfterMultipleStarts_ShouldSuccess, TestSize.Level1)
{
    AppPipeMgr pipeMgr;
    PipeInfo pipeInfo;
    pipeInfo.pipeId = "multi_start_pipe";

    // Start multiple times (though second start should fail with REPEATED_REGISTER)
    pipeMgr.Start(pipeInfo);
    Status secondStart = pipeMgr.Start(pipeInfo);
    EXPECT_EQ(secondStart, Status::REPEATED_REGISTER);

    // Stop should still work
    Status stopStatus = pipeMgr.Stop(pipeInfo);
    EXPECT_EQ(stopStatus, Status::SUCCESS);

    // Verify pipe is removed
    Status secondStop = pipeMgr.Stop(pipeInfo);
    EXPECT_EQ(secondStop, Status::KEY_NOT_FOUND);
}

/**
 * @tc.name: AppPipeMgr_Stop_MemoryCleanup_ShouldReleaseResources
 * @tc.desc: Verify Stop properly cleans up resources and memory
 * @tc.type: FUNC
 * @tc.require:
 * @tc.author:
 */
HWTEST_F(NativeAppPipeMgrTest, Stop_MemoryCleanup_ShouldReleaseResources, TestSize.Level1)
{
    AppPipeMgr pipeMgr;
    PipeInfo pipeInfo;
    pipeInfo.pipeId = "cleanup_test_pipe";

    // Start and get initial handler reference count
    pipeMgr.Start(pipeInfo);

    std::shared_ptr<AppPipeHandler> handlerBeforeStop;
    {
        std::lock_guard<std::mutex> lock(pipeMgr.dataBusMapMutex_);
        auto it = pipeMgr.dataBusMap_.find(pipeInfo.pipeId);
        handlerBeforeStop = it->second;
    }

    // Stop should remove from map and potentially reduce ref count
    Status status = pipeMgr.Stop(pipeInfo);
    EXPECT_EQ(status, Status::SUCCESS);

    // Verify pipe is removed from map
    {
        std::lock_guard<std::mutex> lock(pipeMgr.dataBusMapMutex_);
        auto it = pipeMgr.dataBusMap_.find(pipeInfo.pipeId);
        EXPECT_EQ(it, pipeMgr.dataBusMap_.end());
    }
}
} // namespace
