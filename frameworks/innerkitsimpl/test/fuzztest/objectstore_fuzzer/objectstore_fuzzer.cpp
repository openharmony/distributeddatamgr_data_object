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

#include "objectstore_fuzzer.h"

#include <cstdint>
#include <string>
#include <vector>
#include <fuzzer/FuzzedDataProvider.h>
#include "distributed_object.h"
#include "distributed_objectstore.h"
#include "flat_object_storage_engine.h"
#include "objectstore_errors.h"

using namespace OHOS::ObjectStore;
namespace OHOS {
static DistributedObject *object_ = nullptr;
static DistributedObjectStore *objectStore_ = nullptr;
constexpr const char *SESSIONID = "123456";
constexpr const char *TABLESESSIONID = "654321";
constexpr const char *BUNDLENAME = "com.example.myapplication";
class TableWatcherImpl : public TableWatcher {
public:
    explicit TableWatcherImpl(const std::string &sessionId) : TableWatcher(sessionId) {}
    void OnChanged(
        const std::string &sessionid, const std::vector<std::string> &changedData, bool enableTransfer) override;
    virtual ~TableWatcherImpl();
};
TableWatcherImpl::~TableWatcherImpl() {}
void TableWatcherImpl::OnChanged(
    const std::string &sessionid, const std::vector<std::string> &changedData, bool enableTransfer) {}

class TestObjectWatcher : public ObjectWatcher {
public:
    void OnChanged(const std::string &sessionid, const std::vector<std::string> &changedData) override {

    }
    virtual ~TestObjectWatcher(){}
};

class TestStatusNotifier : public StatusNotifier {
public:
    void OnChanged(
        const std::string &sessionId, const std::string &networkId, const std::string &onlineStatus) override {

    }
    virtual ~TestStatusNotifier(){}
};

std::string FilterSessionId(const std::string& input) {
    std::string result;
    std::copy_if(input.begin(), input.end(), std::back_inserter(result),
        [](char c) {
            // 保留：数字(0-9)、字母(a-z,A-Z)、下划线(_)
            return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
        });
    return result;
}

uint32_t SetUpTestCase()
{
    std::string bundleName = BUNDLENAME;
    DistributedObjectStore *objectStore = nullptr;
    DistributedObject *object = nullptr;
    objectStore = DistributedObjectStore::GetInstance(bundleName);
    if (objectStore != nullptr) {
        objectStore_ = objectStore;
        object = objectStore_->CreateObject(SESSIONID);
        if (object != nullptr) {
            object_ = object;
            return SUCCESS;
        } else {
            return ERR_EXIST;
        }
    } else {
        return ERR_EXIST;
    }
}

bool PutDoubleFuzz(FuzzedDataProvider &provider)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    double sval = provider.ConsumeFloatingPoint<double>();
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    object_->PutDouble(skey, sval);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool PutBooleanFuzz(FuzzedDataProvider &provider)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    bool val = provider.ConsumeBool();
    object_->PutBoolean(skey, val);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool PutStringFuzz(FuzzedDataProvider &provider)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    std::string sval = provider.ConsumeRandomLengthString();
    object_->PutString(skey, sval);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool PutComplexFuzz(FuzzedDataProvider &provider)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    size_t sum = provider.ConsumeIntegralInRange<size_t>(0, 10);
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    std::vector<uint8_t> value;
    for (size_t i = 0; i < sum; i++) {
        uint8_t val = provider.ConsumeIntegral<uint8_t>();
        value.push_back(val);
    }
    object_->PutComplex(skey, value);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool GetDoubleFuzz(FuzzedDataProvider &provider)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    double sval = provider.ConsumeFloatingPoint<double>();
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    object_->PutDouble(skey, sval);
    double val = 0;
    object_->GetDouble(skey, val);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool GetBooleanFuzz(FuzzedDataProvider &provider)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    bool value = provider.ConsumeBool();
    object_->PutBoolean(skey, value);
    bool val = false;
    object_->GetBoolean(skey, val);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool GetStringFuzz(FuzzedDataProvider &provider)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    std::string sval = provider.ConsumeRandomLengthString();
    std::string val;
    object_->PutString(skey, sval);
    object_->GetString(skey, val);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool GetComplexFuzz(FuzzedDataProvider &provider)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }

    size_t sum = provider.ConsumeIntegralInRange<size_t>(0, 10);
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    std::vector<uint8_t> svalue;
    std::vector<uint8_t> val;
    for (size_t i = 0; i < sum; i++) {
        uint8_t val = provider.ConsumeIntegral<uint8_t>();
        svalue.push_back(val);
    }
    object_->PutComplex(skey, svalue);
    object_->GetComplex(skey, val);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool GetTypeFuzz(FuzzedDataProvider &provider)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    Type val;
    object_->GetType(skey, val);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool SaveFuzz(FuzzedDataProvider &provider)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    object_->Save(skey);
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool SaveAndRevokeSaveFuzz(FuzzedDataProvider &provider)
{
    if (SUCCESS != SetUpTestCase()) {
        return false;
    }
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    double sval = provider.ConsumeFloatingPoint<double>();
    object_->PutDouble(skey, sval);
    object_->Save(skey);
    object_->RevokeSave();
    objectStore_->DeleteObject(SESSIONID);
    return true;
}

bool CreateObjectV9Fuzz(FuzzedDataProvider &provider)
{
    std::string bundleName = provider.ConsumeRandomLengthString();
    objectStore_ = DistributedObjectStore::GetInstance(bundleName);
    if (objectStore_ == nullptr) {
        return false;
    }
    uint32_t status = provider.ConsumeIntegral<uint32_t>();
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    auto object = objectStore_->CreateObject(skey, status);
    if (object != nullptr) {
        double val = provider.ConsumeFloatingPoint<double>();
        object->PutDouble(skey, val);
        double getResult = 0;
        object->GetDouble(skey, getResult);
    }
    objectStore_->DeleteObject(skey);
    return true;
}

bool GetFuzz(FuzzedDataProvider &provider)
{
    std::string bundleName = provider.ConsumeRandomLengthString();
    objectStore_ = DistributedObjectStore::GetInstance(bundleName);
    if (objectStore_ == nullptr) {
        return false;
    }
    uint32_t status = provider.ConsumeIntegral<uint32_t>();
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    objectStore_->CreateObject(skey, status);
    DistributedObject *object = nullptr;
    objectStore_->Get(skey, &object);
    objectStore_->DeleteObject(skey);
    return true;
}

bool GetTableFuzz(FuzzedDataProvider &provider)
{
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
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
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    storageEngine->NotifyChange(skey, filteredData);
    storageEngine->NotifyStatus(skey, skey, skey);
    storageEngine->DeleteTable(TABLESESSIONID);
    return true;
}

bool RegisterObserverAndUnRegisterObserverFuzz(FuzzedDataProvider &provider)
{
    std::shared_ptr<FlatObjectStorageEngine> storageEngine = std::make_shared<FlatObjectStorageEngine>();
    storageEngine->Open(BUNDLENAME);
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    auto tableWatcherPtr = std::make_shared<TableWatcherImpl>(SESSIONID);
    storageEngine->RegisterObserver(skey, tableWatcherPtr);
    storageEngine->UnRegisterObserver(skey);
    return true;
}

bool BindAssetStoreFuzz(FuzzedDataProvider &provider)
{
    std::string bundleName = provider.ConsumeRandomLengthString();
    objectStore_ = DistributedObjectStore::GetInstance(bundleName);
    if (objectStore_ == nullptr) {
        return false;
    }
    uint32_t status = provider.ConsumeIntegral<uint32_t>();
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    auto object = objectStore_->CreateObject(skey, status);
    if (object == nullptr) {
        return false;
    }
    AssetBindInfo bindInfo = {
        .storeName = provider.ConsumeRandomLengthString(),
        .tableName = provider.ConsumeRandomLengthString(),
        .primaryKey = {
            {"data1", 123},
            {"data2", "test1"}
        },
        .field = provider.ConsumeRandomLengthString(),
        .assetName = provider.ConsumeRandomLengthString()
    };
    object->BindAssetStore(skey, bindInfo);
    objectStore_->DeleteObject(skey);
    return true;
}

bool GetSessionIdFuzz(FuzzedDataProvider &provider)
{
    std::string bundleName = provider.ConsumeRandomLengthString();
    objectStore_ = DistributedObjectStore::GetInstance(bundleName);
    if (objectStore_ == nullptr) {
        return false;
    }
    uint32_t status = provider.ConsumeIntegral<uint32_t>();
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    auto object = objectStore_->CreateObject(skey, status);
    if (object != nullptr) {
        object->GetSessionId();
    }
    objectStore_->DeleteObject(skey);
    return true;
}

bool WatchAndUnWatchFuzz(FuzzedDataProvider &provider)
{
    std::string bundleName = provider.ConsumeRandomLengthString();
    objectStore_ = DistributedObjectStore::GetInstance(bundleName);
    if (objectStore_ == nullptr) {
        return false;
    }
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    auto object = objectStore_->CreateObject(skey);
    if (object == nullptr) {
        return false;
    }
    objectStore_->Watch(object, std::make_shared<TestObjectWatcher>());
    double sval = provider.ConsumeFloatingPoint<double>();
    object->PutDouble(skey, sval);
    objectStore_->UnWatch(object);
    objectStore_->DeleteObject(skey);
    return true;
}

bool StatusNotifierFuzz(FuzzedDataProvider &provider)
{
    std::string bundleName = provider.ConsumeRandomLengthString();
    objectStore_ = DistributedObjectStore::GetInstance(bundleName);
    if (objectStore_ == nullptr) {
        return false;
    }
    std::string skey = FilterSessionId(provider.ConsumeRandomLengthString());
    auto object = objectStore_->CreateObject(skey);
    if (object == nullptr) {
        return false;
    }
    objectStore_->SetStatusNotifier(std::make_shared<TestStatusNotifier>());
    objectStore_->NotifyCachedStatus(skey);
    objectStore_->DeleteObject(skey);
    return true;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    FuzzedDataProvider provider(data, size);
    OHOS::PutDoubleFuzz(provider);
    OHOS::PutBooleanFuzz(provider);
    OHOS::PutStringFuzz(provider);
    OHOS::PutComplexFuzz(provider);
    OHOS::GetDoubleFuzz(provider);
    OHOS::GetBooleanFuzz(provider);
    OHOS::GetStringFuzz(provider);
    OHOS::GetComplexFuzz(provider);
    OHOS::GetTypeFuzz(provider);
    OHOS::SaveFuzz(provider);
    OHOS::SaveAndRevokeSaveFuzz(provider);
    OHOS::CreateObjectV9Fuzz(provider);
    OHOS::GetFuzz(provider);
    OHOS::GetTableFuzz(provider);
    OHOS::NotifyStatusAndNotifyChangeFuzz(provider);
    OHOS::RegisterObserverAndUnRegisterObserverFuzz(provider);
    OHOS::BindAssetStoreFuzz(provider);
    OHOS::GetSessionIdFuzz(provider);
    OHOS::WatchAndUnWatchFuzz(provider);
    OHOS::StatusNotifierFuzz(provider);
    /* Run your code on data */
    return 0;
}