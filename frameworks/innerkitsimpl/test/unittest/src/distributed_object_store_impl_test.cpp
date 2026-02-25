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

/**
 * @tc.name: CacheObject_001
 * @tc.desc: Normal test for CacheObject
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, CacheObject_001, TestSize.Level1)
{
    string sessionId = "sessionId";
    std::string bundleName = "default";
    FlatObjectStore *flatObjectStore = new FlatObjectStore(bundleName);
    DistributedObjectStoreImpl storeImpl(flatObjectStore);
    auto ret = storeImpl.CacheObject(sessionId, flatObjectStore);
    EXPECT_NE(ret, nullptr);
}

/**
 * @tc.name: RemoveCacheObject_001
 * @tc.desc: Normal test for RemoveCacheObject
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, RemoveCacheObject_001, TestSize.Level1)
{
    string sessionId = "sessionId";
    std::string bundleName = "default";
    FlatObjectStore *flatObjectStore = new FlatObjectStore(bundleName);
    DistributedObjectStoreImpl storeImpl(flatObjectStore);
    storeImpl.CacheObject(sessionId, flatObjectStore);
    storeImpl.RemoveCacheObject(sessionId);
    EXPECT_TRUE(storeImpl.objects_.empty());
}

/**
 * @tc.name: CreateObject_001
 * @tc.desc: Test CreateObject with null flatObjectStore_
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, CreateObject_001, TestSize.Level1)
{
    string sessionId = "sessionId";
    DistributedObjectStoreImpl storeImpl(nullptr);
    auto ret = storeImpl.CreateObject(sessionId);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: CreateObject_002
 * @tc.desc: Test CreateObject with empty sessionId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, CreateObject_002, TestSize.Level1)
{
    string sessionId = "";
    std::string bundleName = "default";
    FlatObjectStore *flatObjectStore = new FlatObjectStore(bundleName);
    DistributedObjectStoreImpl storeImpl(flatObjectStore);
    auto ret = storeImpl.CreateObject(sessionId);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: CreateObject_003
 * @tc.desc: Test CreateObject with status parameter and null flatObjectStore_
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, CreateObject_003, TestSize.Level1)
{
    string sessionId = "sessionId";
    uint32_t status;
    DistributedObjectStoreImpl storeImpl(nullptr);
    auto ret = storeImpl.CreateObject(sessionId, status);
    EXPECT_EQ(ret, nullptr);
    EXPECT_EQ(status, ERR_NULL_OBJECTSTORE);
}

/**
 * @tc.name: DeleteObject_001
 * @tc.desc: Test DeleteObject with null flatObjectStore_
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, DeleteObject_001, TestSize.Level1)
{
    string sessionId = "sessionId";
    DistributedObjectStoreImpl storeImpl(nullptr);
    auto ret = storeImpl.DeleteObject(sessionId);
    EXPECT_EQ(ret, ERR_NULL_OBJECTSTORE);
}

/**
 * @tc.name: Get_001
 * @tc.desc: Test Get when object not found
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, Get_001, TestSize.Level1)
{
    string sessionId = "sessionId";
    DistributedObject *object = nullptr;
    std::string bundleName = "default";
    FlatObjectStore *flatObjectStore = new FlatObjectStore(bundleName);
    DistributedObjectStoreImpl storeImpl(flatObjectStore);
    auto ret = storeImpl.Get(sessionId, &object);
    EXPECT_EQ(ret, ERR_GET_OBJECT);
}

/**
 * @tc.name: Watch_001
 * @tc.desc: Test Watch with null object
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, Watch_001, TestSize.Level1)
{
    std::shared_ptr<MockObjectWatcher> objectWatcher = std::make_shared<MockObjectWatcher>();
    std::string bundleName = "default";
    FlatObjectStore *flatObjectStore = new FlatObjectStore(bundleName);
    DistributedObjectStoreImpl storeImpl(flatObjectStore);
    auto ret = storeImpl.Watch(nullptr, objectWatcher);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: UnWatch_001
 * @tc.desc: Test UnWatch with null object
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, UnWatch_001, TestSize.Level1)
{
    std::string bundleName = "default";
    FlatObjectStore *flatObjectStore = new FlatObjectStore(bundleName);
    DistributedObjectStoreImpl storeImpl(flatObjectStore);
    auto ret = storeImpl.UnWatch(nullptr);
    EXPECT_EQ(ret, ERR_NULL_OBJECT);
}

/**
 * @tc.name: UnWatch_002
 * @tc.desc: Test UnWatch with null flatObjectStore_
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, UnWatch_002, TestSize.Level1)
{
    string sessionId = "sessionId";
    std::string bundleName = "default";
    FlatObjectStore *flatObjectStore = new FlatObjectStore(bundleName);
    DistributedObjectImpl *object = new DistributedObjectImpl(sessionId, flatObjectStore);
    DistributedObjectStoreImpl storeImpl(nullptr);
    auto ret = storeImpl.UnWatch(object);
    EXPECT_EQ(ret, ERR_NULL_OBJECTSTORE);
    delete object;
    delete flatObjectStore;
}

/**
 * @tc.name: SetStatusNotifier_001
 * @tc.desc: Test SetStatusNotifier with null instance member flatObjectStore_
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, SetStatusNotifier_001, TestSize.Level1)
{
    std::shared_ptr<StatusNotifier> notifier = nullptr;
    DistributedObjectStoreImpl storeImpl(nullptr);
    auto ret = storeImpl.SetStatusNotifier(notifier);
    EXPECT_EQ(ret, ERR_NULL_OBJECTSTORE);
}

/**
 * @tc.name: SetProgressNotifier_002
 * @tc.desc: Test SetProgressNotifier with null flatObjectStore_
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, SetProgressNotifier_002, TestSize.Level1)
{
    std::shared_ptr<ProgressNotifier> notifier = nullptr;
    DistributedObjectStoreImpl storeImpl(nullptr);
    auto ret = storeImpl.SetProgressNotifier(notifier);
    EXPECT_EQ(ret, ERR_NULL_OBJECTSTORE);
}

/**
 * @tc.name: OnChanged_002
 * @tc.desc: Test OnChanged with enableTransfer true and empty transferKeys
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, OnChanged_002, TestSize.Level1)
{
    string sessionId = "sessionId";
    std::vector<std::string> changedData = {"key1", "key2"};
    bool enableTransfer = true;
    std::shared_ptr<MockObjectWatcher> objectWatcher = std::make_shared<MockObjectWatcher>();
    WatcherProxy watcherProxy(objectWatcher, sessionId);
    watcherProxy.OnChanged(sessionId, changedData, enableTransfer);
    EXPECT_NE(watcherProxy.objectWatcher_, nullptr);
}

/**
 * @tc.name: OnChanged_003
 * @tc.desc: Test OnChanged with enableTransfer false
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, OnChanged_003, TestSize.Level1)
{
    string sessionId = "sessionId";
    std::vector<std::string> changedData = {"key1", "key2"};
    bool enableTransfer = false;
    std::shared_ptr<MockObjectWatcher> objectWatcher = std::make_shared<MockObjectWatcher>();
    WatcherProxy watcherProxy(objectWatcher, sessionId);
    watcherProxy.OnChanged(sessionId, changedData, enableTransfer);
    EXPECT_NE(watcherProxy.objectWatcher_, nullptr);
}

/**
 * @tc.name: FindChangedAssetKey_002
 * @tc.desc: Test FindChangedAssetKey with MODIFY_TIME_SUFFIX
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, FindChangedAssetKey_002, TestSize.Level1)
{
    string changedKey = "asset.modifyTime";
    string assetKey;
    std::shared_ptr<MockObjectWatcher> objectWatcher = std::make_shared<MockObjectWatcher>();
    WatcherProxy watcherProxy(objectWatcher, "sessionId");
    bool ret = watcherProxy.FindChangedAssetKey(changedKey, assetKey);
    EXPECT_TRUE(ret);
    EXPECT_EQ(assetKey, "asset");
}

/**
 * @tc.name: FindChangedAssetKey_003
 * @tc.desc: Test FindChangedAssetKey with SIZE_SUFFIX
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, FindChangedAssetKey_003, TestSize.Level1)
{
    string changedKey = "asset.size";
    string assetKey;
    std::shared_ptr<MockObjectWatcher> objectWatcher = std::make_shared<MockObjectWatcher>();
    WatcherProxy watcherProxy(objectWatcher, "sessionId");
    bool ret = watcherProxy.FindChangedAssetKey(changedKey, assetKey);
    EXPECT_TRUE(ret);
    EXPECT_EQ(assetKey, "asset");
}

/**
 * @tc.name: SetAssetChangeCallBack_001
 * @tc.desc: Normal test for SetAssetChangeCallBack
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, SetAssetChangeCallBack_001, TestSize.Level1)
{
    string sessionId = "sessionId";
    std::shared_ptr<MockObjectWatcher> objectWatcher = std::make_shared<MockObjectWatcher>();
    WatcherProxy watcherProxy(objectWatcher, sessionId);
    watcherProxy.SetAssetChangeCallBack(nullptr);
    EXPECT_EQ(watcherProxy.assetChangeCallback_, nullptr);
}

/**
 * @tc.name: GetInstance_001
 * @tc.desc: Test GetInstance with empty
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, GetInstance_001, TestSize.Level1)
{
    string bundleName = "";
    auto ret = DistributedObjectStore::GetInstance(bundleName);
    EXPECT_EQ(ret, nullptr);
}

/**
 * @tc.name: StatusNotifierProxy_OnChanged_001
 * @tc.desc: Test StatusNotifierProxy::OnChanged with null notifier
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, StatusNotifierProxy_OnChanged_001, TestSize.Level1)
{
    string sessionId = "sessionId";
    string networkId = "networkId";
    string onlineStatus = "online";
    std::shared_ptr<StatusNotifier> notifier = nullptr;
    StatusNotifierProxy proxy(notifier);
    proxy.OnChanged(sessionId, networkId, onlineStatus);
    EXPECT_EQ(proxy.notifier, nullptr);
}

/**
 * @tc.name: ProgressNotifierProxy_OnChanged_001
 * @tc.desc: Test ProgressNotifierProxy::OnChanged with null notifier
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, ProgressNotifierProxy_OnChanged_001, TestSize.Level1)
{
    string sessionId = "sessionId";
    int32_t progress = 50;
    std::shared_ptr<ProgressNotifier> notifier = nullptr;
    ProgressNotifierProxy proxy(notifier);
    proxy.OnChanged(sessionId, progress);
    EXPECT_EQ(proxy.notifier, nullptr);
}

/**
 * @tc.name: ProgressNotifierProxy_OnChanged_002
 * @tc.desc: Test ProgressNotifierProxy::OnChanged with progress 0
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, ProgressNotifierProxy_OnChanged_002, TestSize.Level1)
{
    string sessionId = "sessionId";
    int32_t progress = 0;
    std::shared_ptr<ProgressNotifier> notifier = nullptr;
    ProgressNotifierProxy proxy(notifier);
    proxy.OnChanged(sessionId, progress);
    EXPECT_EQ(proxy.notifier, nullptr);
}

/**
 * @tc.name: ProgressNotifierProxy_OnChanged_003
 * @tc.desc: Test ProgressNotifierProxy::OnChanged with progress 100
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, ProgressNotifierProxy_OnChanged_003, TestSize.Level1)
{
    string sessionId = "sessionId";
    int32_t progress = 100;
    std::shared_ptr<ProgressNotifier> notifier = nullptr;
    ProgressNotifierProxy proxy(notifier);
    proxy.OnChanged(sessionId, progress);
    EXPECT_EQ(proxy.notifier, nullptr);
}

/**
 * @tc.name: ProgressNotifierProxy_OnChanged_004
 * @tc.desc: Test ProgressNotifierProxy::OnChanged with negative progress
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, ProgressNotifierProxy_OnChanged_004, TestSize.Level1)
{
    string sessionId = "sessionId";
    int32_t progress = -50;
    std::shared_ptr<ProgressNotifier> notifier = nullptr;
    ProgressNotifierProxy proxy(notifier);
    proxy.OnChanged(sessionId, progress);
    EXPECT_EQ(proxy.notifier, nullptr);
}

/**
 * @tc.name: StatusNotifierProxy_OnChanged_002
 * @tc.desc: Test StatusNotifierProxy::OnChanged with empty sessionId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, StatusNotifierProxy_OnChanged_002, TestSize.Level1)
{
    string sessionId = "";
    string networkId = "networkId";
    string onlineStatus = "online";
    std::shared_ptr<StatusNotifier> notifier = nullptr;
    StatusNotifierProxy proxy(notifier);
    proxy.OnChanged(sessionId, networkId, onlineStatus);
    EXPECT_EQ(proxy.notifier, nullptr);
}

/**
 * @tc.name: StatusNotifierProxy_OnChanged_003
 * @tc.desc: Test StatusNotifierProxy::OnChanged with empty networkId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, StatusNotifierProxy_OnChanged_003, TestSize.Level1)
{
    string sessionId = "sessionId";
    string networkId = "";
    string onlineStatus = "online";
    std::shared_ptr<StatusNotifier> notifier = nullptr;
    StatusNotifierProxy proxy(notifier);
    proxy.OnChanged(sessionId, networkId, onlineStatus);
    EXPECT_EQ(proxy.notifier, nullptr);
}

/**
 * @tc.name: StatusNotifierProxy_OnChanged_004
 * @tc.desc: Test StatusNotifierProxy::OnChanged with empty onlineStatus
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, StatusNotifierProxy_OnChanged_004, TestSize.Level1)
{
    string sessionId = "sessionId";
    string networkId = "networkId";
    string onlineStatus = "";
    std::shared_ptr<StatusNotifier> notifier = nullptr;
    StatusNotifierProxy proxy(notifier);
    proxy.OnChanged(sessionId, networkId, onlineStatus);
    EXPECT_EQ(proxy.notifier, nullptr);
}

/**
 * @tc.name: FindChangedAssetKey_004
 * @tc.desc: Test FindChangedAssetKey with empty changedKey
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, FindChangedAssetKey_004, TestSize.Level1)
{
    string changedKey = "";
    string assetKey;
    std::shared_ptr<MockObjectWatcher> objectWatcher = std::make_shared<MockObjectWatcher>();
    WatcherProxy watcherProxy(objectWatcher, "sessionId");
    auto ret = watcherProxy.FindChangedAssetKey(changedKey, assetKey);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: FindChangedAssetKey_005
 * @tc.desc: Test FindChangedAssetKey with non-matching suffix
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, FindChangedAssetKey_005, TestSize.Level1)
{
    string changedKey = "asset.someOtherSuffix";
    string assetKey;
    std::shared_ptr<MockObjectWatcher> objectWatcher = std::make_shared<MockObjectWatcher>();
    WatcherProxy watcherProxy(objectWatcher, "sessionId");
    auto ret = watcherProxy.FindChangedAssetKey(changedKey, assetKey);
    EXPECT_FALSE(ret);
}

/**
 * @tc.name: CacheObject_003
 * @tc.desc: Test CacheObject with empty sessionId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, CacheObject_003, TestSize.Level1)
{
    string sessionId = "";
    std::string bundleName = "default";
    FlatObjectStore *flatObjectStore = new FlatObjectStore(bundleName);
    DistributedObjectStoreImpl storeImpl(flatObjectStore);
    auto ret = storeImpl.CacheObject(sessionId, flatObjectStore);
    EXPECT_NE(ret, nullptr);
}

/**
 * @tc.name: DeleteObject_002
 * @tc.desc: Test DeleteObject with empty sessionId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, DeleteObject_002, TestSize.Level1)
{
    string sessionId = "";
    DistributedObjectStoreImpl storeImpl(nullptr);
    auto ret = storeImpl.DeleteObject(sessionId);
    EXPECT_EQ(ret, ERR_NULL_OBJECTSTORE);
}

/**
 * @tc.name: Get_002
 * @tc.desc: Test Get with empty sessionId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectStoreImplTest, Get_002, TestSize.Level1)
{
    string sessionId = "";
    DistributedObject *object = nullptr;
    DistributedObjectStoreImpl storeImpl(nullptr);
    auto ret = storeImpl.Get(sessionId, &object);
    EXPECT_EQ(ret, ERR_GET_OBJECT);
}
}
