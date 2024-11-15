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
#include "client_adaptor.h"
#include "iservice_registry.h"
#include "itypes_util.h"
#include "logger.h"
#include "objectstore_errors.h"

using namespace testing::ext;
using namespace OHOS::ObjectStore;
using namespace OHOS;
using namespace std;

namespace {
class ClientAdaptorTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ClientAdaptorTest::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void ClientAdaptorTest::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void ClientAdaptorTest::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void ClientAdaptorTest::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: OnRemoteDied_001
 * @tc.desc: Normal test for OnRemoteDied
 * @tc.type: FUNC
 */

HWTEST_F(ClientAdaptorTest, OnRemoteDied_001, TestSize.Level1)
{
    ClientAdaptor clientAdaptor;
    sptr<IRemoteObject> impl = nullptr;
    clientAdaptor.distributedDataMgr_ = std::make_shared<ObjectStoreDataServiceProxy>(impl);
    wptr<IRemoteObject> remote = nullptr;
    ClientAdaptor::ServiceDeathRecipient serviceDeathRecipient;
    serviceDeathRecipient.OnRemoteDied(remote);
    EXPECT_EQ(clientAdaptor.distributedDataMgr_, nullptr);
}

/**
 * @tc.name: RegisterClientDeathListener_001
 * @tc.desc: Abnormal test for RegisterClientDeathListener, remoteObject is nullptr
 * @tc.type: FUNC
 */

HWTEST_F(ClientAdaptorTest, RegisterClientDeathListener_001, TestSize.Level1)
{
    ClientAdaptor clientAdaptor;
    sptr<IRemoteObject> impl = nullptr;
    clientAdaptor.distributedDataMgr_ = nullptr;
    std::string appId = "123";
    sptr<IRemoteObject> remoteObject = nullptr;
    uint32_t ret = clientAdaptor.RegisterClientDeathListener(appId, remoteObject);
    EXPECT_EQ(ret, ERR_EXIST);
}

/**
 * @tc.name: RegisterClientDeathListener_002
 * @tc.desc: Normal test for RegisterClientDeathListener
 * @tc.type: FUNC
 */

HWTEST_F(ClientAdaptorTest, RegisterClientDeathListener_002, TestSize.Level1)
{
    ClientAdaptor clientAdaptor;
    sptr<IRemoteObject> impl = nullptr;
    clientAdaptor.distributedDataMgr_ = nullptr;
    std::string appId = "123";
    sptr<IRemoteObject> remoteObject = new IPCObjectStub();
    uint32_t ret = clientAdaptor.RegisterClientDeathListener(appId, remoteObject);
    EXPECT_EQ(ret, SUCCESS);
    delete remoteObject;
}
}
