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

#ifndef DISTRIBUTED_OBJECT_H
#define DISTRIBUTED_OBJECT_H
#include <map>
#include <memory>
#include <string>
#include <vector>
#include "object_types.h"

namespace OHOS::ObjectStore {
enum Type : uint8_t {
    TYPE_STRING = 0,
    TYPE_BOOLEAN,
    TYPE_DOUBLE,
    TYPE_COMPLEX,
};
class DistributedObject {
public:
    virtual ~DistributedObject(){};
    
    /**
     * @brief Put or update the data whose value type is double into the database, which means that the data of
     * objects in the same sessionId is put or updated.
     *
     * @param key Indicates the key of key-value data to put or update.
     * @param value Indicates the value of key-value data to put or update.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t PutDouble(const std::string &key, double value) = 0;
    
    /**
     * @brief Put or update the data whose value type is bool into the database, which means that the data of
     * objects in the same sessionId is put or updated.
     *
     * @param key Indicates the key of key-value data to put or update.
     * @param value Indicates the value of key-value data to put or update.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t PutBoolean(const std::string &key, bool value) = 0;
    
    /**
     * @brief Put or update the data whose value type is string into the database, which means that the data of
     * objects in the same sessionId is put or updated.
     *
     * @param key Indicates the key of key-value data to put or update.
     * @param value Indicates the value of key-value data to put or update.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t PutString(const std::string &key, const std::string &value) = 0;
    
    /**
     * @brief Put or update the data whose value type is bytes stream into the database, which means that the data of
     * objects in the same sessionId is put or updated.
     *
     * @param key Indicates the key of key-value data to put or update.
     * @param value Indicates the value of key-value data to put or update.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t PutComplex(const std::string &key, const std::vector<uint8_t> &value) = 0;
    
    /**
     * @brief Get the data whose value type is double from the database according to the key,
     * which means that the data of objects in the same sessionId is get.
     *
     * @param key Indicates the key of key-value data to put or update.
     * @param value Indicates the value of key-value data to put or update.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t GetDouble(const std::string &key, double &value) = 0;
    
    /**
     * @brief Get the data whose value type is bool from the database according to the key,
     * which means that the data of objects in the same sessionId is get.
     *
     * @param key Indicates the key of key-value data to get.
     * @param value Indicates the value of key-value data to get.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t GetBoolean(const std::string &key, bool &value) = 0;
    
    /**
     * @brief Get the data whose value type is string from the database according to the key,
     * which means that the data of objects in the same sessionId is get.
     *
     * @param key Indicates the key of key-value data to put or update.
     * @param value Indicates the value of key-value data to put or update.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t GetString(const std::string &key, std::string &value) = 0;
    
    /**
     * @brief Get the data whose value type is complex from the database according to the key,
     * which means that the data of objects in the same sessionId is get.
     *
     * @param key Indicates the key of key-value data to put or update.
     * @param value Indicates the value of key-value data to put or update.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t GetComplex(const std::string &key, std::vector<uint8_t> &value) = 0;
    
    /**
     * @brief Get the value type of key-value data by the key
     *
     * @param key Indicates the key of key-value data.
     * @param value Indicates the value of key-value data.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t GetType(const std::string &key, Type &type) = 0;
    
    /**
     * @brief Save the data to local device.
     *
     * @param deviceId Indicates the device Id.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t Save(const std::string &deviceId) = 0;
    
    /**
     * @brief Revoke save data.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t RevokeSave() = 0;
    
    /**
     * @brief Get the sessionId of the object.
     *
     * @return Returns sessionId of the object.
     */
    virtual std::string &GetSessionId() = 0;

    /**
     * @brief Bind Asset.
     *
     * @param assetKey Indicates the assetKey key.
     * @param bindInfo Indicates asset info.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t BindAssetStore(const std::string &assetKey, AssetBindInfo &bindInfo) = 0;
};

class ObjectWatcher {
public:
    virtual void OnChanged(const std::string &sessionid, const std::vector<std::string> &changedData) = 0;
};
} // namespace OHOS::ObjectStore
#endif // DISTRIBUTED_OBJECT_H
