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

#include "ohos.data.distributedDataObject.impl.hpp"
#include "ohos.data.distributedDataObject.DataObject.hpp"
#include "stdexcept"
#include "logger.h"

namespace {
using namespace OHOS::ObjectStore;

::ohos::data::distributedDataObject::DataObject Create(uintptr_t context, uintptr_t source,
    ::taihe::array_view<::taihe::string> keys)
{
    return taihe::make_holder<DataObjectImpl, ::ohos::data::distributedDataObject::DataObject>(
        reinterpret_cast<ani_object>(context), reinterpret_cast<ani_object>(source), keys);
}

::taihe::string GenSessionId()
{
    auto random = DataObjectImpl::GenerateRandomNum();
    return ::taihe::string(random);
}

} // namespace

// NOLINTBEGIN
TH_EXPORT_CPP_API_CreateImpl(Create);
TH_EXPORT_CPP_API_GenSessionId(GenSessionId);
// NOLINTEND
