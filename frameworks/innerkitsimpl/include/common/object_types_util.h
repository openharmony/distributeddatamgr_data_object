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

#ifndef DISTRIBUTED_OBJECT_TYPES_UTIL_H
#define DISTRIBUTED_OBJECT_TYPES_UTIL_H

#include "itypes_util.h"
#include "object_types.h"

namespace OHOS::ITypesUtil {
using AssetBindInfo = OHOS::ObjectStore::AssetBindInfo;
using Asset = OHOS::ObjectStore::Asset;

template<>
bool Marshalling(const AssetBindInfo &input, MessageParcel &data);
template<>
bool Unmarshalling(AssetBindInfo &output, MessageParcel &data);
template<>
bool Marshalling(const Asset &input, MessageParcel &data);
template<>
bool Unmarshalling(Asset &output, MessageParcel &data);
}
#endif // DISTRIBUTED_RDB_RDB_TYPES_UTIL_H
