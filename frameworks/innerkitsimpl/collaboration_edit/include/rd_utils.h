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

#ifndef COLLABORATION_EDIT_RD_UTILS_H
#define COLLABORATION_EDIT_RD_UTILS_H

#include <map>

#include <string>
#include "grd_error.h"
#include "grd_type_export.h"
#include "napi_errno.h"

namespace OHOS::CollaborationEdit {
static std::map<int32_t, int32_t> g_errMap = {
    {GRD_OK, Status::SUCCESS},
    {GRD_ARRAY_INDEX_NOT_FOUND, Status::INDEX_OUT_OF_RANGE}
};
class RdUtils {
public:
    // 1. Database open/close library interface encapsulation
    static int RdDbOpen(const char *dbFile, const char *configStr, uint32_t flags, GRD_DB **db);
    static int RdDbClose(GRD_DB *db, uint32_t flags);
    static int RdSetLocalId(GRD_DB *db, const char *equipId);
    static int RdGetLocalId(GRD_DB *db, char **equipId);
    static int RdApplyUpdate(GRD_DB *db, char **applyInfo);
    static int RdWriteUpdate(
        GRD_DB *db, const char *equipId, const uint8_t *data, uint32_t size, const std::string &watermark);
    static int RdGetRelativePos(GRD_DB *db, const char *tableName, const char *nodeSize, uint32_t pos, char **relPos);
    static int RdGetAbsolutePos(
        GRD_DB *db, const char *tableName, const char *relPos, const char *nodeSize, uint32_t *pos);
    // 2. Node operation inter
    static int RdInsertElements(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, uint32_t index, GRD_DocNodeInfoT *nodeInfo,
        GRD_ElementIdT **outElementId);
    static int RdDeleteElements(GRD_DB *db, const char *tableName, const GRD_ElementIdT *elementId, uint32_t index,
        uint32_t length);
    static int RdGetElements(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, uint32_t index, uint32_t length,
        char **respXml);
    static int RdFragmentToString(GRD_DB *db, const char *fragmentName, const GRD_ElementIdT *elementId,
        char **replyJson);
    // 3. XmlElement attribute operation interface encapsulation
    static int RdElementSetAttribute(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, const char *attributeName,
        const char *attributeValue, uint32_t flags);
    static int RdElementRemoveAttribute(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, const char *attributeName);
    static int RdElementGetAttributes(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, char **allAttributes);
    // 4. Text operation interface encapsulation
    static int RdTextInsert(GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t index, const char *content,
        const char *formatStr);
    static int RdTextDelete(GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t index, uint32_t length);
    static int RdTextFormat(GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t index, uint32_t length,
        const char *formatStr);
    static int RdTextGetLength(GRD_DB *db, GRD_XmlOpPositionT *opPos, uint32_t *length);
    static int RdTextReadInStrMode(GRD_DB *db, GRD_XmlOpPositionT *opPos, char **value);
    static int RdTextReadInDeltaMode(GRD_DB *db, GRD_XmlOpPositionT *opPos, const char *snapshot,
        const char *snapshotPerv, char **delta);
    // 5. Undo/Redo operation interface encapsulation
    static int RdDocUndoManager(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, GRD_UndoParamT *param);
    static int RdDocCloseUndoManager(GRD_DB *db, GRD_XmlOpPositionT *elementAddr);
    static int RdDocUndo(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, char **modify);
    static int RdDocRedo(GRD_DB *db, GRD_XmlOpPositionT *elementAddr, char **modify);
    static int RdDocStopCapturing(GRD_DB *db, GRD_XmlOpPositionT *elementAddr);
    // 6. Sync operation interface encapsulation
    static int RdSync(GRD_DB *db, GRD_SyncConfig *config);
    static int RdRegistryThreadPool(GRD_DB *db, GRD_ThreadPoolT *threadPool);
    // Last. Memory free and others
    static void RdFreeElementId(GRD_ElementIdT *outElementId);
    static int RdFreeValue(char *value);
    static int RdSetCloudDb(GRD_DB *db, GRD_ICloudDBT *iCloud);

    static int32_t TransferToNapiErrNo(int32_t originNo);
};

} // namepace OHOS::CollaborationEdit

#endif // COLLABORATION_EDIT_RD_UTILS_H
