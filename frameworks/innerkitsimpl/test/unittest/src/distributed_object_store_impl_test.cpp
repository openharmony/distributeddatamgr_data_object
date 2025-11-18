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

#include <thread>
#include <unordered_set>

#include "hitrace.h"
#include "distributed_object_impl.h"
#include "distributed_objectstore_impl.h"
#include "objectstore_errors.h"
#include "softbus_adapter.h"
#include "string_utils.h"
#include "asset_change_timer.h"
#include "object_radar_reporter.h"
#include "mock_object_watcher.h"

using namespace testing::ext;
using namespace OHOS::ObjectStore;
using namespace OHOS;
using namespace std;

namespace {
class DistributedObjectStoreImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DistributedObjectStoreImplTest::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void DistributedObjectStoreImplTest::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void DistributedObjectStoreImplTest::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void DistributedObjectStoreImplTest::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: OnChanged_001
 * @tc.desc: Normal test for OnChanged
 * @tc.type: FUNC
 */

HWTEST_F(DistributedObjectStoreImplTest, OnChanged_001, TestSize.Level0)
{
    string sessionId = "sessionId";
    std::vector<std::string> changedData = {};
    bool enableTransfer = false;
    std::shared_ptr<MockObjectWatcher> objectWatcher = std::make_shared<MockObjectWatcher>();
    WatcherProxy watcherProxy(objectWatcher, sessionId);
    watcherProxy.objectWatcher_ = nullptr;
    watcherProxy.OnChanged(sessionId, changedData, enableTransfer);
    EXPECT_EQ(watcherProxy.objectWatcher_, nullptr);
}

/**
 * @tc.name: FindChangedAssetKey_001
 * @tc.desc: Abnormal test for FindChangedAssetKey, changedKey.size() is less than MODIFY_TIME_SUFFIX
 * @tc.type: FUNC
 */

HWTEST_F(DistributedObjectStoreImplTest, FindChangedAssetKey_001, TestSize.Level0)
{
    string changedKey = "sessionId.";
    string assetKey = "assetKey";
    std::shared_ptr<MockObjectWatcher> objectWatcher = std::make_shared<MockObjectWatcher>();
    WatcherProxy watcherProxy(objectWatcher, changedKey);
    bool ret = watcherProxy.FindChangedAssetKey(changedKey, assetKey);
    EXPECT_EQ(ret, false);
}

/**
 * @tc.name: SetProgressNotifier001
 * @tc.desc: test SetProgressNotifier.
 * @tc.type: FUNC
 */

HWTEST_F(DistributedObjectStoreImplTest, SetProgressNotifier001, TestSize.Level0)
{
    std::string bundleName = "default";
    std::string sessionId = "123456";
    auto objectStore = new DistributedObjectStoreImpl(nullptr);
    auto progressNotifierPtr = std::shared_ptr<ProgressNotifier>();
    bool ret = objectStore->SetProgressNotifier(progressNotifierPtr);
    EXPECT_EQ(ret, true);
}
}
