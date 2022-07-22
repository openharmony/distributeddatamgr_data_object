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

#include <string>
#include <vector>
#include "distributed_object.h"
#include "distributed_objectstore.h"
#include "objectstore_errors.h"

using namespace OHOS::ObjectStore;

namespace OHOS {

bool PutDoubleFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    std::string bundleName = "com.example.myapplication";
    std::string sessionId = "123456";
    double sval = static_cast<double>(size);
    std::string skey(data, data + size);
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(bundleName);
    DistributedObject *object = objectStore->CreateObject(sessionId);
    uint32_t ret = object->PutDouble(skey, sval);
    if(!ret) {
        result = true;
    }
    objectStore->DeleteObject(sessionId);
    return result;
}

bool PutBooleanFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    std::string bundleName = "com.example.myapplication";
    std::string sessionId = "123456";
    std::string skey(data, data + size);
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(bundleName);
    DistributedObject *object = objectStore->CreateObject(sessionId);
    uint32_t ret = object->PutBoolean(skey, true);
    if(!ret) {
        result = true;
    }
    ret = object->PutBoolean(skey, false);
    if(ret) {
        result = false;
    }
    objectStore->DeleteObject(sessionId);
    return result;
}

bool PutStringFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    std::string bundleName = "com.example.myapplication";
    std::string sessionId = "123456";
    std::string skey(data, data + size);
    std::string sval(data, data + size);
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(bundleName);
    DistributedObject *object = objectStore->CreateObject(sessionId);
    uint32_t ret = object->PutString(skey, sval);
    if(!ret) {
        result = true;
    }
    objectStore->DeleteObject(sessionId);
    return result;
}

bool PutComplexFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    std::string bundleName = "com.example.myapplication";
    std::string sessionId = "123456";
    size_t sum = 10;
    std::string skey(data, data + size);
    std::vector<uint8_t> value;
    for (int i = 0; i < sum; i ++) {
        value.push_back(*data + i);
    }
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(bundleName);
    DistributedObject *object = objectStore->CreateObject(sessionId);
    uint32_t ret = object->PutComplex(skey, value);
    if(!ret) {
        result = true;
    }
    objectStore->DeleteObject(sessionId);
    return result;
}

bool GetDoubleFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    std::string bundleName = "com.example.myapplication";
    std::string sessionId = "123456";
    double sval = static_cast<double>(size);
    double val;
    std::string skey(data, data + size);
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(bundleName);
    DistributedObject *object = objectStore->CreateObject(sessionId);
    if(SUCCESS == object->PutDouble(skey, sval)) {
        uint32_t ret = object->GetDouble(skey, val);
        if(!ret) {
            result = true;
        }
    }
    objectStore->DeleteObject(sessionId);
    return result;
}

bool GetBooleanFuzz(const uint8_t *data, size_t size)
{
    bool val, result = false;
    std::string bundleName = "com.example.myapplication";
    std::string sessionId = "123456";
    std::string skey(data, data + size);
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(bundleName);
    DistributedObject *object = objectStore->CreateObject(sessionId);
    if(SUCCESS ==  object->PutBoolean(skey, true)) {
        uint32_t ret = object->GetBoolean(skey, val);
        if(!ret) {
            result = true;
        }
    }
    objectStore->DeleteObject(sessionId);
    return result;
}

bool GetStringFuzz(const uint8_t *data, size_t size)
{
    bool result= false;
    std::string bundleName = "com.example.myapplication";
    std::string sessionId = "123456";
    std::string skey(data, data + size);
    std::string sval(data, data + size);
    std::string val;
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(bundleName);
    DistributedObject *object = objectStore->CreateObject(sessionId);
    if(SUCCESS ==  object->PutString(skey, sval)) {
        uint32_t ret = object->GetString(skey, val);
        if(!ret) {
            result = true;
        }
    }
    objectStore->DeleteObject(sessionId);
    return result;
}

bool GetComplexFuzz(const uint8_t *data, size_t size)
{
    bool result = false;
    std::string bundleName = "com.example.myapplication";
    std::string sessionId = "123456";
    size_t sum = 10;
    std::string skey(data, data + size);
    std::vector<uint8_t> svalue;
    std::vector<uint8_t> val;
    for (int i = 0; i < sum; i ++) {
        svalue.push_back(*data + i);
    }
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(bundleName);
    DistributedObject *object = objectStore->CreateObject(sessionId);
    if(SUCCESS == object->PutComplex(skey, svalue)) {
        uint32_t ret = object->GetComplex(skey, val);
        if(!ret) {
            result = true;
        }
    }
    objectStore->DeleteObject(sessionId);
    return result;
}
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    OHOS::PutDoubleFuzz(data, size);
    OHOS::PutBooleanFuzz(data, size);
    OHOS::PutStringFuzz(data, size);
    OHOS::PutComplexFuzz(data, size);
    OHOS::GetDoubleFuzz(data, size);
    OHOS::GetBooleanFuzz(data, size);
    OHOS::GetStringFuzz(data, size);
    OHOS::GetComplexFuzz(data, size);
    /* Run your code on data */
    return 0;
}