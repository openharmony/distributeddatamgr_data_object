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
#include "object_callback_impl.h"
#include "object_callback_stub.h"
#include <ipc_skeleton.h>
#include <logger.h>
#include "itypes_util.h"
#include "log_print.h"

using namespace testing::ext;
using namespace OHOS::DistributedObject;
using namespace OHOS::ObjectStore;
using namespace std;

namespace {
class ObjectCallbackStubTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ObjectCallbackStubTest::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void ObjectCallbackStubTest::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void ObjectCallbackStubTest::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void ObjectCallbackStubTest::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: OnRemoteRequest_001
 * @tc.desc: Abnormal test for OnRemoteRequest, code is 0
 * @tc.type: FUNC
 */

HWTEST_F(ObjectCallbackStubTest, OnRemoteRequest_001, TestSize.Level1)
{
    uint32_t code = 0;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    const std::function<void(const std::map<std::string, std::vector<uint8_t>> &, bool)> callback;
    ObjectChangeCallback objectChangeCallback(callback);
    OHOS::IPCObjectStub objectStub;
    int ret = objectChangeCallback.OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: OnRemoteRequest_002
 * @tc.desc: Abnormal test for OnRemoteRequest, code is 0
 * @tc.type: FUNC
 */

HWTEST_F(ObjectCallbackStubTest, OnRemoteRequest_002, TestSize.Level1)
{
    uint32_t code = 0;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    const std::function<void(int32_t)> callback;
    ObjectProgressCallback objectProgressCallback(callback);
    OHOS::IPCObjectStub objectStub;
    int ret = objectProgressCallback.OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, -1);
    std::u16string dataObjectInterfaceToken = u"OHOS.DistributedObject.IObjectProgressCallback";
    data.WriteInterfaceToken(dataObjectInterfaceToken);
    ret = objectProgressCallback.OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, -1);
}

/**
 * @tc.name: OnRemoteRequest_003
 * @tc.desc: OnRemoteRequest test
 * @tc.type: FUNC
 */

HWTEST_F(ObjectCallbackStubTest, OnRemoteRequest_003, TestSize.Level1)
{
    uint32_t code = 0;
    OHOS::MessageParcel data;
    OHOS::MessageParcel reply;
    OHOS::MessageOption option;
    const std::function<void(int32_t)> callback;
    ObjectProgressCallback objectProgressCallback(callback);
    OHOS::IPCObjectStub objectStub;
    std::u16string dataObjectInterfaceToken = u"OHOS.DistributedObject.IObjectProgressCallback";
    data.WriteInterfaceToken(dataObjectInterfaceToken);
    uint32_t code_1 = 1;
    auto ret = objectProgressCallback.OnRemoteRequest(code_1, data, reply, option);
    int32_t progress = 100;
    OHOS::ITypesUtil::Marshal(data, progress);
    ret = objectProgressCallback.OnRemoteRequest(code, data, reply, option);
    EXPECT_EQ(ret, -1);
}
}
