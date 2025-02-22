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

#define LOG_TAG "NapiCloudDb"

#include <future>

#include "napi_cloud_db.h"

#include "napi_errno.h"
#include "napi_error_utils.h"
#include "napi_parser.h"
#include "napi_utils.h"
#include "napi/native_node_api.h"

namespace OHOS::CollaborationEdit {
constexpr size_t HANDLER_PARAM_NUM = 1;
constexpr size_t PROMISE_PARAM_NUM = 2;
const std::string CURSOR = "cursor";

enum CallBackResult : int32_t {
    CALL_BACK_RESULT_INVALID = -1,
    CALL_BACK_RESULT_OK = 0,
    CALL_BACK_RESULT_ERR,
    CALL_BACK_RESULT_QUERY_END,
};

struct BatchInsertInfo {
    std::vector<CloudParamsAdapterT> inputParam;
    std::promise<int> promiseRes;
    int32_t callBackErrCode;
};

struct AssetInfo {
    AssetOpConfig config;
    std::promise<void> promiseRes;
    int32_t callBackErrCode;
};

struct QueryInfo {
    std::vector<QueryConditionT> inputParam;
    std::promise<std::vector<CloudParamsAdapterT>> promiseRes;
    int32_t callBackErrCode;
};

NapiCloudDb::NapiCloudDb() {}

NapiCloudDb::~NapiCloudDb() {}

int32_t NapiCloudDb::BatchInsert(const std::vector<CloudParamsAdapterT> &cloudParamsAdapter)
{
    napi_status ret = napi_acquire_threadsafe_function(batchInsertInnerFunc_);
    if (ret != napi_ok) {
        LOG_ERROR("[BatchInsert] acquire thread function go wrong, ret=%{public}d", ret);
        return CALL_BACK_RESULT_ERR;
    }
    BatchInsertInfo *batchInsertInfo = new BatchInsertInfo();
    batchInsertInfo->inputParam = cloudParamsAdapter;

    auto future = batchInsertInfo->promiseRes.get_future();
    ret = napi_call_threadsafe_function(batchInsertInnerFunc_, reinterpret_cast<void *>(batchInsertInfo),
        napi_tsfn_blocking);
    if (ret != napi_ok) {
        LOG_ERROR("[BatchInsert] call thread function go wrong, ret=%{public}d", ret);
        delete batchInsertInfo;
        return CALL_BACK_RESULT_ERR;
    }
    auto result = future.get();

    int32_t errCode = batchInsertInfo->callBackErrCode;
    LOG_INFO("[BatchInsert] batch insert complete, result = %{public}d, errCode = %{public}d", result, errCode);
    delete batchInsertInfo;
    return errCode;
}

int32_t NapiCloudDb::Query(const std::vector<QueryConditionT> &queryConditions,
    std::vector<CloudParamsAdapterT> &extends)
{
    napi_status ret = napi_acquire_threadsafe_function(queryInnerFunc_);
    if (ret != napi_ok) {
        LOG_ERROR("acquire thread function go wrong, ret=%{public}d", ret);
        return CALL_BACK_RESULT_ERR;
    }
    QueryInfo *queryInfo = new QueryInfo();
    queryInfo->inputParam = queryConditions;

    auto future = queryInfo->promiseRes.get_future();
    ret = napi_call_threadsafe_function(queryInnerFunc_, reinterpret_cast<void *>(queryInfo), napi_tsfn_blocking);
    if (ret != napi_ok) {
        LOG_ERROR("call thread function go wrong, ret=%{public}d", ret);
        delete queryInfo;
        return CALL_BACK_RESULT_ERR;
    }
    auto result = future.get();
    extends = std::move(result);

    int32_t errCode = queryInfo->callBackErrCode;
    LOG_INFO("query complete, result size = %{public}zu, errCode = %{public}d", extends.size(), errCode);
    delete queryInfo;
    return errCode;
}

int32_t NapiCloudDb::DownloadAsset(const std::string equipId, std::string path)
{
    napi_status ret = napi_acquire_threadsafe_function(downloadAssetInnerFunc_);
    if (ret != napi_ok) {
        LOG_ERROR("[DownloadAsset] acquire thread function go wrong, ret=%{public}d", ret);
        return CALL_BACK_RESULT_ERR;
    }
    AssetInfo *downloadAssetInfo = new AssetInfo();
    downloadAssetInfo->config.inputPath = path;
    ret = napi_call_threadsafe_function(downloadAssetInnerFunc_, (void*)downloadAssetInfo, napi_tsfn_blocking);
    if (ret != napi_ok) {
        LOG_ERROR("[DownloadAsset] call thread function go wrong, ret=%{public}d", ret);
        delete downloadAssetInfo;
        return CALL_BACK_RESULT_ERR;
    }

    LOG_INFO("[DownloadAsset] download asset complete");
    return CALL_BACK_RESULT_OK;
}

int32_t NapiCloudDb::UploadAsset(const std::string path)
{
    napi_status ret = napi_acquire_threadsafe_function(uploadAssetInnerFunc_);
    if (ret != napi_ok) {
        LOG_ERROR("[UploadAsset] acquire thread function go wrong, ret=%{public}d", ret);
        return CALL_BACK_RESULT_ERR;
    }
    AssetInfo *assetInfo = new AssetInfo();
    assetInfo->config.inputPath = path;
    auto future = assetInfo->promiseRes.get_future();
    ret = napi_call_threadsafe_function(uploadAssetInnerFunc_, (void*)assetInfo, napi_tsfn_blocking);
    if (ret != napi_ok) {
        LOG_ERROR("[UploadAsset] call thread function go wrong, ret=%{public}d", ret);
        delete assetInfo;
        return CALL_BACK_RESULT_ERR;
    }
    future.wait();

    int32_t errCode = assetInfo->callBackErrCode;
    LOG_INFO("[UploadAsset] upload asset complete, errCode=%{public}d", errCode);
    delete assetInfo;
    return errCode;
}

int32_t NapiCloudDb::DeleteAsset(const std::string path)
{
    napi_status ret = napi_acquire_threadsafe_function(deleteAssetInnerFunc_);
    if (ret != napi_ok) {
        LOG_ERROR("[DeleteAsset] acquire thread function go wrong, ret=%{public}d", ret);
        return CALL_BACK_RESULT_ERR;
    }
    AssetInfo *assetInfo = new AssetInfo();
    assetInfo->config.inputPath = path;
    auto future = assetInfo->promiseRes.get_future();
    ret = napi_call_threadsafe_function(deleteAssetInnerFunc_, (void*)assetInfo, napi_tsfn_blocking);
    if (ret != napi_ok) {
        LOG_ERROR("[DeleteAsset] call thread function go wrong, ret=%{public}d", ret);
        delete assetInfo;
        return CALL_BACK_RESULT_ERR;
    }
    future.wait();

    int32_t errCode = assetInfo->callBackErrCode;
    LOG_INFO("[DeleteAsset] Delete asset complete, errCode=%{public}d", errCode);
    delete assetInfo;
    return errCode;
}

int32_t NapiCloudDb::DeleteLocalAsset(const std::string path)
{
    napi_status ret = napi_acquire_threadsafe_function(deleteLocalAssetInnerFunc_);
    if (ret != napi_ok) {
        LOG_ERROR("[DeleteLocalAsset] acquire thread function go wrong, ret=%{public}d", ret);
        return CALL_BACK_RESULT_ERR;
    }
    AssetInfo *assetInfo = new AssetInfo();
    assetInfo->config.inputPath = path;
    ret = napi_call_threadsafe_function(deleteLocalAssetInnerFunc_, (void*)assetInfo, napi_tsfn_blocking);
    if (ret != napi_ok) {
        LOG_ERROR("[DeleteLocalAsset] call thread function go wrong, ret=%{public}d", ret);
        delete assetInfo;
        return CALL_BACK_RESULT_ERR;
    }

    LOG_INFO("[DeleteLocalAsset] delete local asset complete");
    return CALL_BACK_RESULT_OK;
}

void NapiCloudDb::GetCloudParamsVector(napi_env env, void *data, napi_value &arrayParams)
{
    int i = 0;
    std::vector<CloudParamsAdapterT> *cloudParamsAdapter = reinterpret_cast<std::vector<CloudParamsAdapterT> *>(data);
    if (cloudParamsAdapter == nullptr) {
        return;
    }
    for (const auto &paramsAdapter : *cloudParamsAdapter) {
        napi_value cursor = nullptr;
        NapiUtils::SetValue(env, paramsAdapter.cursor, cursor);
        napi_value id = nullptr;
        NapiUtils::SetValue(env, paramsAdapter.id, id);
        napi_value record = nullptr;
        NapiUtils::SetValue(env, paramsAdapter.record, record);

        napi_value editObjectRecord = nullptr;
        napi_create_object(env, &editObjectRecord);
        napi_set_named_property(env, editObjectRecord, "cursor", cursor);
        napi_set_named_property(env, editObjectRecord, "id", id);
        napi_set_named_property(env, editObjectRecord, "data", record);

        napi_set_element(env, arrayParams, i, editObjectRecord);
        i++;
    }
}

void NapiCloudDb::GetJsQueryParams(napi_env env, std::vector<QueryConditionT> conditions, napi_value &arrayParams)
{
    uint32_t i = 0;
    for (QueryConditionT condition : conditions) {
        napi_value predicate = nullptr;
        napi_value fieldName = nullptr;
        napi_value fieldValue = nullptr;

        NapiUtils::SetValue(env, static_cast<int32_t>(condition.predicate), predicate);
        NapiUtils::SetValue(env, condition.fieldName, fieldName);
        if (condition.fieldName == CURSOR) {
            NapiUtils::SetValue(env, condition.fieldValue_num, fieldValue);
        } else {
            NapiUtils::SetValue(env, condition.fieldValue_str, fieldValue);
        }

        napi_value queryCondition = nullptr;
        napi_create_object(env, &queryCondition);
        napi_set_named_property(env, queryCondition, "condition", predicate);
        napi_set_named_property(env, queryCondition, "fieldName", fieldName);
        napi_set_named_property(env, queryCondition, "fieldValue", fieldValue);

        napi_set_element(env, arrayParams, i, queryCondition);
        i++;
    }
}

void NapiCloudDb::BatchInsertInner(napi_env env, napi_value jsCb, void *context, void *data)
{
    BatchInsertInfo *batchInsertInfo = reinterpret_cast<BatchInsertInfo *>(data);
    std::vector<CloudParamsAdapterT> inputParam = batchInsertInfo->inputParam;
    napi_value arrayParams = nullptr;
    napi_status status = napi_create_array(env, &arrayParams);
    if (status != napi_ok) {
        batchInsertInfo->promiseRes.set_value(CALL_BACK_RESULT_ERR);
        return;
    }
    GetCloudParamsVector(env, &inputParam, arrayParams);

    napi_value promise = nullptr;
    status = napi_call_function(env, nullptr, jsCb, HANDLER_PARAM_NUM, &arrayParams, &promise);
    if (status != napi_ok) {
        LOG_ERROR("[BatchInsertInner] napi call function go wrong, status = %{public}d", status);
        batchInsertInfo->promiseRes.set_value(CALL_BACK_RESULT_ERR);
        return;
    }
    napi_value thenFunc = nullptr;
    status = napi_get_named_property(env, promise, "then", &thenFunc);
    if (status != napi_ok) {
        LOG_ERROR("[BatchInsertInner] napi get named property fail, status = %{public}d", status);
        batchInsertInfo->promiseRes.set_value(CALL_BACK_RESULT_ERR);
        return;
    }
    napi_value resolvedCallback;
    napi_value rejectedCallback;
    status = napi_create_function(env, "resolvedCallback", NAPI_AUTO_LENGTH,
        NapiCloudDb::BatchInsertResolvedCallback, data, &resolvedCallback);
    if (status != napi_ok) {
        LOG_ERROR("[BatchInsertInner] resolvedCallback, status = %{public}d", status);
        batchInsertInfo->promiseRes.set_value(CALL_BACK_RESULT_ERR);
        return;
    }
    status = napi_create_function(env, "rejectedCallback", NAPI_AUTO_LENGTH,
        NapiCloudDb::BatchInsertRejectedCallback, data, &rejectedCallback);
    if (status != napi_ok) {
        LOG_ERROR("[BatchInsertInner] rejectedCallback, status = %{public}d", status);
        batchInsertInfo->promiseRes.set_value(CALL_BACK_RESULT_ERR);
        return;
    }
    napi_value argv[2] = {resolvedCallback, rejectedCallback};
    batchInsertInfo->callBackErrCode = CALL_BACK_RESULT_INVALID;
    status = napi_call_function(env, promise, thenFunc, PROMISE_PARAM_NUM, argv, nullptr);
    if (batchInsertInfo->callBackErrCode == CALL_BACK_RESULT_INVALID) {
        batchInsertInfo->callBackErrCode = CALL_BACK_RESULT_ERR;
        batchInsertInfo->promiseRes.set_value(CALL_BACK_RESULT_ERR);
        LOG_ERROR("[BatchInsertInner] napi call function go wrong");
    }
    ClearLastException(env);
}

void NapiCloudDb::QueryInner(napi_env env, napi_value jsCb, void *context, void *data)
{
    QueryInfo *queryInfo = reinterpret_cast<QueryInfo *>(data);
    std::vector<QueryConditionT> inputParam = queryInfo->inputParam;
    napi_value arrayParams = nullptr;
    napi_status status = napi_create_array(env, &arrayParams);
    if (status != napi_ok) {
        queryInfo->promiseRes.set_value(std::vector<CloudParamsAdapterT>());
        return;
    }
    GetJsQueryParams(env, inputParam, arrayParams);

    napi_value promise = nullptr;
    status = napi_call_function(env, nullptr, jsCb, HANDLER_PARAM_NUM, &arrayParams, &promise);
    if (status != napi_ok) {
        LOG_ERROR("napi call function go wrong, status = %{public}d", status);
        queryInfo->promiseRes.set_value(std::vector<CloudParamsAdapterT>());
        return;
    }

    napi_value thenFunc = nullptr;
    status = napi_get_named_property(env, promise, "then", &thenFunc);
    if (status != napi_ok) {
        LOG_ERROR("napi get named property fail, status = %{public}d", status);
        queryInfo->promiseRes.set_value(std::vector<CloudParamsAdapterT>());
        return;
    }
    napi_value resolvedCallback;
    napi_value rejectedCallback;
    status = napi_create_function(env, "resolvedCallback", NAPI_AUTO_LENGTH,
        NapiCloudDb::QueryResolvedCallback, data, &resolvedCallback);
    if (status != napi_ok) {
        LOG_ERROR("resolvedCallback, status = %{public}d", status);
        queryInfo->promiseRes.set_value(std::vector<CloudParamsAdapterT>());
        return;
    }
    status = napi_create_function(env, "rejectedCallback", NAPI_AUTO_LENGTH,
        NapiCloudDb::QueryRejectedCallback, data, &rejectedCallback);
    if (status != napi_ok) {
        LOG_ERROR("rejectedCallback, status = %{public}d", status);
        queryInfo->promiseRes.set_value(std::vector<CloudParamsAdapterT>());
        return;
    }
    napi_value argv[2] = {resolvedCallback, rejectedCallback};
    queryInfo->callBackErrCode = CALL_BACK_RESULT_INVALID;
    status = napi_call_function(env, promise, thenFunc, PROMISE_PARAM_NUM, argv, nullptr);
    if (queryInfo->callBackErrCode == CALL_BACK_RESULT_INVALID) {
        queryInfo->promiseRes.set_value(std::vector<CloudParamsAdapterT>());
        queryInfo->callBackErrCode = CALL_BACK_RESULT_ERR;
        LOG_ERROR("napi call function go wrong");
    }
    ClearLastException(env);
}

void NapiCloudDb::DownloadAssetInner(napi_env env, napi_value jsCb, void *context, void *data)
{
    HandleAssetAsyncInner(env, jsCb, context, data);
}

void NapiCloudDb::UploadAssetInner(napi_env env, napi_value jsCb, void *context, void *data)
{
    HandleAssetInner(env, jsCb, context, data);
}

void NapiCloudDb::DeleteAssetInner(napi_env env, napi_value jsCb, void *context, void *data)
{
    HandleAssetInner(env, jsCb, context, data);
}

void NapiCloudDb::DeleteLocalAssetInner(napi_env env, napi_value jsCb, void *context, void *data)
{
    HandleAssetAsyncInner(env, jsCb, context, data);
}

void NapiCloudDb::HandleAssetInner(napi_env env, napi_value jsCb, void *context, void *data)
{
    AssetInfo *assetInfo = reinterpret_cast<AssetInfo *>(data);
    napi_value param = Parser::ParseFromAssetOpConfig(env, assetInfo->config);
    napi_value promise = nullptr;
    napi_status status = napi_call_function(env, nullptr, jsCb, HANDLER_PARAM_NUM, &param, &promise);
    if (status != napi_ok) {
        LOG_ERROR("[HandleAssetInner] napi call function go wrong, status = %{public}d", status);
        assetInfo->promiseRes.set_value();
        return;
    }

    napi_value thenFunc = nullptr;
    status = napi_get_named_property(env, promise, "then", &thenFunc);
    if (status != napi_ok) {
        LOG_ERROR("[HandleAssetInner] napi get named property fail, status = %{public}d", status);
        assetInfo->promiseRes.set_value();
        return;
    }
    napi_value resolvedCallback;
    napi_value rejectedCallback;
    status = napi_create_function(env, "resolvedCallback", NAPI_AUTO_LENGTH,
        NapiCloudDb::HandleAssetResolvedCallback, data, &resolvedCallback);
    if (status != napi_ok) {
        LOG_ERROR("[HandleAssetInner] napi create resolvedCallback go wrong, status = %{public}d", status);
        assetInfo->promiseRes.set_value();
        return;
    }
    status = napi_create_function(env, "rejectedCallback", NAPI_AUTO_LENGTH,
        NapiCloudDb::HandleAssetRejectedCallback, data, &rejectedCallback);
    if (status != napi_ok) {
        LOG_ERROR("[HandleAssetInner] napi create rejectedCallback go wrong, status = %{public}d", status);
        assetInfo->promiseRes.set_value();
        return;
    }
    napi_value argv[PROMISE_PARAM_NUM] = {resolvedCallback, rejectedCallback};
    assetInfo->callBackErrCode = CALL_BACK_RESULT_INVALID;
    status = napi_call_function(env, promise, thenFunc, PROMISE_PARAM_NUM, argv, nullptr);
    if (assetInfo->callBackErrCode == CALL_BACK_RESULT_INVALID) {
        assetInfo->callBackErrCode = CALL_BACK_RESULT_ERR;
        LOG_ERROR("[HandleAssetInner] napi call function go wrong");
    }
    assetInfo->promiseRes.set_value();
    ClearLastException(env);
}

void NapiCloudDb::HandleAssetAsyncInner(napi_env env, napi_value jsCb, void *context, void *data)
{
    AssetInfo *assetInfo = reinterpret_cast<AssetInfo *>(data);
    napi_value param = Parser::ParseFromAssetOpConfig(env, assetInfo->config);
    napi_value promise = nullptr;
    napi_status status = napi_call_function(env, nullptr, jsCb, HANDLER_PARAM_NUM, &param, &promise);
    if (status != napi_ok) {
        LOG_ERROR("[HandleAssetAsyncInner] napi call function go wrong, status = %{public}d", status);
        return;
    }

    napi_value thenFunc = nullptr;
    status = napi_get_named_property(env, promise, "then", &thenFunc);
    if (status != napi_ok) {
        LOG_ERROR("[HandleAssetAsyncInner] napi get named property fail, status = %{public}d", status);
        return;
    }
    napi_value resolvedCallback;
    napi_value rejectedCallback;
    status = napi_create_function(env, "resolvedCallback", NAPI_AUTO_LENGTH,
        NapiCloudDb::HandleAssetAsyncResolvedCallback, data, &resolvedCallback);
    if (status != napi_ok) {
        LOG_ERROR("[HandleAssetAsyncInner] napi create resolvedCallback go wrong, status = %{public}d", status);
        return;
    }
    status = napi_create_function(env, "rejectedCallback", NAPI_AUTO_LENGTH,
        NapiCloudDb::HandleAssetAsyncRejectedCallback, data, &rejectedCallback);
    if (status != napi_ok) {
        LOG_ERROR("[HandleAssetAsyncInner] napi create rejectedCallback go wrong, status = %{public}d", status);
        return;
    }
    napi_value argv[PROMISE_PARAM_NUM] = {resolvedCallback, rejectedCallback};
    status = napi_call_function(env, promise, thenFunc, PROMISE_PARAM_NUM, argv, nullptr);
    if (status != napi_ok) {
        LOG_ERROR("[HandleAssetAsyncInner] napi call function go wrong, status: %{public}d", status);
    }
    ClearLastException(env);
}

napi_value NapiCloudDb::BatchInsertResolvedCallback(napi_env env, napi_callback_info info)
{
    void *data = nullptr;
    size_t argc = 1;
    napi_value argv[1];
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, &data);
    if (status != napi_ok) {
        LOG_ERROR("[ResolvedCallback] napi get callback info go wrong, status = %{public}d", status);
        return nullptr;
    }

    int32_t num = 0;
    NapiUtils::GetValue(env, argv[0], num);
    BatchInsertInfo *batchInsertInfo = reinterpret_cast<BatchInsertInfo *>(data);
    batchInsertInfo->promiseRes.set_value(num);
    batchInsertInfo->callBackErrCode = CALL_BACK_RESULT_OK;
    return nullptr;
}

napi_value NapiCloudDb::BatchInsertRejectedCallback(napi_env env, napi_callback_info info)
{
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, &data);
    if (status != napi_ok) {
        LOG_ERROR("[RejectedCallback] napi get callback info go wrong, status = %{public}d", status);
        return nullptr;
    }
    BatchInsertInfo *batchInsertInfo = reinterpret_cast<BatchInsertInfo *>(data);
    batchInsertInfo->promiseRes.set_value(CALL_BACK_RESULT_ERR);
    batchInsertInfo->callBackErrCode = CALL_BACK_RESULT_ERR;
    return nullptr;
}

napi_status GetQueryResult(napi_env env, napi_value input, std::vector<CloudParamsAdapterT> &out)
{
    LOG_DEBUG("napi_value -> std::vector<CloudParamsAdapterT>");
    out.clear();
    bool isArray = false;
    napi_is_array(env, input, &isArray);
    ASSERT(isArray, "not an array", napi_invalid_arg);

    uint32_t length = 0;
    napi_status statusMsg = napi_get_array_length(env, input, &length);
    ASSERT((statusMsg == napi_ok), "get_array go wrong!", napi_invalid_arg);
    for (uint32_t i = 0; i < length; ++i) {
        napi_value item = nullptr;
        statusMsg = napi_get_element(env, input, i, &item);
        ASSERT((item != nullptr) && (statusMsg == napi_ok), "no element", napi_invalid_arg);
        CloudParamsAdapterT res;
        napi_value cursor = nullptr;
        napi_value id = nullptr;
        napi_value data = nullptr;
        statusMsg = napi_get_named_property(env, item, "cursor", &cursor);
        ASSERT(statusMsg == napi_ok, "get cursor property go wrong", napi_invalid_arg);
        statusMsg = napi_get_named_property(env, item, "id", &id);
        ASSERT(statusMsg == napi_ok, "get id property go wrong", napi_invalid_arg);
        statusMsg = napi_get_named_property(env, item, "data", &data);
        ASSERT(statusMsg == napi_ok, "get data property go wrong", napi_invalid_arg);
        
        statusMsg = NapiUtils::GetValue(env, cursor, res.cursor);
        ASSERT(statusMsg == napi_ok, "get cursor go wrong", napi_invalid_arg);
        statusMsg = NapiUtils::GetValue(env, id, res.id);
        ASSERT(statusMsg == napi_ok, "get id go wrong", napi_invalid_arg);
        statusMsg = NapiUtils::GetValue(env, data, res.record);
        ASSERT(statusMsg == napi_ok, "get record go wrong", napi_invalid_arg);

        out.push_back(res);
    }
    return statusMsg;
}

napi_value NapiCloudDb::QueryResolvedCallback(napi_env env, napi_callback_info info)
{
    void *data = nullptr;
    size_t argc = 1;
    napi_value argv[1];
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, &data);
    if (status != napi_ok) {
        LOG_ERROR("napi get callback info go wrong, status = %{public}d", status);
        return nullptr;
    }

    QueryInfo *queryInfo = reinterpret_cast<QueryInfo *>(data);
    std::vector<CloudParamsAdapterT> queryResult;
    napi_status ret = GetQueryResult(env, argv[0], queryResult);
    if (ret != napi_ok) {
        LOG_ERROR("get callback result go wrong, ret=%{public}d", ret);
        queryResult.clear();
        queryInfo->callBackErrCode = CALL_BACK_RESULT_ERR;
    } else if (ret == napi_ok && queryResult.size() == 0) {
        queryInfo->callBackErrCode = CALL_BACK_RESULT_QUERY_END;
    } else {
        queryInfo->callBackErrCode = CALL_BACK_RESULT_OK;
    }
    queryInfo->promiseRes.set_value(queryResult);
    return nullptr;
}

napi_value NapiCloudDb::QueryRejectedCallback(napi_env env, napi_callback_info info)
{
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, &data);
    if (status != napi_ok) {
        LOG_ERROR("napi get callback info go wrong, status = %{public}d", status);
        return nullptr;
    }
    QueryInfo *queryInfo = reinterpret_cast<QueryInfo *>(data);
    queryInfo->promiseRes.set_value(std::vector<CloudParamsAdapterT>());
    queryInfo->callBackErrCode = CALL_BACK_RESULT_ERR;
    return nullptr;
}

napi_value NapiCloudDb::HandleAssetResolvedCallback(napi_env env, napi_callback_info info)
{
    void *data = nullptr;
    size_t argc = 1;
    napi_value argv[1];
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, &data);
    if (status != napi_ok) {
        LOG_ERROR("[ResolvedCallback] napi get callback info go wrong, status = %{public}d", status);
        return nullptr;
    }

    AssetInfo *assetInfo = reinterpret_cast<AssetInfo *>(data);
    assetInfo->callBackErrCode = CALL_BACK_RESULT_OK;
    return nullptr;
}

napi_value NapiCloudDb::HandleAssetRejectedCallback(napi_env env, napi_callback_info info)
{
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, &data);
    if (status != napi_ok) {
        LOG_ERROR("[RejectedCallback] napi get callback info go wrong, status = %{public}d", status);
        return nullptr;
    }
    AssetInfo *assetInfo = reinterpret_cast<AssetInfo *>(data);
    assetInfo->callBackErrCode = CALL_BACK_RESULT_ERR;
    return nullptr;
}

napi_value NapiCloudDb::HandleAssetAsyncResolvedCallback(napi_env env, napi_callback_info info)
{
    void *data = nullptr;
    size_t argc = 1;
    napi_value argv[1];
    napi_status status = napi_get_cb_info(env, info, &argc, argv, nullptr, &data);
    if (status != napi_ok) {
        LOG_ERROR("[ResolvedCallback] napi get callback info go wrong, status = %{public}d", status);
        return nullptr;
    }

    AssetInfo *assetInfo = reinterpret_cast<AssetInfo *>(data);
    delete assetInfo;
    return nullptr;
}

napi_value NapiCloudDb::HandleAssetAsyncRejectedCallback(napi_env env, napi_callback_info info)
{
    void *data = nullptr;
    napi_status status = napi_get_cb_info(env, info, nullptr, nullptr, nullptr, &data);
    if (status != napi_ok) {
        LOG_ERROR("[RejectedCallback] napi get callback info go wrong, status = %{public}d", status);
        return nullptr;
    }
    AssetInfo *assetInfo = reinterpret_cast<AssetInfo *>(data);
    delete assetInfo;
    return nullptr;
}

void NapiCloudDb::ClearLastException(const napi_env& env)
{
    bool isExistException = false;
    napi_is_exception_pending(env, &isExistException);
    if (isExistException) {
        napi_value exception = nullptr;
        napi_status status = napi_get_and_clear_last_exception(env, &exception);
        LOG_ERROR("[ClearLastException] get and clear last exception, status: %{public}d, exception: %{public}p",
            status, exception);
    }
}
} // namespace OHOS::CollaborationEdit
