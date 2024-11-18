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

using namespace testing::ext;
using namespace OHOS::ObjectStore;
using namespace OHOS;
using namespace std;

namespace {
class DistributedObjectImplTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void DistributedObjectImplTest::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void DistributedObjectImplTest::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void DistributedObjectImplTest::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void DistributedObjectImplTest::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: BindAssetStore_001
 * @tc.desc: Abnormal test for BindAssetStore, data is not stored
 * @tc.type: FUNC
 */

HWTEST_F(DistributedObjectImplTest, BindAssetStore_001, TestSize.Level1)
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
}