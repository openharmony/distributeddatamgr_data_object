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
class ObjectServiceProxyAdditionalTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ObjectServiceProxyAdditionalTest::SetUpTestCase(void)
{
}

void ObjectServiceProxyAdditionalTest::TearDownTestCase(void)
{
}

void ObjectServiceProxyAdditionalTest::SetUp(void)
{
}

void ObjectServiceProxyAdditionalTest::TearDown(void)
{
}

/**
 * @tc.name: ObjectStoreSave_001
 * @tc.desc: Abnormal test for ObjectStoreSave
 * @tc.type: FUNC
 */
HWTEST_F(ObjectServiceProxyAdditionalTest, ObjectStoreSave_001, TestSize.Level1)
{
    string bundleName = "testBundle";
    string sessionId = "testSession";
    string deviceId = "testDevice";
    map<string, vector<uint8_t>> objectData = {
        { "key1", { 1, 2, 3 } }
    };
    sptr<IRemoteObject> impl = nullptr;
    sptr<IRemoteObject> callback = nullptr;
    ObjectServiceProxy proxy(impl);
    auto ret = proxy.ObjectStoreSave(bundleName, sessionId, deviceId, objectData, callback);
    EXPECT_EQ(ret, OHOS::ObjectStore::ERR_IPC);
}

/**
 * @tc.name: ObjectStoreRevokeSave_001
 * @tc.desc: Abnormal test for ObjectStoreRevokeSave
 * @tc.type: FUNC
 */
HWTEST_F(ObjectServiceProxyAdditionalTest, ObjectStoreRevokeSave_001, TestSize.Level1)
{
    string bundleName = "testBundle";
    string sessionId = "testSession";
    sptr<IRemoteObject> impl = nullptr;
    sptr<IRemoteObject> callback = nullptr;
    ObjectServiceProxy proxy(impl);
    auto ret = proxy.ObjectStoreRevokeSave(bundleName, sessionId, callback);
    EXPECT_EQ(ret, OHOS::ObjectStore::ERR_IPC);
}

/**
 * @tc.name: ObjectStoreRetrieve_001
 * @tc.desc: Abnormal test for ObjectStoreRetrieve
 * @tc.type: FUNC
 */
HWTEST_F(ObjectServiceProxyAdditionalTest, ObjectStoreRetrieve_001, TestSize.Level1)
{
    string bundleName = "testBundle";
    string sessionId = "testSession";
    sptr<IRemoteObject> impl = nullptr;
    sptr<IRemoteObject> callback = nullptr;
    ObjectServiceProxy proxy(impl);
    auto ret = proxy.ObjectStoreRetrieve(bundleName, sessionId, callback);
    EXPECT_EQ(ret, OHOS::ObjectStore::ERR_IPC);
}

/**
 * @tc.name: RegisterDataObserver_001
 * @tc.desc: Abnormal test for RegisterDataObserver
 * @tc.type: FUNC
 */
HWTEST_F(ObjectServiceProxyAdditionalTest, RegisterDataObserver_001, TestSize.Level1)
{
    string bundleName = "testBundle";
    string sessionId = "testSession";
    sptr<IRemoteObject> impl = nullptr;
    sptr<IRemoteObject> callback = nullptr;
    ObjectServiceProxy proxy(impl);
    auto ret = proxy.RegisterDataObserver(bundleName, sessionId, callback);
    EXPECT_EQ(ret, OHOS::ObjectStore::ERR_IPC);
}

/**
 * @tc.name: UnregisterDataChangeObserver_001
 * @tc.desc: Abnormal test for UnregisterDataChangeObserver
 * @tc.type: FUNC
 */
HWTEST_F(ObjectServiceProxyAdditionalTest, UnregisterDataChangeObserver_001, TestSize.Level1)
{
    string bundleName = "testBundle";
    string sessionId = "testSession";
    sptr<IRemoteObject> impl = nullptr;
    ObjectServiceProxy proxy(impl);
    auto ret = proxy.UnregisterDataChangeObserver(bundleName, sessionId);
    EXPECT_EQ(ret, OHOS::ObjectStore::ERR_IPC);
}

/**
 * @tc.name: DeleteSnapshot_001
 * @tc.desc: Abnormal test for DeleteSnapshot
 * @tc.type: FUNC
 */
HWTEST_F(ObjectServiceProxyAdditionalTest, DeleteSnapshot_001, TestSize.Level1)
{
    string bundleName = "testBundle";
    string sessionId = "testSession";
    sptr<IRemoteObject> impl = nullptr;
    ObjectServiceProxy proxy(impl);
    auto ret = proxy.DeleteSnapshot(bundleName, sessionId);
    EXPECT_EQ(ret, OHOS::ObjectStore::ERR_IPC);
}

/**
 * @tc.name: IsContinue_001
 * @tc.desc: Abnormal test for IsContinue
 * @tc.type: FUNC
 */
HWTEST_F(ObjectServiceProxyAdditionalTest, IsContinue_001, TestSize.Level1)
{
    bool result = false;
    sptr<IRemoteObject> impl = nullptr;
    ObjectServiceProxy proxy(impl);
    auto ret = proxy.IsContinue(result);
    EXPECT_EQ(ret, OHOS::ObjectStore::ERR_IPC);
}
} // namespace
