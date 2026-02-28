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

#include "object_storage_engine_fuzzer.h"

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
static DistributedObjectStore *objectStore_ = nullptr;
constexpr const char *SESSIONID = "123456";
constexpr const char *TABLESESSIONID = "654321";
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

std::string FilterSessionId(const std::string &input)
{
    std::string result;
    std::copy_if(input.begin(), input.end(), std::back_inserter(result), [](char c) {
        // Reservations: Numbers (0-9), letters (a-z, A-Z), underscores (_)
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    });
    if (result.empty()) {
        result = "default_session_id";
    }
    return result;
}

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

bool GetFuzz(FuzzedDataProvider &provider)
{
    if (objectStore_ == nullptr) {
        return false;
    }
    uint32_t status = provider.ConsumeIntegral<uint32_t>();
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString(10));
    objectStore_->CreateObject(skey, status);
    DistributedObject *object = nullptr;
    objectStore_->Get(skey, &object);
    objectStore_->DeleteObject(skey);
    return true;
}

bool GetTableFuzz(FuzzedDataProvider &provider)
{
    std::string skey = provider.ConsumeRandomLengthString(10);
    std::shared_ptr<FlatObjectStorageEngine> storageEngine = std::make_shared<FlatObjectStorageEngine>();
    storageEngine->Open(BUNDLENAME);
    storageEngine->CreateTable(TABLESESSIONID);
    std::map<std::string, Value> tableResult;
    storageEngine->GetTable(skey, tableResult);
    storageEngine->DeleteTable(TABLESESSIONID);
    return true;
}

bool NotifyStatusAndNotifyChangeFuzz(FuzzedDataProvider &provider)
{
    std::shared_ptr<FlatObjectStorageEngine> storageEngine = std::make_shared<FlatObjectStorageEngine>();
    storageEngine->Open(BUNDLENAME);
    storageEngine->CreateTable(TABLESESSIONID);
    std::map<std::string, std::vector<uint8_t>> filteredData;
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString(10));
    storageEngine->NotifyChange(skey, filteredData);
    storageEngine->NotifyStatus(skey, skey, skey);
    storageEngine->DeleteTable(TABLESESSIONID);
    return true;
}

bool RegisterObserverAndUnRegisterObserverFuzz(FuzzedDataProvider &provider)
{
    std::shared_ptr<FlatObjectStorageEngine> storageEngine = std::make_shared<FlatObjectStorageEngine>();
    storageEngine->Open(BUNDLENAME);
    std::string skey = provider.ConsumeRandomLengthString(10);
    auto tableWatcherPtr = std::make_shared<TableWatcherImpl>(SESSIONID);
    storageEngine->RegisterObserver(skey, tableWatcherPtr);
    storageEngine->UnRegisterObserver(skey);
    return true;
}
} // namespace OHOS

extern "C" int LLVMFuzzerInitialize(int *argc, char ***argv)
{
    OHOS::FuzzTestGetPermission();
    OHOS::objectStore_ = DistributedObjectStore::GetInstance("com.example.myapplication");
    std::this_thread::sleep_for(std::chrono::seconds(1));
    (void)argc;
    (void)argv;
    return 0;
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    OHOS::GetFuzz(provider);
    OHOS::GetTableFuzz(provider);
    OHOS::NotifyStatusAndNotifyChangeFuzz(provider);
    OHOS::RegisterObserverAndUnRegisterObserverFuzz(provider);
    /* Run your code on data */
    return 0;
}