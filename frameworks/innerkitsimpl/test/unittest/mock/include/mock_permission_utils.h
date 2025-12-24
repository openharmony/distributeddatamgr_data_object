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

#ifndef DATA_OBJECT_MOCK_PERMISSION_UTILS_H
#define DATA_OBJECT_MOCK_PERMISSION_UTILS_H

#include "access_token.h"
#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
namespace ObjectStore {
using namespace OHOS::Security::AccessToken;
class MockNativeToken {
public:
    explicit MockNativeToken(const std::string& process);
    ~MockNativeToken();
private:
    uint64_t selfToken_;
};

class ObjectStoreTestUtils {
public:
    static constexpr int32_t DEFAULT_API_VERSION = 12;
    static void SetTestEvironment(uint64_t shellTokenId);
    static void ResetTestEvironment();
    static uint64_t GetShellTokenId();

    static AccessTokenID GetNativeTokenIdFromProcess(const std::string& process);
};
}  // namespace ObjectStore
}  // namespace OHOS
#endif  // DATA_OBJECT_MOCK_PERMISSION_UTILS_H
