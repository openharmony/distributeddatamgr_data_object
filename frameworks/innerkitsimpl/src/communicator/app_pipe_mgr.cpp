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

#include "app_pipe_mgr.h"

namespace OHOS {
namespace ObjectStore {
static const int MAX_TRANSFER_SIZE = 1024 * 1024 * 5;
Status AppPipeMgr::StartWatchDataChange(const AppDataChangeListener *observer, const PipeInfo &pipeInfo)
{
    LOG_INFO("begin");
    if (observer == nullptr || pipeInfo.pipeId.empty()) {
        LOG_ERROR("argument invalid");
        return Status::INVALID_ARGUMENT;
    }
    std::lock_guard<std::mutex> lock(dataBusMapMutex_);
    auto it = dataBusMap_.find(pipeInfo.pipeId);
    if (it == dataBusMap_.end()) {
        LOG_ERROR("pipeid not found");
        return Status::ERROR;
    }
    LOG_INFO("end");
    return it->second->StartWatchDataChange(observer, pipeInfo);
}

// stop DataChangeListener to watch data change;
Status AppPipeMgr::StopWatchDataChange(const AppDataChangeListener *observer, const PipeInfo &pipeInfo)
{
    LOG_INFO("begin");
    if (observer == nullptr || pipeInfo.pipeId.empty()) {
        LOG_ERROR("argument invalid");
        return Status::INVALID_ARGUMENT;
    }
    std::lock_guard<std::mutex> lock(dataBusMapMutex_);
    auto it = dataBusMap_.find(pipeInfo.pipeId);
    if (it == dataBusMap_.end()) {
        LOG_ERROR("pipeid not found");
        return Status::ERROR;
    }
    LOG_INFO("end");
    return it->second->StopWatchDataChange(observer, pipeInfo);
}

// Send data to other device, function will be called back after sent to notify send result.
Status AppPipeMgr::SendData(const PipeInfo &pipeInfo, const DeviceId &deviceId, const DataInfo &dataInfo,
    uint32_t totalLength, const MessageInfo &info)
{
    if (dataInfo.length > MAX_TRANSFER_SIZE || dataInfo.length <= 0 || dataInfo.data == nullptr
        || pipeInfo.pipeId.empty() || deviceId.deviceId.empty()) {
        LOG_WARN("Input is invalid, maxSize:%{public}d, current size:%{public}u", MAX_TRANSFER_SIZE, dataInfo.length);
        return Status::ERROR;
    }
    LOG_DEBUG("pipeInfo:%{public}s ,size:%{public}u", pipeInfo.pipeId.c_str(), dataInfo.length);
    std::shared_ptr<AppPipeHandler> appPipeHandler;
    {
        std::lock_guard<std::mutex> lock(dataBusMapMutex_);
        auto it = dataBusMap_.find(pipeInfo.pipeId);
        if (it == dataBusMap_.end()) {
            LOG_WARN("pipeInfo:%{public}s not found", pipeInfo.pipeId.c_str());
            return Status::KEY_NOT_FOUND;
        }
        appPipeHandler = it->second;
    }
    return appPipeHandler->SendData(pipeInfo, deviceId, dataInfo, totalLength, info);
}

// start server
Status AppPipeMgr::Start(const PipeInfo &pipeInfo)
{
    if (pipeInfo.pipeId.empty()) {
        LOG_WARN("Start Failed, pipeInfo is empty.");
        return Status::INVALID_ARGUMENT;
    }
    std::lock_guard<std::mutex> lock(dataBusMapMutex_);
    auto it = dataBusMap_.find(pipeInfo.pipeId);
    if (it != dataBusMap_.end()) {
        LOG_WARN("repeated start, pipeInfo:%{public}s.", pipeInfo.pipeId.c_str());
        return Status::REPEATED_REGISTER;
    }
    LOG_DEBUG("Start pipeInfo:%{public}s ", pipeInfo.pipeId.c_str());
    auto handler = std::make_shared<AppPipeHandler>(pipeInfo);
    int ret = handler->CreateSessionServer(pipeInfo.pipeId);
    if (ret != 0) {
        LOG_WARN("Start pipeInfo:%{public}s, failed ret:%{public}d.", pipeInfo.pipeId.c_str(), ret);
        return Status::ILLEGAL_STATE;
    }

    dataBusMap_.insert(std::pair<std::string, std::shared_ptr<AppPipeHandler>>(pipeInfo.pipeId, handler));
    return Status::SUCCESS;
}

// stop server
Status AppPipeMgr::Stop(const PipeInfo &pipeInfo)
{
    std::shared_ptr<AppPipeHandler> appPipeHandler;
    {
        std::lock_guard<std::mutex> lock(dataBusMapMutex_);
        auto it = dataBusMap_.find(pipeInfo.pipeId);
        if (it == dataBusMap_.end()) {
            LOG_WARN("pipeInfo:%{public}s not found", pipeInfo.pipeId.c_str());
            return Status::KEY_NOT_FOUND;
        }
        appPipeHandler = it->second;
        int ret = appPipeHandler->RemoveSessionServer(pipeInfo.pipeId);
        if (ret != 0) {
            LOG_WARN("Stop pipeInfo:%{public}s ret:%{public}d.", pipeInfo.pipeId.c_str(), ret);
            return Status::ERROR;
        }
        dataBusMap_.erase(pipeInfo.pipeId);
        return Status::SUCCESS;
    }
    return Status::KEY_NOT_FOUND;
}

bool AppPipeMgr::IsSameStartedOnPeer(const struct PipeInfo &pipeInfo, const struct DeviceId &peer)
{
    LOG_INFO("start");
    if (pipeInfo.pipeId.empty() || peer.deviceId.empty()) {
        LOG_ERROR("pipeId or deviceId is empty. Return false.");
        return false;
    }
    LOG_INFO("pipeInfo == [%{public}s]", pipeInfo.pipeId.c_str());
    std::shared_ptr<AppPipeHandler> appPipeHandler;
    {
        std::lock_guard<std::mutex> lock(dataBusMapMutex_);
        auto it = dataBusMap_.find(pipeInfo.pipeId);
        if (it == dataBusMap_.end()) {
            LOG_ERROR("pipeInfo:%{public}s not found. Return false.", pipeInfo.pipeId.c_str());
            return false;
        }
        appPipeHandler = it->second;
    }
    return appPipeHandler->IsSameStartedOnPeer(pipeInfo, peer);
}
} // namespace ObjectStore
} // namespace OHOS
