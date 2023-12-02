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

#ifndef BYTES_H
#define BYTES_H

#include <memory>
#include <vector>

namespace OHOS::ObjectStore {
using Bytes = std::vector<uint8_t>;
static const char *FIELDS_PREFIX = "p_";
static const int32_t FIELDS_PREFIX_LEN = 2;

static constexpr const char * STATUS_SUFFIX = ".status";
static constexpr const char * NAME_SUFFIX = ".name";
static constexpr const char * URI_SUFFIX = ".uri";
static constexpr const char * PATH_SUFFIX = ".path";
static constexpr const char * CREATE_TIME_SUFFIX = ".createTime";
static constexpr const char * MODIFY_TIME_SUFFIX = ".modifyTime";
static constexpr const char * SIZE_SUFFIX = ".size";
} // namespace OHOS::ObjectStore

#endif // BYTES_H
