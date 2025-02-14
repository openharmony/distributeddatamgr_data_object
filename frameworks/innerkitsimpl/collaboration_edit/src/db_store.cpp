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

#include <random>

#include "log_print.h"
#include "rd_utils.h"

namespace OHOS::CollaborationEdit {
DBStore::DBStore(GRD_DB *db, std::string name) : db_(db), name_(name)
{
    GetLocalId();
}

DBStore::~DBStore()
{}

std::string DBStore::GetLocalId()
{
    if (localId_) {
        return *localId_;
    }
    char *retStr = nullptr;
    int ret = RdUtils::RdGetLocalId(db_, &retStr);
    if (ret != GRD_OK || retStr == nullptr) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<uint64_t> dis(0, std::numeric_limits<uint64_t>::max());
        uint64_t random_num = dis(gen);
        localId_ = std::make_shared<std::string>(std::to_string(random_num));
        ret = RdUtils::RdSetLocalId(db_, (*localId_).c_str());
        if (ret != GRD_OK) {
            LOG_ERROR("Set local id go wrong. err: %{public}d", ret);
            return "";
        }
    } else {
        localId_ = std::make_shared<std::string>(retStr);
        (void)RdUtils::RdFreeValue(retStr);
    }
    return *localId_;
}

GRD_DB *DBStore::GetDB()
{
    return db_;
}
} // namespace OHOS::CollaborationEdit
