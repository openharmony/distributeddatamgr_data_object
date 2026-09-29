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

/**
 * @tc.name: Marshalling_006
 * @tc.desc: Test Asset Marshalling with extension field set
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Marshalling_006, TestSize.Level1)
{
    Asset input = {
        .version = 1,
        .status = Asset::STATUS_INSERT,
        .id = "id_006",
        .name = "photo.jpg",
        .uri = "file://com.example.myapp/data/dir/photo.jpg",
        .createTime = "2024/10/26 19:48:00",
        .modifyTime = "2024/10/26 20:10:00",
        .size = "1024",
        .hash = "abc123def456",
        .path = "/dir/photo.jpg",
        .extension = "thumbnail_uri=file://com.example.myapp/data/dir/thumb.jpg",
    };
    MessageParcel data;
    auto ret = ITypesUtil::Marshalling(input, data);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: Unmarshalling_003
 * @tc.desc: Test Asset Unmarshalling round-trip with extension field
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Unmarshalling_003, TestSize.Level1)
{
    Asset input = {
        .version = 1,
        .status = Asset::STATUS_INSERT,
        .id = "id_003",
        .name = "photo.jpg",
        .uri = "file://com.example.myapp/data/dir/photo.jpg",
        .createTime = "2024/10/26 19:48:00",
        .modifyTime = "2024/10/26 20:10:00",
        .size = "1024",
        .hash = "abc123def456",
        .path = "/dir/photo.jpg",
        .extension = "thumbnail_uri=file://com.example.myapp/data/dir/thumb.jpg",
    };
    MessageParcel data;
    ITypesUtil::Marshalling(input, data);
    Asset output;
    auto ret = ITypesUtil::Unmarshalling(output, data);
    EXPECT_TRUE(ret);
    EXPECT_EQ(output.extension, input.extension);
}

/**
 * @tc.name: Marshalling_007
 * @tc.desc: Test Asset Marshalling with empty extension field
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Marshalling_007, TestSize.Level1)
{
    Asset input = {
        .version = 0,
        .status = Asset::STATUS_NORMAL,
        .id = "id_007",
        .name = "empty_ext.txt",
        .uri = "file://com.example.myapp/data/dir/empty_ext.txt",
        .createTime = "2024/10/26 19:48:00",
        .modifyTime = "2024/10/26 20:10:00",
        .size = "0",
        .hash = "",
        .path = "/dir/empty_ext.txt",
        .extension = "",
    };
    MessageParcel data;
    auto ret = ITypesUtil::Marshalling(input, data);
    EXPECT_TRUE(ret);
}

/**
 * @tc.name: Unmarshalling_004
 * @tc.desc: Test Asset round-trip with empty extension field
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Unmarshalling_004, TestSize.Level1)
{
    Asset input = {
        .version = 0,
        .status = Asset::STATUS_NORMAL,
        .id = "id_004",
        .name = "empty_ext.txt",
        .uri = "file://com.example.myapp/data/dir/empty_ext.txt",
        .createTime = "2024/10/26 19:48:00",
        .modifyTime = "2024/10/26 20:10:00",
        .size = "0",
        .hash = "",
        .path = "/dir/empty_ext.txt",
        .extension = "",
    };
    MessageParcel data;
    ITypesUtil::Marshalling(input, data);
    Asset output;
    auto ret = ITypesUtil::Unmarshalling(output, data);
    EXPECT_TRUE(ret);
    EXPECT_EQ(output.extension, "");
}

/**
 * @tc.name: Marshalling_008
 * @tc.desc: Test Asset round-trip with special characters in extension field
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Marshalling_008, TestSize.Level1)
{
    Asset input = {
        .version = 2,
        .status = Asset::STATUS_UPDATE,
        .id = "id_008",
        .name = "special@file.dat",
        .uri = "file://com.example.myapp/data/dir/special@file.dat",
        .createTime = "2024/10/26 19:48:00",
        .modifyTime = "2024/10/26 20:10:00",
        .size = "2048",
        .hash = "hash@#$%^&*()",
        .path = "/dir/special@file.dat",
        .extension = "key1=value1;key2=value2@#$%;path=/a/b/c",
    };
    MessageParcel data;
    auto ret = ITypesUtil::Marshalling(input, data);
    EXPECT_TRUE(ret);
    Asset output;
    data.RewindRead(0);
    auto unmarshalRet = ITypesUtil::Unmarshalling(output, data);
    EXPECT_TRUE(unmarshalRet);
    EXPECT_EQ(output.extension, "key1=value1;key2=value2@#$%;path=/a/b/c");
    EXPECT_EQ(output.name, "special@file.dat");
    EXPECT_EQ(output.hash, "hash@#$%^&*()");
}

/**
 * @tc.name: Unmarshalling_005
 * @tc.desc: Test Asset full round-trip verifying all fields including extension
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Unmarshalling_005, TestSize.Level1)
{
    Asset input = {
        .status = Asset::STATUS_UPDATE,
        .name = "full_round_trip.png",
        .uri = "file://com.example.myapp/data/dir/full_round_trip.png",
        .createTime = "2024/11/01 10:00:00",
        .modifyTime = "2024/11/01 12:30:00",
        .size = "4096",
        .hash = "sha256:abcdef0123456789",
        .path = "/dir/full_round_trip.png",
        .extension = "mime_type=image/png;width=1920;height=1080",
    };
    MessageParcel data;
    ITypesUtil::Marshalling(input, data);
    Asset output;
    auto ret = ITypesUtil::Unmarshalling(output, data);
    EXPECT_TRUE(ret);
    EXPECT_EQ(output.status, input.status);
    EXPECT_EQ(output.name, input.name);
    EXPECT_EQ(output.uri, input.uri);
    EXPECT_EQ(output.createTime, input.createTime);
    EXPECT_EQ(output.modifyTime, input.modifyTime);
    EXPECT_EQ(output.size, input.size);
    EXPECT_EQ(output.hash, input.hash);
    EXPECT_EQ(output.path, input.path);
    EXPECT_EQ(output.extension, input.extension);
}

/**
 * @tc.name: Marshalling_009
 * @tc.desc: Test Asset Marshalling with very long extension string
 * @tc.type: FUNC
 */
HWTEST_F(ObjectTypesUtilTest, Marshalling_009, TestSize.Level1)
{
    Asset input = {
        .version = 0,
        .status = Asset::STATUS_NORMAL,
        .id = "id_009",
        .name = "long_ext.bin",
        .uri = "file://com.example.myapp/data/dir/long_ext.bin",
        .createTime = "2024/10/26 19:48:00",
        .modifyTime = "2024/10/26 20:10:00",
        .size = "512",
        .hash = "hash_long",
        .path = "/dir/long_ext.bin",
        .extension = std::string(256, 'x'),
    };
    MessageParcel data;
    auto ret = ITypesUtil::Marshalling(input, data);
    EXPECT_TRUE(ret);
    Asset output;
    data.RewindRead(0);
    auto unmarshalRet = ITypesUtil::Unmarshalling(output, data);
    EXPECT_TRUE(unmarshalRet);
    EXPECT_EQ(output.extension, std::string(256, 'x'));
}
}
