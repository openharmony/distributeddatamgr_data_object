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

#ifndef OBJECT_EVENT_H
#define OBJECT_EVENT_H

#include "hisysevent_c.h"

#ifdef __cplusplus
extern "C" {
#endif

#define DOMAIN  "DISDATAMGR"
#define ORG_PKG "distributeddata"
#define OBJECT_EVENT_TYPE_BEHAVIOR 4
#define OBJECT_EVENT_TYPE_SECURITY 3
#define OBJECT_EVENT_TYPE_STATISTIC 2
#define OBJECT_EVENT_TYPE_FAULT 1

typedef enum {
    CREATE = 1,
    SAVE = 2,
    DATA_RESTORE = 3,
} BizScene;

typedef enum {
    INIT_STORE = 1,
    CREATE_TABLE = 2,
    RESTORE = 3,
    TRANSFER = 4,
} CreateStage;

typedef enum {
    SAVE_TO_SERVICE = 1,
    SAVE_TO_STORE = 2,
    PUSH_ASSETS = 3,
    SYNC_DATA = 4,
} SaveStage;

typedef enum {
    DATA_RECV = 1,
    ASSETS_RECV = 2,
    NOTIFY = 3,
} DataRestoreStage;

typedef enum {
    IDLE = 0,
    RADAR_SUCCESS = 1,
    RADAR_FAILED = 2,
    CANCELLED = 3,
} StageRes;

typedef enum {
    START = 1,
    FINISHED = 2,
} BizState;

typedef enum {
    OFFSET = 27525120,
    DUPLICATE_CREATE = OFFSET,
    NO_MEMORY = OFFSET + 1,
    SA_DIED = OFFSET + 2,
    TIMEOUT = OFFSET + 3,
    IPC_ERROR = OFFSET + 4,
    NO_PERMISSION = OFFSET + 5,
    GETKV_FAILED = OFFSET + 6,
    DB_NOT_INIT = OFFSET + 7,
} ErrorCode;

typedef struct {
    int32_t stageRes;   //StageRes
    int32_t errCode;    //ERROR_CODE
    int32_t state;      //BIZ_STATE
    const char *appCaller; //APP_CALLER
} ObjectEventExtra;

typedef struct {
    int32_t errCode;    //ERROR_CODE
} ObjectAlarmExtra;

typedef struct {
    int32_t scene;      //BIZ_SCENE
    int32_t stage;      //BIS_STAGE
    int32_t eventType;  //EVENT_TYPE
    int32_t line;       //CODE_LINE
    const char *domain; //DOMAIN
    const char *eventName;  //EVENT_NAME
    const char *orgPkg;     //ORG_PKG
    const char *func;       //FUNC
    union {
        ObjectEventExtra *objectExtra;
        ObjectAlarmExtra *objectAlarmExtra;
    };
} DataEventForm;

typedef enum {
    EVENT_MODULE_OBJECT,
    EVENT_MODULE_OBJECT_ALARM,
    EVENT_MODULE_MAX,
} DataEventModule;

/*For inner use only*/
void OBJECT_EVENT(int scene, int stage, ObjectEventExtra *extra);
void OBJECT_ALARM(int scene, int type, ObjectAlarmExtra *extra);
size_t getDataValidSize(HiSysEventParam params[], size_t size, DataEventForm *form);
size_t getEventValidSize(HiSysEventParam params[], size_t size, DataEventForm *form);
size_t getAlarmValidSize(HiSysEventParam params[], size_t size, DataEventForm *form);

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif // OBJECT_EVENT_H
