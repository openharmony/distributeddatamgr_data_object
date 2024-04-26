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
#define LOG_TAG "AssetChangeTimer"
#include "asset_change_timer.h"

#include "client_adaptor.h"
#include "logger.h"
#include "objectstore_errors.h"

namespace OHOS::ObjectStore {
std::mutex AssetChangeTimer::instanceMutex;
AssetChangeTimer *AssetChangeTimer::instance = nullptr;

AssetChangeTimer *AssetChangeTimer::GetInstance(FlatObjectStore *flatObjectStore)
{
    if (instance == nullptr) {
        std::lock_guard<decltype(instanceMutex)> lockGuard(instanceMutex);
        if (instance == nullptr) {
            instance = new (std::nothrow) AssetChangeTimer(flatObjectStore);
        }
    }
    return instance;
}

AssetChangeTimer::AssetChangeTimer(FlatObjectStore *flatObjectStore)
{
    flatObjectStore_ = flatObjectStore;
    executor_ = std::make_shared<ExecutorPool>(MAX_THREADS, MIN_THREADS);
}

void AssetChangeTimer::OnAssetChanged(
    const std::string &sessionId, const std::string &assetKey, std::shared_ptr<ObjectWatcher> watcher)
{
    StartTimer(sessionId, assetKey, watcher);
}

void AssetChangeTimer::StartTimer(
    const std::string &sessionId, const std::string &assetKey, std::shared_ptr<ObjectWatcher> watcher)
{
    std::string key = sessionId + ASSET_SEPARATOR + assetKey;
    std::lock_guard<decltype(mutex_)> lockGuard(mutex_);
    if (assetChangeTasks_.find(key) == assetChangeTasks_.end()) {
        assetChangeTasks_[key] =
            executor_->Schedule(std::chrono::milliseconds(WAIT_INTERVAL), ProcessTask(sessionId, assetKey, watcher));
    } else {
        assetChangeTasks_[key] = executor_->Reset(assetChangeTasks_[key], std::chrono::milliseconds(WAIT_INTERVAL));
    }
}

std::function<void()> AssetChangeTimer::ProcessTask(
    const std::string &sessionId, const std::string &assetKey, std::shared_ptr<ObjectWatcher> watcher)
{
    return [=]() {
        LOG_DEBUG("Start working on a task, sessionId: %{public}s, assetKey: %{public}s", sessionId.c_str(),
            assetKey.c_str());
        StopTimer(sessionId, assetKey);
        uint32_t status = HandleAssetChanges(sessionId, assetKey);
        if (status == SUCCESS) {
            LOG_DEBUG("Asset change task end, start callback, sessionId: %{public}s, assetKey: %{public}s",
                sessionId.c_str(), assetKey.c_str());
            watcher->OnChanged(sessionId, { assetKey });
        }
    };
}

void AssetChangeTimer::StopTimer(const std::string &sessionId, const std::string &assetKey)
{
    std::string key = sessionId + ASSET_SEPARATOR + assetKey;
    std::lock_guard<decltype(mutex_)> lockGuard(mutex_);
    executor_->Remove(assetChangeTasks_[key]);
    assetChangeTasks_.erase(key);
}

uint32_t AssetChangeTimer::HandleAssetChanges(const std::string &sessionId, const std::string &assetKey)
{
    Asset assetValue;
    if (!GetAssetValue(sessionId, assetKey, assetValue)) {
        LOG_ERROR("GetAssetValue assetValue is not complete, sessionId: %{public}s, assetKey: %{public}s",
            sessionId.c_str(), assetKey.c_str());
        return ERR_DB_GET_FAIL;
    }

    std::string deviceId;
    uint32_t status = flatObjectStore_->GetString(sessionId, DEVICEID_KEY, deviceId);
    if (status != SUCCESS) {
        LOG_ERROR("get deviceId failed %{public}d", status);
        return status;
    }

    sptr<OHOS::DistributedObject::IObjectService> proxy = ClientAdaptor::GetObjectService();
    if (proxy == nullptr) {
        LOG_ERROR("proxy is nullptr.");
        return ERR_NULL_PTR;
    }
    int32_t res = proxy->OnAssetChanged(flatObjectStore_->GetBundleName(), sessionId, deviceId, assetValue);
    if (res != SUCCESS) {
        LOG_ERROR("OnAssetChanged failed status: %{public}d, sessionId: %{public}s, assetKey: %{public}s", status,
            sessionId.c_str(), assetKey.c_str());
    }
    return res;
}

bool AssetChangeTimer::GetAssetValue(const std::string &sessionId, const std::string &assetKey, Asset &assetValue)
{
    double doubleStatus;
    if (flatObjectStore_->GetDouble(sessionId, assetKey + STATUS_SUFFIX, doubleStatus) == SUCCESS) {
        assetValue.status = static_cast<uint32_t>(doubleStatus);
    }
    bool isComplete = true;
    isComplete = isComplete &&
        (flatObjectStore_->GetString(sessionId, assetKey + NAME_SUFFIX, assetValue.name) == SUCCESS);
    isComplete = isComplete &&
        (flatObjectStore_->GetString(sessionId, assetKey + URI_SUFFIX, assetValue.uri) == SUCCESS);
    isComplete = isComplete &&
        (flatObjectStore_->GetString(sessionId, assetKey + PATH_SUFFIX, assetValue.path) == SUCCESS);
    isComplete = isComplete &&
        (flatObjectStore_->GetString(sessionId, assetKey + CREATE_TIME_SUFFIX, assetValue.createTime) == SUCCESS);
    isComplete = isComplete &&
        (flatObjectStore_->GetString(sessionId, assetKey + MODIFY_TIME_SUFFIX, assetValue.modifyTime) == SUCCESS);
    isComplete = isComplete &&
        (flatObjectStore_->GetString(sessionId, assetKey + SIZE_SUFFIX, assetValue.size) == SUCCESS);
    if (isComplete) {
        assetValue.name = assetValue.name.substr(STRING_PREFIX_LEN);
        assetValue.uri = assetValue.uri.substr(STRING_PREFIX_LEN);
        assetValue.path = assetValue.path.substr(STRING_PREFIX_LEN);
        assetValue.createTime = assetValue.createTime.substr(STRING_PREFIX_LEN);
        assetValue.modifyTime = assetValue.modifyTime.substr(STRING_PREFIX_LEN);
        assetValue.size = assetValue.size.substr(STRING_PREFIX_LEN);
        assetValue.hash = assetValue.modifyTime + "_" + assetValue.size;
    }
    return isComplete;
}
} // namespace OHOS::ObjectStore