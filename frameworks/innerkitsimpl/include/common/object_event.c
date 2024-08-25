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

#include "hisysevent_c.h"

#include "object_event.h"
#include "object_event_converter.h"

#define HISYSEVENT_WRITE_SUCCESS 0
#define EVENT_NAME "DISTRIBUTED_DATA_OBJECT_BEHAVIOR"

typedef void (*WriteEventAndFree)(HiSysEventParam *params, size_t size, DataEventForm *form);

static void ConstructHiSysEventParams(HiSysEventParam *eventParams, const HiSysEventParam *params, size_t size,
    const HiSysEventParam *extraParams, size_t extraSize)
{
    size_t index = 0;
    for (size_t i = 0; i < size; ++i) {
        eventParams[index++] = params[i];
    }
    for (size_t j = 0; j < extraSize; ++j) {
        eventParams[index++] = extraParams[j];
    }
}

size_t getDataValidSize(HiSysEventParam params[], size_t size, DataEventForm *form)
{
    size_t validSize = 0;
    for (size_t i = 0; i < size; ++i) {
        HiSysEventParamAssigner assigner = g_dataAssigners[i];
        if (assigner.Assign(assigner.name, assigner.type, form, &params[validSize])) {
            ++validSize;
        }
    }
    return validSize;
}

size_t getEventValidSize(HiSysEventParam params[], size_t size, DataEventForm *form)
{
    size_t validSize = 0;
    for (size_t i = 0; i < size; ++i) {
        HiSysEventParamAssigner assigner = g_objectAssigners[i];
        if (assigner.Assign(assigner.name, assigner.type, form, &params[validSize])) {
            ++validSize;
        }
    }
    return validSize;
}

size_t getAlarmValidSize(HiSysEventParam params[], size_t size, DataEventForm *form)
{
    size_t validSize = 0;
    for (size_t i = 0; i < size; ++i) {
        HiSysEventParamAssigner assigner = g_objectAlarmAssigners[i];
        if (assigner.Assign(assigner.name, assigner.type, form, &params[validSize])) {
            ++validSize;
        }
    }
    return validSize;
}

static void WriteHiSysEvent(
    HiSysEventParam params[], size_t size, HiSysEventParam extraParams[], size_t extraSize, DataEventForm *form)
{
    size_t validParamSize = size + extraSize;
    HiSysEventParam eventParams[validParamSize];
    ConstructHiSysEventParams(eventParams, params, size, extraParams, extraSize);
    int32_t ret = HiSysEvent_Write(
        form->func, form->line, form->domain, form->eventName, form->eventType, eventParams, validParamSize);
    if (ret != HISYSEVENT_WRITE_SUCCESS) {
        return;
    }
}

static void HiSysEventParamsFree(HiSysEventParam params[], size_t size)
{
    for (size_t i = 0; i < size; ++i) {
        if (params[i].t == HISYSEVENT_STRING) {
            free(params[i].v.s);
        }
    }
}

static void WriteObjectEventAndFree(HiSysEventParam *params, size_t size, DataEventForm *form)
{
    HiSysEventParam objectParams[OBJECT_ASSIGNER_SIZE] = { 0 };
    size_t objectSize = ConvertObjectForm2Param(objectParams, OBJECT_ASSIGNER_SIZE, form);
    WriteHiSysEvent(params, size, objectParams, objectSize, form);
    HiSysEventParamsFree(objectParams, objectSize);
}

static void WriteObjectAlarmEventAndFree(HiSysEventParam *params, size_t size, DataEventForm *form)
{
    HiSysEventParam alarmParams[OBJECT_ALARM_ASSIGNER_SIZE] = { 0 };
    size_t alarmSize = ConvertObjectAlarmForm2Param(alarmParams, OBJECT_ALARM_ASSIGNER_SIZE, form);
    WriteHiSysEvent(params, size, alarmParams, alarmSize, form);
    HiSysEventParamsFree(alarmParams, alarmSize);
}

static WriteEventAndFree g_eventFunc[] = {
    [EVENT_MODULE_OBJECT] = WriteObjectEventAndFree,
    [EVENT_MODULE_OBJECT_ALARM] = WriteObjectAlarmEventAndFree,
};

static void WriteObjectEvent(DataEventModule module, DataEventForm *form)
{
    HiSysEventParam params[DATA_ASSIGNER_SIZE] = { 0 };
    size_t size = ConvertDataForm2Param(params, DATA_ASSIGNER_SIZE, form);
    if (module >= 0 && module < EVENT_MODULE_MAX) {
        g_eventFunc[module](params, size, form);
    }
    HiSysEventParamsFree(params, size);
}

static void ObjectEventToHisys(DataEventModule module, DataEventForm *form)
{
    if (form == NULL) {
        return;
    }
    form->domain = DOMAIN;
    form->eventType = HISYSEVENT_BEHAVIOR;
    form->orgPkg = ORG_PKG;
    WriteObjectEvent(module, form);
}

static void ObjectEventInner(int32_t scene, int32_t stage, const char *func, int32_t line, ObjectEventExtra *extra)
{
    if (func == NULL || extra == NULL) {
        return;
    }
    DataEventForm form = {
        .eventName = EVENT_NAME,
        .scene = scene,
        .stage = stage,
        .func = func,
        .line = line,
        .objectExtra = extra,
    };
    ObjectEventToHisys(EVENT_MODULE_OBJECT, &form);
}

static void ObjectAlarmInner(int32_t scene, int32_t type, const char *func, int32_t line, ObjectAlarmExtra *extra)
{
    DataEventForm form = {
        .eventName = EVENT_NAME,
        .scene = scene,
        .func = func,
        .line = line,
        .objectAlarmExtra = extra,
    };
    ObjectEventToHisys(EVENT_MODULE_OBJECT_ALARM, &form);
}

void OBJECT_EVENT(int scene, int stage, ObjectEventExtra *extra)
{
    ObjectEventInner(scene, stage, __FUNCTION__, __LINE__, extra);
}

void OBJECT_ALARM(int scene, int32_t type, ObjectAlarmExtra *extra)
{
    ObjectAlarmInner(scene, type, __FUNCTION__, __LINE__, extra);
}
