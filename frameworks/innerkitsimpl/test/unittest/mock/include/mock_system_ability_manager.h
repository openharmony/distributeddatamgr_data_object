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

#ifndef DATA_OBJECT_MOCK_SYSTEM_ABILITY_MANAGER_H
#define DATA_OBJECT_MOCK_SYSTEM_ABILITY_MANAGER_H

#include "gmock/gmock.h"
#include "if_system_ability_manager.h"
#include "iremote_object.h"
#include "iremote_stub.h"

namespace OHOS {
namespace ObjectStore {
class MockSystemAbilityManager : public IRemoteStub<ISystemAbilityManager> {
public:
    MockSystemAbilityManager() {};
    virtual ~MockSystemAbilityManager() {};

    MOCK_METHOD(CheckSystemAbility, sptr<IRemoteObject>(int32_t));
};
} // namespace ObjectStore
} // namespace OHOS
#endif // DATA_OBJECT_MOCK_SYSTEM_ABILITY_MANAGER_H