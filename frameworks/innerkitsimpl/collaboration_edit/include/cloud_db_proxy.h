/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#ifndef COLLABORATION_EDIT_CLOUD_DB_PROXY_H
#define COLLABORATION_EDIT_CLOUD_DB_PROXY_H

#include <map>
#include "stdint.h"
#include "grd_type_export.h"
#include "napi_cloud_db.h"

namespace OHOS::CollaborationEdit {
typedef struct ExtendRecordField {
    GRD_CloudFieldTypeE type;
    std::string fieldName;
    std::string fieldValue_str;
    const void *valuePtr;
    uint32_t valueLen;
} ExtendRecordFieldT;

typedef void (*ExtendFieldParser)(CloudParamsAdapterT &extend, ExtendRecordFieldT &destField);

class CloudDbProxy {
public:
    ~CloudDbProxy();
    static int32_t BatchInsert(void *cloudDB, GRD_CloudParamsT *cloudParams);
    static int32_t Query(void *cloudDB, GRD_CloudParamsT *cloudParams);
    static int32_t DownloadAsset(void *cloudDB, const char *equipId, char *path);
    static int32_t UploadAsset(void *cloudDB, char *path);
    static int32_t DeleteAsset(void *cloudDB, char *path);
    static int32_t DeleteLocalAsset(void *cloudDB, char *path);

    static int32_t SendAwarenessData(void *cloudDB, const uint8_t *data, uint32_t dataSize);
    static int32_t Lock(void *cloudDB, uint32_t *lockTimeMs);
    static int32_t UnLock(void *cloudDB);
    static int32_t HeartBeat(void *cloudDB);
    static int32_t Close(void *cloudDB);

    static void CursorParseFunc(CloudParamsAdapterT &extend, ExtendRecordFieldT &destField);
    static void EquipIdParseFunc(CloudParamsAdapterT &extend, ExtendRecordFieldT &destField);
    static void TimestampParseFunc(CloudParamsAdapterT &extend, ExtendRecordFieldT &destField);
    static void SyncLogParseFunc(CloudParamsAdapterT &extend, ExtendRecordFieldT &destField);

    void SetNapiCloudDb(NapiCloudDb *napiCloudDb);
private:
    static int32_t GetCloudRecord(GRD_CloudRecordT &record, CloudParamsAdapterT &paramsAdapter);
    static int32_t GetCloudParamsVector(GRD_CloudParamsT *cloudParams,
        std::vector<CloudParamsAdapterT> &cloudParamsVector);
    static int32_t GetQueryParams(GRD_CloudParamsT *cloudParams, std::vector<QueryConditionT> &queryConditions);
    static int32_t CreateSingleField(GRD_CloudFieldT &field, ExtendRecordFieldT &srcField);
    static int32_t ParseExtendVector(GRD_CloudParamsT *cloudParams, std::vector<CloudParamsAdapterT> &extends);
    static int32_t ParseExtendField(CloudParamsAdapterT &extend, uint32_t index, GRD_CloudRecordT *records,
        uint32_t maxRecordSize);
    static void FreeExtendFields(GRD_CloudFieldT *fields, uint8_t fieldSize);
    static int64_t GetCursorValue(GRD_CloudFieldT &field);

    NapiCloudDb *napiCloudDb_ = nullptr;
};
} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_CLOUD_DB_PROXY_H
