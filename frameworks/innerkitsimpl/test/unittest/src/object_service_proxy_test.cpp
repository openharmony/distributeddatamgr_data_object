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

#include "object_service_proxy.h"
#include "objectstore_errors.h"

using namespace testing::ext;
using namespace OHOS::ObjectStore;
using namespace OHOS::DistributedObject;
using namespace OHOS;
using namespace std;

namespace {
class ObjectServiceProxyTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ObjectServiceProxyTest::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void ObjectServiceProxyTest::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void ObjectServiceProxyTest::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void ObjectServiceProxyTest::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: OnAssetChanged_001
 * @tc.desc: Abnormal test for OnAssetChanged, impl is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ObjectServiceProxyTest, OnAssetChanged_001, TestSize.Level1)
{
    string bundleName = "com.example.myapplication";
    string sessionId = "123456";
    string deviceId = "devideId";
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
    sptr<IRemoteObject> impl;
    ObjectServiceProxy objectServiceProxy(impl);
    int32_t ret = objectServiceProxy.OnAssetChanged(bundleName, sessionId, deviceId, assetValue);
    EXPECT_EQ(ret, OHOS::ObjectStore::ERR_IPC);
}

/**
 * @tc.name: BindAssetStore_001
 * @tc.desc: Abnormal test for BindAssetStore, impl is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ObjectServiceProxyTest, BindAssetStore_001, TestSize.Level1)
{
    string bundleName = "com.example.myapplication";
    string sessionId = "123456";
    Asset asset = {
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
    sptr<IRemoteObject> impl;
    ObjectServiceProxy objectServiceProxy(impl);
    int32_t ret = objectServiceProxy.BindAssetStore(bundleName, sessionId, asset, bindInfo);
    EXPECT_EQ(ret, OHOS::ObjectStore::ERR_IPC);
}

/**
 * @tc.name: RegisterProgressObserver_001
 * @tc.desc: Abnormal test for RegisterProgressObserver, impl is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ObjectServiceProxyTest, RegisterProgressObserver_001, TestSize.Level1)
{
    string bundleName = "com.example.myapplication";
    string sessionId = "123456";
    sptr<IRemoteObject> impl;
    ObjectServiceProxy objectServiceProxy(impl);
    int32_t ret = objectServiceProxy.RegisterProgressObserver(bundleName, sessionId, impl);
    EXPECT_EQ(ret, OHOS::ObjectStore::ERR_IPC);
}

/**
 * @tc.name: UnregisterProgressObserver_001
 * @tc.desc: Abnormal test for UnregisterProgressObserver, impl is nullptr
 * @tc.type: FUNC
 */
HWTEST_F(ObjectServiceProxyTest, UnregisterProgressObserver_001, TestSize.Level1)
{
    string bundleName = "com.example.myapplication";
    string sessionId = "123456";
    sptr<IRemoteObject> impl;
    ObjectServiceProxy objectServiceProxy(impl);
    int32_t ret = objectServiceProxy.UnregisterProgressObserver(bundleName, sessionId);
    EXPECT_EQ(ret, OHOS::ObjectStore::ERR_IPC);
}
}
