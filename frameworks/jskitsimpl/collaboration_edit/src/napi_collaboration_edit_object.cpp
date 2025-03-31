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
#define LOG_TAG "CollaborationEditObject"

#include <future>

#include "napi_collaboration_edit_object.h"

#include "db_store_config.h"
#include "db_store_manager.h"
#include "napi_edit_unit.h"
#include "napi_error_utils.h"
#include "napi_parser.h"
#include "napi_sync_service.h"
#include "napi/native_node_api.h"

namespace OHOS::CollaborationEdit {
static constexpr const uint8_t BATCH_INSERT_FUNC_INDEX = 0;
static constexpr const uint8_t QUERY_FUNC_INDEX = 1;
static constexpr const uint8_t DOWNLOAD_ASSET_FUNC_INDEX = 2;
static constexpr const uint8_t UPLOAD_ASSET_FUNC_INDEX = 3;
static constexpr const uint8_t DELETE_ASSET_FUNC_INDEX = 4;
static constexpr const uint8_t DELETE_LOCAL_ASSET_FUNC_INDEX = 5;

static constexpr const uint8_t CLOUD_SYNC_PARAM_NUMBER = 2;
static const std::map<SyncMode, GRD_SyncModeE> SYNC_MODE_MAP = {
    {SyncMode::PUSH, GRD_SYNC_MODE_UPLOAD},
    {SyncMode::PULL, GRD_SYNC_MODE_DOWNLOAD_LOG},
    {SyncMode::PULL_PUSH, GRD_SYNC_MODE_UP_DOWN_LOG}
};

CollaborationEditObject::CollaborationEditObject(std::string docName, ContextParam param)
    : docName_(docName), param_(std::make_shared<ContextParam>(std::move(param)))
{}

CollaborationEditObject::~CollaborationEditObject()
{}

napi_value CollaborationEditObject::Initialize(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    ContextParam context;
    napi_status status = NapiUtils::GetValue(env, argv[0], context);
    ASSERT_THROW_BASE(env, status == napi_ok, Status::INVALID_ARGUMENT, "read context param go wrong", self);
    std::string docName;
    status = NapiUtils::GetNamedProperty(env, argv[1], "name", docName);
    ASSERT_THROW_BASE(env, status == napi_ok, Status::INVALID_ARGUMENT, "read docName param go wrong", self);
    ASSERT_THROW_BASE(env, !docName.empty(), Status::INVALID_ARGUMENT, "Param Error: invalid name", self);
    std::string dbFilePath = context.baseDir + "/" + docName;
    DBStoreConfig config(dbFilePath, docName);
    std::shared_ptr<DBStore> dbStore = DBStoreManager::GetInstance().GetDBStore(config);
    if (dbStore == nullptr) {
        ThrowNapiError(env, Status::INTERNAL_ERROR, "open doc go wrong");
        return self;
    }

    CollaborationEditObject *editObject = new (std::nothrow) CollaborationEditObject(docName, context);
    ASSERT_THROW_BASE(env, editObject != nullptr, Status::INTERNAL_ERROR, "Initialize: new editObject go wrong", self);
    editObject->SetDBStore(dbStore);
    auto finalize = [](napi_env env, void *data, void *hint) {
        CollaborationEditObject *editObject = reinterpret_cast<CollaborationEditObject *>(data);
        delete editObject;
    };
    status = napi_wrap(env, self, editObject, finalize, nullptr, nullptr);
    if (status != napi_ok) {
        LOG_ERROR("napi_wrap failed. code:%{public}d", status);
        delete editObject;
        return nullptr;
    }
    return self;
}

napi_value CollaborationEditObject::Constructor(napi_env env)
{
    auto lambda = []() -> std::vector<napi_property_descriptor> {
        std::vector<napi_property_descriptor> properties = {
            DECLARE_NAPI_FUNCTION("getEditUnit", GetEditUnit),
            DECLARE_NAPI_FUNCTION("getUndoRedoManager", GetUndoRedoManager),
            DECLARE_NAPI_FUNCTION("deleteUndoRedoManager", DeleteUndoRedoManager),
            DECLARE_NAPI_FUNCTION("getName", GetName),
            DECLARE_NAPI_FUNCTION("cloudSync", CloudSync),
            DECLARE_NAPI_FUNCTION("setCloudDB", SetCloudDb),
            DECLARE_NAPI_FUNCTION("getLocalId", GetLocalId),
            DECLARE_NAPI_FUNCTION("applyUpdate", ApplyUpdate),
            DECLARE_NAPI_FUNCTION("writeUpdate", WriteUpdate),
        };
        return properties;
    };
    return NapiUtils::DefineClass(
        env, "ohos.data.collaborationEditObject", "CollaborationEditObject", lambda, Initialize);
}

napi_value CollaborationEditObject::NewInstance(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value editObject = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    napi_status status = napi_new_instance(env, Constructor(env), argc, argv, &editObject);
    if (editObject == nullptr || status != napi_ok) {
        LOG_ERROR("[NewInstance] new instance go wrong");
    }
    return editObject;
}

napi_value CollaborationEditObject::GetEditUnit(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    CollaborationEditObject *editObject = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&editObject)));
    napi_value napiEditUnit = EditUnit::NewInstance(env, info, self);
    EditUnit *editUnit = nullptr;
    napi_status status = napi_unwrap(env, napiEditUnit, reinterpret_cast<void **>(&editUnit));
    if (status != napi_ok || editUnit == nullptr) {
        LOG_ERROR("unwrap EditUnit go wrong, status = %{public}d", status);
        return nullptr;
    }
    editUnit->SetDBStore(editObject->GetDBStore());
    return napiEditUnit;
}

napi_value CollaborationEditObject::GetUndoRedoManager(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    CollaborationEditObject *editObject = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&editObject)));
    napi_value jsUndoManager = UndoManager::NewInstance(env, info);
    UndoManager *undoManager = nullptr;
    napi_status status = napi_unwrap(env, jsUndoManager, reinterpret_cast<void **>(&undoManager));
    if (status != napi_ok || undoManager == nullptr) {
        LOG_ERROR("unwrap UndoManager go wrong, status = %{public}d", status);
        return nullptr;
    }
    undoManager->SetDBStore(editObject->GetDBStore());
    int32_t retCode = undoManager->GetAdapter()->CreateUndoManager(undoManager->GetCaptureTimeout());
    ASSERT_THROW(env, retCode == SUCCESS, retCode, "create undo manager go wrong.");
    return jsUndoManager;
}

napi_value CollaborationEditObject::DeleteUndoRedoManager(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    CollaborationEditObject *editObject = nullptr;
    NAPI_CALL(env, napi_unwrap(env, self, reinterpret_cast<void **>(&editObject)));
    std::string editUnitName;
    napi_status status = NapiUtils::GetValue(env, argv[0], editUnitName);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "read editUnitName go wrong");
    std::shared_ptr<RdAdapter> adapter = std::make_shared<RdAdapter>();
    std::string tableName = std::to_string(LABEL_FRAGMENT) + "_" + editUnitName;
    adapter->SetTableName(tableName);
    adapter->SetDBStore(editObject->GetDBStore());
    int32_t retCode = adapter->CloseUndoManager();
    ASSERT_THROW(env, retCode == SUCCESS, retCode, "close undo manager go wrong.");
    LOG_INFO("Close undo manager successfully.");
    return nullptr;
}

napi_value CollaborationEditObject::GetName(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    CollaborationEditObject *editObject = nullptr;
    napi_status status = napi_unwrap(env, self, reinterpret_cast<void **>(&editObject));
    if (status != napi_ok) {
        ThrowNapiError(env, status, "unwrap object go wrong");
        return nullptr;
    }
    napi_value result;
    NapiUtils::SetValue(env, editObject->docName_, result);
    return result;
}

napi_value CollaborationEditObject::SetCloudDb(napi_env env, napi_callback_info info)
{
    size_t argc = 1;
    napi_value argv[1] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));

    std::vector<napi_value> cloudDbFunc;
    int ret = Parser::ParseCloudDbFields(env, argv[0], cloudDbFunc);
    if (ret != OK) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "parse cloudDb fields go wrong.");
        return nullptr;
    }
    NapiCloudDb *napiCloudDb = new (std::nothrow) NapiCloudDb();
    ASSERT_THROW(env, napiCloudDb != nullptr, Status::INTERNAL_ERROR, "new cloud db instance go wrong.");
    napi_status status = CollaborationEditObject::CreateHandlerFunc(env, cloudDbFunc, napiCloudDb);
    if (status != napi_ok) {
        ReleaseHandlerFunc(napiCloudDb);
        delete napiCloudDb;
        ThrowNapiError(env, Status::INTERNAL_ERROR, "create handler func go wrong.");
        return nullptr;
    }

    CollaborationEditObject *editObject = nullptr;
    status = napi_unwrap(env, self, reinterpret_cast<void **>(&editObject));
    if (status != napi_ok) {
        ReleaseHandlerFunc(napiCloudDb);
        delete napiCloudDb;
        ThrowNapiError(env, Status::INTERNAL_ERROR, "unwrap editObject go wrong.");
        return nullptr;
    }
    ret = DBStoreManager::GetInstance().SetCloudDb(editObject->GetDBStore(), napiCloudDb);
    if (ret != 0) {
        ReleaseHandlerFunc(napiCloudDb);
        delete napiCloudDb;
        ThrowNapiError(env, ret, "set cloud db go wrong.");
    }
    return nullptr;
}

napi_status CollaborationEditObject::CreateHandlerFunc(napi_env env, std::vector<napi_value> &cloudDbFunc,
    NapiCloudDb *napiCloudDb)
{
    napi_value name = nullptr;
    napi_status status = napi_create_string_utf8(env, "batchInsert", NAPI_AUTO_LENGTH, &name);
    ASSERT(status == napi_ok, "create batchInsert string wrong", status);
    status = napi_create_threadsafe_function(env, cloudDbFunc[BATCH_INSERT_FUNC_INDEX], nullptr, name, 0, 1,
        nullptr, nullptr, nullptr, NapiCloudDb::BatchInsertInner, &napiCloudDb->batchInsertInnerFunc_);
    ASSERT(status == napi_ok, "create batchInsert func wrong", status);

    name = nullptr;
    status = napi_create_string_utf8(env, "query", NAPI_AUTO_LENGTH, &name);
    ASSERT(status == napi_ok, "create query string wrong", status);
    status = napi_create_threadsafe_function(env, cloudDbFunc[QUERY_FUNC_INDEX], nullptr, name, 0, 1,
        nullptr, nullptr, nullptr, NapiCloudDb::QueryInner, &napiCloudDb->queryInnerFunc_);
    ASSERT(status == napi_ok, "create query func wrong", status);

    name = nullptr;
    status = napi_create_string_utf8(env, "downloadAsset", NAPI_AUTO_LENGTH, &name);
    ASSERT(status == napi_ok, "create downloadAsset string wrong", status);
    status = napi_create_threadsafe_function(env, cloudDbFunc[DOWNLOAD_ASSET_FUNC_INDEX], nullptr, name, 0, 1,
        nullptr, nullptr, nullptr, NapiCloudDb::DownloadAssetInner, &napiCloudDb->downloadAssetInnerFunc_);
    ASSERT(status == napi_ok, "create downloadAsset func wrong", status);

    name = nullptr;
    status = napi_create_string_utf8(env, "uploadAsset", NAPI_AUTO_LENGTH, &name);
    ASSERT(status == napi_ok, "create uploadAsset string wrong", status);
    status = napi_create_threadsafe_function(env, cloudDbFunc[UPLOAD_ASSET_FUNC_INDEX], nullptr, name, 0, 1,
        nullptr, nullptr, nullptr, NapiCloudDb::UploadAssetInner, &napiCloudDb->uploadAssetInnerFunc_);
    ASSERT(status == napi_ok, "create uploadAsset func wrong", status);

    name = nullptr;
    status = napi_create_string_utf8(env, "deleteAsset", NAPI_AUTO_LENGTH, &name);
    ASSERT(status == napi_ok, "create deleteAsset string wrong", status);
    status = napi_create_threadsafe_function(env, cloudDbFunc[DELETE_ASSET_FUNC_INDEX], nullptr, name, 0, 1,
        nullptr, nullptr, nullptr, NapiCloudDb::DeleteAssetInner, &napiCloudDb->deleteAssetInnerFunc_);
    ASSERT(status == napi_ok, "create deleteAsset func wrong", status);

    name = nullptr;
    status = napi_create_string_utf8(env, "deleteLocalAsset", NAPI_AUTO_LENGTH, &name);
    ASSERT(status == napi_ok, "create deleteLocalAsset string wrong", status);
    status = napi_create_threadsafe_function(env, cloudDbFunc[DELETE_LOCAL_ASSET_FUNC_INDEX], nullptr, name, 0, 1,
        nullptr, nullptr, nullptr, NapiCloudDb::DeleteLocalAssetInner, &napiCloudDb->deleteLocalAssetInnerFunc_);
    ASSERT(status == napi_ok, "create deleteLocalAsset func wrong", status);
    return napi_ok;
}

void CollaborationEditObject::ReleaseHandlerFunc(NapiCloudDb *napiCloudDb)
{
    if (napiCloudDb == nullptr) {
        return;
    }
    napi_release_threadsafe_function(napiCloudDb->batchInsertInnerFunc_, napi_tsfn_release);
    napi_release_threadsafe_function(napiCloudDb->queryInnerFunc_, napi_tsfn_release);
    napi_release_threadsafe_function(napiCloudDb->downloadAssetInnerFunc_, napi_tsfn_release);
    napi_release_threadsafe_function(napiCloudDb->uploadAssetInnerFunc_, napi_tsfn_release);
    napi_release_threadsafe_function(napiCloudDb->deleteAssetInnerFunc_, napi_tsfn_release);
    napi_release_threadsafe_function(napiCloudDb->deleteLocalAssetInnerFunc_, napi_tsfn_release);
    
    napiCloudDb->batchInsertInnerFunc_ = nullptr;
    napiCloudDb->queryInnerFunc_ = nullptr;
    napiCloudDb->downloadAssetInnerFunc_ = nullptr;
    napiCloudDb->downloadAssetInnerFunc_ = nullptr;
    napiCloudDb->uploadAssetInnerFunc_ = nullptr;
    napiCloudDb->deleteAssetInnerFunc_ = nullptr;
    napiCloudDb->deleteLocalAssetInnerFunc_ = nullptr;
}

napi_value CollaborationEditObject::Delete(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    ContextParam context;
    napi_status status = NapiUtils::GetValue(env, argv[0], context);
    ASSERT_THROW_BASE(env, status == napi_ok, Status::INVALID_ARGUMENT, "read context param go wrong", self);
    std::string docName;
    status = NapiUtils::GetNamedProperty(env, argv[1], "name", docName);
    ASSERT_THROW_BASE(env, status == napi_ok, Status::INVALID_ARGUMENT, "read docName param go wrong", self);
    ASSERT_THROW_BASE(env, !docName.empty(), Status::INVALID_ARGUMENT, "Param Error: invalid name", self);
    std::string dbFilePath = context.baseDir + "/" + docName;
    DBStoreConfig config(dbFilePath, docName);
    int ret = DBStoreManager::GetInstance().DeleteDBStore(config);
    if (ret != 0) {
        ThrowNapiError(env, Status::INTERNAL_ERROR, "remove dir go wrong");
    }
    return nullptr;
}

int CollaborationEditObject::ParseCloudSyncMode(const napi_env env, const napi_value arg,
    std::shared_ptr<SyncContext> context)
{
    int32_t mode = 0;
    napi_status status = NapiUtils::GetValue(env, arg, mode);
    if (status != napi_ok || mode < 0 || mode > SyncMode::PULL_PUSH) {
        LOG_ERROR("CloudSync parse syncMode go wrong, mode: %d", mode);
        return ERR;
    }
    context->syncMode_ = mode;
    return OK;
}

int CollaborationEditObject::ParseThis(const napi_env &env, const napi_value &self,
    std::shared_ptr<SyncContext> context)
{
    CollaborationEditObject *editObject = nullptr;
    napi_status status = napi_unwrap(env, self, reinterpret_cast<void **>(&editObject));
    if (status != napi_ok || editObject == nullptr) {
        LOG_ERROR("CloudSync unwrap object go wrong");
        return ERR;
    }
    context->dbStore_ = editObject->GetDBStore();
    context->boundObj = editObject;
    return OK;
}

InputAction CollaborationEditObject::GetCloudSyncInput(std::shared_ptr<SyncContext> context)
{
    return [context](napi_env env, size_t argc, napi_value *argv, napi_value self) -> int {
        LOG_DEBUG("get CloudSync input start.");
        ASSERT(argc == CLOUD_SYNC_PARAM_NUMBER, "CloudSync read param go wrong", ERR);
        // set params to sync context
        ASSERT(OK == ParseThis(env, self, context), "CloudSync get editObject go wrong", ERR);
        ASSERT(OK == ParseCloudSyncMode(env, argv[0], context), "parse syncMode go wrong", ERR);

        napi_value name = nullptr;
        ASSERT(napi_ok == napi_create_string_utf8(env, "syncCallback", NAPI_AUTO_LENGTH, &name),
            "create function name string wrong", ERR);
        ASSERT(napi_ok == napi_create_threadsafe_function(env, argv[1], nullptr, name, 0, 1, nullptr, nullptr, nullptr,
            AsyncCall::CloudSyncCallback, &context->callback_), "create syncCallback function wrong", ERR);
        return OK;
    };
}

ExecuteAction CollaborationEditObject::GetCloudSyncExec(std::shared_ptr<SyncContext> context)
{
    return [context]() -> int {
        LOG_INFO("CollaborationEditObject::CloudSync Async execute.");
        auto *editObject = reinterpret_cast<CollaborationEditObject *>(context->boundObj);
        ASSERT(editObject != nullptr && context->dbStore_ != nullptr, "editObject is null or dbStore is null", ERR);

        std::shared_ptr<SyncService> syncService = SyncService::GetInstance();
        uint64_t syncId = syncService->GetSyncId();
        context->syncId = syncId;
        syncService->AddSyncContext(syncId, context);
        int32_t ret = context->dbStore_->Sync(GetGRDSyncMode(context->syncMode_), syncId,
            CollaborationEditObject::SyncCallbackFunc);
        if (ret != GRD_OK) {
            LOG_ERROR("dbStore sync go wrong, errCode: %{public}d", ret);
            GRD_SyncProcessT process = {};
            process.status = GRD_SYNC_PROCESS_FINISHED;
            process.errCode = ret;
            process.mode = GRD_SYNC_MODE_INVALID;
            process.cloudDB = nullptr;
            process.syncId = context->syncId;
            SyncCallbackFunc(&process);
            return ERR;
        }
        return OK;
    };
}

napi_value CollaborationEditObject::CloudSync(napi_env env, napi_callback_info info)
{
    LOG_DEBUG("CloudSync start.");
    auto context = std::make_shared<SyncContext>();
    auto input = CollaborationEditObject::GetCloudSyncInput(context);
    // create cloudSync execute function
    auto exec = GetCloudSyncExec(context);
    // create cloudSync output function
    auto output = [context](napi_env env, napi_value &result) {
        LOG_DEBUG("CollaborationEditObject::CloudSync output.");
        if (context->execCode_ != OK) {
            SyncService::GetInstance()->RemoveSyncContext(context->syncId);
        }
    };
    // parse napi info and register exec/output functions to context
    int ret = context->SetAll(env, info, input, exec, output);
    if (ret != OK) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "parse params go wrong");
        return nullptr;
    }
    // register cloudSync task to napi queue
    ret = AsyncCall::Call(env, context, SYNC_FUNCTION_NAME);
    if (ret != OK) {
        if (context->callback_ != nullptr) {
            napi_release_threadsafe_function(context->callback_, napi_tsfn_release);
            context->callback_ = nullptr;
        }
        context->ReleaseInnerReference();
        context.reset();
        ThrowNapiError(env, Status::INTERNAL_ERROR, "register sync task go wrong");
    }
    return nullptr;
}

GRD_SyncModeE CollaborationEditObject::GetGRDSyncMode(int32_t mode)
{
    SyncMode syncMode = static_cast<SyncMode>(mode);
    auto it = SYNC_MODE_MAP.find(syncMode);
    if (it == SYNC_MODE_MAP.end()) {
        return GRD_SYNC_MODE_INVALID;
    }
    return it->second;
}

ProgressCode CollaborationEditObject::GetProgressCode(int32_t errCode)
{
    switch (errCode) {
        case GRD_OK:
            return ProgressCode::CLOUD_SYNC_SUCCESS;
        case GRD_SYNC_PREREQUISITES_ABNORMAL:
            return ProgressCode::CLOUD_NOT_SET;
        case GRD_INNER_ERR:
            return ProgressCode::SYNC_INTERNAL_ERROR;
        default:
            break;
    }
    return ProgressCode::SYNC_EXTERNAL_ERROR;
}

void CollaborationEditObject::SyncCallbackFunc(GRD_SyncProcessT *syncProcess)
{
    LOG_INFO("syncId=%{public}" PRIu64 " status=%{public}d, errCode=%{public}d.",
        syncProcess->syncId, syncProcess->status, syncProcess->errCode);

    // call arkTs callback
    std::shared_ptr<SyncService> syncService = SyncService::GetInstance();
    std::shared_ptr<SyncContext> context = syncService->GetSyncContext(syncProcess->syncId);
    syncService->RemoveSyncContext(syncProcess->syncId);
    if (context == nullptr) {
        LOG_ERROR("context is null");
        return;
    }
    napi_threadsafe_function js_cb = context->callback_;
    if (js_cb == nullptr) {
        LOG_ERROR("callback is null");
        context->ReleaseInnerReference();
        return;
    }

    napi_status ret = napi_acquire_threadsafe_function(js_cb);
    if (ret != napi_ok) {
        LOG_ERROR("acquire thread safe function failed, ret: %{public}d", static_cast<int32_t>(ret));
        napi_release_threadsafe_function(js_cb, napi_tsfn_release);
        context->callback_ = nullptr;
        context->ReleaseInnerReference();
        return;
    }
    SyncCallbackParamT param = {};
    param.detail.code = CollaborationEditObject::GetProgressCode(syncProcess->errCode);

    std::future<int> future = param.promise.get_future();
    ret = napi_call_threadsafe_function(js_cb, &param, napi_tsfn_blocking);
    if (ret != napi_ok) {
        LOG_ERROR("call function go wrong, ret=%{public}d", static_cast<int32_t>(ret));
        napi_release_threadsafe_function(js_cb, napi_tsfn_release);
        context->callback_ = nullptr;
        context->ReleaseInnerReference();
        return;
    }
    std::future_status fstatus = future.wait_for(std::chrono::duration_cast<std::chrono::seconds>(TIME_THRESHOLD));
    if (fstatus == std::future_status::ready) {
        future.get();
    } else {
        LOG_ERROR("wait for js callback timeout");
    }

    napi_release_threadsafe_function(js_cb, napi_tsfn_release);
    context->callback_ = nullptr;
    context->ReleaseInnerReference();
    LOG_INFO("syncCallback end");
}

void CollaborationEditObject::SetDBStore(std::shared_ptr<DBStore> dbStore)
{
    this->dbStore_ = dbStore;
}

std::shared_ptr<DBStore> CollaborationEditObject::GetDBStore()
{
    return this->dbStore_;
}

napi_value CollaborationEditObject::GetLocalId(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    CollaborationEditObject *editObject = nullptr;
    napi_status status = napi_unwrap(env, self, reinterpret_cast<void **>(&editObject));
    ASSERT_THROW(env, status == napi_ok, status, "unwrap object go wrong");
    std::string localId = (*editObject->dbStore_).GetLocalId();
    napi_value result;
    NapiUtils::SetValue(env, localId, result);
    return result;
}

napi_value CollaborationEditObject::ApplyUpdate(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    CollaborationEditObject *editObject = nullptr;
    napi_status status = napi_unwrap(env, self, reinterpret_cast<void **>(&editObject));
    ASSERT_THROW(env, status == napi_ok, status, "unwrap object go wrong");

    std::string applyInfo;
    int32_t ret = (*editObject->dbStore_).ApplyUpdate(applyInfo);
    ASSERT_THROW(env, ret == OK, ret, "ApplyUpdate go wrong");
    napi_value result;
    ret = Parser::ParseJsonStrToJsUpdateNode(env, applyInfo, editObject->dbStore_, result);
    ASSERT_THROW(env, ret == OK, ret, "ParseJsonStrToJsUpdateNode go wrong");
    return result;
}

napi_value CollaborationEditObject::WriteUpdate(napi_env env, napi_callback_info info)
{
    size_t argc = 3;
    napi_value argv[3] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    CollaborationEditObject *editObject = nullptr;
    napi_status status = napi_unwrap(env, self, reinterpret_cast<void **>(&editObject));
    ASSERT_THROW(env, status == napi_ok, status, "unwrap object go wrong");

    std::string equipId;
    status = NapiUtils::GetNamedProperty(env, argv[0], "id", equipId);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "read equipId param go wrong");

    std::vector<uint8_t> data;
    status = NapiUtils::GetNamedProperty(env, argv[0], "data", data);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "read data param go wrong");

    int64_t watermark;
    status = NapiUtils::GetNamedProperty(env, argv[0], "cursor", watermark);
    ASSERT_THROW(env, status == napi_ok, Status::INVALID_ARGUMENT, "read cursor param go wrong");

    int32_t ret =
        (*editObject->dbStore_).WriteUpdate(equipId.c_str(), data.data(), data.size(), std::to_string(watermark));
    ASSERT_THROW(env, ret == OK, ret, "WriteUpdate go wrong");
    return nullptr;
}
} // namespace OHOS::CollaborationEdit
