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

#ifndef GRD_TYPE_EXPORT_H
#define GRD_TYPE_EXPORT_H

#include "stdint.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#ifdef GRD_DLL_EXPORT
#define GRD_API __declspec(dllexport)
#else
#define GRD_API
#endif
#else
#define GRD_API __attribute__((visibility("default")))
#endif

#define GRD_SYMBOL GRD_API

typedef struct GRD_DB GRD_DB;

/**
 * @brief Open database config.
 */
#define GRD_DB_OPEN_ONLY 0x00    // open database if exists, return error if database not exists
#define GRD_DB_OPEN_CREATE 0x01  // open database if exists, create database and open if database not exists
#define GRD_DB_OPEN_CHECK_FOR_ABNORMAL \
    0x02                        // check data in database if close abnormally last time,
                                // if data is corrupted, rebuild the database
#define GRD_DB_OPEN_CHECK 0x04  // check data in database when open database, if data is corrupted, rebuild the database
#define GRD_DB_OPEN_SHARED_READ_ONLY 0x08  // open database in as read-only mode when sharedModeEnable = 1

/**
 * @brief close database config.
 */
#define GRD_DB_CLOSE 0x00
#define GRD_DB_CLOSE_IGNORE_ERROR 0x01

/**
 * @brief flush database config.
 */
#define GRD_DB_FLUSH_ASYNC 0x00
#define GRD_DB_FLUSH_SYNC 0x01

#define GRD_DOC_ID_DISPLAY 0x01

/**
 * @brief meaning attr is an asset.
 */
#define GRD_DOC_ASSET_FLAG 0x01

typedef struct Query {
    const char *filter;
    const char *projection;
} Query;

typedef struct GRD_KVItem {
    void *data;
    uint32_t dataLen;
} GRD_KVItemT;

typedef struct GRD_ElementId {
    const char *equipId;
    uint64_t incrClock;
} GRD_ElementIdT;

typedef struct GRD_XmlOpPosition {
    const char *tableName;
    GRD_ElementIdT *elementId;
} GRD_XmlOpPositionT;

typedef struct GRD_UndoParam {
    uint64_t captureTimeout;
} GRD_UndoParamT;

typedef enum GRD_EncodingCalMode {
    UTF16 = 0,
    UTF8,
    ENCODE_BUTT,
} GRD_EncodingCalModeE;

typedef enum GRD_DocNodeType {
    GRD_TEXT_TYPE = 0,
    GRD_ARRAY_TYPE,
    GRD_MAP_TYPE,
    GRD_XML_TEXT_TYPE,
    GRD_XML_ELEMENT_TYPE,
    GRD_XML_FRAGMENT_TYPE,
} GRD_DocNodeTypeE;

typedef struct GRD_DocNodeInfo {
    GRD_DocNodeTypeE type;
    const char *content;
} GRD_DocNodeInfoT;

typedef enum GRD_SyncMode {
    GRD_SYNC_MODE_UPLOAD = 0,
    GRD_SYNC_MODE_DOWNLOAD,
    GRD_SYNC_MODE_UP_DOWN,
    GRD_SYNC_MODE_DOWNLOAD_LOG,
    GRD_SYNC_MODE_UP_DOWN_LOG,
    GRD_SYNC_MODE_INVALID,
} GRD_SyncModeE;

typedef enum GRD_SyncProcessStatus {
    GRD_SYNC_PROCESS_PREPARED = 0,
    GRD_SYNC_PROCESS_PROCESSING,
    GRD_SYNC_PROCESS_FINISHED,
} GRD_SyncProcessStatusE;

typedef struct GRD_SyncProcess {
    GRD_SyncProcessStatusE status;
    int32_t errCode;
    GRD_SyncModeE mode;
    void *cloudDB;
    uint64_t syncId;
} GRD_SyncProcessT;

typedef void (*GRD_SyncTaskCallbackFuncT)(GRD_SyncProcessT *syncProcess);

typedef struct GRD_SyncConfig {
    GRD_SyncModeE mode;
    char **equipIds;
    uint8_t size; // equipId size
    GRD_SyncTaskCallbackFuncT callbackFunc;
    uint64_t timeout;
    uint64_t syncId;
} GRD_SyncConfigT;

typedef void (*GRD_ScheduleFunc)(void *func, void *param);

typedef struct GRD_ThreadPool {
    GRD_ScheduleFunc schedule;
} GRD_ThreadPoolT;

typedef const char *(*GrdEquipIdGetFuncT)(void);

typedef enum GRD_QueryConditionType {
    GRD_QUERY_CONDITION_TYPE_NOT_USE = 0,
    GRD_QUERY_CONDITION_TYPE_EQUAL_TO,
    GRD_QUERY_CONDITION_TYPE_NOT_EQUAL_TO,
    GRD_QUERY_CONDITION_TYPE_GREATER_THAN,
    GRD_QUERY_CONDITION_TYPE_LESS_THAN,
    GRD_QUERY_CONDITION_TYPE_GREATER_THAN_OR_EQUAL_TO,
    GRD_QUERY_CONDITION_TYPE_LESS_THAN_OR_EQUAL_TO,
} GRD_QueryConditionTypeE;

typedef enum GRD_CloudFieldType {
    GRD_CLOUD_FIELD_TYPE_INT = 0,
    GRD_CLOUD_FIELD_TYPE_DOUBLE,
    GRD_CLOUD_FIELD_TYPE_STRING,
    GRD_CLOUD_FIELD_TYPE_BOOL,
    GRD_CLOUD_FIELD_TYPE_BYTES,
} GRD_CloudFieldTypeE;

typedef enum CloudErrorCode {
    E_CLOUD_OK = 0,
    E_CLOUD_ERROR,
    E_QUERY_END,
} CloudErrorCodeE;

typedef struct GRD_CloudField {
    GRD_CloudFieldTypeE type;           // field type
    char *key;                          // field name
    uint32_t valueLen;                  // value length
    void *value;                        // value
    GRD_QueryConditionTypeE condition;  // query condition
} GRD_CloudFieldT;

typedef struct GRD_CloudRecord {
    GRD_CloudFieldT *fields;  // fields list
    uint8_t fieldSize;
} GRD_CloudRecordT;

typedef struct GRD_CloudParams {
    const char *tableName;
    GRD_CloudRecordT *records;
    uint32_t recordSize;
    GRD_CloudRecordT **extends;
    uint32_t *extendSize;
} GRD_CloudParamsT;

typedef struct AssetLoader {
    int32_t (*downloadAsset)(void *cloudDB, const char *equipId, char *path);
    int32_t (*uploadAsset)(void *cloudDB, char *path);
    int32_t (*deleteAsset)(void *cloudDB, char *path);
    int32_t (*deleteLocalAsset)(void *cloudDB, char *path);
} AssetLoaderT;

typedef struct GRD_ICloudDB {
    void *cloudDB;
    AssetLoaderT *assetLoader;
    int32_t (*batchInsert)(void *cloudDB, GRD_CloudParamsT *cloudParams);
    int32_t (*query)(void *cloudDB, GRD_CloudParamsT *cloudParams);
    int32_t (*sendAwarenessData)(void *cloudDB, const uint8_t *data, uint32_t dataSize);
    int32_t (*lock)(void *cloudDB, uint32_t *lockTimeMs);
    int32_t (*unLock)(void *cloudDB);
    int32_t (*heartBeat)(void *cloudDB);
    int32_t (*close)(void *cloudDB);
} GRD_ICloudDBT;
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // GRD_TYPE_EXPORT_H
