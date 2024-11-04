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

#include "object_types_util.h"

using namespace testing::ext;
using namespace OHOS::ObjectStore;
using namespace OHOS;

namespace {
class ObjectTypesUtilTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ObjectTypesUtilTest::SetUpTestCase(void)
{
    // input testsuit setup step，setup invoked before all testcases
}

void ObjectTypesUtilTest::TearDownTestCase(void)
{
    // input testsuit teardown step，teardown invoked after all testcases
}

void ObjectTypesUtilTest::SetUp(void)
{
    // input testcase setup step，setup invoked before each testcases
}

void ObjectTypesUtilTest::TearDown(void)
{
    // input testcase teardown step，teardown invoked after each testcases
}

/**
 * @tc.name: Marshalling_001
 * @tc.desc: Normal test for Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Marshalling_001, TestSize.Level1)
{
    AssetBindInfo input = {
        .storeName = "storeName",
        .tableName = "tableName",
        .primaryKey = {
            {"data1", 123},
            {"data2", "test1"}
        },
        .field = "field",
        .assetName = "assetName"
    };
    MessageParcel data;
    bool ret = ITypesUtil::Marshalling(input, data);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: Unmarshalling_001
 * @tc.desc: Normal test for Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Unmarshalling_001, TestSize.Level1)
{
    AssetBindInfo input = {
        .storeName = "storeName",
        .tableName = "tableName",
        .primaryKey = {
            {"data1", 123},
            {"data2", "test1"}
        },
        .field = "field",
        .assetName = "assetName"
    };
    MessageParcel data;
    ITypesUtil::Marshalling(input, data);
    AssetBindInfo output;
    bool ret = ITypesUtil::Unmarshalling(output, data);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: Marshalling_002
 * @tc.desc: Normal test for Marshalling
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Marshalling_002, TestSize.Level1)
{
    Asset input = {
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
    MessageParcel data;
    bool ret = ITypesUtil::Marshalling(input, data);
    EXPECT_TRUE(ret);
}
}
