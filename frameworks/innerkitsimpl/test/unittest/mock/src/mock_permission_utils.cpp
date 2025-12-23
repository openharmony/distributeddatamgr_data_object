/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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
#include "mock_permission_utils.h"
#include "gtest/gtest.h"
#include <thread>

namespace OHOS {
namespace ObjectStore {
using namespace OHOS::Security::AccessToken;
namespace {
std::mutex g_lockSetToken;
uint64_t g_shellTokenId = 0;
} // namespace

void ObjectStoreTestUtils::SetTestEvironment(uint64_t shellTokenId)
{
    std::lock_guard<std::mutex> lock(g_lockSetToken);
    g_shellTokenId = shellTokenId;
}

void ObjectStoreTestUtils::ResetTestEvironment()
{
    std::lock_guard<std::mutex> lock(g_lockSetToken);
    g_shellTokenId = 0;
}

uint64_t ObjectStoreTestUtils::GetShellTokenId()
{
    std::lock_guard<std::mutex> lock(g_lockSetToken);
    return g_shellTokenId;
}

AccessTokenID ObjectStoreTestUtils::GetNativeTokenIdFromProcess(const std::string &process)
{
    uint64_t selfTokenId = GetSelfTokenID();
    EXPECT_EQ(0, SetSelfTokenID(ObjectStoreTestUtils::GetShellTokenId())); // set shell token

    std::string dumpInfo;
    AtmToolsParamInfo info;
    info.processName = process;
    AccessTokenKit::DumpTokenInfo(info, dumpInfo);
    size_t pos = dumpInfo.find("\"tokenID\": ");
    if (pos == std::string::npos) {
        return 0;
    }
    pos += std::string("\"tokenID\": ").length();
    std::string numStr;
    while (pos < dumpInfo.length() && std::isdigit(dumpInfo[pos])) {
        numStr += dumpInfo[pos];
        ++pos;
    }
    // restore
    EXPECT_EQ(0, SetSelfTokenID(selfTokenId));

    std::istringstream iss(numStr);
    AccessTokenID tokenID;
    iss >> tokenID;
    return tokenID;
}

MockNativeToken::MockNativeToken(const std::string& process)
{
    selfToken_ = GetSelfTokenID();
    uint32_t tokenId = ObjectStoreTestUtils::GetNativeTokenIdFromProcess(process);
    SetSelfTokenID(tokenId);
}

MockNativeToken::~MockNativeToken()
{
    SetSelfTokenID(selfToken_);
}
}  // namespace Security
}  // namespace OHOS