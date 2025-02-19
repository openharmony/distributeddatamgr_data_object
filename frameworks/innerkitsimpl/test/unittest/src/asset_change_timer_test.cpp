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

#include "asset_change_timer.h"

#include "client_adaptor.h"
#include "logger.h"
#include "objectstore_errors.h"
#include "js_watcher.h"
#include "distributed_objectstore_impl.h"
#include "app_types.h"
#include "flat_object_storage_engine.h"
#include "distributed_object_impl.h"

using namespace testing::ext;
using namespace OHOS::ObjectStore;
using namespace OHOS;
using namespace std;

namespace {
static constexpr uint32_t WAIT_INTERVAL = 100;
static constexpr char ASSET_SEPARATOR = '#';
class AssetChangeTimerTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void AssetChangeTimerTest::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void AssetChangeTimerTest::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void AssetChangeTimerTest::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void AssetChangeTimerTest::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: StartTimer_001
 * @tc.desc: Normal test for StartTimer
 * @tc.type: FUNC
 */

HWTEST_F(AssetChangeTimerTest, StartTimer_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    string assetKey = "assetKey1";
    std::weak_ptr<JSWatcher> watcher;
    const std::string bundleName = "bundleName";
    std::shared_ptr<WatcherImpl> watcherImpl = std::make_shared<WatcherImpl>(watcher);
    FlatObjectStore *flatObjectStore = nullptr;
    AssetChangeTimer assetChangeTimer(flatObjectStore);
    assetChangeTimer.StartTimer(sessionId, assetKey, watcherImpl);
    EXPECT_EQ(assetChangeTimer.assetChangeTasks_[sessionId + ASSET_SEPARATOR + assetKey],
        assetChangeTimer.executor_->Reset(assetChangeTimer.assetChangeTasks_[sessionId +
        ASSET_SEPARATOR + assetKey], std::chrono::milliseconds(WAIT_INTERVAL)));
}

/**
 * @tc.name: StopTimer_001
 * @tc.desc: Normal test for StopTimer
 * @tc.type: FUNC
 */

HWTEST_F(AssetChangeTimerTest, StopTimer_001, TestSize.Level1)
{
    string sessionId = "sessionId2";
    string assetKey = "assetKey2";
    FlatObjectStore *flatObjectStore = nullptr;
    AssetChangeTimer assetChangeTimer(flatObjectStore);
    std::shared_ptr<ExecutorPool> executor_ = std::make_shared<ExecutorPool>(2, 3);
    assetChangeTimer.StopTimer(sessionId, assetKey);
    EXPECT_TRUE(assetChangeTimer.assetChangeTasks_.empty());
}

/**
 * @tc.name: GetInstance_001
 * @tc.desc: Normal test for GetInstance
 * @tc.type: FUNC
 */

HWTEST_F(AssetChangeTimerTest, GetInstance_001, TestSize.Level1)
{
    FlatObjectStore *flatObjectStore = nullptr;
    AssetChangeTimer assetChangeTimer(flatObjectStore);
    auto ret = assetChangeTimer.GetInstance(flatObjectStore);
    EXPECT_NE(ret, nullptr);
}

/**
 * @tc.name: OnAssetChanged_001
 * @tc.desc: Normal test for OnAssetChanged
 * @tc.type: FUNC
 */

HWTEST_F(AssetChangeTimerTest, OnAssetChanged_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    string assetKey = "assetKey1";
    std::weak_ptr<JSWatcher> watcher;
    const std::string bundleName = "bundleName";
    std::shared_ptr<WatcherImpl> watcherImpl = std::make_shared<WatcherImpl>(watcher);
    FlatObjectStore *flatObjectStore = nullptr;
    AssetChangeTimer assetChangeTimer(flatObjectStore);
    assetChangeTimer.OnAssetChanged(sessionId, assetKey, watcherImpl);
    EXPECT_EQ(assetChangeTimer.assetChangeTasks_[sessionId + ASSET_SEPARATOR + assetKey],
        assetChangeTimer.executor_->Reset(assetChangeTimer.assetChangeTasks_[sessionId +
        ASSET_SEPARATOR + assetKey], std::chrono::milliseconds(WAIT_INTERVAL)));
}

/**
 * @tc.name: HandleAssetChanges_001
 * @tc.desc: Abnormal test for HandleAssetChanges, no sessionid is stored in delegates_
 * @tc.type: FUNC
 */

HWTEST_F(AssetChangeTimerTest, HandleAssetChanges_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    string assetKey = "assetKey1";
    std::string bundleName = "default";
    FlatObjectStore *flatObjectStore = new FlatObjectStore(bundleName);
    AssetChangeTimer assetChangeTimer(flatObjectStore);
    uint32_t ret = assetChangeTimer.HandleAssetChanges(sessionId, assetKey);
    EXPECT_EQ(ret, ERR_DB_GET_FAIL);
    delete flatObjectStore;
}

/**
 * @tc.name: GetAssetValue_001
 * @tc.desc: Abnormal test for GetAssetValue, no sessionid is stored in delegates_
 * @tc.type: FUNC
 */

HWTEST_F(AssetChangeTimerTest, GetAssetValue_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    string assetKey = "assetKey1";
    Asset assetValue = {
            .version = 0,
            .status = 0,
            .id = "id",
            .name = "1.txt",
            .uri = "file://com.example.myapp/data/dir/1.txt",
            .createTime = "2024/10/26 19:48:00",
            .modifyTime = "2024/10/26 20:10:00",
            .size = "1",
            .hash = "hash",
            .path = "/dir/1.txt",
    };
    std::string bundleName = "default";
    FlatObjectStore *flatObjectStore = new FlatObjectStore(bundleName);
    AssetChangeTimer assetChangeTimer(flatObjectStore);
    uint32_t ret = assetChangeTimer.GetAssetValue(sessionId, assetKey, assetValue);
    EXPECT_FALSE(ret);
    delete flatObjectStore;
}
}
