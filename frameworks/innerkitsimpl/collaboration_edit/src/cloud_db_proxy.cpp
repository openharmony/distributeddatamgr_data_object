/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License")
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

#define LOG_TAG "CloudDbProxy"

#include "cloud_db_proxy.h"

#include <cerrno>
#include <cstring>
#include <securec.h>

#include "db_error.h"
#include "log_print.h"

namespace OHOS::CollaborationEdit {
// batchInsert log field index
constexpr int32_t LOG_EQUIP_ID_FIELD_IDX = 0;
constexpr int32_t LOG_TIMESTAMP_FIELD_IDX = 1;
constexpr int32_t LOG_SYNC_LOG_FIELD_IDX = 2;
constexpr int32_t LOG_MAX_FIELD_SIZE = 3;

// query result field index
constexpr uint8_t QRY_RSP_EQUIP_ID_IDX = 0u;
constexpr uint8_t QRY_RSP_TIMESTAMP_IDX = 1u;
constexpr uint8_t QRY_RSP_SYNC_LOG_IDX = 2u;
constexpr uint8_t QRY_RSP_CURSOR_IDX = 3u;
constexpr uint8_t QRY_MAX_FIELD_SIZE = 4u;

const std::string EQUIP_ID = "equipId";
const std::string CURSOR = "cursor";
const std::string SYNC_LOG_EVENT = "syncLogEvent";
const std::string TIMESTAMP = "timestamp";

constexpr int CURSOR_BASE = 10;

static const std::map<GRD_QueryConditionTypeE, Predicate> PREDICATE_MAP = {
    {GRD_QUERY_CONDITION_TYPE_EQUAL_TO, EQUAL_TO},
    {GRD_QUERY_CONDITION_TYPE_NOT_EQUAL_TO, NOT_EQUAL_TO},
    {GRD_QUERY_CONDITION_TYPE_GREATER_THAN, GREATER_THAN},
    {GRD_QUERY_CONDITION_TYPE_LESS_THAN, LESS_THAN},
    {GRD_QUERY_CONDITION_TYPE_GREATER_THAN_OR_EQUAL_TO, GREATER_THAN_OR_EQUAL_TO},
    {GRD_QUERY_CONDITION_TYPE_LESS_THAN_OR_EQUAL_TO, LESS_THAN_OR_EQUAL_TO}
};

static const std::map<uint8_t, ExtendFieldParser> QRY_RSP_PARSER_MAP = {
    {QRY_RSP_EQUIP_ID_IDX, CloudDbProxy::EquipIdParseFunc},
    {QRY_RSP_TIMESTAMP_IDX, CloudDbProxy::TimestampParseFunc},
    {QRY_RSP_SYNC_LOG_IDX, CloudDbProxy::SyncLogParseFunc},
    {QRY_RSP_CURSOR_IDX, CloudDbProxy::CursorParseFunc}
};

CloudDbProxy::~CloudDbProxy()
{
    if (napiCloudDb_ != nullptr) {
        delete napiCloudDb_;
        napiCloudDb_ = nullptr;
    }
}

void CloudDbProxy::SetNapiCloudDb(NapiCloudDb *napiCloudDb)
{
    napiCloudDb_ = napiCloudDb;
}

int32_t CloudDbProxy::GetCloudRecord(GRD_CloudRecordT &record, CloudParamsAdapterT &paramsAdapter)
{
    if (record.fieldSize != LOG_MAX_FIELD_SIZE) {
        LOG_ERROR("[GetCloudRecord] field size go wrong, size = %{public}d", record.fieldSize);
        return E_INVALID_ARGS;
    }
    GRD_CloudFieldT equipId = record.fields[LOG_EQUIP_ID_FIELD_IDX];
    paramsAdapter.id.assign(static_cast<char *>(equipId.value));

    GRD_CloudFieldT syncLog = record.fields[LOG_SYNC_LOG_FIELD_IDX];
    paramsAdapter.record.assign(static_cast<uint8_t *>(syncLog.value),
        static_cast<uint8_t *>(syncLog.value) + syncLog.valueLen);

    GRD_CloudFieldT timestamp = record.fields[LOG_TIMESTAMP_FIELD_IDX];
    uint64_t *timestampPtr = static_cast<uint64_t *>(timestamp.value);
    paramsAdapter.timestamp = *timestampPtr;
    return E_OK;
}

void CloudDbProxy::FreeExtendFields(GRD_CloudFieldT *fields, uint8_t fieldSize)
{
    for (uint8_t i = 0; i < fieldSize; i++) {
        if (fields[i].key != nullptr) {
            free(fields[i].key);
            fields[i].key = nullptr;
        }

        if (fields[i].value != nullptr) {
            free(fields[i].value);
            fields[i].value = nullptr;
        }
    }
}

int32_t CloudDbProxy::CreateSingleField(GRD_CloudFieldT &field, ExtendRecordFieldT &srcField)
{
    void *fieldVal = malloc(srcField.valueLen);
    if (fieldVal == nullptr) {
        LOG_ERROR("alloc field value go wrong");
        return E_OUT_OF_MEMORY;
    }
    (void)memset_s(fieldVal, srcField.valueLen, 0, srcField.valueLen);
    errno_t err = memcpy_s(fieldVal, srcField.valueLen, srcField.valuePtr, srcField.valueLen);
    if (err != EOK) {
        LOG_ERROR("copy field value go wrong, err: %{public}d", err);
        free(fieldVal);
        return E_MEMORY_OPERATION_ERROR;
    }

    char *fieldKey = static_cast<char *>(malloc(srcField.fieldName.length() + 1));
    if (fieldKey == nullptr) {
        LOG_ERROR("alloc field key go wrong");
        free(fieldVal);
        return E_OUT_OF_MEMORY;
    }
    err = strcpy_s(fieldKey, srcField.fieldName.length() + 1, srcField.fieldName.c_str());
    if (err != EOK) {
        LOG_ERROR("copy field key go wrong, err: %{public}d", err);
        free(fieldVal);
        free(fieldKey);
        return E_MEMORY_OPERATION_ERROR;
    }
    field.key = fieldKey;
    field.value = fieldVal;
    field.type = srcField.type;
    field.valueLen = srcField.valueLen;
    return E_OK;
}

int32_t CloudDbProxy::ParseExtendField(CloudParamsAdapterT &extend, uint32_t index, GRD_CloudRecordT *records,
    uint32_t maxRecordSize)
{
    if (index >= maxRecordSize) {
        LOG_ERROR("index is wrong, index:%{public}u, max:%{public}u", index, maxRecordSize);
        return E_INVALID_ARGS;
    }
    size_t fieldSize = sizeof(GRD_CloudFieldT) * QRY_MAX_FIELD_SIZE;
    records[index].fields = static_cast<GRD_CloudFieldT *>(malloc(fieldSize));
    if (records[index].fields == nullptr) {
        LOG_ERROR("alloc for extend field go wrong");
        return E_OUT_OF_MEMORY;
    }
    (void)memset_s(records[index].fields, fieldSize, 0, fieldSize);

    int32_t ret = E_OK;
    for (uint8_t i = 0; i < QRY_MAX_FIELD_SIZE; i++) {
        ExtendRecordFieldT field = {};
        auto it = QRY_RSP_PARSER_MAP.find(i);
        if (it == QRY_RSP_PARSER_MAP.end()) {
            LOG_ERROR("field content index is wrong, index=%{public}u", i);
            FreeExtendFields(records[index].fields, records[index].fieldSize);
            free(records[index].fields);
            records[index].fields = nullptr;
            return E_INVALID_ARGS;
        }
        it->second(extend, field);

        ret = CreateSingleField(records[index].fields[i], field);
        if (ret != E_OK) {
            LOG_ERROR("create single field go wrong, index=%{public}u, ret=%{public}d", i, ret);
            FreeExtendFields(records[index].fields, records[index].fieldSize);
            free(records[index].fields);
            records[index].fields = nullptr;
            return ret;
        }
        records[index].fieldSize++;
    }
    return E_OK;
}

int32_t CloudDbProxy::ParseExtendVector(GRD_CloudParamsT *cloudParams, std::vector<CloudParamsAdapterT> &extends)
{
    if (cloudParams == nullptr) {
        LOG_ERROR("cloud params is nullptr");
        return E_INVALID_ARGS;
    }
    if (cloudParams->extends == nullptr || *cloudParams->extends != nullptr || cloudParams->extendSize == nullptr) {
        LOG_ERROR("extend field is nullptr");
        return E_INVALID_ARGS;
    }
    size_t totalSize = sizeof(GRD_CloudRecordT) * extends.size();
    *cloudParams->extends = (GRD_CloudRecordT *)malloc(totalSize);
    if (*cloudParams->extends == nullptr) {
        LOG_ERROR("alloc extends go wrong");
        return E_INVALID_ARGS;
    }
    (void)memset_s(*cloudParams->extends, totalSize, 0, totalSize);

    int32_t ret = E_OK;
    for (CloudParamsAdapterT extend : extends) {
        ret = ParseExtendField(extend, *cloudParams->extendSize, *cloudParams->extends, extends.size());
        if (ret != E_OK) {
            LOG_ERROR("parse extend field go wrong, ret=%{public}d", ret);
            free(*cloudParams->extends);
            *(cloudParams->extendSize) = 0;
            return ret;
        }
        (*cloudParams->extendSize)++;
    }
    return E_OK;
}

int32_t CloudDbProxy::GetCloudParamsVector(GRD_CloudParamsT *cloudParams,
    std::vector<CloudParamsAdapterT> &cloudParamsVector)
{
    if (cloudParams == nullptr) {
        LOG_ERROR("[GetCloudParamsVector] cloud params is nullptr");
        return E_INVALID_ARGS;
    }
    for (uint32_t i = 0; i < cloudParams->recordSize; i++) {
        CloudParamsAdapterT paramsAdapter;
        int32_t ret = GetCloudRecord(cloudParams->records[i], paramsAdapter);
        if (ret != 0) {
            LOG_ERROR("[GetCloudParamsVector] get cloud record go wrong, ret = %{public}d", ret);
            return ret;
        }
        cloudParamsVector.push_back(paramsAdapter);
    }
    return E_OK;
}

int32_t CloudDbProxy::GetQueryParams(GRD_CloudParamsT *cloudParams, std::vector<QueryConditionT> &queryConditions)
{
    if (cloudParams == nullptr) {
        LOG_ERROR("cloud params is nullptr");
        return E_INVALID_ARGS;
    }
    if (cloudParams->records == nullptr || cloudParams->recordSize == 0) {
        LOG_ERROR("record field is wrong");
        return E_INVALID_ARGS;
    }

    GRD_CloudRecordT record = cloudParams->records[0];
    for (uint8_t i = 0; i < record.fieldSize; i++) {
        GRD_CloudFieldT field = record.fields[i];
        QueryConditionT condition;
        condition.fieldName = std::string(field.key);
        auto it = PREDICATE_MAP.find(field.condition);
        if (it == PREDICATE_MAP.end()) {
            LOG_ERROR("predicate is wrong, field name: %{public}s, predicate = %{public}d", field.key, field.condition);
            queryConditions.clear();
            return E_INVALID_ARGS;
        }
        condition.predicate = it->second;
        if (condition.fieldName == CURSOR) {
            if (field.valueLen <= 1) {
                condition.fieldValue_num = 0;
                queryConditions.push_back(condition);
                continue;
            }
            condition.fieldValue_num = GetCursorValue(field);
            if (condition.fieldValue_num < 0) {
                queryConditions.clear();
                return E_INVALID_ARGS;
            }
        } else if (condition.fieldName == EQUIP_ID) {
            condition.fieldValue_str = std::string(static_cast<char *>(field.value));
        } else {
            LOG_ERROR("condition field name is wrong, field name: %{public}s", field.key);
            queryConditions.clear();
            return E_INVALID_ARGS;
        }
        queryConditions.push_back(condition);
    }
    return E_OK;
}

int64_t CloudDbProxy::GetCursorValue(GRD_CloudFieldT &field)
{
    int64_t cursor = -1;
    if (field.value == nullptr) {
        LOG_ERROR("field value is null");
        return cursor;
    }
    std::string tmpString;
    tmpString.resize(field.valueLen);
    errno_t errNo = memcpy_s(&tmpString[0], field.valueLen, field.value, field.valueLen);
    if (errNo != EOK) {
        LOG_ERROR("copy cursor value wrong");
        return cursor;
    }
    errno = 0;
    cursor = std::strtol(tmpString.c_str(), nullptr, CURSOR_BASE);
    if (errno == EINVAL) {
        LOG_ERROR("cursor field is not integer");
        cursor = -1;
    }
    if (errno == ERANGE) {
        LOG_ERROR("cursor is too long");
        cursor = -1;
    }
    return cursor;
}

void CloudDbProxy::CursorParseFunc(CloudParamsAdapterT &extend, ExtendRecordFieldT &destField)
{
    destField.type = GRD_CLOUD_FIELD_TYPE_STRING;
    destField.fieldName = CURSOR;
    destField.valueLen = std::to_string(extend.cursor).length() + 1;
    destField.fieldValue_str = std::to_string(extend.cursor);
    destField.valuePtr = reinterpret_cast<const void *>(destField.fieldValue_str.c_str());
}

void CloudDbProxy::EquipIdParseFunc(CloudParamsAdapterT &extend, ExtendRecordFieldT &destField)
{
    destField.type = GRD_CLOUD_FIELD_TYPE_STRING;
    destField.fieldName = EQUIP_ID;
    destField.valueLen = extend.id.length() + 1;
    destField.valuePtr = reinterpret_cast<const void *>(extend.id.c_str());
}

void CloudDbProxy::TimestampParseFunc(CloudParamsAdapterT &extend, ExtendRecordFieldT &destField)
{
    destField.type = GRD_CLOUD_FIELD_TYPE_INT;
    destField.fieldName = TIMESTAMP;
    destField.valueLen = sizeof(uint64_t);
    destField.valuePtr = &extend.timestamp;
}

void CloudDbProxy::SyncLogParseFunc(CloudParamsAdapterT &extend, ExtendRecordFieldT &destField)
{
    destField.type = GRD_CLOUD_FIELD_TYPE_BYTES;
    destField.fieldName = SYNC_LOG_EVENT;
    destField.valueLen = extend.record.size() * sizeof(uint8_t);
    destField.valuePtr = reinterpret_cast<const void *>(extend.record.data());
}

int32_t CloudDbProxy::BatchInsert(void *cloudDB, GRD_CloudParamsT *cloudParams)
{
    std::vector<CloudParamsAdapterT> cloudParamsVector;
    int32_t ret = GetCloudParamsVector(cloudParams, cloudParamsVector);
    if (ret != E_OK) {
        LOG_ERROR("[BatchInsert] get cloud params go wrong, ret = %{public}d", ret);
        return ret;
    }
    CloudDbProxy *cloudDbProxy = static_cast<CloudDbProxy *>(cloudDB);
    return cloudDbProxy->napiCloudDb_->BatchInsert(cloudParamsVector);
}

int32_t CloudDbProxy::Query(void *cloudDB, GRD_CloudParamsT *cloudParams)
{
    std::vector<QueryConditionT> queryConditions;
    int32_t ret = GetQueryParams(cloudParams, queryConditions);
    if (ret != E_OK) {
        LOG_ERROR("get cloud params go wrong, ret = %{public}d", ret);
        return CloudErrorCodeE::E_CLOUD_ERROR;
    }
    std::vector<CloudParamsAdapterT> extends;
    CloudDbProxy *cloudDbProxy = static_cast<CloudDbProxy *>(cloudDB);
    ret = cloudDbProxy->napiCloudDb_->Query(queryConditions, extends);
    if (ret == CloudErrorCodeE::E_CLOUD_ERROR) {
        LOG_ERROR("cloudDb query go wrong, ret = %{public}d", ret);
        return ret;
    }
    if (ret == CloudErrorCodeE::E_QUERY_END) {
        return ret;
    }
    // set query result to cloudParams
    ret = ParseExtendVector(cloudParams, extends);
    if (ret != E_OK) {
        LOG_ERROR("cloudDb query parse result go wrong, ret = %{public}d", ret);
        return CloudErrorCodeE::E_CLOUD_ERROR;
    }
    return CloudErrorCodeE::E_CLOUD_OK;
}

int32_t CloudDbProxy::DownloadAsset(void *cloudDB, const char *equipId, char *path)
{
    CloudDbProxy *cloudDbProxy = static_cast<CloudDbProxy *>(cloudDB);
    std::string equipIdStr(equipId);
    std::string pathStr(path);
    return cloudDbProxy->napiCloudDb_->DownloadAsset(equipIdStr, pathStr);
}

int32_t CloudDbProxy::UploadAsset(void *cloudDB, char *path)
{
    CloudDbProxy *cloudDbProxy = static_cast<CloudDbProxy *>(cloudDB);
    std::string pathStr(path);
    return cloudDbProxy->napiCloudDb_->UploadAsset(pathStr);
}

int32_t CloudDbProxy::DeleteAsset(void *cloudDB, char *path)
{
    CloudDbProxy *cloudDbProxy = static_cast<CloudDbProxy *>(cloudDB);
    std::string pathStr(path);
    return cloudDbProxy->napiCloudDb_->DeleteAsset(pathStr);
}

int32_t CloudDbProxy::DeleteLocalAsset(void *cloudDB, char *path)
{
    CloudDbProxy *cloudDbProxy = static_cast<CloudDbProxy *>(cloudDB);
    std::string pathStr(path);
    return cloudDbProxy->napiCloudDb_->DeleteLocalAsset(pathStr);
}

int32_t CloudDbProxy::SendAwarenessData(void *cloudDB, const uint8_t *data, uint32_t dataSize)
{
    return E_OK;
}

int32_t CloudDbProxy::Lock(void *cloudDB, uint32_t *lockTimeMs)
{
    return E_OK;
}

int32_t CloudDbProxy::UnLock(void *cloudDB)
{
    return E_OK;
}

int32_t CloudDbProxy::HeartBeat(void *cloudDB)
{
    return E_OK;
}

int32_t CloudDbProxy::Close(void *cloudDB)
{
    return E_OK;
}
} // namespace OHOS::CollaborationEdit
