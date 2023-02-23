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

#ifndef OBJECT_CALLBACK_STUB_H
#define OBJECT_CALLBACK_STUB_H
#include <iremote_broker.h>
#include <iremote_stub.h>
#include "object_callback.h"

namespace OHOS {
namespace DistributedObject {
class ObjectSaveCallbackBroker : public IObjectSaveCallback, public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DistributedObject.IObjectSaveCallback");
};

class ObjectSaveCallbackStub : public IRemoteStub<ObjectSaveCallbackBroker> {
public:
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
};

class ObjectRevokeSaveCallbackBroker : public IObjectRevokeSaveCallback, public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DistributedObject.IObjectRevokeSaveCallback");
};

class ObjectRevokeSaveCallbackStub : public IRemoteStub<ObjectRevokeSaveCallbackBroker> {
public:
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
};

class ObjectRetrieveCallbackBroker : public IObjectRetrieveCallback, public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DistributedObject.IObjectRetrieveCallback");
};

class ObjectRetrieveCallbackStub : public IRemoteStub<ObjectRetrieveCallbackBroker> {
public:
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
};

class ObjectChangeCallbackBroker : public IObjectChangeCallback, public IRemoteBroker {
public:
    DECLARE_INTERFACE_DESCRIPTOR(u"OHOS.DistributedObject.IObjectChangeCallback");
};

class ObjectChangeCallbackStub : public IRemoteStub<ObjectChangeCallbackBroker> {
public:
    int OnRemoteRequest(uint32_t code, MessageParcel &data, MessageParcel &reply, MessageOption &option) override;
};
} // namespace DistributedObject
} // namespace OHOS

#endif // OBJECT_CALLBACK_STUB_H
