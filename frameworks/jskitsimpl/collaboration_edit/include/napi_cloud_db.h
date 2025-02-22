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

#ifndef COLLABORATION_EDIT_CLOUD_DB_H
#define COLLABORATION_EDIT_CLOUD_DB_H

#include <string>
#include <vector>
#include "napi_error_utils.h"
#include "napi/native_node_api.h"
#include "rd_type.h"

namespace OHOS::CollaborationEdit {

struct CloudParamsAdapterT {
    int64_t cursor = 0;
    uint64_t timestamp = 0;
    std::string id;
    std::vector<uint8_t> record;
};

struct QueryConditionT {
    Predicate predicate;
    std::string fieldName;
    std::string fieldValue_str;
    int64_t fieldValue_num;
};

struct AssetOpConfig {
    std::string inputPath;
};

class NapiCloudDb {
public:
    NapiCloudDb();
    ~NapiCloudDb();
    int32_t BatchInsert(const std::vector<CloudParamsAdapterT> &cloudParamsAdapter);
    int32_t Query(const std::vector<QueryConditionT> &queryConditions, std::vector<CloudParamsAdapterT> &extends);
    int32_t DownloadAsset(const std::string equipId, const std::string path);
    int32_t UploadAsset(const std::string path);
    int32_t DeleteAsset(const std::string path);
    int32_t DeleteLocalAsset(const std::string path);

    static void BatchInsertInner(napi_env env, napi_value js_cb, void *context, void *data);
    static void QueryInner(napi_env env, napi_value js_cb, void *context, void *data);
    static void DownloadAssetInner(napi_env env, napi_value js_cb, void *context, void *data);
    static void UploadAssetInner(napi_env env, napi_value js_cb, void *context, void *data);
    static void DeleteAssetInner(napi_env env, napi_value js_cb, void *context, void *data);
    static void DeleteLocalAssetInner(napi_env env, napi_value js_cb, void *context, void *data);

    static napi_value BatchInsertResolvedCallback(napi_env env, napi_callback_info info);
    static napi_value BatchInsertRejectedCallback(napi_env env, napi_callback_info info);
    static napi_value HandleAssetResolvedCallback(napi_env env, napi_callback_info info);
    static napi_value HandleAssetRejectedCallback(napi_env env, napi_callback_info info);
    static napi_value HandleAssetAsyncResolvedCallback(napi_env env, napi_callback_info info);
    static napi_value HandleAssetAsyncRejectedCallback(napi_env env, napi_callback_info info);
    static napi_value QueryResolvedCallback(napi_env env, napi_callback_info info);
    static napi_value QueryRejectedCallback(napi_env env, napi_callback_info info);

    napi_threadsafe_function batchInsertInnerFunc_ = nullptr;
    napi_threadsafe_function queryInnerFunc_ = nullptr;
    napi_threadsafe_function downloadAssetInnerFunc_ = nullptr;
    napi_threadsafe_function uploadAssetInnerFunc_ = nullptr;
    napi_threadsafe_function deleteAssetInnerFunc_ = nullptr;
    napi_threadsafe_function deleteLocalAssetInnerFunc_ = nullptr;

private:
    static void GetCloudParamsVector(napi_env env, void *data, napi_value &arrayParams);
    static void GetJsQueryParams(napi_env env, std::vector<QueryConditionT> conditions, napi_value &arrayParams);
    static void HandleAssetInner(napi_env env, napi_value jsCb, void *context, void *data);
    static void HandleAssetAsyncInner(napi_env env, napi_value jsCb, void *context, void *data);
    static void ClearLastException(const napi_env& env);
};
} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_CLOUD_DB_H
