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
#include "uv_queue.h"
#include "logger.h"

namespace OHOS::ObjectStore {
UvQueue::UvQueue(napi_env env) : env_(env)
{
    napi_get_uv_event_loop(env, &loop_);
}

UvQueue::~UvQueue()
{
    LOG_DEBUG("no memory leak for queue-callback");
}

void UvQueue::ExecUvWork(UvEntry *entry)
{
    if (entry == nullptr) {
        LOG_ERROR("entry is nullptr");
        return;
    }
    auto queue = entry->uvQueue_.lock();
    if (queue != nullptr) {
        std::unique_lock<std::shared_mutex> cacheLock(queue->mutex_);
        for (auto &item : queue->args_) {
            item.first(queue->env_, item.second);
        }
        queue->args_.clear();
    }
    delete entry;
    entry = nullptr;
}

bool UvQueue::CallFunction(Process process, void *argv)
{
    if (process == nullptr || argv == nullptr) {
        LOG_ERROR("nullptr");
        return false;
    }
    auto *uvEntry = new (std::nothrow) UvEntry{ weak_from_this() };
    if (uvEntry == nullptr) {
        LOG_ERROR("no memory for UvEntry");
        return false;
    }
    auto rollbackAddition = [this, process, argv]() {
        std::unique_lock<std::shared_mutex> lock(mutex_);
        auto it = args_.find(process);
        if (it != args_.end() && !it->second.empty()) {
            it->second.pop_back();
            if (it->second.empty()) {
                args_.erase(it);
            }
        }
    };
    {
        std::unique_lock<std::shared_mutex> cacheLock(mutex_);
        auto &processList = args_[process];
        processList.push_back(argv);
    }

    auto task = [uvEntry]() { UvQueue::ExecUvWork(uvEntry); };
    auto ret = napi_send_event(env_, task, napi_eprio_high);
    if (ret != 0) {
        LOG_ERROR("napi_send_event failed, ret: %{public}d.", ret);
        rollbackAddition();
        delete uvEntry;
        return false;
    }
    return true;
}
} // namespace OHOS::ObjectStore
