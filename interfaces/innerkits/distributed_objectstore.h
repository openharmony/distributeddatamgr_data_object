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

#ifndef DISTRIBUTED_OBJECTSTORE_H
#define DISTRIBUTED_OBJECTSTORE_H
#include <memory>
#include <string>
#include <vector>
#include <functional>

#include "distributed_object.h"

namespace OHOS::ObjectStore {
class StatusNotifier {
public:
    virtual void OnChanged(
        const std::string &sessionId, const std::string &networkId, const std::string &onlineStatus) = 0;
};
class DistributedObjectStore {
public:
    virtual ~DistributedObjectStore(){};
    
    /**
     * @brief Get the instance to handle the object, such as create the object.
     *
     * @param bundleName Indicates the bundleName.
     *
     * @return Returns the pointer to the DistributedObjectStore class.
     */
    static DistributedObjectStore *GetInstance(const std::string &bundleName = "");
    
    /**
     * @brief Create a object according to the sessionId.
     *
     * @param sessionId Indicates the sessionId.
     *
     * @return Returns the pointer to the DistributedObject class.
     */
    virtual DistributedObject *CreateObject(const std::string &sessionId) = 0;
    
    /**
     * @brief Create a object according to the sessionId.
     *
     * @param sessionId Indicates the sessionId.
     * @param status Indicates whether the distributed object is created successfully,
     * 0 means success, other means fail.
     *
     * @return Returns the pointer to the DistributedObject class.
     */
    virtual DistributedObject *CreateObject(const std::string &sessionId, uint32_t &status) = 0;
    
    /**
     * @brief Get the double pointer to the object.
     *
     * @param sessionId Indicates the sessionId.
     * @param object Indicates the double pointer to the object.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t Get(const std::string &sessionId, DistributedObject **object) = 0;
    
    /**
     * @brief Delete the object according to the sessionId.
     *
     * @param sessionId Indicates the sessionId.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t DeleteObject(const std::string &sessionId) = 0;
    
    /**
     * @brief Set listening for data changes.
     *
     * @param object Indicates the pointer to the DistributedObject class.
     * @param objectWatcher Indicates callback function for data changes.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t Watch(DistributedObject *object, std::shared_ptr<ObjectWatcher> objectWatcher) = 0;
    
    /**
     * @brief Undo listening for data changes.
     *
     * @param object Indicates the pointer to the DistributedObject class.
     *
     * @return Returns the pointer to the DistributedObject class.
     */
    virtual uint32_t UnWatch(DistributedObject *object) = 0;
    
    /**
     * @brief Set listening for device online and offline .
     *
     * @param notifier Indicates callback function for device online ond offline.
     *
     * @return Returns 0 for success, others for failure.
     */
    virtual uint32_t SetStatusNotifier(std::shared_ptr<StatusNotifier> notifier) = 0;
    
    /**
     * @brief Notify the status of the local device from the cached callback function according to the sessionId.
     *
     * @param sessionId Indicates the sessionId.
     *
     */
    virtual void NotifyCachedStatus(const std::string &sessionId) = 0;
};
} // namespace OHOS::ObjectStore

#endif // DISTRIBUTED_OBJECTSTORE_H
