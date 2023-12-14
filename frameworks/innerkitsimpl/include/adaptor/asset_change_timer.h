/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
#ifndef ASSET_CHANGE_TIMER_H
#define ASSET_CHANGE_TIMER_H
#include <unordered_map>

#include "executor_pool.h"
#include "flat_object_store.h"
#include "object_types.h"

namespace OHOS::ObjectStore {
class AssetChangeTimer {
public:
    static AssetChangeTimer *GetInstance(FlatObjectStore *flatObjectStore);
    void OnAssetChanged(
        const std::string &sessionId, const std::string &assetKey, std::shared_ptr<ObjectWatcher> watcher);

private:
    AssetChangeTimer() = default;
    ~AssetChangeTimer() = default;
    AssetChangeTimer(const AssetChangeTimer &) = delete;
    AssetChangeTimer &operator=(const AssetChangeTimer &) = delete;
    AssetChangeTimer(FlatObjectStore *flatObjectStore);
    void StartTimer(const std::string &sessionId, const std::string &assetKey, std::shared_ptr<ObjectWatcher> watcher);
    void StopTimer(const std::string &sessionId, const std::string &assetKey);
    std::function<void()> ProcessTask(
        const std::string &sessionId, const std::string &assetKey, std::shared_ptr<ObjectWatcher> watcher);
    uint32_t HandleAssetChanges(const std::string &sessionId, const std::string &assetKey);
    bool GetAssetValue(const std::string &sessionId, const std::string &assetKey, Asset &assetValue);

    std::mutex mutex_;
    std::unordered_map<std::string, Executor::TaskId> assetChangeTasks_;
    FlatObjectStore *flatObjectStore_ = nullptr;
    std::shared_ptr<ExecutorPool> executor_;

    static std::mutex instanceMutex;
    static AssetChangeTimer *instance;
    static constexpr size_t MAX_THREADS = 3;
    static constexpr size_t MIN_THREADS = 0;
    static constexpr uint32_t WAIT_INTERVAL = 100;
    static constexpr char ASSET_SEPARATOR = '#';
};
} // namespace OHOS::ObjectStore
#endif // ASSET_CHANGE_TIMER_H