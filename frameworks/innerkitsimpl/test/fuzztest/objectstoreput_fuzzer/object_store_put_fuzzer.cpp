/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

#include "object_store_put_fuzzer.h"

#include <cstdint>
#include <fuzzer/FuzzedDataProvider.h>
#include <string>
#include <vector>
#include <iostream>
#include <thread>
#include <chrono>

#include "accesstoken_kit.h"
#include "distributed_object.h"
#include "distributed_objectstore.h"
#include "flat_object_storage_engine.h"
#include "nativetoken_kit.h"
#include "objectstore_errors.h"
#include "token_setproc.h"

using namespace OHOS::ObjectStore;
namespace OHOS {
static DistributedObject *object_ = nullptr;
static DistributedObjectStore *objectStore_ = nullptr;
constexpr const char *SESSIONID = "123456";
constexpr const char *BUNDLENAME = "com.example.myapplication";
bool g_hasPermission = false;
class TableWatcherImpl : public TableWatcher {
public:
    explicit TableWatcherImpl(const std::string &sessionId) : TableWatcher(sessionId)
    {
    }
    void OnChanged(const std::string &sessionid, const std::vector<std::string> &changedData,
        bool enableTransfer) override;
    virtual ~TableWatcherImpl();
};
TableWatcherImpl::~TableWatcherImpl()
{
}
void TableWatcherImpl::OnChanged(const std::string &sessionid, const std::vector<std::string> &changedData,
    bool enableTransfer)
{
    (void)sessionid;
    (void)changedData;
    (void)enableTransfer;
}

class TestObjectWatcher : public ObjectWatcher {
public:
    void OnChanged(const std::string &sessionid, const std::vector<std::string> &changedData) override
    {
        (void)sessionid;
        (void)changedData;
    }
    virtual ~TestObjectWatcher()
    {
    }
};

class TestStatusNotifier : public StatusNotifier {
public:
    void OnChanged(const std::string &sessionId, const std::string &networkId, const std::string &onlineStatus) override
    {
        (void)sessionId;
        (void)networkId;
        (void)onlineStatus;
    }
    virtual ~TestStatusNotifier()
    {
    }
};

void FuzzTestGetPermission()
{
    if (!g_hasPermission) {
        uint64_t tokenId;
        constexpr int perNum = 3;
        const char *perms[perNum] = {
            "ohos.permission.DISTRIBUTED_DATASYNC",
            "ohos.permission.DISTRIBUTED_SOFTBUS_CENTER",
            "ohos.permission.MONITOR_DEVICE_NETWORK_STATE"
        };

        NativeTokenInfoParams infoInstance = {
            .dcapsNum = 0,
            .permsNum = 3,
            .aclsNum = 0,
            .dcaps = nullptr,
            .perms = perms,
            .acls = nullptr,
            .processName = "objectfuzztest",
            .aplStr = "system_basic",
        };
        tokenId = GetAccessTokenId(&infoInstance);
        SetSelfTokenID(tokenId);
        OHOS::Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
        g_hasPermission = true;
    }
}

bool PutDoubleFuzz(FuzzedDataProvider &provider)
{
    if (objectStore_ == nullptr || object_ == nullptr) {
        return false;
    }
    double sval = provider.ConsumeFloatingPoint<double>();
    std::string skey = provider.ConsumeRandomLengthString(10);
    object_->PutDouble(skey, sval);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool PutBooleanFuzz(FuzzedDataProvider &provider)
{
    if (objectStore_ == nullptr || object_ == nullptr) {
        return false;
    }
    std::string skey = provider.ConsumeRandomLengthString(10);
    bool val = provider.ConsumeBool();
    object_->PutBoolean(skey, val);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool PutStringFuzz(FuzzedDataProvider &provider)
{
    if (objectStore_ == nullptr || object_ == nullptr) {
        return false;
    }
    std::string skey = provider.ConsumeRandomLengthString(10);
    std::string sval = provider.ConsumeRandomLengthString(10);
    object_->PutString(skey, sval);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool PutComplexFuzz(FuzzedDataProvider &provider)
{
    if (objectStore_ == nullptr || object_ == nullptr) {
        return false;
    }
    size_t sum = provider.ConsumeIntegralInRange<size_t>(0, 100);
    std::string skey = provider.ConsumeRandomLengthString(10);
    std::vector<uint8_t> value;
    for (size_t i = 0; i < sum; i++) {
        uint8_t val = 0;
        value.push_back(val);
    }
    object_->PutComplex(skey, value);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    OHOS::FuzzTestGetPermission();
    OHOS::objectStore_ = DistributedObjectStore::GetInstance(OHOS::BUNDLENAME);
    if (OHOS::objectStore_ != nullptr) {
        OHOS::object_ = OHOS::objectStore_->CreateObject(OHOS::SESSIONID);
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    (void)argc;
    (void)argv;
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    OHOS::PutDoubleFuzz(provider);
    OHOS::PutBooleanFuzz(provider);
    OHOS::PutStringFuzz(provider);
    OHOS::PutComplexFuzz(provider);
    /* Run your code on data */
    return 0;
}