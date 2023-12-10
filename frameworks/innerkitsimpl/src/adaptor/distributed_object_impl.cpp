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

#include "distributed_object_impl.h"

#include "hitrace.h"
#include "objectstore_errors.h"
#include "string_utils.h"
#include "dev_manager.h"
#include "bytes_utils.h"

namespace OHOS::ObjectStore {
DistributedObjectImpl::~DistributedObjectImpl()
{
}

uint32_t DistributedObjectImpl::PutDouble(const std::string &key, double value)
{
    DataObjectHiTrace trace("DistributedObjectImpl::PutDouble");
    return flatObjectStore_->PutDouble(sessionId_, key, value);
}

uint32_t DistributedObjectImpl::PutBoolean(const std::string &key, bool value)
{
    DataObjectHiTrace trace("DistributedObjectImpl::PutBoolean");
    return flatObjectStore_->PutBoolean(sessionId_, key, value);
}

uint32_t DistributedObjectImpl::PutString(const std::string &key, const std::string &value)
{
    DataObjectHiTrace trace("DistributedObjectImpl::PutString");
    if (key.find(ASSET_DOT) != std::string::npos) {
        PutDeviceId();
    }
    return flatObjectStore_->PutString(sessionId_, key, value);
}

uint32_t DistributedObjectImpl::GetDouble(const std::string &key, double &value)
{
    return flatObjectStore_->GetDouble(sessionId_, key, value);
}

uint32_t DistributedObjectImpl::GetBoolean(const std::string &key, bool &value)
{
    return flatObjectStore_->GetBoolean(sessionId_, key, value);
}

uint32_t DistributedObjectImpl::GetString(const std::string &key, std::string &value)
{
    return flatObjectStore_->GetString(sessionId_, key, value);
}

uint32_t DistributedObjectImpl::GetType(const std::string &key, Type &type)
{
    return flatObjectStore_->GetType(sessionId_, key, type);
}

std::string &DistributedObjectImpl::GetSessionId()
{
    return sessionId_;
}

DistributedObjectImpl::DistributedObjectImpl(const std::string &sessionId, FlatObjectStore *flatObjectStore)
    : sessionId_(sessionId), flatObjectStore_(flatObjectStore)
{
}

uint32_t DistributedObjectImpl::PutComplex(const std::string &key, const std::vector<uint8_t> &value)
{
    DataObjectHiTrace trace("DistributedObjectImpl::PutComplex");
    return flatObjectStore_->PutComplex(sessionId_, key, value);
}

uint32_t DistributedObjectImpl::GetComplex(const std::string &key, std::vector<uint8_t> &value)
{
    return flatObjectStore_->GetComplex(sessionId_, key, value);
}

uint32_t DistributedObjectImpl::Save(const std::string &deviceId)
{
    uint32_t status = flatObjectStore_->Save(sessionId_, deviceId);
    if (status != SUCCESS) {
        LOG_ERROR("DistributedObjectImpl:Save failed. status = %{public}d", status);
        return status;
    }
    return status;
}

uint32_t DistributedObjectImpl::RevokeSave()
{
    uint32_t status = flatObjectStore_->RevokeSave(sessionId_);
    if (status != SUCCESS) {
        LOG_ERROR("DistributedObjectImpl:RevokeSave failed. status = %{public}d", status);
        return status;
    }
    return status;
}

uint32_t DistributedObjectImpl::PutDeviceId()
{
    DevManager::DetailInfo detailInfo = DevManager::GetInstance()->GetLocalDevice();
    return flatObjectStore_->PutString(sessionId_, DEVICEID_KEY, detailInfo.networkId);
}

uint32_t DistributedObjectImpl::GetAssetValue(const std::string &assetKey, Asset &assetValue)
{
    double assetStatus = 0.0;
    auto status = GetDouble(assetKey + STATUS_SUFFIX, assetStatus);
    if (status == SUCCESS) {
        assetValue.status = static_cast<uint32_t>(assetStatus);
    }
    status = GetString(assetKey + NAME_SUFFIX, assetValue.name);
    LOG_ERROR_RETURN(status == SUCCESS, "get name failed!", status);
    status = GetString(assetKey + URI_SUFFIX, assetValue.uri);
    LOG_ERROR_RETURN(status == SUCCESS, "get uri failed!", status);
    status = GetString(assetKey + PATH_SUFFIX, assetValue.path);
    LOG_ERROR_RETURN(status == SUCCESS, "get path failed!", status);
    status = GetString(assetKey + CREATE_TIME_SUFFIX, assetValue.createTime);
    LOG_ERROR_RETURN(status == SUCCESS, "get createTime failed!", status);
    status = GetString(assetKey + MODIFY_TIME_SUFFIX, assetValue.modifyTime);
    LOG_ERROR_RETURN(status == SUCCESS, "get modifyTime failed!", status);
    status = GetString(assetKey + SIZE_SUFFIX, assetValue.size);
    LOG_ERROR_RETURN(status == SUCCESS, "get size failed!", status);
    RemovePrefix(assetValue);
    return status;
}

void DistributedObjectImpl::RemovePrefix(Asset &assetValue)
{
    assetValue.name = assetValue.name.substr(STRING_PREFIX_LEN);
    assetValue.uri = assetValue.uri.substr(STRING_PREFIX_LEN);
    assetValue.path = assetValue.path.substr(STRING_PREFIX_LEN);
    assetValue.createTime = assetValue.createTime.substr(STRING_PREFIX_LEN);
    assetValue.modifyTime = assetValue.modifyTime.substr(STRING_PREFIX_LEN);
    assetValue.size = assetValue.size.substr(STRING_PREFIX_LEN);
    assetValue.hash = assetValue.modifyTime + "_" + assetValue.size;
}

uint32_t DistributedObjectImpl::BindAssetStore(const std::string &assetKey, AssetBindInfo &bindInfo)
{
    Asset assetValue;
    auto status = GetAssetValue(assetKey, assetValue);
    if (status != SUCCESS) {
        LOG_ERROR("DistributedObjectImpl:GetAssetValue failed. status = %{public}d", status);
        return status;
    }
    status = flatObjectStore_->BindAssetStore(sessionId_, bindInfo, assetValue);
    if (status != SUCCESS) {
        LOG_ERROR("DistributedObjectImpl:BindAssetStore failed. status = %{public}d", status);
        return status;
    }
    return status;
}
} // namespace OHOS::ObjectStore