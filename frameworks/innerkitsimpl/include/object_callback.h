/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#ifndef OBJECT_CALLBACK_H
#define OBJECT_CALLBACK_H

#include <map>
#include <string>
#include <vector>

namespace OHOS {
namespace DistributedObject {
enum {
    COMPLETED = 0,
};

class IObjectSaveCallback {
public:
    virtual void Completed(const std::map<std::string, int32_t> &results) = 0;
};

class IObjectRevokeSaveCallback {
public:
    virtual void Completed(int32_t status) = 0;
};

class IObjectRetrieveCallback {
public:
    virtual void Completed(const std::map<std::string, std::vector<uint8_t>> &results) = 0;
};

class IObjectChangeCallback {
public:
    virtual void Completed(const std::map<std::string, std::vector<uint8_t>> &results) = 0;
};
} // namespace DistributedObject
} // namespace OHOS

#endif // OBJECT_CALLBACK_H
