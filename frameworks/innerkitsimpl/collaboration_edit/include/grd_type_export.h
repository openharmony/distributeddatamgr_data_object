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

typedef const char *(*GrdEquipIdGetFuncT)(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif  // GRD_TYPE_EXPORT_H
