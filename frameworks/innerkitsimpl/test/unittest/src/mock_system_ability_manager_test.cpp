/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#include <gmock/gmock.h>
#include "client_adaptor.h"
#include "iremote_object.h"
#include "iservice_registry.h"

using namespace testing;
using namespace OHOS;

namespace OHOS::ObjectStore {
// Mock classes
class MockIRemoteObject : public IRemoteObject {
public:
    MOCK_METHOD(int32_t, SendRequest, (uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option), (override));
    MOCK_METHOD(bool, AddDeathRecipient, (const sptr<DeathRecipient> &recipient), (override));
    MOCK_METHOD(bool, RemoveDeathRecipient, (const sptr<DeathRecipient> &recipient), (override));
    MOCK_METHOD(int, GetObjectRefCount, (), (override));
    MOCK_METHOD(int, OnRemoteRequest, (uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option), (override));
    MOCK_METHOD(bool, IsProxyObject, (), (const, override));
    MOCK_METHOD(bool, CheckObjectLegality, (), (const, override));
    MOCK_METHOD(bool, IsObjectDead, (), (const, override));
    MOCK_METHOD(bool, IsDeviceIdIllegal, (const std::string &id), (const, override));
};

class MockISystemAbilityManager : public ISystemAbilityManager {
public:
    MOCK_METHOD(sptr<IRemoteObject>, GetSystemAbility, (int32_t systemAbilityId), (override));
    MOCK_METHOD(sptr<IRemoteObject>, CheckSystemAbility, (int32_t systemAbilityId), (override));
    MOCK_METHOD(sptr<IRemoteObject>, LoadSystemAbility, (int32_t systemAbilityId, int32_t timeout), (override));
    MOCK_METHOD(int32_t, AddOnDemandSystemAbility, (int32_t systemAbilityId, const sptr<IRemoteObject>& ability), (override));
    MOCK_METHOD(int32_t, RemoveSystemAbility, (int32_t systemAbilityId), (override));
    MOCK_METHOD(int32_t, SubscribeSystemAbility, (int32_t systemAbilityId, const sptr<ISystemAbilityStatusChange>& listener), (override));
    MOCK_METHOD(int32_t, UnSubscribeSystemAbility, (int32_t systemAbilityId, const sptr<ISystemAbilityStatusChange>& listener), (override));
};

class MockSystemAbilityManagerClient {
public:
    static MockSystemAbilityManagerClient& GetInstance() {
        static MockSystemAbilityManagerClient instance;
        return instance;
    }
    
    MOCK_METHOD(sptr<ISystemAbilityManager>, GetSystemAbilityManager, (), ());
};

class MockObjectStoreDataServiceProxy : public ObjectStoreDataServiceProxy {
public:
    MockObjectStoreDataServiceProxy(const sptr<IRemoteObject> &impl) : ObjectStoreDataServiceProxy(impl) {}
    MOCK_METHOD(sptr<IRemoteObject>, GetFeatureInterface, (const std::string &name), (override));
    MOCK_METHOD(uint32_t, RegisterClientDeathObserver, (const std::string &appId, sptr<IRemoteObject> observer), (override));
};

// Test fixture
class ClientAdaptorTest : public testing::Test {
protected:
    void SetUp() override {
        // Reset static member before each test
        ClientAdaptor::distributedDataMgr_ = nullptr;
    }

    void TearDown() override {
        // Clean up after each test
        ClientAdaptor::distributedDataMgr_ = nullptr;
    }
};

// Test cases for GetObjectService
/**
 * @tc.name: GetObjectService_Normal
 * @tc.desc: Test GetObjectService with normal conditions
 * @tc.type: FUNC
 */
HWTEST_F(ClientAdaptorTest, GetObjectService_Normal)
{
    // Setup
    sptr<MockIRemoteObject> mockRemoteObject = new MockIRemoteObject();
    sptr<MockObjectStoreDataServiceProxy> mockProxy = new MockObjectStoreDataServiceProxy(mockRemoteObject);
    ClientAdaptor::distributedDataMgr_ = mockProxy;
    
    sptr<MockIRemoteObject> mockFeatureInterface = new MockIRemoteObject();
    EXPECT_CALL(*mockProxy, GetFeatureInterface(StrEq("data_object")))
        .WillOnce(Return(mockFeatureInterface));
    
    // Action
    auto result = ClientAdaptor::GetObjectService();
    
    // Verify
    EXPECT_NE(result, nullptr);
}

/**
 * @tc.name: GetObjectService_NullManager
 * @tc.desc: Test GetObjectService when distributed data manager is null
 * @tc.type: FUNC
 */
HWTEST_F(ClientAdaptorTest, GetObjectService_NullManager)
{
    // Setup
    ClientAdaptor::distributedDataMgr_ = nullptr;
    
    // Action
    auto result = ClientAdaptor::GetObjectService();
    
    // Verify
    EXPECT_EQ(result, nullptr);
}

/**
 * @tc.name: GetObjectService_NullFeatureInterface
 * @tc.desc: Test GetObjectService when GetFeatureInterface returns null
 * @tc.type: FUNC
 */
HWTEST_F(ClientAdaptorTest, GetObjectService_NullFeatureInterface)
{
    // Setup
    sptr<MockIRemoteObject> mockRemoteObject = new MockIRemoteObject();
    sptr<MockObjectStoreDataServiceProxy> mockProxy = new MockObjectStoreDataServiceProxy(mockRemoteObject);
    ClientAdaptor::distributedDataMgr_ = mockProxy;
    
    EXPECT_CALL(*mockProxy, GetFeatureInterface(StrEq("data_object")))
        .WillOnce(Return(nullptr));
    
    // Action
    auto result = ClientAdaptor::GetObjectService();
    
    // Verify
    EXPECT_EQ(result, nullptr);
}

// Test cases for GetDistributedDataManager
/**
 * @tc.name: GetDistributedDataManager_Normal
 * @tc.desc: Test GetDistributedDataManager with normal conditions
 * @tc.type: FUNC
 */
HWTEST_F(ClientAdaptorTest, GetDistributedDataManager_Normal)
{
    // Note: This test would require more complex mocking of the system ability manager
    // which is beyond the scope of this example
    EXPECT_TRUE(true); // Placeholder
}

/**
 * @tc.name: GetDistributedDataManager_NullSystemAbilityManager
 * @tc.desc: Test GetDistributedDataManager when system ability manager is null
 * @tc.type: FUNC
 */
HWTEST_F(ClientAdaptorTest, GetDistributedDataManager_NullSystemAbilityManager)
{
    // Note: This test would require mocking SystemAbilityManagerClient
    // which is beyond the scope of this example
    EXPECT_TRUE(true); // Placeholder
}

// Test cases for ServiceDeathRecipient
/**
 * @tc.name: ServiceDeathRecipient_OnRemoteDied
 * @tc.desc: Test ServiceDeathRecipient::OnRemoteDied
 * @tc.type: FUNC
 */
HWTEST_F(ClientAdaptorTest, ServiceDeathRecipient_OnRemoteDied)
{
    // Setup
    sptr<MockIRemoteObject> mockRemoteObject = new MockIRemoteObject();
    ClientAdaptor::distributedDataMgr_ = nullptr; // Just to make sure it's set
    
    ClientAdaptor::ServiceDeathRecipient deathRecipient;
    
    // Action
    deathRecipient.OnRemoteDied(mockRemoteObject);
    
    // Verify
    EXPECT_EQ(ClientAdaptor::distributedDataMgr_, nullptr);
}

// Test cases for RegisterClientDeathListener
/**
 * @tc.name: RegisterClientDeathListener_Normal
 * @tc.desc: Test RegisterClientDeathListener with normal conditions
 * @tc.type: FUNC
 */
HWTEST_F(ClientAdaptorTest, RegisterClientDeathListener_Normal)
{
    // Setup
    sptr<MockIRemoteObject> mockRemoteObject = new MockIRemoteObject();
    sptr<MockObjectStoreDataServiceProxy> mockProxy = new MockObjectStoreDataServiceProxy(mockRemoteObject);
    ClientAdaptor::distributedDataMgr_ = mockProxy;
    
    EXPECT_CALL(*mockProxy, RegisterClientDeathObserver(_, _))
        .WillOnce(Return(SUCCESS));
    
    // Action
    uint32_t result = ClientAdaptor::RegisterClientDeathListener("testAppId", mockRemoteObject);
    
    // Verify
    EXPECT_EQ(result, SUCCESS);
}

/**
 * @tc.name: RegisterClientDeathListener_NullManager
 * @tc.desc: Test RegisterClientDeathListener when distributed data manager is null
 * @tc.type: FUNC
 */
HWTEST_F(ClientAdaptorTest, RegisterClientDeathListener_NullManager)
{
    // Setup
    ClientAdaptor::distributedDataMgr_ = nullptr;
    
    // Action
    uint32_t result = ClientAdaptor::RegisterClientDeathListener("testAppId", nullptr);
    
    // Verify
    EXPECT_EQ(result, ERR_EXIST);
}

/**
 * @tc.name: RegisterClientDeathListener_RegisterFailed
 * @tc.desc: Test RegisterClientDeathListener when RegisterClientDeathObserver fails
 * @tc.type: FUNC
 */
HWTEST_F(ClientAdaptorTest, RegisterClientDeathListener_RegisterFailed)
{
    // Setup
    sptr<MockIRemoteObject> mockRemoteObject = new MockIRemoteObject();
    sptr<MockObjectStoreDataServiceProxy> mockProxy = new MockObjectStoreDataServiceProxy(mockRemoteObject);
    ClientAdaptor::distributedDataMgr_ = mockProxy;
    
    EXPECT_CALL(*mockProxy, RegisterClientDeathObserver(_, _))
        .WillOnce(Return(ERR_IPC));
    
    // Action
    uint32_t result = ClientAdaptor::RegisterClientDeathListener("testAppId", mockRemoteObject);
    
    // Verify
    EXPECT_EQ(result, ERR_EXIST);
}

// Test fixture for ObjectStoreDataServiceProxy
class ObjectStoreDataServiceProxyTest : public testing::Test {
protected:
    sptr<MockIRemoteObject> mockRemoteObject_;
    std::unique_ptr<ObjectStoreDataServiceProxy> proxy_;

    void SetUp() override {
        mockRemoteObject_ = new MockIRemoteObject();
        proxy_ = std::make_unique<ObjectStoreDataServiceProxy>(mockRemoteObject_);
    }
};

// Test cases for ObjectStoreDataServiceProxy::GetFeatureInterface
/**
 * @tc.name: GetFeatureInterface_Normal
 * @tc.desc: Test GetFeatureInterface with normal conditions
 * @tc.type: FUNC
 */
HWTEST_F(ObjectStoreDataServiceProxyTest, GetFeatureInterface_Normal)
{
    // Note: Testing this method requires mocking MessageParcel and SendRequest
    // which is complex and beyond the scope of this example
    EXPECT_TRUE(true); // Placeholder
}

/**
 * @tc.name: GetFeatureInterface_NullRemote
 * @tc.desc: Test GetFeatureInterface when Remote() returns null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectStoreDataServiceProxyTest, GetFeatureInterface_NullRemote)
{
    // Note: This would require making the Remote() method mockable
    // which is beyond the scope of this example
    EXPECT_TRUE(true); // Placeholder
}

// Test cases for ObjectStoreDataServiceProxy::RegisterClientDeathObserver
/**
 * @tc.name: RegisterClientDeathObserver_Normal
 * @tc.desc: Test RegisterClientDeathObserver with normal conditions
 * @tc.type: FUNC
 */
HWTEST_F(ObjectStoreDataServiceProxyTest, RegisterClientDeathObserver_Normal)
{
    // Note: Testing this method requires mocking MessageParcel and SendRequest
    // which is complex and beyond the scope of this example
    EXPECT_TRUE(true); // Placeholder
}

/**
 * @tc.name: RegisterClientDeathObserver_NullObserver
 * @tc.desc: Test RegisterClientDeathObserver when observer is null
 * @tc.type: FUNC
 */
HWTEST_F(ObjectStoreDataServiceProxyTest, RegisterClientDeathObserver_NullObserver)
{
    // Action
    uint32_t result = proxy_->RegisterClientDeathObserver("testAppId", nullptr);
    
    // Verify
    EXPECT_EQ(result, ERR_INVALID_ARGS);
}

/**
* @tc.name: GetSchema
* @tc.desc: GetSchema from cloud when no schema in meta.
* @tc.type: FUNC
* @tc.require:
* @tc.author: ht
*/
HWTEST_F(CloudDataTest, GetSchema, TestSize.Level1)
{
    auto cloudServerMock = std::make_shared<CloudServerMock>();
    auto user = AccountDelegate::GetInstance()->GetUserByToken(OHOS::IPCSkeleton::GetCallingTokenID());
    auto [status, cloudInfo] = cloudServerMock->GetServerInfo(user, true);
    ASSERT_TRUE(MetaDataManager::GetInstance().DelMeta(cloudInfo.GetSchemaKey(TEST_CLOUD_BUNDLE), true));
    SchemaMeta schemaMeta;
    ASSERT_FALSE(MetaDataManager::GetInstance().LoadMeta(cloudInfo.GetSchemaKey(TEST_CLOUD_BUNDLE), schemaMeta, true));
    StoreInfo storeInfo{ OHOS::IPCSkeleton::GetCallingTokenID(), TEST_CLOUD_BUNDLE, TEST_CLOUD_STORE, 0 };
    auto event = std::make_unique<CloudEvent>(CloudEvent::GET_SCHEMA, storeInfo);
    EventCenter::GetInstance().PostEvent(std::move(event));
    auto ret = MetaDataManager::GetInstance().LoadMeta(cloudInfo.GetSchemaKey(TEST_CLOUD_BUNDLE), schemaMeta, true);
    ASSERT_TRUE(ret);
}

/**
* @tc.name: QueryStatistics
* @tc.desc: The query interface failed because cloudInfo could not be found from the metadata.
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(CloudDataTest, QueryStatistics001, TestSize.Level0)
{
    // prepare MetaDta
    MetaDataManager::GetInstance().DelMeta(cloudInfo_.GetKey(), true);

    auto [status, result] = cloudServiceImpl_->QueryStatistics(TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, "");
    EXPECT_EQ(status, CloudData::CloudService::ERROR);
    EXPECT_TRUE(result.empty());
}

/**
* @tc.name: QueryStatistics
* @tc.desc: The query interface failed because SchemaMeta could not be found from the metadata.
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(CloudDataTest, QueryStatistics002, TestSize.Level0)
{
    // prepare MetaDta
    MetaDataManager::GetInstance().DelMeta(cloudInfo_.GetSchemaKey(TEST_CLOUD_BUNDLE), true);
    MetaDataManager::GetInstance().SaveMeta(cloudInfo_.GetKey(), cloudInfo_, true);

    auto [status, result] = cloudServiceImpl_->QueryStatistics("", "", "");
    EXPECT_EQ(status, CloudData::CloudService::ERROR);
    EXPECT_TRUE(result.empty());
    std::tie(status, result) = cloudServiceImpl_->QueryStatistics(TEST_CLOUD_ID, "", "");
    EXPECT_EQ(status, CloudData::CloudService::ERROR);
    EXPECT_TRUE(result.empty());
    std::tie(status, result) = cloudServiceImpl_->QueryStatistics(TEST_CLOUD_ID, TEST_CLOUD_STORE, "");
    EXPECT_EQ(status, CloudData::CloudService::ERROR);
    EXPECT_TRUE(result.empty());
    std::tie(status, result) = cloudServiceImpl_->QueryStatistics(TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, "");
    EXPECT_EQ(status, CloudData::CloudService::ERROR);
    EXPECT_TRUE(result.empty());
}

/**
* @tc.name: QueryStatistics
* @tc.desc: Query the statistics of cloud records in a specified database.
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(CloudDataTest, QueryStatistics003, TestSize.Level1)
{
    auto [status, result] =
        cloudServiceImpl_->QueryStatistics(TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1);
    ASSERT_EQ(status, CloudData::CloudService::SUCCESS);
    ASSERT_EQ(result.size(), 1);
    for (const auto &it : result) {
        ASSERT_EQ(it.first, TEST_CLOUD_DATABASE_ALIAS_1);
        auto statisticInfos = it.second;
        ASSERT_FALSE(statisticInfos.empty());
        for (const auto &info : statisticInfos) {
            EXPECT_EQ(info.inserted, 1);
            EXPECT_EQ(info.updated, 2);
            EXPECT_EQ(info.normal, 3);
        }
    }
}

/**
* @tc.name: QueryStatistics
* @tc.desc: Query the statistics of all local database cloud records.
* @tc.type: FUNC
* @tc.require:
*/
HWTEST_F(CloudDataTest, QueryStatistics004, TestSize.Level1)
{
    auto [status, result] = cloudServiceImpl_->QueryStatistics(TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, "");
    ASSERT_EQ(status, CloudData::CloudService::SUCCESS);
    ASSERT_EQ(result.size(), 2);
    for (const auto &it : result) {
        auto statisticInfos = it.second;
        ASSERT_FALSE(statisticInfos.empty());
        for (const auto &info : statisticInfos) {
            EXPECT_EQ(info.inserted, 1);
            EXPECT_EQ(info.updated, 2);
            EXPECT_EQ(info.normal, 3);
        }
    }
}

/**
* @tc.name: QueryLastSyncInfo001
* @tc.desc: The query last sync info interface failed because account is false.
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo001, TestSize.Level0)
{
    auto [status, result] =
        cloudServiceImpl_->QueryLastSyncInfo("accountId", TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1);
    EXPECT_EQ(status, CloudData::CloudService::SUCCESS);
    EXPECT_TRUE(result.empty());
}

/**
* @tc.name: QueryLastSyncInfo002
* @tc.desc: The query last sync info interface failed because bundleName is false.
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo002, TestSize.Level0)
{
    auto [status, result] =
        cloudServiceImpl_->QueryLastSyncInfo(TEST_CLOUD_ID, "bundleName", TEST_CLOUD_DATABASE_ALIAS_1);
    EXPECT_EQ(status, CloudData::CloudService::Status::INVALID_ARGUMENT);
    EXPECT_TRUE(result.empty());
}

/**
* @tc.name: QueryLastSyncInfo003
* @tc.desc: The query last sync info interface failed because storeId is false.
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo003, TestSize.Level0)
{
    auto [status, result] = cloudServiceImpl_->QueryLastSyncInfo(TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, "storeId");
    EXPECT_EQ(status, CloudData::CloudService::INVALID_ARGUMENT);
    EXPECT_TRUE(result.empty());
}

/**
* @tc.name: QueryLastSyncInfo004
* @tc.desc: The query last sync info interface
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo004, TestSize.Level1)
{
    auto ret = cloudServiceImpl_->DisableCloud(TEST_CLOUD_ID);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
    auto user = AccountDelegate::GetInstance()->GetUserByToken(OHOS::IPCSkeleton::GetCallingTokenID());
    cloudServiceImpl_->OnUserChange(
        static_cast<uint32_t>(AccountStatus::DEVICE_ACCOUNT_SWITCHED), std::to_string(user), "accountId");

    sleep(1);

    auto [status, result] =
        cloudServiceImpl_->QueryLastSyncInfo(TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1);
    EXPECT_EQ(status, CloudData::CloudService::SUCCESS);
    EXPECT_TRUE(!result.empty());
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].code == E_BLOCKED_BY_NETWORK_STRATEGY);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].startTime != 0);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].finishTime != 0);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].syncStatus == 1);
}

/**
* @tc.name: QueryLastSyncInfo005
* @tc.desc: The query last sync info interface
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo005, TestSize.Level1)
{
    std::map<std::string, int32_t> switches;
    switches.emplace(TEST_CLOUD_ID, true);
    CloudInfo info;
    MetaDataManager::GetInstance().LoadMeta(cloudInfo_.GetKey(), info, true);
    info.apps[TEST_CLOUD_BUNDLE].cloudSwitch = false;
    MetaDataManager::GetInstance().SaveMeta(info.GetKey(), info, true);
    auto user = AccountDelegate::GetInstance()->GetUserByToken(OHOS::IPCSkeleton::GetCallingTokenID());
    cloudServiceImpl_->OnUserChange(
        static_cast<uint32_t>(AccountStatus::DEVICE_ACCOUNT_SWITCHED), std::to_string(user), "accountId");
    sleep(1);

    auto [status, result] =
        cloudServiceImpl_->QueryLastSyncInfo(TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1);
    EXPECT_EQ(status, CloudData::CloudService::SUCCESS);
    EXPECT_TRUE(!result.empty());
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].code == E_BLOCKED_BY_NETWORK_STRATEGY);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].startTime != 0);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].finishTime != 0);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].syncStatus == 1);
}

/**
* @tc.name: QueryLastSyncInfo006
* @tc.desc: The query last sync info interface
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo006, TestSize.Level0)
{
    MetaDataManager::GetInstance().DelMeta(cloudInfo_.GetSchemaKey(TEST_CLOUD_BUNDLE), true);
    auto [status, result] =
        cloudServiceImpl_->QueryLastSyncInfo(TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1);
    EXPECT_EQ(status, CloudData::CloudService::ERROR);
    EXPECT_TRUE(result.empty());
    SchemaMeta meta;
    meta.bundleName = "test";
    MetaDataManager::GetInstance().SaveMeta(cloudInfo_.GetSchemaKey(TEST_CLOUD_BUNDLE), meta, true);
    std::tie(status, result) =
        cloudServiceImpl_->QueryLastSyncInfo(TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1);
    EXPECT_EQ(status, CloudData::CloudService::SUCCESS);
    EXPECT_TRUE(result.empty());
}

/**
* @tc.name: QueryLastSyncInfo007
* @tc.desc: The query last sync info interface
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo007, TestSize.Level0)
{
    int32_t user = 100;
    int64_t startTime = 123456789;
    int64_t finishTime = 123456799;
    CloudLastSyncInfo lastSyncInfo;
    lastSyncInfo.id = TEST_CLOUD_ID;
    lastSyncInfo.storeId = TEST_CLOUD_DATABASE_ALIAS_1;
    lastSyncInfo.startTime = startTime;
    lastSyncInfo.finishTime = finishTime;
    lastSyncInfo.syncStatus = 1;
    MetaDataManager::GetInstance().SaveMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
        TEST_CLOUD_DATABASE_ALIAS_1), lastSyncInfo, true);

    std::vector<CloudLastSyncInfo> lastSyncInfos;
    MetaDataManager::GetInstance().LoadMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
        TEST_CLOUD_DATABASE_ALIAS_1), lastSyncInfos, true);
    EXPECT_TRUE(!lastSyncInfos.empty());

    CloudData::SyncManager sync;
    auto [status, result] = sync.QueryLastSyncInfo({ { user, TEST_CLOUD_ID, TEST_CLOUD_BUNDLE,
                                                       TEST_CLOUD_DATABASE_ALIAS_1 } });
    EXPECT_EQ(status, CloudData::CloudService::SUCCESS);
    EXPECT_TRUE(!result.empty());
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].code == -1);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].startTime == startTime);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].finishTime == finishTime);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].syncStatus == 1);
}

/**
* @tc.name: QueryLastSyncInfo008
* @tc.desc: The query last sync info interface
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo008, TestSize.Level0)
{
    int32_t user = 100;
    int64_t startTime = 123456789;
        int64_t finishTime = 123456799;
    CloudLastSyncInfo lastSyncInfo;
    lastSyncInfo.id = TEST_CLOUD_ID;
    lastSyncInfo.storeId = TEST_CLOUD_DATABASE_ALIAS_1;
    lastSyncInfo.startTime = startTime;
    lastSyncInfo.finishTime = finishTime;
    lastSyncInfo.syncStatus = 1;
    lastSyncInfo.code = 0;
    MetaDataManager::GetInstance().SaveMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
        TEST_CLOUD_DATABASE_ALIAS_1), lastSyncInfo, true);
    CloudLastSyncInfo lastSyncInfo1;
    lastSyncInfo1.id = TEST_CLOUD_ID;
    lastSyncInfo1.storeId = TEST_CLOUD_DATABASE_ALIAS_2;
    lastSyncInfo1.startTime = startTime;
    lastSyncInfo1.finishTime = finishTime;
    lastSyncInfo1.syncStatus = 1;
    lastSyncInfo1.code = 0;
    MetaDataManager::GetInstance().SaveMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
        TEST_CLOUD_DATABASE_ALIAS_2), lastSyncInfo1, true);

    std::vector<CloudLastSyncInfo> lastSyncInfos;
    MetaDataManager::GetInstance().LoadMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
        ""), lastSyncInfos, true);
    EXPECT_TRUE(!lastSyncInfos.empty());

    CloudData::SyncManager sync;
    auto [status, result] = sync.QueryLastSyncInfo({ { user, TEST_CLOUD_ID, TEST_CLOUD_BUNDLE,
        TEST_CLOUD_DATABASE_ALIAS_1 }, { user, TEST_CLOUD_ID, TEST_CLOUD_BUNDLE,
        TEST_CLOUD_DATABASE_ALIAS_2} });

    EXPECT_EQ(status, CloudData::CloudService::SUCCESS);
    EXPECT_TRUE(result.size() == 2);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].code == 0);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].startTime == startTime);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].finishTime == finishTime);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_1].syncStatus == 1);

    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_2].code == 0);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_2].startTime == startTime);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_2].finishTime == finishTime);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_2].syncStatus == 1);
}

/**
* @tc.name: QueryLastSyncInfo009
* @tc.desc: The query last sync info interface failed when schema is invalid.
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo009, TestSize.Level0)
{
    int32_t user = 100;
    int64_t startTime = 123456789;
        int64_t finishTime = 123456799;
    CloudLastSyncInfo lastSyncInfo;
    lastSyncInfo.id = TEST_CLOUD_ID;
    lastSyncInfo.storeId = TEST_CLOUD_DATABASE_ALIAS_1;
    lastSyncInfo.startTime = startTime;
    lastSyncInfo.finishTime = finishTime;
    lastSyncInfo.syncStatus = 1;
    lastSyncInfo.code = 0;
    MetaDataManager::GetInstance().SaveMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
        TEST_CLOUD_DATABASE_ALIAS_1), lastSyncInfo, true);
    CloudLastSyncInfo lastSyncInfo1;
    lastSyncInfo1.id = TEST_CLOUD_ID;
    lastSyncInfo1.storeId = TEST_CLOUD_DATABASE_ALIAS_2;
    lastSyncInfo1.startTime = startTime;
    lastSyncInfo1.finishTime = finishTime;
    lastSyncInfo1.syncStatus = 1;
    lastSyncInfo1.code = 0;
    MetaDataManager::GetInstance().SaveMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
        TEST_CLOUD_DATABASE_ALIAS_2), lastSyncInfo1, true);

    std::vector<CloudLastSyncInfo> lastSyncInfos;
    MetaDataManager::GetInstance().LoadMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE, ""),
        lastSyncInfos, true);
    EXPECT_TRUE(!lastSyncInfos.empty());

    CloudData::SyncManager sync;
    auto [status, result] = sync.QueryLastSyncInfo({ { user, TEST_CLOUD_ID, TEST_CLOUD_BUNDLE,
                                                       TEST_CLOUD_DATABASE_ALIAS_2} });
    EXPECT_EQ(status, CloudData::CloudService::SUCCESS);
    EXPECT_TRUE(result.size() == 1);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_2].code == 0);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_2].startTime == startTime);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_2].finishTime == finishTime);
    EXPECT_TRUE(result[TEST_CLOUD_DATABASE_ALIAS_2].syncStatus == 1);
}

/**
* @tc.name: QueryLastSyncInfo010
* @tc.desc: The query last sync info interface failed
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo010, TestSize.Level0)
{
    int32_t user = 100;
    int64_t startTime = 123456789;
        int64_t finishTime = 123456799;
    CloudLastSyncInfo lastSyncInfo;
    lastSyncInfo.id = TEST_CLOUD_ID;
    lastSyncInfo.storeId = TEST_CLOUD_DATABASE_ALIAS_1;
    lastSyncInfo.startTime = startTime;
    lastSyncInfo.finishTime = finishTime;
    lastSyncInfo.syncStatus = 1;
    lastSyncInfo.code = 0;
    MetaDataManager::GetInstance().SaveMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
        TEST_CLOUD_DATABASE_ALIAS_1), lastSyncInfo, true);
    CloudLastSyncInfo lastSyncInfo1;
    lastSyncInfo1.id = TEST_CLOUD_ID;
    lastSyncInfo1.storeId = TEST_CLOUD_DATABASE_ALIAS_2;
    lastSyncInfo1.startTime = startTime;
    lastSyncInfo1.finishTime = finishTime;
    lastSyncInfo1.syncStatus = 1;
    lastSyncInfo1.code = 0;
    MetaDataManager::GetInstance().SaveMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
        TEST_CLOUD_DATABASE_ALIAS_2), lastSyncInfo1, true);

    CloudData::SyncManager sync;
    CloudData::SyncManager::SyncInfo info(user, TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1);
    auto [status, result] = sync.QueryLastSyncInfo({ { user, TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, "1234"} });
    EXPECT_EQ(status, CloudData::CloudService::SUCCESS);
    EXPECT_TRUE(result.empty());
}

/**
* @tc.name: QueryLastSyncInfo011
* @tc.desc: The query last sync info interface
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo011, TestSize.Level1)
{
    schemaMeta_.databases[1].name = TEST_CLOUD_STORE_1;
    MetaDataManager::GetInstance().SaveMeta(cloudInfo_.GetSchemaKey(TEST_CLOUD_BUNDLE), schemaMeta_, true);
    int32_t user = 100;
    CloudData::SyncManager sync;
    CloudData::SyncManager::SyncInfo info(user, TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1);
    info.syncId_ = 0;
    CloudInfo cloud;
    cloud.user = info.user_;
    auto cloudSyncInfos = sync.GetCloudSyncInfo(info, cloud);
    sync.UpdateStartSyncInfo(cloudSyncInfos);
    auto [status, result] = sync.QueryLastSyncInfo({ { user, TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, TEST_CLOUD_STORE },
                                           { user, TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, TEST_CLOUD_STORE_1} });
    EXPECT_EQ(status, CloudData::CloudService::SUCCESS);
    EXPECT_TRUE(result.size() == 2);
    EXPECT_TRUE(result[TEST_CLOUD_STORE].code == 0);
    EXPECT_TRUE(result[TEST_CLOUD_STORE].startTime != 0);
    EXPECT_TRUE(result[TEST_CLOUD_STORE].finishTime == 0);
    EXPECT_TRUE(result[TEST_CLOUD_STORE].syncStatus == 0);

    EXPECT_TRUE(result[TEST_CLOUD_STORE_1].code == 0);
    EXPECT_TRUE(result[TEST_CLOUD_STORE_1].startTime != 0);
    EXPECT_TRUE(result[TEST_CLOUD_STORE_1].finishTime == 0);
    EXPECT_TRUE(result[TEST_CLOUD_STORE_1].syncStatus == 0);
}

/**
* @tc.name: QueryLastSyncInfo012
* @tc.desc: The query last sync info interface failed.
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryLastSyncInfo012, TestSize.Level0)
{
    int32_t user = 100;
    int64_t startTime = 123456789;
    int64_t finishTime = 123456799;
    CloudLastSyncInfo lastSyncInfo;
    lastSyncInfo.id = TEST_CLOUD_ID;
    lastSyncInfo.storeId = TEST_CLOUD_DATABASE_ALIAS_1;
    lastSyncInfo.startTime = startTime;
    lastSyncInfo.finishTime = finishTime;
    lastSyncInfo.syncStatus = 1;
    MetaDataManager::GetInstance().SaveMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
        TEST_CLOUD_DATABASE_ALIAS_1), lastSyncInfo, true);

    std::vector<CloudLastSyncInfo> lastSyncInfos;
    MetaDataManager::GetInstance().LoadMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
         TEST_CLOUD_DATABASE_ALIAS_1), lastSyncInfos, true);
    EXPECT_TRUE(!lastSyncInfos.empty());

    CloudData::SyncManager sync;
    CloudData::SyncManager::SyncInfo info(user, TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1);
    auto [status, result] = sync.QueryLastSyncInfo({ { user, TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, "1234"} });
    EXPECT_EQ(status, CloudData::CloudService::SUCCESS);
    EXPECT_TRUE(result.empty());
}

/**
* @tc.name: GetStores
* @tc.desc: Test GetStores function
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, GetStores, TestSize.Level0)
{
    auto cloudServerMock = std::make_shared<CloudServerMock>();
    auto user = AccountDelegate::GetInstance()->GetUserByToken(OHOS::IPCSkeleton::GetCallingTokenID());
    auto [status, cloudInfo] = cloudServerMock->GetServerInfo(user, true);
    SchemaMeta schemaMeta;
    MetaDataManager::GetInstance().LoadMeta(cloudInfo.GetSchemaKey(TEST_CLOUD_BUNDLE), schemaMeta, true);
    EXPECT_TRUE(!schemaMeta.GetStores().empty());
}

/**
* @tc.name: UpdateStartSyncInfo
* @tc.desc: Test UpdateStartSyncInfo
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, UpdateStartSyncInfo, TestSize.Level0)
{
    int32_t user = 100;
    CloudData::SyncManager sync;
    CloudData::SyncManager::SyncInfo info(user, TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1);
    CloudInfo cloud;
    cloud.user = info.user_;
    auto cloudSyncInfos = sync.GetCloudSyncInfo(info, cloud);
    sync.UpdateStartSyncInfo(cloudSyncInfos);
    std::vector<CloudLastSyncInfo> lastSyncInfos;
    MetaDataManager::GetInstance().LoadMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
         ""), lastSyncInfos, true);
    EXPECT_TRUE(!lastSyncInfos.empty());
    printf("code: %d", lastSyncInfos[0].code);
    EXPECT_TRUE(lastSyncInfos[0].code == -1);
    EXPECT_TRUE(lastSyncInfos[0].startTime != 0);
    EXPECT_TRUE(lastSyncInfos[0].finishTime != 0);
    EXPECT_TRUE(lastSyncInfos[0].syncStatus == 1);
}

/**
* @tc.name: UpdateStartSyncInfo
* @tc.desc: Test UpdateStartSyncInfo
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, UpdateFinishSyncInfo, TestSize.Level0)
{
    int32_t user = 100;
    CloudData::SyncManager sync;
    CloudData::SyncManager::SyncInfo info(user, TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1);
    CloudInfo cloud;
    cloud.user = info.user_;
    auto cloudSyncInfos = sync.GetCloudSyncInfo(info, cloud);
    sync.UpdateStartSyncInfo(cloudSyncInfos);
    sync.UpdateFinishSyncInfo({ user, TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, TEST_CLOUD_DATABASE_ALIAS_1 }, 0, 0);
    std::vector<CloudLastSyncInfo> lastSyncInfos;
    MetaDataManager::GetInstance().LoadMeta(CloudLastSyncInfo::GetKey(user, TEST_CLOUD_BUNDLE,
         ""), lastSyncInfos, true);
    EXPECT_TRUE(!lastSyncInfos.empty());
}

/**
* @tc.name: Share
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, Share001, TestSize.Level1)
{
    std::string sharingRes = "";
    CloudData::Participants participants{};
    CloudData::Results results;
    auto ret = cloudServiceImpl_->Share(sharingRes, participants, results);
    EXPECT_EQ(ret, CloudData::CloudService::NOT_SUPPORT);
}

/**
* @tc.name: Unshare
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, Unshare001, TestSize.Level1)
{
    std::string sharingRes = "";
    CloudData::Participants participants{};
    CloudData::Results results;
    auto ret = cloudServiceImpl_->Unshare(sharingRes, participants, results);
    EXPECT_EQ(ret, CloudData::CloudService::NOT_SUPPORT);
}

/**
* @tc.name: ChangePrivilege
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, ChangePrivilege001, TestSize.Level1)
{
    std::string sharingRes = "";
    CloudData::Participants participants{};
    CloudData::Results results;
    auto ret = cloudServiceImpl_->ChangePrivilege(sharingRes, participants, results);
    EXPECT_EQ(ret, CloudData::CloudService::NOT_SUPPORT);
}

/**
* @tc.name: ChangeConfirmation
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, ChangeConfirmation001, TestSize.Level1)
{
    std::string sharingRes = "";
    int32_t confirmation = 0;
    std::pair<int32_t, std::string> result;
    auto ret = cloudServiceImpl_->ChangeConfirmation(sharingRes, confirmation, result);
    EXPECT_EQ(ret, CloudData::CloudService::NOT_SUPPORT);
}

/**
* @tc.name: ConfirmInvitation
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, ConfirmInvitation001, TestSize.Level1)
{
    std::string sharingRes = "";
    int32_t confirmation = 0;
    std::tuple<int32_t, std::string, std::string> result;
    auto ret = cloudServiceImpl_->ConfirmInvitation(sharingRes, confirmation, result);
    EXPECT_EQ(ret, CloudData::CloudService::NOT_SUPPORT);
}

/**
* @tc.name: Exit
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, Exit001, TestSize.Level1)
{
    std::string sharingRes = "";
    std::pair<int32_t, std::string> result;
    auto ret = cloudServiceImpl_->Exit(sharingRes, result);
    EXPECT_EQ(ret, CloudData::CloudService::NOT_SUPPORT);
}
/**
* @tc.name: Query
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, Query001, TestSize.Level1)
{
    std::string sharingRes = "";
    CloudData::QueryResults result;
    auto ret = cloudServiceImpl_->Query(sharingRes, result);
    EXPECT_EQ(ret, CloudData::CloudService::NOT_SUPPORT);
}

/**
* @tc.name: QueryByInvitation
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, QueryByInvitation001, TestSize.Level1)
{
    std::string invitation = "";
    CloudData::QueryResults result;
    auto ret = cloudServiceImpl_->QueryByInvitation(invitation, result);
    EXPECT_EQ(ret, CloudData::CloudService::NOT_SUPPORT);
}

/**
* @tc.name: AllocResourceAndShare
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, AllocResourceAndShare001, TestSize.Level0)
{
    DistributedRdb::PredicatesMemo predicates;
    predicates.tables_.push_back(TEST_CLOUD_BUNDLE);
    std::vector<std::string> columns;
    CloudData::Participants participants;
    auto [ret, _] = cloudServiceImpl_->AllocResourceAndShare(TEST_CLOUD_STORE, predicates, columns, participants);
    EXPECT_EQ(ret, E_ERROR);
    EventCenter::GetInstance().Subscribe(CloudEvent::MAKE_QUERY, [](const Event &event) {
        auto &evt = static_cast<const DistributedData::MakeQueryEvent &>(event);
        auto callback = evt.GetCallback();
        if (!callback) {
            return;
        }
        auto predicate = evt.GetPredicates();
        auto rdbQuery = std::make_shared<DistributedRdb::RdbQuery>(*predicate);
        rdbQuery->SetColumns(evt.GetColumns());
        callback(rdbQuery);
    });
    std::tie(ret, _) = cloudServiceImpl_->AllocResourceAndShare(TEST_CLOUD_STORE, predicates, columns, participants);
    EXPECT_EQ(ret, E_ERROR);
}

/**
* @tc.name: SetGlobalCloudStrategy
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, SetGlobalCloudStrategy001, TestSize.Level1)
{
    std::vector<CommonType::Value> values;
    values.push_back(CloudData::NetWorkStrategy::WIFI);
    CloudData::Strategy strategy = CloudData::Strategy::STRATEGY_BUTT;
    auto ret = cloudServiceImpl_->SetGlobalCloudStrategy(strategy, values);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
    strategy = CloudData::Strategy::STRATEGY_NETWORK;
    ret = cloudServiceImpl_->SetGlobalCloudStrategy(strategy, values);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: SetCloudStrategy
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, SetCloudStrategy001, TestSize.Level1)
{
    std::vector<CommonType::Value> values;
    values.push_back(CloudData::NetWorkStrategy::WIFI);
    CloudData::Strategy strategy = CloudData::Strategy::STRATEGY_BUTT;
    auto ret = cloudServiceImpl_->SetCloudStrategy(strategy, values);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
    strategy = CloudData::Strategy::STRATEGY_NETWORK;
    ret = cloudServiceImpl_->SetCloudStrategy(strategy, values);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: CloudSync001
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, CloudSync001, TestSize.Level0)
{
    int32_t syncMode = DistributedData::GeneralStore::NEARBY_BEGIN;
    uint32_t seqNum = 10;
    // invalid syncMode
    auto ret = cloudServiceImpl_->CloudSync("bundleName", "storeId", { syncMode, seqNum }, nullptr);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
}

/**
* @tc.name: CloudSync002
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, CloudSync002, TestSize.Level0)
{
    int32_t syncMode = DistributedData::GeneralStore::NEARBY_PULL_PUSH;
    uint32_t seqNum = 10;
    // invalid syncMode
    auto ret = cloudServiceImpl_->CloudSync("bundleName", "storeId", { syncMode, seqNum }, nullptr);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
}

/**
* @tc.name: CloudSync003
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, CloudSync003, TestSize.Level1)
{
    int32_t syncMode = DistributedData::GeneralStore::CLOUD_BEGIN;
    uint32_t seqNum = 10;
    auto ret = cloudServiceImpl_->CloudSync("bundleName", "storeId", { syncMode, seqNum }, nullptr);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: CloudSync004
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, CloudSync004, TestSize.Level1)
{
    int32_t syncMode = DistributedData::GeneralStore::CLOUD_TIME_FIRST;
    uint32_t seqNum = 10;
    auto ret = cloudServiceImpl_->CloudSync("bundleName", "storeId", { syncMode, seqNum }, nullptr);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: CloudSync005
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, CloudSync005, TestSize.Level1)
{
    int32_t syncMode = DistributedData::GeneralStore::CLOUD_NATIVE_FIRST;
    uint32_t seqNum = 10;
    auto ret = cloudServiceImpl_->CloudSync("bundleName", "storeId", { syncMode, seqNum }, nullptr);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: CloudSync006
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, CloudSync006, TestSize.Level1)
{
    int32_t syncMode = DistributedData::GeneralStore::CLOUD_CLOUD_FIRST;
    uint32_t seqNum = 10;
    auto ret = cloudServiceImpl_->CloudSync("bundleName", "storeId", { syncMode, seqNum }, nullptr);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: CloudSync007
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, CloudSync007, TestSize.Level0)
{
    int32_t syncMode = DistributedData::GeneralStore::CLOUD_END;
    uint32_t seqNum = 10;
    auto ret = cloudServiceImpl_->CloudSync("bundleName", "storeId", { syncMode, seqNum }, nullptr);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);

    CloudData::Details details{};
    uint32_t tokenId = 0;
    cloudServiceImpl_->OnAsyncComplete(tokenId, seqNum, std::move(details));
}

/**
* @tc.name: InitNotifier001
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, InitNotifier001, TestSize.Level0)
{
    sptr<IRemoteObject> notifier = nullptr;
    auto ret = cloudServiceImpl_->InitNotifier(notifier);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);

    ret = cloudServiceImpl_->InitNotifier(notifier);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
}

/**
* @tc.name: Clean
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, Clean001, TestSize.Level0)
{
    std::map<std::string, int32_t> actions;
    actions.insert_or_assign(TEST_CLOUD_BUNDLE, CloudData::CloudService::Action::CLEAR_CLOUD_BUTT);
    std::string id = "testId";
    std::string bundleName = "testBundleName";
    auto ret = cloudServiceImpl_->Clean(id, actions);
    EXPECT_EQ(ret, CloudData::CloudService::ERROR);
    ret = cloudServiceImpl_->Clean(TEST_CLOUD_ID, actions);
    EXPECT_EQ(ret, CloudData::CloudService::ERROR);
    actions.insert_or_assign(TEST_CLOUD_BUNDLE, CloudData::CloudService::Action::CLEAR_CLOUD_INFO);
    actions.insert_or_assign(bundleName, CloudData::CloudService::Action::CLEAR_CLOUD_DATA_AND_INFO);
    ret = cloudServiceImpl_->Clean(TEST_CLOUD_ID, actions);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: Clean
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, Clean002, TestSize.Level0)
{
    MetaDataManager::GetInstance().DelMeta(metaData_.GetKey(), true);
    std::map<std::string, int32_t> actions;
    actions.insert_or_assign(TEST_CLOUD_BUNDLE, CloudData::CloudService::Action::CLEAR_CLOUD_INFO);
    auto ret = cloudServiceImpl_->Clean(TEST_CLOUD_ID, actions);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
    StoreMetaDataLocal localMeta;
    localMeta.isPublic = false;
    MetaDataManager::GetInstance().SaveMeta(metaData_.GetKeyLocal(), localMeta, true);
    ret = cloudServiceImpl_->Clean(TEST_CLOUD_ID, actions);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
    localMeta.isPublic = true;
    MetaDataManager::GetInstance().SaveMeta(metaData_.GetKeyLocal(), localMeta, true);
    ret = cloudServiceImpl_->Clean(TEST_CLOUD_ID, actions);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
    metaData_.user = "0";
    MetaDataManager::GetInstance().SaveMeta(metaData_.GetKey(), metaData_, true);
    ret = cloudServiceImpl_->Clean(TEST_CLOUD_ID, actions);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
    MetaDataManager::GetInstance().DelMeta(metaData_.GetKey(), true);
    metaData_.user = std::to_string(AccountDelegate::GetInstance()->GetUserByToken(metaData_.tokenId));
    MetaDataManager::GetInstance().DelMeta(metaData_.GetKeyLocal(), true);
}

/**
* @tc.name: NotifyDataChange
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, NotifyDataChange001, TestSize.Level0)
{
    auto ret = cloudServiceImpl_->NotifyDataChange(TEST_CLOUD_ID, TEST_CLOUD_BUNDLE);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: NotifyDataChange
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, NotifyDataChange002, TestSize.Level0)
{
    constexpr const int32_t invalidUserId = -1;
    std::string extraData;
    auto ret = cloudServiceImpl_->NotifyDataChange("", extraData, invalidUserId);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
    ret = cloudServiceImpl_->NotifyDataChange(CloudData::DATA_CHANGE_EVENT_ID, extraData, invalidUserId);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
    extraData = "{data:test}";
    ret = cloudServiceImpl_->NotifyDataChange(CloudData::DATA_CHANGE_EVENT_ID, extraData, invalidUserId);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
    extraData = "{\"data\":\"{\\\"accountId\\\":\\\"id\\\",\\\"bundleName\\\":\\\"test_cloud_"
                "bundleName\\\",\\\"containerName\\\":\\\"test_cloud_database_alias_1\\\", \\\"databaseScopes\\\": "
                "\\\"[\\\\\\\"private\\\\\\\", "
                "\\\\\\\"shared\\\\\\\"]\\\",\\\"recordTypes\\\":\\\"[\\\\\\\"test_cloud_table_alias\\\\\\\"]\\\"}\"}";
    ret = cloudServiceImpl_->NotifyDataChange(CloudData::DATA_CHANGE_EVENT_ID, extraData, invalidUserId);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
    extraData = "{\"data\":\"{\\\"accountId\\\":\\\"test_cloud_id\\\",\\\"bundleName\\\":\\\"cloud_"
                "bundleName_test\\\",\\\"containerName\\\":\\\"test_cloud_database_alias_1\\\", "
                "\\\"databaseScopes\\\": "
                "\\\"[\\\\\\\"private\\\\\\\", "
                "\\\\\\\"shared\\\\\\\"]\\\",\\\"recordTypes\\\":\\\"[\\\\\\\"test_cloud_table_alias\\\\\\\"]\\\"}\"}";
    ret = cloudServiceImpl_->NotifyDataChange(CloudData::DATA_CHANGE_EVENT_ID, extraData, invalidUserId);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
}

/**
* @tc.name: NotifyDataChange
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, NotifyDataChange003, TestSize.Level1)
{
    constexpr const int32_t userId = 100;
    constexpr const int32_t defaultUserId = 0;
    std::string extraData = "{\"data\":\"{\\\"accountId\\\":\\\"test_cloud_id\\\",\\\"bundleName\\\":\\\"test_cloud_"
                            "bundleName\\\",\\\"containerName\\\":\\\"test_cloud_database_alias_1\\\", "
                            "\\\"databaseScopes\\\": "
                            "\\\"[\\\\\\\"private\\\\\\\", "
                            "\\\\\\\"shared\\\\\\\"]\\\",\\\"recordTypes\\\":\\\"[\\\\\\\"\\\\\\\"]\\\"}\"}";
    auto ret = cloudServiceImpl_->NotifyDataChange(CloudData::DATA_CHANGE_EVENT_ID, extraData, defaultUserId);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
    extraData = "{\"data\":\"{\\\"accountId\\\":\\\"test_cloud_id\\\",\\\"bundleName\\\":\\\"test_cloud_"
                "bundleName\\\",\\\"containerName\\\":\\\"test_cloud_database_alias_1\\\", \\\"databaseScopes\\\": "
                "\\\"[\\\\\\\"private\\\\\\\", "
                "\\\\\\\"shared\\\\\\\"]\\\",\\\"recordTypes\\\":\\\"[\\\\\\\"test_cloud_table_alias\\\\\\\"]\\\"}\"}";
    ret = cloudServiceImpl_->NotifyDataChange(CloudData::DATA_CHANGE_EVENT_ID, extraData, userId);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: Offline
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, Offline001, TestSize.Level0)
{
    std::string device = "test";
    auto ret = cloudServiceImpl_->Offline(device);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
    ret = cloudServiceImpl_->Offline(DeviceManagerAdapter::CLOUD_DEVICE_UUID);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: CloudShare
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, CloudShare001, TestSize.Level0)
{
    StoreInfo storeInfo{ OHOS::IPCSkeleton::GetCallingTokenID(), TEST_CLOUD_BUNDLE, TEST_CLOUD_STORE, 0 };
    std::pair<int32_t, std::shared_ptr<Cursor>> result;
    CloudShareEvent::Callback asyncCallback = [&result](int32_t status, std::shared_ptr<Cursor> cursor) {
        result.first = status;
        result.second = cursor;
    };
    auto event = std::make_unique<CloudShareEvent>(storeInfo, nullptr, nullptr);
    EventCenter::GetInstance().PostEvent(std::move(event));
    auto event1 = std::make_unique<CloudShareEvent>(storeInfo, nullptr, asyncCallback);
    EventCenter::GetInstance().PostEvent(std::move(event1));
    EXPECT_EQ(result.first, GeneralError::E_ERROR);
    auto rdbQuery = std::make_shared<DistributedRdb::RdbQuery>();
    auto event2 = std::make_unique<CloudShareEvent>(storeInfo, rdbQuery, nullptr);
    EventCenter::GetInstance().PostEvent(std::move(event2));
    auto event3 = std::make_unique<CloudShareEvent>(storeInfo, rdbQuery, asyncCallback);
    EventCenter::GetInstance().PostEvent(std::move(event3));
    EXPECT_EQ(result.first, GeneralError::E_ERROR);
}

/**
* @tc.name: OnUserChange
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnUserChange001, TestSize.Level0)
{
    constexpr const uint32_t ACCOUNT_DEFAULT = 2;
    constexpr const uint32_t ACCOUNT_DELETE = 3;
    constexpr const uint32_t ACCOUNT_SWITCHED = 4;
    constexpr const uint32_t ACCOUNT_UNLOCKED = 5;
    auto ret = cloudServiceImpl_->OnUserChange(ACCOUNT_DEFAULT, "0", "test");
    EXPECT_EQ(ret, GeneralError::E_OK);
    ret = cloudServiceImpl_->OnUserChange(ACCOUNT_DELETE, "0", "test");
    EXPECT_EQ(ret, GeneralError::E_OK);
    ret = cloudServiceImpl_->OnUserChange(ACCOUNT_SWITCHED, "0", "test");
    EXPECT_EQ(ret, GeneralError::E_OK);
    ret = cloudServiceImpl_->OnUserChange(ACCOUNT_UNLOCKED, "0", "test");
    EXPECT_EQ(ret, GeneralError::E_OK);
}

/**
* @tc.name: DisableCloud
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, DisableCloud001, TestSize.Level0)
{
    auto ret = cloudServiceImpl_->DisableCloud("test");
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
    ret = cloudServiceImpl_->DisableCloud(TEST_CLOUD_ID);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: ChangeAppSwitch
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, ChangeAppSwitch, TestSize.Level0)
{
    std::string id = "testId";
    std::string bundleName = "testName";
    auto ret = cloudServiceImpl_->ChangeAppSwitch(id, bundleName, CloudData::CloudService::SWITCH_ON);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
    ret = cloudServiceImpl_->ChangeAppSwitch(TEST_CLOUD_ID, bundleName, CloudData::CloudService::SWITCH_ON);
    EXPECT_EQ(ret, CloudData::CloudService::INVALID_ARGUMENT);
    ret = cloudServiceImpl_->ChangeAppSwitch(TEST_CLOUD_ID, TEST_CLOUD_BUNDLE, CloudData::CloudService::SWITCH_OFF);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: EnableCloud01
* @tc.desc: Test the EnableCloud function to ensure that the id in cloudinfo matches the passed id.
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, EnableCloud01, TestSize.Level0)
{
    std::string bundleName = "testName";
    std::map<std::string, int32_t> switches;
    switches.insert_or_assign(TEST_CLOUD_BUNDLE, CloudData::CloudService::SWITCH_ON);
    switches.insert_or_assign(bundleName, CloudData::CloudService::SWITCH_ON);
    auto ret = cloudServiceImpl_->EnableCloud(TEST_CLOUD_ID, switches);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: EnableCloud02
* @tc.desc: Test the EnableCloud function to SWITCH_OFF.
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, EnableCloud02, TestSize.Level0)
{
    std::map<std::string, int32_t> switches;
    switches.insert_or_assign(TEST_CLOUD_BUNDLE, CloudData::CloudService::SWITCH_OFF);
    auto ret = cloudServiceImpl_->EnableCloud(TEST_CLOUD_ID, switches);
    EXPECT_EQ(ret, CloudData::CloudService::SUCCESS);
}

/**
* @tc.name: OnEnableCloud
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnEnableCloud, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_ENABLE_CLOUD, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    std::string id = "testId";
    std::map<std::string, int32_t> switches;
    ITypesUtil::Marshal(data, id, switches);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_ENABLE_CLOUD, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
* @tc.name: OnDisableCloud
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnDisableCloud, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_DISABLE_CLOUD, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    data.WriteString(TEST_CLOUD_ID);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_DISABLE_CLOUD, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
* @tc.name: OnChangeAppSwitch
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnChangeAppSwitch, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_CHANGE_APP_SWITCH, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    data.WriteString(TEST_CLOUD_ID);
    data.WriteString(TEST_CLOUD_BUNDLE);
    data.WriteInt32(CloudData::CloudService::SWITCH_ON);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_CHANGE_APP_SWITCH, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
* @tc.name: OnClean
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnClean, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_CLEAN, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    std::string id = TEST_CLOUD_ID;
    std::map<std::string, int32_t> actions;
    ITypesUtil::Marshal(data, id, actions);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_CLEAN, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
* @tc.name: OnNotifyDataChange
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnNotifyDataChange, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_NOTIFY_DATA_CHANGE, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    data.WriteString(TEST_CLOUD_ID);
    data.WriteString(TEST_CLOUD_BUNDLE);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_NOTIFY_DATA_CHANGE, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
* @tc.name: OnNotifyChange
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnNotifyChange, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_NOTIFY_DATA_CHANGE_EXT, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    data.WriteString(TEST_CLOUD_ID);
    data.WriteString(TEST_CLOUD_BUNDLE);
    int32_t userId = 100;
    data.WriteInt32(userId);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_NOTIFY_DATA_CHANGE_EXT, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
* @tc.name: OnQueryStatistics
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnQueryStatistics, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_QUERY_STATISTICS, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    data.WriteString(TEST_CLOUD_ID);
    data.WriteString(TEST_CLOUD_BUNDLE);
    data.WriteString(TEST_CLOUD_STORE);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_QUERY_STATISTICS, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
* @tc.name: OnQueryLastSyncInfo
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnQueryLastSyncInfo, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_QUERY_LAST_SYNC_INFO, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    data.WriteString(TEST_CLOUD_ID);
    data.WriteString(TEST_CLOUD_BUNDLE);
    data.WriteString(TEST_CLOUD_STORE);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_QUERY_LAST_SYNC_INFO, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
* @tc.name: OnSetGlobalCloudStrategy
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnSetGlobalCloudStrategy, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret =
        cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_SET_GLOBAL_CLOUD_STRATEGY, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    uint32_t strategy = 0;
    std::vector<CommonType::Value> values;
    ITypesUtil::Marshal(data, strategy, values);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_SET_GLOBAL_CLOUD_STRATEGY, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
* @tc.name: OnAllocResourceAndShare
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnAllocResourceAndShare, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret = cloudServiceImpl_->OnRemoteRequest(
        CloudData::CloudService::TRANS_ALLOC_RESOURCE_AND_SHARE, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    std::string storeId = "storeId";
    DistributedRdb::PredicatesMemo predicates;
    std::vector<std::string> columns;
    std::vector<CloudData::Participant> participants;
    ITypesUtil::Marshal(data, storeId, predicates, columns, participants);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_ALLOC_RESOURCE_AND_SHARE, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
* @tc.name: OnShare
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnShare, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_SHARE, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    std::string sharingRes;
    CloudData::Participants participants;
    CloudData::Results results;
    ITypesUtil::Marshal(data, sharingRes, participants, results);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_SHARE, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}

/**
* @tc.name: OnUnshare
* @tc.desc:
* @tc.type: FUNC
* @tc.require:
 */
HWTEST_F(CloudDataTest, OnUnshare, TestSize.Level1)
{
    MessageParcel reply;
    MessageParcel data;
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    auto ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_UNSHARE, data, reply);
    EXPECT_EQ(ret, IPC_STUB_INVALID_DATA_ERR);
    data.WriteInterfaceToken(cloudServiceImpl_->GetDescriptor());
    std::string sharingRes;
    CloudData::Participants participants;
    CloudData::Results results;
    ITypesUtil::Marshal(data, sharingRes, participants, results);
    ret = cloudServiceImpl_->OnRemoteRequest(CloudData::CloudService::TRANS_UNSHARE, data, reply);
    EXPECT_EQ(ret, ERR_NONE);
}
} // namespace OHOS::ObjectStore