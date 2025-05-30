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

#ifndef DISTRIBUTEDDATAFWK_IOBJECT_SERVICE_H
#define DISTRIBUTEDDATAFWK_IOBJECT_SERVICE_H

#include "object_service.h"
#include "distributeddata_object_store_ipc_interface_code.h"

namespace OHOS::DistributedObject {
class IObjectService : public ObjectService, public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DistributedObject.IObjectService");
};

class IKvStoreDataService : public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DistributedKv.IKvStoreDataService");

    virtual sptr<IRemoteObject> GetFeatureInterface(const std::string &name) = 0;

    virtual uint32_t RegisterClientDeathObserver(const std::string &appId, sptr<IRemoteObject> observer) = 0;
};
} // namespace OHOS::DistributedObject
#endif
