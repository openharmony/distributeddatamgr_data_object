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

#ifndef COLLABORATION_EDIT_DB_STORE_H
#define COLLABORATION_EDIT_DB_STORE_H

#include "db_store_config.h"
#include "grd_type_export.h"

namespace OHOS::CollaborationEdit {
class DBStore {
public:
    DBStore(GRD_DB *db, std::string name);
    ~DBStore();
    std::string GetLocalId();
    GRD_DB *GetDB();
    int32_t Sync(GRD_SyncModeE mode, uint64_t syncId, GRD_SyncTaskCallbackFuncT callbackFunc);
    int32_t SetThreadPool();

    int ApplyUpdate(std::string &applyInfo);
    int WriteUpdate(const char *equipId, const uint8_t *data, uint32_t size, const std::string &watermark);
    int GetRelativePos(const char *tableName, const char *nodeSize, uint32_t pos, std::string &relPos);
    int GetAbsolutePos(const char *tableName, const char *relPos, const char *nodeSize, uint32_t *pos);
private:
    GRD_DB *db_;
    std::string name_;
    std::shared_ptr<std::string> localId_;
};
} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_DB_STORE_H
