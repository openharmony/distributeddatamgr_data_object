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

#ifndef COLLABORATION_EDIT_DB_COMMON_H
#define COLLABORATION_EDIT_DB_COMMON_H

#include <sys/stat.h>
#include <cstdint>
#include <string>
#include "unistd.h"

#define UNUSED_FUNCTION __attribute__((unused))
namespace OHOS::CollaborationEdit {
class DBCommon final {
public:
    static int CreateDirectory(const std::string &dbPath);
};

static const std::string OP_LOG_DIR = "_oplog";
static constexpr mode_t DIR_RIGHT = 0771;
static UNUSED_FUNCTION uint32_t GetUid()
{
#ifdef WINDOWS_PLATFORM
    return 0;
#else
    return getuid();
#endif
}

static UNUSED_FUNCTION int MkDir(const std::string &filePath, mode_t dirRight = DIR_RIGHT)
{
#ifdef WINDOWS_PLATFORM
    return mkdir(filePath.c_str());
#else
    return mkdir(filePath.c_str(), dirRight);
#endif
}

} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_DB_COMMON_H