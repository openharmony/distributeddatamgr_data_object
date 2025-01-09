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

#include "napi_collaboration_edit_object.h"

#include "db_store_config.h"
#include "db_store_manager.h"
#include "napi_edit_unit.h"
#include "napi_error_utils.h"

namespace OHOS::CollaborationEdit {

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
    editObject->SetDBStore(dbStore);
    auto finalize = [](napi_env env, void *data, void *hint) {
        CollaborationEditObject *editObject = reinterpret_cast<CollaborationEditObject *>(data);
        delete editObject;
    };
    napi_wrap(env, self, editObject, finalize, nullptr, nullptr);
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
    if (retCode != SUCCESS) {
        ThrowNapiError(env, retCode, "create undo manager go wrong.");
        return nullptr;
    }
    return jsUndoManager;
}

napi_value CollaborationEditObject::DeleteUndoRedoManager(napi_env env, napi_callback_info info)
{
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

napi_value CollaborationEditObject::Delete(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    ContextParam context;
    napi_status status = NapiUtils::GetValue(env, argv[0], context);
    if (status != napi_ok) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "read context param go wrong");
        return self;
    }
    std::string docName;
    status = NapiUtils::GetNamedProperty(env, argv[1], "name", docName);
    if (status != napi_ok) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "read docName param go wrong");
        return self;
    }
    std::string dbFilePath = context.baseDir + "/" + docName;
    DBStoreConfig config(dbFilePath, docName);
    int ret = DBStoreManager::GetInstance().DeleteDBStore(config);
    if (ret != 0) {
        ThrowNapiError(env, Status::INTERNAL_ERROR, "remove dir go wrong");
    }
    return nullptr;
}

void CollaborationEditObject::SetDBStore(std::shared_ptr<DBStore> dbStore)
{
    this->dbStore_ = dbStore;
}

std::shared_ptr<DBStore> CollaborationEditObject::GetDBStore()
{
    return this->dbStore_;
}
} // namespace OHOS::CollaborationEdit
