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

#ifndef GRD_API_MANAGER_H
#define GRD_API_MANAGER_H

#include "grd_type_export.h"

namespace OHOS::CollaborationEdit {
// 1. Database open/close library interface encapsulation
typedef int32_t (*DBOpen)(const char *dbPath, const char *configStr, uint32_t flags, GRD_DB **db);
typedef int32_t (*DBClose)(GRD_DB *db, uint32_t flags);
typedef int32_t (*RegisterEquipId)(GRD_DB *db, GrdEquipIdGetFuncT func);
// 2. Node operation interface encapsulation
typedef int32_t (*InsertElements)(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, uint32_t index,
    GRD_DocNodeInfoT *nodeInfo, GRD_ElementIdT **outElementId);
typedef int32_t (*DeleteElements)(GRD_DB *db, const char *tableName, const GRD_ElementIdT *elementId, uint32_t index,
    uint32_t length);
typedef int32_t (*GetElements)(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, uint32_t index, uint32_t length,
    char **respXml);
typedef int32_t (*FragmentToString)(GRD_DB *db, const char *tableName, const GRD_ElementIdT *elementId,
    char **replyJson);
// 3. XmlElement attribute operation interface encapsulation
typedef int32_t (*ElementSetAttr)(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, const char *attributeName,
    const char *attributeValue, uint32_t flags);
typedef int32_t (*ElementRemoveAttr)(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, const char *attributeName);
typedef int32_t (*ElementGetAttrs)(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, char **allAttributes);
// 4. Text operation interface encapsulation
typedef int32_t (*TextInsert)(GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t index, const char *content,
    const char *formatStr);
typedef int32_t (*TextDelete)(GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t index, uint32_t length);
typedef int32_t (*TextFormat)(GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t index, uint32_t length,
    const char *formatStr);
typedef int32_t (*TextGetLength)(GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t *length);
typedef int32_t (*TextReadInStrMode)(GRD_DB *db, GRD_XmlOpPositionT *opPos, char **value);
typedef int32_t (*TextReadInDeltaMode)(GRD_DB *db, GRD_XmlOpPositionT *opPos, const char *snapshot,
    const char *snapshotPrev, char **delta);
// 5. Undo/Redo operation interface encapsulation
typedef int32_t (*DocUndoManager)(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, GRD_UndoParamT *param);
typedef int32_t (*DocUndo)(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, char **modify);
typedef int32_t (*DocRedo)(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, char **modify);
typedef int32_t (*DocStopCapturing)(GRD_DB *db, GRD_XmlOpPositionT *elementAddr);
// Last. Memory free and others
typedef void (*FreeElementId)(GRD_ElementIdT *outElementId);
typedef int32_t (*FreeValue)(char *value);

struct GRD_APIInfo {
    // 1. Database open/close library interface encapsulation
    DBOpen DBOpenApi = nullptr;
    DBClose DBCloseApi = nullptr;
    RegisterEquipId RegisterEquipIdApi = nullptr;
    // 2. Node operation inter
    InsertElements InsertElementsApi = nullptr;
    DeleteElements DeleteElementsApi = nullptr;
    GetElements GetElementsApi = nullptr;
    FragmentToString FragmentToStringApi = nullptr;
    // 3. XmlElement attribute operation interface encapsulation
    ElementSetAttr ElementSetAttrApi = nullptr;
    ElementRemoveAttr ElementRemoveAttrApi = nullptr;
    ElementGetAttrs ElementGetAttrsApi = nullptr;
    // 4. Text operation interface encapsulation
    TextInsert TextInsertApi = nullptr;
    TextDelete TextDeleteApi = nullptr;
    TextFormat TextFormatApi = nullptr;
    TextGetLength TextGetLengthApi = nullptr;
    TextReadInStrMode TextReadInStrModeApi = nullptr;
    TextReadInDeltaMode TextReadInDeltaModeApi = nullptr;
    // 5. Undo/Redo operation interface encapsulation
    DocUndoManager DocUndoManagerApi = nullptr;
    DocUndo DocUndoApi = nullptr;
    DocRedo DocRedoApi = nullptr;
    DocStopCapturing DocStopCapturingApi = nullptr;
    // Last. Memory free and others
    FreeElementId FreeElementIdApi = nullptr;
    FreeValue FreeValueApi = nullptr;
};

GRD_APIInfo GetApiInfoInstance();
}  // namespace NativeRdb

#endif  // GRD_API_MANAGER_H
