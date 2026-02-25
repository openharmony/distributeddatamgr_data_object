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

/**
 * @tc.name: Unmarshalling_002
 * @tc.desc: Normal test for Unmarshalling
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Unmarshalling_002, TestSize.Level1)
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
    ITypesUtil::Marshalling(input, data);
    Asset output;
    bool ret = ITypesUtil::Unmarshalling(output, data);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: Marshalling_003
 * @tc.desc: Test Marshalling with empty AssetBind fanc
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Marshalling_003, TestSize.Level1)
{
    AssetBindInfo input = {
        .storeName = "",
        .tableName = "",
        .primaryKey = {},
        .field = "",
        .assetName = ""
    };
    MessageParcel data;
    auto ret = ITypesUtil::Marshalling(input, data);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: Marshalling_004
 * @tc.desc: Test Marshalling with very long strings
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Marshalling_004, TestSize.Level1)
{
    AssetBindInfo input = {
        .storeName = std::string(256, 'a'),
        .tableName = std::string(256, 'b'),
        .primaryKey = {
            {std::string(128, 'c'), 123},
            {std::string(128, 'd'), "test1"}
        },
        .field = std::string(256, 'e'),
        .assetName = std::string(256, 'f')
    };
    MessageParcel data;
    auto ret = ITypesUtil::Marshalling(input, data);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: Marshalling_005
 * @tc.desc: Test Marshalling with special characters
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Marshalling_005, TestSize.Level1)
{
    AssetBindInfo input = {
        .storeName = "store@name#with$special%chars",
        .tableName = "table@name#with$special%chars",
        .primaryKey = {
            {"key@1#with$special%chars", 123},
            {"key@2#with$special%chars", "test1"}
        },
        .field = "field@name#with$special%chars",
        .assetName = "asset@name#with$special%chars"
    };
    MessageParcel data;
    auto ret = ITypesUtil::Marshalling(input, data);
    EXPECT_TRUE(ret);
    AssetBindInfo output;
    data.RewindRead(0);
    auto unmarshalRet = ITypesUtil::Unmarshalling(output, data);
    EXPECT_TRUE(unmarshalRet);
    EXPECT_EQ(output.storeName, "store@name#with$special%chars");
    EXPECT_EQ(output.tableName, "table@name#with$special%chars");
    EXPECT_EQ(output.field, "field@name#with$special%chars");
    EXPECT_EQ(output.assetName, "asset@name#with$special%chars");
}
}
