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

#include "sa_mgr_client.h"
#include "hilog_tag_wrapper.h"

#include "if_system_ability_manager.h"
#include "iservice_registry.h"

namespace OHOS {
namespace ObjectStore {
SaMgrClient::SaMgrClient()
{}

SaMgrClient::~SaMgrClient()
{}

sptr<IRemoteObject> SaMgrClient::GetSystemAbility(const int32_t systemAbilityId)
{
    TAG_LOGI(AAFwkTag::TEST, "Test GetSystemAbility id : %{public}d", systemAbilityId);
    if (servicesMap_[systemAbilityId] == nullptr) {
        OHOS::sptr<ISystemAbilityManager> systemAbilityManager =
            SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            TAG_LOGE(AAFwkTag::TEST, "%s:fail to get Registry", __func__);
            return nullptr;
        }
        OHOS::sptr<OHOS::IRemoteObject> object = systemAbilityManager->GetSystemAbility(systemAbilityId);
        servicesMap_[systemAbilityId] = object;
    }
    return servicesMap_[systemAbilityId];
}

sptr<IRemoteObject> SaMgrClient::CheckSystemAbility(const int32_t systemAbilityId)
{
    TAG_LOGI(AAFwkTag::TEST, "Test CheckSystemAbility id : %{public}d", systemAbilityId);
    if (servicesMap_[systemAbilityId] == nullptr) {
        OHOS::sptr<ISystemAbilityManager> systemAbilityManager =
                SystemAbilityManagerClient::GetInstance().GetSystemAbilityManager();
        if (systemAbilityManager == nullptr) {
            TAG_LOGE(AAFwkTag::TEST, "%s:fail to get Registry", __func__);
            return nullptr;
        }
        OHOS::sptr<OHOS::IRemoteObject> object = systemAbilityManager->CheckSystemAbility(systemAbilityId);
        servicesMap_[systemAbilityId] = object;
    }
    return servicesMap_[systemAbilityId];
}

void SaMgrClient::RegisterSystemAbility(const int32_t systemAbilityId, sptr<IRemoteObject> broker)
{
    TAG_LOGI(AAFwkTag::TEST, "Test RegisterSystemAbility id : %{public}d", systemAbilityId);
    servicesMap_[systemAbilityId] = broker;
}
}  // namespace ObjectStore
}  // namespace OHOS
