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
#include "objectstore_errors.h"
#include "client_adaptor.h"
#include "logger.h"

namespace OHOS::ObjectStore {

std::mutex AssetChangeTimer::instanceMutex_;
AssetChangeTimer* AssetChangeTimer::INSTANCE = nullptr;
const std::string AssetChangeTimer::assetSeparator = "#";

AssetChangeTimer *AssetChangeTimer::GetInstance(FlatObjectStore *flatObjectStore, 
    std::shared_ptr<ObjectWatcher> watcher)
{
    std::lock_guard<decltype(instanceMutex_)> lockGuard(instanceMutex_);
    if (INSTANCE == nullptr){
        INSTANCE = new(std::nothrow) AssetChangeTimer(flatObjectStore, watcher);
    }
    return INSTANCE;
}

AssetChangeTimer::AssetChangeTimer(FlatObjectStore *flatObjectStore, std::shared_ptr<ObjectWatcher> watcher){
    flatObjectStore_ = flatObjectStore;
    watcher_ = watcher;
    executor_ = std::make_shared<ExecutorPool>(MAX_THREADS, MIN_THREADS);
}

void AssetChangeTimer::OnAssetChanged(const std::string &sessionId, const std::string &assetName, 
    const std::vector<std::string> &assetKeys)
{
    StartTimer(sessionId, assetName, assetKeys);
}

void AssetChangeTimer::StartTimer(const std::string &sessionId, const std::string &assetName, 
    const std::vector<std::string> &assetKeys)
{
    std::string key = sessionId + assetSeparator + assetName;
    std::lock_guard<decltype(mutex_)> lockGuard(mutex_);
    if (assetChangeTasks_.find(key) == assetChangeTasks_.end()){
        assetChangeTasks_[key] = 
            executor_->Schedule(std::chrono::milliseconds(WAIT_INTERVAL), 
                ProcessTask(sessionId, assetName, assetKeys));
    } else {
        assetChangeTasks_[key] = 
            executor_->Reset(assetChangeTasks_[key], std::chrono::milliseconds(WAIT_INTERVAL));
    }
}

std::function<void()> AssetChangeTimer::ProcessTask(const std::string &sessionId, 
    const std::string &assetName, const std::vector<std::string> &assetKeys)
{
    return [=]() {
        StopTimer(sessionId, assetName);
        uint32_t status = HandleAssetChanges(sessionId, assetName);
        if (status == SUCCESS){
            watcher_->OnChanged(sessionId, {assetName});
        }
    };
}

void AssetChangeTimer::StopTimer(const std::string &sessionId, const std::string &assetName)
{
    std::string key = sessionId + assetSeparator + assetName;
    std::lock_guard<decltype(mutex_)> lockGuard(mutex_);
    executor_->Remove(assetChangeTasks_[key]);
    assetChangeTasks_.erase(key);
}

uint32_t AssetChangeTimer::HandleAssetChanges(const std::string &sessionId, const std::string &assetName)
{
    Asset assetValue;
    if(!GetAssetValue(sessionId, assetName, assetValue)){
        LOG_ERROR("GetAssetValue assetValue is not complete");
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
    status = proxy->OnAssetChanged(flatObjectStore_->GetBundleName(), sessionId, deviceId, assetValue);
    if (status != SUCCESS) {
        LOG_ERROR("CacheManager OnAssetChanged failed code=%d.", static_cast<int>(status));
    }
    return status;
}

bool AssetChangeTimer::GetAssetValue(const std::string &sessionId, const std::string &assetName, 
    Asset &assetValue)
{
    double doubleStatus;
    if (flatObjectStore_->GetDouble(sessionId, assetName + STATUS_SUFFIX, doubleStatus) == SUCCESS){
        assetValue.status = static_cast<uint32_t>(doubleStatus);
    }
    bool isComplete = true;
    isComplete &= (flatObjectStore_->GetString(sessionId, assetName + NAME_SUFFIX, assetValue.name) == SUCCESS);
    isComplete &= (flatObjectStore_->GetString(sessionId, assetName + URI_SUFFIX, assetValue.uri) == SUCCESS);
    isComplete &= (flatObjectStore_->GetString(sessionId, assetName + PATH_SUFFIX, assetValue.path) == SUCCESS);
    isComplete &= (flatObjectStore_->GetString(
        sessionId, assetName + CREATE_TIME_SUFFIX, assetValue.createTime) == SUCCESS);
    isComplete &= (flatObjectStore_->GetString(
        sessionId, assetName + MODIFY_TIME_SUFFIX, assetValue.modifyTime) == SUCCESS);
    isComplete &= (flatObjectStore_->GetString(sessionId, assetName + SIZE_SUFFIX, assetValue.size) == SUCCESS);

    if (isComplete){
        assetValue.name = assetValue.name.substr(STRING_PREFIX_LEN);
        assetValue.uri = assetValue.uri.substr(STRING_PREFIX_LEN);
        assetValue.path = assetValue.path.substr(STRING_PREFIX_LEN);
        assetValue.createTime = assetValue.createTime.substr(STRING_PREFIX_LEN);
        assetValue.modifyTime = assetValue.modifyTime.substr(STRING_PREFIX_LEN);
        assetValue.size = assetValue.size.substr(STRING_PREFIX_LEN);
    }
    return isComplete;
}
} // namespace OHOS::ObjectStore