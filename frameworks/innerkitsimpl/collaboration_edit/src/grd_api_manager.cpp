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

#include "grd_api_manager.h"

#ifndef _WIN32
#include <dlfcn.h>
#endif

#ifndef _WIN32
static void *g_library = nullptr;
#endif

namespace OHOS::CollaborationEdit {
void GRD_DBApiInitEnhance(GRD_APIInfo &GRD_DBApiInfo)
{
#ifndef _WIN32
    // 1. Database open/close library interface encapsulation
    GRD_DBApiInfo.DBOpenApi = (DBOpen)dlsym(g_library, "GRD_DBOpen");
    GRD_DBApiInfo.DBCloseApi = (DBClose)dlsym(g_library, "GRD_DBClose");
    GRD_DBApiInfo.SetLocalIdApi = (SetLocalId)dlsym(g_library, "GRD_SetEquipId");
    GRD_DBApiInfo.GetLocalIdApi = (GetLocalId)dlsym(g_library, "GRD_GetEquipId");
    GRD_DBApiInfo.ApplyUpdateApi = (ApplyUpdate)dlsym(g_library, "GRD_OplogRelayApply");
    GRD_DBApiInfo.WriteUpdateApi = (WriteUpdate)dlsym(g_library, "GRD_WriteLogWithEquipId");
    GRD_DBApiInfo.GetRelativePosApi = (GetRelativePos)dlsym(g_library, "GRD_AbsolutePositionToRelativePosition");
    GRD_DBApiInfo.GetAbsolutePosApi = (GetAbsolutePos)dlsym(g_library, "GRD_RelativePositionToAbsolutePosition");
    // 2. Node operation interface encapsulation
    GRD_DBApiInfo.InsertElementsApi = (InsertElements)dlsym(g_library, "GRD_XmlFragmentInsert");
    GRD_DBApiInfo.DeleteElementsApi = (DeleteElements)dlsym(g_library, "GRD_XmlFragmentDelete");
    GRD_DBApiInfo.GetElementsApi = (GetElements)dlsym(g_library, "GRD_XmlFragmentGet");
    GRD_DBApiInfo.FragmentToStringApi = (FragmentToString)dlsym(g_library, "GRD_XmlFragmentToString");
    // 3. XmlElement attribute operation interface encapsulation
    GRD_DBApiInfo.ElementSetAttrApi = (ElementSetAttr)dlsym(g_library, "GRD_XmlElementSetAttribute");
    GRD_DBApiInfo.ElementRemoveAttrApi = (ElementRemoveAttr)dlsym(g_library, "GRD_XmlElementRemoveAttribute");
    GRD_DBApiInfo.ElementGetAttrsApi = (ElementGetAttrs)dlsym(g_library, "GRD_XmlElementGetAttributes");
    // 4. Text operation interface encapsulation
    GRD_DBApiInfo.TextInsertApi = (TextInsert)dlsym(g_library, "GRD_TextInsert");
    GRD_DBApiInfo.TextDeleteApi = (TextDelete)dlsym(g_library, "GRD_TextDelete");
    GRD_DBApiInfo.TextFormatApi = (TextFormat)dlsym(g_library, "GRD_TextAssignFormats");
    GRD_DBApiInfo.TextGetLengthApi = (TextGetLength)dlsym(g_library, "GRD_TextGetLength");
    GRD_DBApiInfo.TextReadInStrModeApi = (TextReadInStrMode)dlsym(g_library, "GRD_TextReadInStrMode");
    GRD_DBApiInfo.TextReadInDeltaModeApi = (TextReadInDeltaMode)dlsym(g_library, "GRD_TextReadInDeltaMode");
    // 5. Undo/Redo operation interface encapsulation
    GRD_DBApiInfo.DocUndoManagerApi = (DocUndoManager)dlsym(g_library, "GRD_DocUndoManager");
    GRD_DBApiInfo.DocCloseUndoManagerApi = (DocCloseUndoManager)dlsym(g_library, "GRD_DocUndoManagerClose");
    GRD_DBApiInfo.DocUndoApi = (DocUndo)dlsym(g_library, "GRD_DocUndo");
    GRD_DBApiInfo.DocRedoApi = (DocRedo)dlsym(g_library, "GRD_DocRedo");
    GRD_DBApiInfo.DocStopCapturingApi = (DocStopCapturing)dlsym(g_library, "GRD_DocStopCapturing");
    // 6. Sync operation interface encapsulation
    GRD_DBApiInfo.SyncApi = (Sync)dlsym(g_library, "GRD_Sync");
    GRD_DBApiInfo.RegistryThreadPoolApi = (RegistryThreadPool)dlsym(g_library, "GRD_RegistryThreadPool");
    // Last. Memory free and others
    GRD_DBApiInfo.FreeElementIdApi = (FreeElementId)dlsym(g_library, "GRD_XmlFreeElementId");
    GRD_DBApiInfo.FreeValueApi = (FreeValue)dlsym(g_library, "GRD_DocFree");
    GRD_DBApiInfo.SetCloudDbApi = (SetCloudDb)dlsym(g_library, "GRD_RegistryCloudDB");
#endif
}

GRD_APIInfo GetApiInfoInstance()
{
    GRD_APIInfo GRD_TempApiStruct;
#ifndef _WIN32
    g_library = dlopen("libarkdata_db_core.z.so", RTLD_LAZY);
    if (g_library) {
        GRD_DBApiInitEnhance(GRD_TempApiStruct);
    }
#endif
    return GRD_TempApiStruct;
}
}  // namespace OHOS::CollaborationEdit
