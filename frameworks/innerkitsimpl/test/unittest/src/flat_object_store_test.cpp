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

#include "flat_object_store.h"

#include "accesstoken_kit.h"
#include "block_data.h"
#include "bytes_utils.h"
#include "client_adaptor.h"
#include "distributed_objectstore_impl.h"
#include "ipc_skeleton.h"
#include "logger.h"
#include "object_callback_impl.h"
#include "object_radar_reporter.h"
#include "object_service_proxy.h"
#include "objectstore_errors.h"
#include "softbus_adapter.h"
#include "string_utils.h"

#define OMIT_MULTI_VER

using namespace testing::ext;
using namespace OHOS::ObjectStore;
using namespace OHOS;
using namespace std;

namespace {
class FlatObjectStoreTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void FlatObjectStoreTest::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void FlatObjectStoreTest::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void FlatObjectStoreTest::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void FlatObjectStoreTest::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: BindAssetStore_001
 * @tc.desc: Abnormal test for BindAssetStore, data is not stored
 * @tc.type: FUNC
 */

HWTEST_F(FlatObjectStoreTest, BindAssetStore_001, TestSize.Level0)
{
    string sessionId = "123";
    AssetBindInfo bindInfo = {
        .storeName = "storeName",
        .tableName = "tableName",
        .primaryKey = {
            {"data1", 123},
            {"data2", "test1"}
        },
        .field = "field",
        .assetName = "assetName"
    };
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
    FlatObjectStore flatObjectStore = FlatObjectStore(bundleName);
    uint32_t ret = flatObjectStore.BindAssetStore(sessionId, bindInfo, assetValue);
    EXPECT_NE(ret, SUCCESS);
}

/**
 * @tc.name: GetBundleName_001
 * @tc.desc: Normal test for GetBundleName
 * @tc.type: FUNC
 */

HWTEST_F(FlatObjectStoreTest, GetBundleName_001, TestSize.Level0)
{
    std::string bundleName = "default";
    FlatObjectStore flatObjectStore = FlatObjectStore(bundleName);
    flatObjectStore.bundleName_ = bundleName;
    std::string ret = flatObjectStore.GetBundleName();
    EXPECT_EQ(ret, bundleName);
}

/**
 * @tc.name: CreateObject_001
 * @tc.desc: Test CreateObject when storage engine open fails
 * @tc.type: FUNC
 */
HWTEST_F(FlatObjectStoreTest, CreateObject_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    std::string bundleName = "default";
    FlatObjectStore flatObjectStore(bundleName);
    flatObjectStore.storageEngine_->isOpened_ = false;
    auto ret = flatObjectStore.CreateObject(sessionId);
    EXPECT_EQ(ret, ERR_NO_PERMISSION);
}

/**
 * @tc.name: Delete_001
 * @tc.desc: Test Delete when storage engine is not opened
 * @tc.type: FUNC
 */
HWTEST_F(FlatObjectStoreTest, Delete_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    std::string bundleName = "default";
    FlatObjectStore flatObjectStore(bundleName);
    flatObjectStore.storageEngine_->isOpened_ = false;
    auto ret = flatObjectStore.Delete(sessionId);
    EXPECT_EQ(ret, ERR_DB_NOT_EXIST);
}

/**
 * @tc.name: Watch_001
 * @tc.desc: Test Watch when storage engine is not opened
 * @tc.type: FUNC
 */
HWTEST_F(FlatObjectStoreTest, Watch_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    std::shared_ptr<FlatObjectWatcher> watcher = nullptr;
    std::string bundleName = "default";
    FlatObjectStore flatObjectStore(bundleName);
    flatObjectStore.storageEngine_->isOpened_ = false;
    auto ret = flatObjectStore.Watch(sessionId, watcher);
    EXPECT_EQ(ret, ERR_DB_NOT_EXIST);
}

/**
 * @tc.name: UnWatch_001
 * @tc.desc: Test UnWatch when storage engine is not opened
 * @tc.type: FUNC
 */
HWTEST_F(FlatObjectStoreTest, UnWatch_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    std::string bundleName = "default";
    FlatObjectStore flatObjectStore(bundleName);
    flatObjectStore.storageEngine_->isOpened_ = false;
    auto ret = flatObjectStore.UnWatch(sessionId);
    EXPECT_EQ(ret, ERR_DB_NOT_EXIST);
}

/**
 * @tc.name: Put_001
 * @tc.desc: Test Put when storage engine is not opened
 * @tc.type: FUNC
 */
HWTEST_F(FlatObjectStoreTest, Put_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    string key = "key1";
    std::vector<uint8_t> value = {1, 2, 3};
    std::string bundleName = "default";
    FlatObjectStore flatObjectStore(bundleName);
    flatObjectStore.storageEngine_->isOpened_ = false;
    auto ret = flatObjectStore.Put(sessionId, key, value);
    EXPECT_EQ(ret, ERR_DB_NOT_EXIST);
}

/**
 * @tc.name: Get_001
 * @tc.desc: Test Get when storage engine is not opened
 * @tc.type: FUNC
 */
HWTEST_F(FlatObjectStoreTest, Get_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    string key = "key1";
    Bytes value;
    std::string bundleName = "default";
    FlatObjectStore flatObjectStore(bundleName);
    flatObjectStore.storageEngine_->isOpened_ = false;
    auto ret = flatObjectStore.Get(sessionId, key, value);
    EXPECT_EQ(ret, ERR_DB_NOT_EXIST);
}

/**
 * @tc.name: SetStatusNotifier_001
 * @tc.desc: Test SetStatusNotifier when storage engine is not opened
 * @tc.type: FUNC
 */
HWTEST_F(FlatObjectStoreTest, SetStatusNotifier_001, TestSize.Level1)
{
    std::shared_ptr<StatusWatcher> notifier = nullptr;
    std::string bundleName = "default";
    FlatObjectStore flatObjectStore(bundleName);
    flatObjectStore.storageEngine_->isOpened_ = false;
    auto ret = flatObjectStore.SetStatusNotifier(notifier);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: SetProgressNotifier_001
 * @tc.desc: Test SetProgressNotifier when storage engine is not opened
 * @tc.type: FUNC
 */
HWTEST_F(FlatObjectStoreTest, SetProgressNotifier_001, TestSize.Level1)
{
    std::shared_ptr<ProgressWatcher> notifier = nullptr;
    std::string bundleName = "default";
    FlatObjectStore flatObjectStore(bundleName);
    flatObjectStore.storageEngine_->isOpened_ = false;
    auto ret = flatObjectStore.SetProgressNotifier(notifier);
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: Save_001
 * @tc.desc: Test Save when cacheManager is null
 * @tc.type: FUNC
 */
HWTEST_F(FlatObjectStoreTest, Save_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    string deviceId = "deviceId1";
    std::string bundleName = "default";
    FlatObjectStore flatObjectStore(bundleName);
    flatObjectStore.cacheManager_ = nullptr;
    auto ret = flatObjectStore.Save(sessionId, deviceId);
    EXPECT_EQ(ret, ERR_NULL_PTR);
}

/**
 * @tc.name: RevokeSave_001
 * @tc.desc: Test RevokeSave when cacheManager is null
 * @tc.type: FUNC
 */
HWTEST_F(FlatObjectStoreTest, RevokeSave_001, TestSize.Level1)
{
    string sessionId = "sessionId1";
    std::string bundleName = "default";
    FlatObjectStore flatObjectStore(bundleName);
    flatObjectStore.cacheManager_ = nullptr;
    auto ret = flatObjectStore.RevokeSave(sessionId);
    EXPECT_EQ(ret, ERR_NULL_PTR);
}
}
