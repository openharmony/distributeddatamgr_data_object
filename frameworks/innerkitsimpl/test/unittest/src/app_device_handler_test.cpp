/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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
#include <unistd.h>

#include "app_device_handler.h"
#include "app_types.h"

using namespace testing::ext;
using namespace OHOS::ObjectStore;
using namespace OHOS;
using namespace std;
using namespace testing;

namespace {
class AppDeviceHandlerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AppDeviceHandlerTest::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void AppDeviceHandlerTest::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void AppDeviceHandlerTest::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void AppDeviceHandlerTest::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: GetLocalBasicInfo_001
 * @tc.desc: Normal test for GetLocalBasicInfo
 * @tc.type: FUNC
 */

HWTEST_F(AppDeviceHandlerTest, GetLocalBasicInfo_001, TestSize.Level1)
{
    AppDeviceHandler appDeviceHandler;
    appDeviceHandler.softbusAdapter_ = std::make_shared<SoftBusAdapter>();
    ObjectStore::DeviceInfo ret = appDeviceHandler.GetLocalBasicInfo();
    auto ret2 = appDeviceHandler.softbusAdapter_->GetLocalBasicInfo();
    EXPECT_EQ(ret.deviceId, ret2.deviceId);
    EXPECT_EQ(ret.deviceName, ret2.deviceName);
    EXPECT_EQ(ret.deviceType, ret2.deviceType);
}

/**
 * @tc.name: GetRemoteNodesBasicInfo_001
 * @tc.desc: Normal test for GetRemoteNodesBasicInfo
 * @tc.type: FUNC
 */

HWTEST_F(AppDeviceHandlerTest, GetRemoteNodesBasicInfo_001, TestSize.Level1)
{
    AppDeviceHandler appDeviceHandler;
    appDeviceHandler.softbusAdapter_ = std::make_shared<SoftBusAdapter>();
    std::vector<ObjectStore::DeviceInfo> ret = appDeviceHandler.GetRemoteNodesBasicInfo();
    auto ret2 = appDeviceHandler.softbusAdapter_->GetRemoteNodesBasicInfo();
    EXPECT_TRUE(ret.size() == ret2.size());
}

/**
 * @tc.name: GetUuidByNodeId_001
 * @tc.desc: Normal test for GetUuidByNodeId
 * @tc.type: FUNC
 */

HWTEST_F(AppDeviceHandlerTest, GetUuidByNodeId_001, TestSize.Level1)
{
    std::string nodeId = "123";
    AppDeviceHandler appDeviceHandler;
    appDeviceHandler.devManager_ = DevManager::GetInstance();
    std::string ret = appDeviceHandler.GetUuidByNodeId(nodeId);
    EXPECT_EQ(ret, appDeviceHandler.devManager_->GetUuidByNodeId(nodeId));
}
}
