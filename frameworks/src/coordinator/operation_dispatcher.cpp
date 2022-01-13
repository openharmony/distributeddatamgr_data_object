/*
 * Copyright (c) 2021 Huawei Device Co., Ltd.
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

#include "operation_dispatcher.h"

namespace OHOS::ObjectStore {
OperationDispatcher &OperationDispatcher::GetInstance()
{
    static OperationDispatcher executor;
    return executor;
}

uint32_t OperationDispatcher::RegisterExecutor(DataType type, std::shared_ptr<OperationExecutor> executor)
{
    if (executor == nullptr) {
        LOG_ERROR("OperationDispatcher-%s: fail to register executor", __func__);
        return ERR_INVAL;
    }
    std::unique_lock<std::shared_mutex> lock(executorMutex_);
    auto ite = executorMap_.find(type);
    if (ite == executorMap_.end()) {
        auto result = executorMap_.emplace(type, std::set<std::shared_ptr<OperationExecutor>>());
        ite = result.first;
    }
    ite->second.insert(executor);
    return SUCCESS;
}

uint32_t OperationDispatcher::UnregisterExecutor(DataType type, std::shared_ptr<OperationExecutor> executor)
{
    if (executor == nullptr) {
        LOG_ERROR("OperationDispatcher-%s: fail to unregister executor", __func__);
        return ERR_INVAL;
    }
    std::unique_lock<std::shared_mutex> lock(executorMutex_);
    auto ite = executorMap_.find(type);
    if (ite != executorMap_.end()) {
        ite->second.erase(executor);
    }
    return SUCCESS;
}

uint32_t OperationDispatcher::Publish(DataType type, const Bytes &key, const std::any &value)
{
    std::shared_lock<std::shared_mutex> lock(executorMutex_);
    auto ite = executorMap_.find(type);
    if (ite == executorMap_.end()) {
        LOG_ERROR("OperationDispatcher-%s: fail to find the executors", __func__);
        return ERR_INIT;
    }
    for (auto &executor : ite->second) {
        auto ret = executor->Publish(key, value);
        if (ret != SUCCESS) {
            LOG_ERROR("OperationDispatcher-%s: fail to publish", __func__);
            return ret;
        }
    }
    return SUCCESS;
}

uint32_t OperationDispatcher::PublishDelete(DataType type, const Bytes &key)
{
    std::shared_lock<std::shared_mutex> lock(executorMutex_);
    auto ite = executorMap_.find(type);
    if (ite == executorMap_.end()) {
        LOG_ERROR("OperationDispatcher-%s: fail to find the executors", __func__);
        return ERR_INIT;
    }
    for (auto &executor : ite->second) {
        auto ret = executor->PublishDelete(key);
        if (ret != SUCCESS) {
            LOG_ERROR("OperationDispatcher-%s: fail to publish", __func__);
            return ret;
        }
    }
    return SUCCESS;
}

uint32_t OperationDispatcher::Put(DataType type, const Bytes &key, const std::any &value)
{
    std::shared_lock<std::shared_mutex> lock(executorMutex_);
    auto ite = executorMap_.find(type);
    if (ite == executorMap_.end()) {
        LOG_ERROR("OperationDispatcher-%s: fail to find the executors", __func__);
        return ERR_INIT;
    }
    for (auto &executor : ite->second) {
        auto ret = executor->Put(key, value);
        if (ret != SUCCESS) {
            LOG_ERROR("OperationDispatcher-%s: fail to put", __func__);
            return ret;
        }
    }
    return SUCCESS;
}

uint32_t OperationDispatcher::Get(DataType type, const Bytes &key, std::any &value)
{
    std::shared_lock<std::shared_mutex> lock(executorMutex_);
    auto ite = executorMap_.find(type);
    if (ite == executorMap_.end()) {
        LOG_ERROR("OperationDispatcher-%s: fail to find the executors", __func__);
        return ERR_INIT;
    }
    for (auto &executor : ite->second) {
        auto ret = executor->Get(key, value);
        if (ret != SUCCESS) {
            LOG_ERROR("OperationDispatcher-%s: fail to publish", __func__);
            return ret;
        }
    }
    return SUCCESS;
}

uint32_t OperationDispatcher::Delete(DataType type, const Bytes &key)
{
    std::shared_lock<std::shared_mutex> lock(executorMutex_);
    auto ite = executorMap_.find(type);
    if (ite == executorMap_.end()) {
        LOG_ERROR("OperationDispatcher-%s: fail to find the executors", __func__);
        return ERR_INIT;
    }
    for (auto &executor : ite->second) {
        auto ret = executor->Delete(key);
        if (ret != SUCCESS) {
            LOG_ERROR("OperationDispatcher-%s: fail to publish", __func__);
            return ret;
        }
    }
    return SUCCESS;
}
}  // namespace OHOS::ObjectStore