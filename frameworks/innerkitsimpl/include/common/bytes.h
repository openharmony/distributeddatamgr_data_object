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
static const std::string STRING_PREFIX = "[STRING]";
static const int32_t STRING_PREFIX_LEN = STRING_PREFIX.length();

static const std::string DEVICEID_KEY = "#DEVICE_ID#";
static const std::string STATUS_SUFFIX = ".status";
static const std::string NAME_SUFFIX = ".name";
static const std::string URI_SUFFIX = ".uri";
static const std::string PATH_SUFFIX = ".path";
static const std::string CREATE_TIME_SUFFIX = ".createTime";
static const std::string MODIFY_TIME_SUFFIX = ".modifyTime";
static const std::string SIZE_SUFFIX = ".size";
static const std::string ASSET_DOT = ".";
} // namespace OHOS::ObjectStore

#endif // BYTES_H
