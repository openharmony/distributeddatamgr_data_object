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

#define LOG_TAG "DBStore"

#include "db_store.h"

#include "log_print.h"
#include "rd_utils.h"

namespace OHOS::CollaborationEdit {
DBStore::DBStore(GRD_DB *db, std::string name) : db_(db), name_(name)
{
    int ret = RdUtils::RdRegisterEquipId(db, reinterpret_cast<GrdEquipIdGetFuncT>(DBStore::GetEquipId));
    if (ret != GRD_OK) {
        LOG_ERROR("register equip id go wrong. err: %{public}d", ret);
    }
}

DBStore::~DBStore()
{
    RdUtils::RdDbClose(db_, GRD_DB_CLOSE);
}

const char *DBStore::GetEquipId(void)
{
    std::string *str = new std::string("A");
    return (*str).c_str();
}

GRD_DB *DBStore::GetDB()
{
    return db_;
}
} // namespace OHOS::CollaborationEdit
