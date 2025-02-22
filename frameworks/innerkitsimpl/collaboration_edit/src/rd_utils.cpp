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

int RdUtils::RdSetLocalId(GRD_DB *db, const char *equipId)
{
    if (GRD_ApiInfo.SetLocalIdApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.SetLocalIdApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.SetLocalIdApi(db, equipId);
}

int RdUtils::RdGetLocalId(GRD_DB *db, char **localId)
{
    if (GRD_ApiInfo.GetLocalIdApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.GetLocalIdApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.GetLocalIdApi(db, localId);
}

int RdUtils::RdApplyUpdate(GRD_DB *db, char **applyInfo)
{
    if (GRD_ApiInfo.ApplyUpdateApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.ApplyUpdateApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.ApplyUpdateApi(db, nullptr, applyInfo);
}

int RdUtils::RdWriteUpdate(
    GRD_DB *db, const char *equipId, const uint8_t *data, uint32_t size, const std::string &watermark)
{
    if (GRD_ApiInfo.WriteUpdateApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.WriteUpdateApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.WriteUpdateApi(db, equipId, data, size, watermark.c_str());
}

int RdUtils::RdGetRelativePos(
    GRD_DB *db, const char *tableName, const char *nodeSize, uint32_t pos, char **relPos)
{
    if (GRD_ApiInfo.GetRelativePosApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.GetRelativePosApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.GetRelativePosApi(db, tableName, nodeSize, pos, relPos);
}

int RdUtils::RdGetAbsolutePos(
    GRD_DB *db, const char *tableName, const char *relPos, const char *nodeSize, uint32_t *pos)
{
    if (GRD_ApiInfo.GetAbsolutePosApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.GetAbsolutePosApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.GetAbsolutePosApi(db, tableName, relPos, nodeSize, pos);
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

int32_t RdUtils::RdDocCloseUndoManager(GRD_DB *db, GRD_XmlOpPositionT *elementAddr)
{
    if (GRD_ApiInfo.DocCloseUndoManagerApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
        if (GRD_ApiInfo.DocCloseUndoManagerApi == nullptr) {
            return GRD_NOT_SUPPORT;
        }
    }

    return GRD_ApiInfo.DocCloseUndoManagerApi(db, elementAddr);
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

// 6. Sync operation interface encapsulation
int32_t RdUtils::RdSync(GRD_DB *db, GRD_SyncConfig *config)
{
    if (GRD_ApiInfo.SyncApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.SyncApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.SyncApi(db, config);
}

int32_t RdUtils::RdRegistryThreadPool(GRD_DB *db, GRD_ThreadPoolT *threadPool)
{
    if (GRD_ApiInfo.RegistryThreadPoolApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.RegistryThreadPoolApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.RegistryThreadPoolApi(db, threadPool);
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

int32_t RdUtils::RdSetCloudDb(GRD_DB *db, GRD_ICloudDBT *iCloud)
{
    if (GRD_ApiInfo.SetCloudDbApi == nullptr) {
        GRD_ApiInfo = GetApiInfoInstance();
    }
    if (GRD_ApiInfo.SetCloudDbApi == nullptr) {
        return GRD_NOT_SUPPORT;
    }
    return GRD_ApiInfo.SetCloudDbApi(db, iCloud);
}

int32_t RdUtils::TransferToNapiErrNo(int32_t originNo)
{
    auto it = g_errMap.find(originNo);
    if (it == g_errMap.end()) {
        return Status::DB_ERROR;
    }
    return it->second;
}
} // namespace OHOS::CollaborationEdit
