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

#include "process_communicator_impl.h"

#include <logger.h>

namespace OHOS {
namespace ObjectStore {
using namespace DistributedDB;
ProcessCommunicatorImpl::ProcessCommunicatorImpl()
{
}

ProcessCommunicatorImpl::~ProcessCommunicatorImpl()
{
    LOG_ERROR("destructor.");
}

DBStatus ProcessCommunicatorImpl::Start(const std::string &processLabel)
{
    LOG_INFO("init commProvider");
    thisProcessLabel_ = processLabel;
    PipeInfo pi = { thisProcessLabel_ };
    Status errCode = CommunicationProvider::GetInstance().Start(pi);
    if (errCode != Status::SUCCESS) {
        LOG_ERROR("commProvider_ Start Fail.");
        return DBStatus::DB_ERROR;
    }
    return DBStatus::OK;
}

DBStatus ProcessCommunicatorImpl::Stop()
{
    PipeInfo pi = { thisProcessLabel_ };
    Status errCode = CommunicationProvider::GetInstance().Stop(pi);
    if (errCode != Status::SUCCESS) {
        LOG_ERROR("commProvider_ Stop Fail.");
        return DBStatus::DB_ERROR;
    }
    return DBStatus::OK;
}

DBStatus ProcessCommunicatorImpl::RegOnDeviceChange(const OnDeviceChange &callback)
{
    {
        std::lock_guard<std::mutex> onDeviceChangeLockGard(onDeviceChangeMutex_);
        onDeviceChangeHandler_ = callback;
    }

    PipeInfo pi = { thisProcessLabel_ };
    if (callback) {
        Status errCode = CommunicationProvider::GetInstance().StartWatchDeviceChange(this, pi);
        if (errCode != Status::SUCCESS) {
            LOG_ERROR("commProvider_ StartWatchDeviceChange Fail.");
            return DBStatus::DB_ERROR;
        }
    } else {
        Status errCode = CommunicationProvider::GetInstance().StopWatchDeviceChange(this, pi);
        if (errCode != Status::SUCCESS) {
            LOG_ERROR("commProvider_ StopWatchDeviceChange Fail.");
            return DBStatus::DB_ERROR;
        }
    }

    return DBStatus::OK;
}

DBStatus ProcessCommunicatorImpl::RegOnDataReceive(const OnDataReceive &callback)
{
    {
        std::lock_guard<std::mutex> onDataReceiveLockGard(onDataReceiveMutex_);
        onDataReceiveHandler_ = callback;
    }

    PipeInfo pi = { thisProcessLabel_ };
    if (callback) {
        Status errCode = CommunicationProvider::GetInstance().StartWatchDataChange(this, pi);
        if (errCode != Status::SUCCESS) {
            LOG_ERROR("commProvider_ StartWatchDataChange Fail.");
            return DBStatus::DB_ERROR;
        }
    } else {
        Status errCode = CommunicationProvider::GetInstance().StopWatchDataChange(this, pi);
        if (errCode != Status::SUCCESS) {
            LOG_ERROR("commProvider_ StopWatchDataChange Fail.");
            return DBStatus::DB_ERROR;
        }
    }
    return DBStatus::OK;
}

DBStatus ProcessCommunicatorImpl::SendData(const DeviceInfos &dstDevInfo, const uint8_t *data, uint32_t length)
{
    uint32_t totalLength = 0;
    return SendData(dstDevInfo, data, length, totalLength);
}

DBStatus ProcessCommunicatorImpl::SendData(
    const DeviceInfos &dstDevInfo, const uint8_t *data, uint32_t length, uint32_t totalLength)
{
    PipeInfo pi = { thisProcessLabel_ };
    const DataInfo dataInfo = { const_cast<uint8_t *>(data), length };
    DeviceId destination;
    destination.deviceId = dstDevInfo.identifier;
    Status errCode = CommunicationProvider::GetInstance().SendData(pi, destination, dataInfo, totalLength);
    if (errCode != Status::SUCCESS) {
        LOG_ERROR("commProvider_ SendData Fail.");
        return DBStatus::DB_ERROR;
    }

    return DBStatus::OK;
}

uint32_t ProcessCommunicatorImpl::GetMtuSize()
{
    return MTU_SIZE;
}

uint32_t ProcessCommunicatorImpl::GetMtuSize(const DeviceInfos &devInfo)
{
    return MTU_SIZE;
}

DeviceInfos ProcessCommunicatorImpl::GetLocalDeviceInfos()
{
    DeviceInfos localDevInfos;
    DeviceInfo devInfo = CommunicationProvider::GetInstance().GetLocalDevice();
    localDevInfos.identifier = devInfo.deviceId;
    return localDevInfos;
}

std::vector<DeviceInfos> ProcessCommunicatorImpl::GetRemoteOnlineDeviceInfosList()
{
    std::vector<DeviceInfos> remoteDevInfos;
    std::vector<DeviceInfo> devInfoVec = CommunicationProvider::GetInstance().GetDeviceList();
    for (auto const &entry : devInfoVec) {
        DeviceInfos remoteDev;
        remoteDev.identifier = entry.deviceId;
        remoteDevInfos.push_back(remoteDev);
    }
    return remoteDevInfos;
}

bool ProcessCommunicatorImpl::IsSameProcessLabelStartedOnPeerDevice(const DeviceInfos &peerDevInfo)
{
    PipeInfo pi = { thisProcessLabel_ };
    DeviceId di = { peerDevInfo.identifier };
    return CommunicationProvider::GetInstance().IsSameStartedOnPeer(pi, di);
}

void ProcessCommunicatorImpl::OnMessage(
    const DeviceInfo &info, const uint8_t *ptr, const int size, __attribute__((unused)) const PipeInfo &pipeInfo) const
{
    std::lock_guard<std::mutex> onDataReceiveLockGuard(onDataReceiveMutex_);
    if (onDataReceiveHandler_ == nullptr) {
        LOG_ERROR("onDataReceiveHandler_ invalid.");
        return;
    }
    DeviceInfos devInfo;
    devInfo.identifier = info.deviceId;
    onDataReceiveHandler_(devInfo, ptr, static_cast<uint32_t>(size));
}

void ProcessCommunicatorImpl::OnDeviceChanged(const DeviceInfo &info, const DeviceChangeType &type) const
{
    std::lock_guard<std::mutex> onDeviceChangeLockGuard(onDeviceChangeMutex_);
    if (onDeviceChangeHandler_ == nullptr) {
        LOG_ERROR("onDeviceChangeHandler_ invalid.");
        return;
    }
    std::vector<DeviceInfo> canHandleDev = CommunicationProvider::GetInstance().GetDeviceList();
    if (canHandleDev.empty()) {
        LOG_WARN("Can handle device is empty");
        return;
    }
    DeviceInfos devInfo;
    devInfo.identifier = info.deviceId;
    onDeviceChangeHandler_(devInfo, (type == DeviceChangeType::DEVICE_ONLINE));
}
} // namespace ObjectStore
} // namespace OHOS
