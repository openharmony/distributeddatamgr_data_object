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

#include "distributed_object_impl.h"

#include "hitrace.h"
#include "objectstore_errors.h"
#include "string_utils.h"
#include "dev_manager.h"
#include "bytes_utils.h"
#include "object_radar_reporter.h"
#include "mock_permission_utils.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

using namespace testing::ext;
using namespace OHOS::ObjectStore;
using namespace OHOS;
using namespace OHOS::Security::AccessToken;
using namespace std;

namespace {
static ObjectStore::MockNativeToken *g_mock = nullptr;
static uint64_t g_selfTokenId = 0;

void GrantPermissionNative()
{
    const char *perms[] = {"ohos.permission.DISTRIBUTED_DATASYNC"};
    TokenInfoParams infoInstance = {
        .dcapsNum = 0,
        .permsNum = 1,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "distributed_object",
        .aplStr = "system_basic",
    };
    auto tokenId = GetAccessTokenId(&infoInstance);
    SetSelfTokenID(tokenId);
    AccessTokenKit::ReloadNativeTokenInfo();
}

class DistributedObjectImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
    FlatObjectStore *flatObjectStore_;
};

void DistributedObjectImplTest::SetUpTestCase(void)
{
    g_selfTokenId = GetSelfTokenID();
    ObjectStoreTestUtils::SetTestEvironment(g_selfTokenId);
    g_mock = new (std::nothrow) MockNativeToken("foundation");
}

void DistributedObjectImplTest::TearDownTestCase(void)
{
    if (g_mock != nullptr) {
        delete g_mock;
        g_mock = nullptr;
    }
    ObjectStoreTestUtils::ResetTestEvironment();
}

void DistributedObjectImplTest::SetUp(void)
{
    GrantPermissionNative();
    flatObjectStore_ = new FlatObjectStore("default");
}

void DistributedObjectImplTest::TearDown(void)
{
    if (flatObjectStore_ != nullptr) {
        delete flatObjectStore_;
        flatObjectStore_ = nullptr;
    }
}

/**
 * @tc.name: BindAssetStore_001
 * @tc.desc: Abnormal test for BindAssetStore, data is not stored
 * @tc.type: FUNC
 */

HWTEST_F(DistributedObjectImplTest, BindAssetStore_001, TestSize.Level0)
{
    string assetKey = "assetKey";
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
    std::string bundleName = "default";
    FlatObjectStore *flatObjectStore = new FlatObjectStore(bundleName);
    DistributedObjectImpl distributedObjectImpl(bundleName, flatObjectStore);
    uint32_t ret = distributedObjectImpl.BindAssetStore(assetKey, bindInfo);
    EXPECT_NE(ret, SUCCESS);
    delete flatObjectStore;
}

/**
 * @tc.name: GetType_001
 * @tc.desc: Normal test for GetType
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectImplTest, GetType_001, TestSize.Level1)
{
    string sessionId = "sessionIdGetType";
    string key = "keyGetType";
    Type type;
    flatObjectStore_->CreateObject(sessionId);
    DistributedObjectImpl distributedObjectImpl(sessionId, flatObjectStore_);
    auto ret = distributedObjectImpl.GetType(key, type);
    EXPECT_NE(ret, SUCCESS);
}

/**
 * @tc.name: GetSessionId_001
 * @tc.desc: Normal test for GetSessionId
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectImplTest, GetSessionId_001, TestSize.Level1)
{
    string sessionId = "sessionIdGetSessionId";
    DistributedObjectImpl distributedObjectImpl(sessionId, flatObjectStore_);
    auto ret = distributedObjectImpl.GetSessionId();
    EXPECT_EQ(ret, sessionId);
}

/**
 * @tc.name: Save_001
 * @tc.desc: Normal test for Save
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectImplTest, Save_001, TestSize.Level1)
{
    string sessionId = "sessionIdSave";
    string deviceId = "deviceIdSave";
    flatObjectStore_->CreateObject(sessionId);
    DistributedObjectImpl distributedObjectImpl(sessionId, flatObjectStore_);
    auto ret = distributedObjectImpl.Save(deviceId);
    EXPECT_NE(ret, SUCCESS);
}

/**
 * @tc.name: RevokeSave_001
 * @tc.desc: Normal test for RevokeSave
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectImplTest, RevokeSave_001, TestSize.Level1)
{
    string sessionId = "sessionIdRevokeSave";
    flatObjectStore_->CreateObject(sessionId);
    DistributedObjectImpl distributedObjectImpl(sessionId, flatObjectStore_);
    auto ret = distributedObjectImpl.RevokeSave();
    EXPECT_EQ(ret, SUCCESS);
}

/**
 * @tc.name: GetAssetValue_001
 * @tc.desc: Test GetAssetValue when asset key does not exist
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectImplTest, GetAssetValue_001, TestSize.Level1)
{
    string sessionId = "sessionIdGetAssetValue";
    string assetKey = "assetKeyGetAssetValue";
    Asset assetValue;
    flatObjectStore_->CreateObject(sessionId);
    DistributedObjectImpl distributedObjectImpl(sessionId, flatObjectStore_);
    auto ret = distributedObjectImpl.GetAssetValue(assetKey, assetValue);
    EXPECT_NE(ret, SUCCESS);
}

/**
 * @tc.name: RemovePrefix_001
 * @tc.desc: Test RemovePrefix function
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectImplTest, RemovePrefix_001, TestSize.Level1)
{
    Asset assetValue;
    assetValue.name = "[STRING]test.txt";
    assetValue.uri = "[STRING]file://test.txt";
    assetValue.path = "[STRING]/test/test.txt";
    assetValue.createTime = "[STRING]2024-01-01 00:00:00";
    assetValue.modifyTime = "[STRING]2024-01-01 00:00:01";
    assetValue.size = "[STRING]1024";
    
    flatObjectStore_->CreateObject("sessionIdRemovePrefix");
    DistributedObjectImpl distributedObjectImpl("sessionIdRemovePrefix", flatObjectStore_);
    distributedObjectImpl.RemovePrefix(assetValue);
    
    EXPECT_EQ(assetValue.name, "test.txt");
    EXPECT_EQ(assetValue.uri, "file://test.txt");
    EXPECT_EQ(assetValue.path, "/test/test.txt");
    EXPECT_EQ(assetValue.createTime, "2024-01-01 00:00:00");
    EXPECT_EQ(assetValue.modifyTime, "2024-01-01 00:00:01");
    EXPECT_EQ(assetValue.size, "1024");
    EXPECT_EQ(assetValue.hash, "2024-01-01 00:00:01_1024");
}

/**
 * @tc.name: BindAssetStore_002
 * @tc.desc: Test BindAssetStore with empty fields
 * @tc.type: FUNC
 */
HWTEST_F(DistributedObjectImplTest, BindAssetStore_002, TestSize.Level1)
{
    string assetKey = "assetKeyBindAssetStore002";
    string sessionId = "sessionIdBindAssetStore002";
    AssetBindInfo bindInfo = {
        .storeName = "",
        .tableName = "",
        .primaryKey = {},
        .field = "",
        .assetName = ""
    };
    DistributedObjectImpl distributedObjectImpl(sessionId, flatObjectStore_);
    auto ret = distributedObjectImpl.BindAssetStore(assetKey, bindInfo);
    EXPECT_NE(ret, SUCCESS);
}
}