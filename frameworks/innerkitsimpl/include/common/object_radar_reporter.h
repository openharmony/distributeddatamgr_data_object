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

#ifndef OBJECT_RADAR_REPORTER_H
#define OBJECT_RADAR_REPORTER_H

#include "hisysevent.h"

namespace OHOS::ObjectStore {
enum BizScene {
    CREATE = 1,
    SAVE = 2,
};
enum CreateStage {
    INIT_STORE = 1,
    CREATE_TABLE = 2,
    RESTORE = 3,
    TRANSFER = 4,
};
enum SaveStage {
    SAVE_TO_SERVICE = 1,
    SAVE_TO_STORE = 2,
    PUSH_ASSETS = 3,
};
enum StageRes {
    IDLE = 0,
    RADAR_SUCCESS = 1,
    RADAR_FAILED = 2,
    CANCELLED = 3,
};
enum BizState {
    START = 1,
    FINISHED = 2,
};
enum ErrorCode {
    OFFSET = 27525120,
    DUPLICATE_CREATE = OFFSET,
    NO_MEMORY = OFFSET + 1,
    SA_DIED = OFFSET + 2,
    TIMEOUT = OFFSET + 3,
    IPC_ERROR = OFFSET + 4,
    NO_PERMISSION = OFFSET + 5,
    GETKV_FAILED = OFFSET + 6,
    DB_NOT_INIT = OFFSET + 7,
};
static constexpr char DOMAIN[] = "DISTDATAMGR";
const std::string EVENT_NAME = "DISTRIBUTED_DATA_OBJECT_BEHAVIOR";
static constexpr HiviewDFX::HiSysEvent::EventType TYPE = HiviewDFX::HiSysEvent::EventType::BEHAVIOR;
const std::string ORG_PKG = "distributeddata";
const std::string ERROR_CODE = "ERROR_CODE";
const std::string BIZ_STATE = "BIZ_STATE";

#define RADAR_REPORT(bizScene, bizStage, stageRes, ...)                                            \
({                                                                                                 \
    HiSysEventWrite(ObjectStore::DOMAIN, ObjectStore::EVENT_NAME, ObjectStore::TYPE,               \
        "ORG_PKG", ObjectStore::ORG_PKG, "FUNC", __FUNCTION__,                                     \
        "BIZ_SCENE", bizScene, "BIZ_STAGE", bizStage, "STAGE_RES", stageRes, ##__VA_ARGS__);       \
})
} // namespace OHOS::ObjectStore
#endif // OBJECT_RADAR_REPORTER_H
