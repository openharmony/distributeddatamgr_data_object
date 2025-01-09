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

#define LOG_TAG "RdUtils"
#include "rd_utils.h"

#include "grd_api_manager.h"
#include "grd_error.h"

namespace OHOS::CollaborationEdit {

static GRD_APIInfo GRD_ApiInfo;

// 1. Database open/close library interface encapsulation
int RdUtils::RdDbOpen(const char *dbFile, const char *configStr, uint32_t flags, GRD_DB **db)
{
    if (GRD_ApiInfo.DBOpenApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.DBOpenApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.DBOpenApi(dbFile, configStr, flags, db);
}

int RdUtils::RdDbClose(GRD_DB *db, uint32_t flags)
{
    if (GRD_ApiInfo.DBCloseApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.DBCloseApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.DBCloseApi(db, flags);
}

int RdUtils::RdRegisterEquipId(GRD_DB *db, GrdEquipIdGetFuncT func)
{
    if (GRD_ApiInfo.RegisterEquipIdApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.RegisterEquipIdApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.RegisterEquipIdApi(db, func);
}

// 2. Node operation interface encapsulation
int32_t RdUtils::RdInsertElements(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, uint32_t index,
    GRD_DocNodeInfoT *nodeInfo, GRD_ElementIdT **outElementId)
{
    if (GRD_ApiInfo.InsertElementsApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.InsertElementsApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.InsertElementsApi(db, elementAddr, index, nodeInfo, outElementId);
}

int32_t RdUtils::RdDeleteElements(GRD_DB *db, const char *tableName, const GRD_ElementIdT *elementId, uint32_t index,
    uint32_t length)
{
    if (GRD_ApiInfo.DeleteElementsApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.DeleteElementsApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.DeleteElementsApi(db, tableName, elementId, index, length);
}

int32_t RdUtils::RdGetElements(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, uint32_t index, uint32_t length,
    char **respXml)
{
    if (GRD_ApiInfo.GetElementsApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.GetElementsApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.GetElementsApi(db, elementAddr, index, length, respXml);
}

int32_t RdUtils::RdFragmentToString(GRD_DB *db, const char *tableName, const GRD_ElementIdT *elementId,
    char **replyJson)
{
    if (GRD_ApiInfo.FragmentToStringApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.FragmentToStringApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.FragmentToStringApi(db, tableName, elementId, replyJson);
}

// 3. XmlElement attribute operation interface encapsulation
int32_t RdUtils::RdElementSetAttribute(
    GRD_DB *db, GRD_XmlOpPositionT *elementAddr, const char *attributeName, const char *attributeValue, uint32_t flags)
{
    if (GRD_ApiInfo.ElementSetAttrApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.ElementSetAttrApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.ElementSetAttrApi(db, elementAddr, attributeName, attributeValue, flags);
}

int32_t RdUtils::RdElementRemoveAttribute(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, const char *attributeName)
{
    if (GRD_ApiInfo.ElementRemoveAttrApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.ElementRemoveAttrApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.ElementRemoveAttrApi(db, elementAddr, attributeName);
}

int32_t RdUtils::RdElementGetAttributes(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, char **allAttributes)
{
    if (GRD_ApiInfo.ElementGetAttrsApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.ElementGetAttrsApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.ElementGetAttrsApi(db, elementAddr, allAttributes);
}

// 4. Text operation interface encapsulation
int32_t RdUtils::RdTextInsert(
    GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t index, const char *content, const char *formatStr)
{
    if (GRD_ApiInfo.TextInsertApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.TextInsertApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.TextInsertApi(db, opPos, index, content, formatStr);
}

int32_t RdUtils::RdTextDelete(GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t index, uint32_t length)
{
    if (GRD_ApiInfo.TextDeleteApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.TextDeleteApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.TextDeleteApi(db, opPos, index, length);
}

int32_t RdUtils::RdTextFormat(
    GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t index, uint32_t length, const char *formatStr)
{
    if (GRD_ApiInfo.TextFormatApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.TextFormatApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.TextFormatApi(db, opPos, index, length, formatStr);
}

int32_t RdUtils::RdTextGetLength(GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t *length)
{
    if (GRD_ApiInfo.TextGetLengthApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.TextGetLengthApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.TextGetLengthApi(db, opPos, length);
}

int32_t RdUtils::RdTextReadInStrMode(GRD_DB *db, GRD_XmlOpPositionT *opPos, char **value)
{
    if (GRD_ApiInfo.TextReadInStrModeApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.TextReadInStrModeApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.TextReadInStrModeApi(db, opPos, value);
}

int32_t RdUtils::RdTextReadInDeltaMode(
    GRD_DB *db, GRD_XmlOpPositionT *opPos, const char *snapshot, const char *snapshotPerv, char **delta)
{
    if (GRD_ApiInfo.TextReadInDeltaModeApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.TextReadInDeltaModeApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.TextReadInDeltaModeApi(db, opPos, snapshot, snapshotPerv, delta);
}

// 5. Undo/Redo operation interface encapsulation
int32_t RdUtils::RdDocUndoManager(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, GRD_UndoParamT *param)
{
    if (GRD_ApiInfo.DocUndoManagerApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
        if (GRD_ApiInfo.DocUndoManagerApi == nullptr) {
            return GRD_NOT_SUPPORT;
        }
    }

    return GRD_ApiInfo.DocUndoManagerApi(db, elementAddr, param);
}

int32_t RdUtils::RdDocUndo(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, char **modify)
{
    if (GRD_ApiInfo.DocUndoApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
        if (GRD_ApiInfo.DocUndoApi == nullptr) {
            return GRD_NOT_SUPPORT;
        }
    }

    return GRD_ApiInfo.DocUndoApi(db, elementAddr, modify);
}

int32_t RdUtils::RdDocRedo(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, char **modify)
{
    if (GRD_ApiInfo.DocRedoApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
        if (GRD_ApiInfo.DocRedoApi == nullptr) {
            return GRD_NOT_SUPPORT;
        }
    }

    return GRD_ApiInfo.DocRedoApi(db, elementAddr, modify);
}

int32_t RdUtils::RdDocStopCapturing(GRD_DB *db, GRD_XmlOpPositionT *elementAddr)
{
    if (GRD_ApiInfo.DocStopCapturingApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
        if (GRD_ApiInfo.DocStopCapturingApi == nullptr) {
            return GRD_NOT_SUPPORT;
        }
    }

    return GRD_ApiInfo.DocStopCapturingApi(db, elementAddr);
}

// Last. Memory free and others
void RdUtils::RdFreeElementId(GRD_ElementIdT *outElementId)
{
    if (GRD_ApiInfo.FreeElementIdApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.FreeElementIdApi == nullptr) {
        return;
    }
    GRD_ApiInfo.FreeElementIdApi(outElementId);
}

int32_t RdUtils::RdFreeValue(char *value)
{
    if (GRD_ApiInfo.FreeValueApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.FreeValueApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.FreeValueApi(value);
}
} // namespace OHOS::CollaborationEdit
