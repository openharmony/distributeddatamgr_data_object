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

using namespace testing::ext;
using namespace OHOS::ObjectStore;
using namespace OHOS;
using namespace std;

namespace {
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
    EXPECT_EQ(assetChangeTimer.assetChangeTasks_[sessionId + assetChangeTimer.ASSET_SEPARATOR + assetKey],
        assetChangeTimer.executor_->Reset(assetChangeTimer.assetChangeTasks_[sessionId +
        assetChangeTimer.ASSET_SEPARATOR + assetKey], std::chrono::milliseconds(assetChangeTimer.WAIT_INTERVAL)));
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
}
