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

#include "hisysevent_c.h"
#include <string>
#include "visibility.h"

namespace OHOS::ObjectStore {
enum BizScene : int8_t {
    CREATE = 1,
    SAVE = 2,
    DATA_RESTORE = 3,
};
enum CreateStage : int8_t {
    INIT_STORE = 1,
    CREATE_TABLE = 2,
    RESTORE = 3,
    TRANSFER = 4,
};
enum SaveStage : int8_t {
    SAVE_TO_SERVICE = 1,
    SAVE_TO_STORE = 2,
    PUSH_ASSETS = 3,
    SYNC_DATA = 4,
};
enum DataRestoreStage : int8_t {
    DATA_RECV = 1,
    ASSETS_RECV = 2,
    NOTIFY = 3,
};
enum StageRes : int8_t {
    IDLE = 0,
    RADAR_SUCCESS = 1,
    RADAR_FAILED = 2,
    CANCELLED = 3,
};
enum BizState : int8_t {
    START = 1,
    FINISHED = 2,
};
enum ErrorCode : int32_t {
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
constexpr char DOMAIN[] = "DISTDATAMGR";
constexpr char EVENT_NAME[] = "DISTRIBUTED_DATA_OBJECT_BEHAVIOR";
constexpr char ORG_PKG[] = "distributeddata";

class API_EXPORT RadarReporter {
public:
    static void ReportStateStart(std::string func, int32_t scene, int32_t stage, int32_t stageRes,
        int32_t state, std::string appCaller);
    static void ReportStateFinished(std::string func, int32_t scene, int32_t stage,
        int32_t stageRes, int32_t state);
    static void ReportStateError(std::string func, int32_t scene, int32_t stage, int32_t stageRes,
        int32_t errCode, int32_t state);
    static void ReportStage(std::string func, int32_t scene, int32_t stage, int32_t stageRes);
    static void ReportStageError(std::string func, int32_t scene, int32_t stage,
        int32_t stageRes, int32_t errCode);
};
} // namespace OHOS::ObjectStore
#endif // OBJECT_RADAR_REPORTER_H
