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
#include <random>

#include "db_store.h"

#include "db_store_manager.h"
#include "grd_type_export.h"
#include "log_print.h"
#include "rd_utils.h"

namespace OHOS::CollaborationEdit {
DBStore::DBStore(GRD_DB *db, std::string name) : db_(db), name_(name)
{
    GetLocalId();
    int32_t ret = SetThreadPool();
    if (ret != GRD_OK) {
        LOG_ERROR("registry threadpool go wrong, ret: %{public}d", ret);
    }
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

int32_t DBStore::SetThreadPool()
{
    DBStoreManager::GetInstance().InitThreadPool();
    int32_t ret = RdUtils::RdRegistryThreadPool(db_, &DBStoreManager::threadPool_);
    return ret;
}

int32_t DBStore::Sync(GRD_SyncModeE mode, uint64_t syncId, GRD_SyncTaskCallbackFuncT callbackFunc)
{
    LOG_INFO("Sync START, mode=%{public}d", static_cast<int32_t>(mode));
    if (mode == GRD_SYNC_MODE_INVALID) {
        LOG_ERROR("sync mode is wrong");
        return GRD_INVALID_ARGS;
    }
    char equipId[] = "cloud";
    char *g_equipIds[] = {equipId};
    GRD_SyncConfig config = {
        .mode = mode,
        .equipIds = g_equipIds,
        .size = 1,
        .callbackFunc = callbackFunc,
        .timeout = 1u,
        .syncId = syncId,
    };
    int32_t ret = RdUtils::RdSync(db_, &config);
    LOG_INFO("Sync END, ret=%{public}d", ret);
    return ret;
}

GRD_DB *DBStore::GetDB()
{
    return db_;
}

int DBStore::ApplyUpdate(std::string &applyInfo)
{
    char *retStr = nullptr;
    int ret = RdUtils::RdApplyUpdate(db_, &retStr);
    if (ret != GRD_OK || retStr == nullptr) {
        LOG_ERROR("ApplyUpdate go wrong. err: %{public}d", ret);
        return RdUtils::TransferToNapiErrNo(ret);
    }
    applyInfo = std::string(retStr);
    (void)RdUtils::RdFreeValue(retStr);
    return RdUtils::TransferToNapiErrNo(ret);
}

int DBStore::WriteUpdate(const char *equipId, const uint8_t *data, uint32_t size, const std::string &watermark)
{
    int ret = RdUtils::RdWriteUpdate(db_, equipId, data, size, watermark);
    if (ret != GRD_OK) {
        LOG_ERROR("WriteUpdate go wrong. err: %{public}d", ret);
    }
    return RdUtils::TransferToNapiErrNo(ret);
}

int DBStore::GetRelativePos(const char *tableName, const char *nodeSize, uint32_t pos, std::string &relPos)
{
    char *retStr = nullptr;
    int ret = RdUtils::RdGetRelativePos(db_, tableName, nodeSize, pos, &retStr);
    if (ret != GRD_OK || retStr == nullptr) {
        LOG_ERROR("GetRelativePos go wrong. err: %{public}d", ret);
        return RdUtils::TransferToNapiErrNo(ret);
    }
    relPos = std::string(retStr);
    (void)RdUtils::RdFreeValue(retStr);
    return RdUtils::TransferToNapiErrNo(ret);
}

int DBStore::GetAbsolutePos(const char *tableName, const char *relPos, const char *nodeSize, uint32_t *pos)
{
    int ret = RdUtils::RdGetAbsolutePos(db_, tableName, relPos, nodeSize, pos);
    if (ret != GRD_OK) {
        LOG_ERROR("GetAbsolutePos go wrong. err: %{public}d", ret);
    }
    return RdUtils::TransferToNapiErrNo(ret);
}
} // namespace OHOS::CollaborationEdit
