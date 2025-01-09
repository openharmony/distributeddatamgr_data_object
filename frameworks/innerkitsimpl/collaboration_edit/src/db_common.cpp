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

#define LOG_TAG "DBCommon"

#include "db_common.h"

#include "acl.h"
#include "log_print.h"

namespace OHOS::CollaborationEdit {
using namespace OHOS::DATABASE_UTILS;
constexpr int32_t SERVICE_GID = 3012;
int DBCommon::CreateDirectory(const std::string &dbPath)
{
    std::string tempPath = dbPath;
    std::vector<std::string> directories;

    size_t pos = tempPath.find('/');
    while (pos != std::string::npos) {
        std::string dir = tempPath.substr(0, pos);
        directories.push_back(dir);
        tempPath = tempPath.substr(pos + 1);
        pos = tempPath.find('/');
    }
    directories.push_back(tempPath);

    std::string targetDir;
    for (const std::string &directory : directories) {
        targetDir = targetDir + "/" + directory;
        if (access(targetDir.c_str(), F_OK) != 0) {
            if (MkDir(targetDir)) {
                LOG_ERROR("[CreateDirectory] mkdir errno[%{public}d] %{public}s", errno, targetDir.c_str());
                return -1;
            }

            Acl acl(targetDir);
            acl.SetDefaultUser(GetUid(), Acl::R_RIGHT | Acl::W_RIGHT);
            acl.SetDefaultGroup(SERVICE_GID, Acl::R_RIGHT | Acl::W_RIGHT);
        }
    }
    return 0;
}
} // namespace OHOS::CollaborationEdit
