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
#define LOG_TAG "UndoManager"

#include "napi_undo_manager.h"

#include "napi_error_utils.h"

namespace OHOS::CollaborationEdit {

UndoManager::UndoManager(std::string tableName, int64_t captureTimeout) : captureTimeout_(captureTimeout)
{
    this->adapter_ = std::make_shared<RdAdapter>();
    this->adapter_->SetTableName(tableName);
}

UndoManager::~UndoManager()
{}

napi_value UndoManager::Initialize(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &self, nullptr));
    std::string tableName;
    napi_status status = NapiUtils::GetValue(env, argv[0], tableName);
    ASSERT_THROW_BASE(env, status == napi_ok, Status::INVALID_ARGUMENT, "read tableName go wrong", self);
    int64_t captureTimeout = 0;
    status = NapiUtils::GetNamedProperty(env, argv[1], "captureTimeout", captureTimeout);
    if (status != napi_ok) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "read captureTimeout param go wrong");
        return self;
    }
    ASSERT_THROW_BASE(env, status == napi_ok, Status::INVALID_ARGUMENT, "read captureTimeout param go wrong", self);
    ASSERT_THROW_BASE(env, captureTimeout >= 0, Status::INVALID_ARGUMENT,
        "Param Error: captureTimeout cannot be negative.", self);
    tableName = std::to_string(LABEL_FRAGMENT) + "_" + tableName;
    UndoManager *undoManager = new (std::nothrow) UndoManager(tableName, captureTimeout);
    auto finalize = [](napi_env env, void *data, void *hint) {
        UndoManager *undoManager = reinterpret_cast<UndoManager *>(data);
        delete undoManager;
    };
    napi_wrap(env, self, undoManager, finalize, nullptr, nullptr);
    return self;
}

napi_value UndoManager::Constructor(napi_env env)
{
    auto lambda = []() -> std::vector<napi_property_descriptor> {
        std::vector<napi_property_descriptor> properties = {
            DECLARE_NAPI_FUNCTION("undo", Undo),
            DECLARE_NAPI_FUNCTION("redo", Redo),
        };
        return properties;
    };
    return NapiUtils::DefineClass(
        env, "ohos.data.collaborationEditObject", "UndoManager", lambda, Initialize);
}

napi_value UndoManager::NewInstance(napi_env env, napi_callback_info info)
{
    size_t argc = 2;
    napi_value argv[2] = {nullptr};
    napi_value undoManager = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr));

    napi_status status = napi_new_instance(env, Constructor(env), argc, argv, &undoManager);
    if (undoManager == nullptr || status != napi_ok) {
        LOG_ERROR("[NewInstance] new instance go wrong");
    }
    return undoManager;
}

napi_value UndoManager::Undo(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    UndoManager *manager = nullptr;
    napi_status status = napi_unwrap(env, self, reinterpret_cast<void **>(&manager));
    if (status != napi_ok || manager == nullptr) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "unwrap node go wrong");
        return nullptr;
    }
    int32_t retCode = manager->GetAdapter()->Undo();
    if (retCode != 0) {
        ThrowNapiError(env, Status::DB_ERROR, "undo go wrong.");
    }
    return nullptr;
}

napi_value UndoManager::Redo(napi_env env, napi_callback_info info)
{
    napi_value self = nullptr;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &self, nullptr));
    UndoManager *manager = nullptr;
    napi_status status = napi_unwrap(env, self, reinterpret_cast<void **>(&manager));
    if (status != napi_ok || manager == nullptr) {
        ThrowNapiError(env, Status::INVALID_ARGUMENT, "unwrap self go wrong");
        return nullptr;
    }
    int32_t retCode = manager->GetAdapter()->Redo();
    if (retCode != 0) {
        ThrowNapiError(env, Status::DB_ERROR, "redo go wrong.");
    }
    return nullptr;
}

std::string UndoManager::GetTableName()
{
    return this->adapter_->GetTableName();
}

int64_t UndoManager::GetCaptureTimeout()
{
    return this->captureTimeout_;
}

std::shared_ptr<RdAdapter> UndoManager::GetAdapter()
{
    return this->adapter_;
}

void UndoManager::SetDBStore(std::shared_ptr<DBStore> dbStore)
{
    this->adapter_->SetDBStore(dbStore);
}

} // namespace OHOS::CollaborationEdit
