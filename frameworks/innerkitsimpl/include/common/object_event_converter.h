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

#ifndef OBJECT_EVENT_CONVERTER_H
#define OBJECT_EVENT_CONVERTER_H

#include <securec.h>
#include <string.h>

#include "hisysevent_c.h"
#include "object_event.h"

#ifdef __cplusplus
extern "C" {
#endif

#define INVALID_INT_VALUE               0
#define PARAM_STRING_VALUE_MAX_LEN      256
#define PARAM_LONG_STRING_VALUE_MAX_LEN (256 * 1024)

typedef struct {
    char name[MAX_LENGTH_OF_PARAM_NAME];
    HiSysEventParamType type;
    bool (*Assign)(const char[], HiSysEventParamType, DataEventForm *, HiSysEventParam *);
} HiSysEventParamAssigner;

static inline bool InitString(char **str, size_t maxLen)
{
    if (maxLen == 0) {
        return false;
    }
    *str = (char *)malloc(maxLen);
    if (*str == NULL) {
        return false;
    }
    if (memset_s(*str, maxLen, 0, maxLen) != EOK) {
        return false;
    }
    return true;
}

static inline bool CopyString(char *destName, const char *srcName, size_t maxLen)
{
    if (strcpy_s(destName, maxLen, srcName) != EOK) {
        return false;
    }
    return true;
}

/* Used by ASSIGNER macros */
static inline bool AssignerInt32(int32_t value, HiSysEventParam **param)
{
    if (value <= INVALID_INT_VALUE) {
        (*param)->v.i32 = INVALID_INT_VALUE;
        return false;
    }
    (*param)->v.i32 = value;
    return true;
}

/* Used by ASSIGNER macros */
static inline bool AssignerString(const char *value, HiSysEventParam **param)
{
    if (value == NULL || value[0] == '\0' || strnlen(value, PARAM_STRING_VALUE_MAX_LEN) == PARAM_STRING_VALUE_MAX_LEN) {
        (*param)->v.s = NULL;
        return false;
    }
    return InitString(&(*param)->v.s, PARAM_STRING_VALUE_MAX_LEN) &&
        CopyString((*param)->v.s, value, PARAM_STRING_VALUE_MAX_LEN);
}

/* Used by ASSIGNER macros */
static inline bool AssignerLongString(const char *value, HiSysEventParam **param)
{
    if (value == NULL || value[0] == '\0' || strnlen(value,
        PARAM_LONG_STRING_VALUE_MAX_LEN) == PARAM_LONG_STRING_VALUE_MAX_LEN) {
        (*param)->v.s = NULL;
        return false;
    }
    return InitString(&(*param)->v.s, PARAM_LONG_STRING_VALUE_MAX_LEN) &&
        CopyString((*param)->v.s, value, PARAM_LONG_STRING_VALUE_MAX_LEN);
}

/* Used by ASSIGNER macros */
static inline bool AssignerErrcode(int32_t value, HiSysEventParam **param)
{
    (*param)->v.i32 = (value < 0) ? (-value) : value;
    return true;
}

/* Used by ASSIGNER macros */
static inline bool AssignerUint64(uint64_t value, HiSysEventParam **param)
{
    (*param)->v.ui64 = value;
    return true;
}

/* Used by ASSIGNER macros */
static inline bool AssignerUint32(uint32_t value, HiSysEventParam **param)
{
    (*param)->v.ui32 = value;
    return true;
}

#define DATA_ASSIGNER(type, fieldName, field)                                                                   \
    static inline bool DataAssigner##fieldName(                                                                 \
        const char *eventName, HiSysEventParamType paramType, DataEventForm *form, HiSysEventParam *param)      \
    {                                                                                                              \
        if (Assigner##type(form->field, &param) && CopyString(param->name, eventName, MAX_LENGTH_OF_PARAM_NAME)) { \
            param->t = paramType;                                                                                  \
            return true;                                                                                           \
        }                                                                                                          \
        return false;                                                                                              \
    }

DATA_ASSIGNER(Int32, Scene, scene)
DATA_ASSIGNER(Int32, Stage, stage)
DATA_ASSIGNER(String, OrgPkg, orgPkg)
DATA_ASSIGNER(String, Func, func)

#define DATA_ASSIGNER_SIZE 4 // Size of g_dataAssigners
static HiSysEventParamAssigner g_dataAssigners[] = {
    { "BIZ_SCENE", HISYSEVENT_INT32,  DataAssignerScene },
    { "BIZ_STAGE", HISYSEVENT_INT32,  DataAssignerStage },
    { "ORG_PKG",   HISYSEVENT_STRING, DataAssignerOrgPkg},
    { "FUNC",      HISYSEVENT_STRING, DataAssignerFunc  },
    // Modification Note: remember updating DATA_ASSIGNER_SIZE
};

#define OBJECT_ASSIGNER(type, fieldName, field)                                                                \
    static inline bool ObjectAssigner##fieldName(                                                              \
        const char *eventName, HiSysEventParamType paramType, DataEventForm *form, HiSysEventParam *param)     \
    {                                                                                                         \
        if (Assigner##type(form->objectExtra->field, &param) &&                                                \
            CopyString(param->name, eventName, MAX_LENGTH_OF_PARAM_NAME)) {                                   \
            param->t = paramType;                                                                             \
            return true;                                                                                      \
        }                                                                                                     \
        return false;                                                                                         \
    }


OBJECT_ASSIGNER(Int32, StageRes, stageRes)
OBJECT_ASSIGNER(Int32, ErrCode, errCode)
OBJECT_ASSIGNER(Int32, BizState, state)
OBJECT_ASSIGNER(String, AppCaller, appCaller)

#define OBJECT_ASSIGNER_SIZE 4 // Size of g_objectAssigners
static HiSysEventParamAssigner g_objectAssigners[] = {
    { "STAGE_RES",   HISYSEVENT_INT32,  ObjectAssignerStageRes },
    { "ERROR_CODE",  HISYSEVENT_INT32,  ObjectAssignerErrCode },
    { "BIZ_STATE",   HISYSEVENT_INT32,  ObjectAssignerBizState},
    { "APP_CALLER",  HISYSEVENT_STRING, ObjectAssignerAppCaller},
    // Modification Note: remember updating OBJECT_ASSIGNER_SIZE
};

#define OBJECT_ALARM_ASSIGNER(type, fieldName, field)                                                          \
    static inline bool ObjectAlarmAssigner##fieldName(                                                          \
        const char *eventName, HiSysEventParamType paramType, DataEventForm *form, HiSysEventParam *param)      \
    {                                                                                                         \
        if (Assigner##type(form->objectAlarmExtra->field, &param) &&                                           \
            CopyString(param->name, eventName, MAX_LENGTH_OF_PARAM_NAME)) {                                   \
            param->t = paramType;                                                                             \
            return true;                                                                                      \
        }                                                                                                     \
        return false;                                                                                         \
    }

OBJECT_ALARM_ASSIGNER(Int32, ErrCode, errCode)

#define OBJECT_ALARM_ASSIGNER_SIZE 1 // Size of g_objectAssigners
static HiSysEventParamAssigner g_objectAlarmAssigners[] = {
    { "ERROR_CODE",  HISYSEVENT_INT32,  ObjectAlarmAssignerErrCode },
    // Modification Note: remember updating OBJECT_ALARM_ASSIGNER_SIZE
};

static inline size_t ConvertDataForm2Param(HiSysEventParam params[], size_t size, DataEventForm *form)
{
    size_t validSize = 0;
    if (form == NULL) {
        return validSize;
    }
    return getDataValidSize(&params[validSize], size, form);
}

static inline size_t ConvertObjectForm2Param(HiSysEventParam params[], size_t size, DataEventForm *form)
{
    size_t validSize = 0;
    if (form == NULL || form->objectExtra == NULL) {
        return validSize;
    }
    return getEventValidSize(&params[validSize], size, form);
}

static inline size_t ConvertObjectAlarmForm2Param(HiSysEventParam params[], size_t size, DataEventForm *form)
{
    size_t validSize = 0;
    if (form == NULL || form->objectAlarmExtra == NULL) {
        return validSize;
    }
    return getAlarmValidSize(&params[validSize], size, form);
}

#ifdef __cplusplus
}
#endif /*__cplusplus*/
#endif // OBJECT_EVENT_CONVERTER_H
