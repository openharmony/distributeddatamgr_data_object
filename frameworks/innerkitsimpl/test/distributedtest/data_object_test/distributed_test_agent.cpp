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

#include <gtest/gtest.h>

#include <string>
#include <thread>
#include "distributed_object.h"
#include "distributed_objectstore.h"
#include "objectstore_errors.h"
#include "hilog/log.h"
#include "distributed_agent.h"
#include "ipc_skeleton.h"
#include "accesstoken_kit.h"
#include "token_setproc.h"
#include "softbus_adapter.h"

using namespace testing;
using namespace testing::ext;
using namespace OHOS;
using namespace OHOS::DistributeSystemTest;
using namespace OHOS::HiviewDFX;
using namespace OHOS::ObjectStore;
using namespace OHOS::Security::AccessToken;

namespace {
constexpr HiLogLabel LABEL = {LOG_CORE, 0, "DistributedTestAgent"};
const std::string DISTRIBUTED_DATASYNC = "ohos.permission.DISTRIBUTED_DATASYNC";
const std::string BUNDLENAME = "com.example.myapplication";
const std::string SESSIONID = "123456";
constexpr int MAX_RETRY_TIMES = 15;

class DistributedTestAgent : public DistributedAgent {
public:
    DistributedTestAgent();
    ~DistributedTestAgent();
    virtual bool SetUp();
    virtual bool TearDown();
    virtual int OnProcessMsg(const std::string &strMsg, int len, std::string &strReturnValue, int returnBufL);
    int ProcessMsg(const std::string &strMsg, std::string &strReturnValue);
    int RecallMessage(const std::string &strMsg, std::string &strReturnValue);
    int PutItem(const std::string &strMsg, std::string &strReturnValue);
    int GetItem(const std::string &strMsg, std::string &strReturnValue);
    int RevokeSave(const std::string &strMsg, std::string &strReturnValue);
    int DestroyObject(const std::string &strMsg, std::string &strReturnValue);
    static DistributedObjectStore *object_; 
private:
    using MsgFunc = int (DistributedTestAgent::*)(const std::string &, std::string &);
    std::map<std::string, MsgFunc> msgFunMap_;
};

class ObjectWatcherImpl : public ObjectWatcher {
public:
    bool GetDataStatus();
    void OnChanged(const std::string &sessionid, const std::vector<std::string> &changedData) override;
    virtual ~ObjectWatcherImpl();
private:
    bool dataChanged_ = false;
};

class StatusNotifierImpl : public StatusNotifier {
public:
    std::string GetOnlineStatus();
    void OnChanged(
        const std::string &sessionId, const std::string &networkId, const std::string &onlineStatus) override;
    virtual ~StatusNotifierImpl();
private:
    std::string onlineStatus_ = "offline";
};

ObjectWatcherImpl::~ObjectWatcherImpl()
{
}

bool ObjectWatcherImpl::GetDataStatus()
{
    return dataChanged_;
}

void ObjectWatcherImpl::OnChanged(const std::string &sessionid, const std::vector<std::string> &changedData)
{
    if (changedData.empty()) {
        HiLog::Info(LABEL, "empty change");
        return;
    }
    HiLog::Info(LABEL, "start %{public}s, %{public}s", sessionid.c_str(), changedData.at(0).c_str());
    dataChanged_ = true;
}

StatusNotifierImpl::~StatusNotifierImpl()
{
}

std::string StatusNotifierImpl::GetOnlineStatus()
{
    return onlineStatus_;
}

void StatusNotifierImpl::OnChanged(const std::string &sessionId, const std::string &networkId, const std::string &onlineStatus)
{
    HiLog::Info(
        LABEL,"status changed %{public}s %{public}s %{public}s", sessionId.c_str(), networkId.c_str(), onlineStatus.c_str());
    onlineStatus_ = onlineStatus;
}

DistributedObjectStore *DistributedTestAgent::object_ = nullptr;

DistributedTestAgent::DistributedTestAgent()
{
}

DistributedTestAgent::~DistributedTestAgent()
{  
}

bool DistributedTestAgent::SetUp()
{
    msgFunMap_["recall"] = &DistributedTestAgent::RecallMessage;
    msgFunMap_["PutItem"] = &DistributedTestAgent::PutItem;
    msgFunMap_["GetItem"] = &DistributedTestAgent::GetItem;
    msgFunMap_["RevokeSave"] = &DistributedTestAgent::RevokeSave;
    msgFunMap_["DestroyObject"] = &DistributedTestAgent::DestroyObject;
    return true;
}

bool DistributedTestAgent::TearDown()
{
    return true;
}

int DistributedTestAgent::OnProcessMsg(const std::string &strMsg, int len, std::string &strReturnValue, int returnBufL)
{
    return DistributedTestAgent::ProcessMsg(strMsg, strReturnValue);
}

int DistributedTestAgent::ProcessMsg(const std::string &strMsg, std::string &strReturnValue)
{
    int index = strMsg.find(",");
    std::string argsMsg = strMsg.substr(0, index);
    std::map<std::string, MsgFunc>::iterator it = msgFunMap_.find(argsMsg);
    if (it != msgFunMap_.end()) {
        MsgFunc MsgFunc = msgFunMap_[argsMsg];
        return (this->*MsgFunc)(strMsg, strReturnValue);
    }
    return -1;
}

void GrantPermission(const std::string &appId, std::string permissionName)
{
    HapInfoParams hapInfoParams = {
        .userID = 1,
        .bundleName = appId,
        .instIndex = 0,
        .appIDDesc = "app need sync permission"
    };
    PermissionDef permissionDef = {
        .permissionName = permissionName,
        .bundleName = appId,
        .grantMode = 1,
        .availableLevel = ATokenAplEnum::APL_NORMAL,
        .label = "label",
        .labelId = 1,
        .description = "permission define",
        .descriptionId = 1
    };
    PermissionStateFull permissionStateFull = {
        .permissionName = permissionName,
        .isGeneral = true,
        .resDeviceID = { "local" },
        .grantStatus = { PermissionState::PERMISSION_GRANTED },
        .grantFlags = { 1 }
    };
    HapPolicyParams hapPolicyParams = {
        .apl = ATokenAplEnum::APL_NORMAL,
        .domain = "test.domain",
        .permList = { permissionDef },
        .permStateList = { permissionStateFull }
    };
    AccessTokenIDEx tokenIdEx = AccessTokenKit::AllocHapToken(hapInfoParams,hapPolicyParams);
    if (tokenIdEx.tokenIdExStruct.tokenID == 0) {
        unsigned int tokenIdOld = 0;
        tokenIdOld = AccessTokenKit::GetHapTokenID(hapInfoParams.userID, hapInfoParams.bundleName,
                                                                hapInfoParams.instIndex);
        if (tokenIdOld == 0) {
            return;
        }
        int32_t ret = AccessTokenKit::DeleteToken(tokenIdOld);
        if (ret != 0) {
            return;
        }
        tokenIdEx = AccessTokenKit::AllocHapToken(hapInfoParams, hapPolicyParams);
        if (tokenIdEx.tokenIdExStruct.tokenID == 0) {
            return;
        }
    }
    SetSelfTokenID(tokenIdEx.tokenIdExStruct.tokenID);
    AccessTokenKit::GrantPermission(tokenIdEx.tokenIdExStruct.tokenID, permissionName,PERMISSION_USER_FIXED);
}

int DistributedTestAgent::RecallMessage(const std::string &strMsg, std::string &strReturnValue)
{
    strReturnValue = "recall Message";
    return strReturnValue.size();
}

int DistributedTestAgent::PutItem(const std::string &strMsg, std::string &strReturnValue)
{
    GrantPermission(BUNDLENAME, DISTRIBUTED_DATASYNC);
    std::string sessionId = SESSIONID;
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(BUNDLENAME);
    if ( objectStore != nullptr)
    {
        DistributedObject *object = objectStore->CreateObject(sessionId);
        DistributedTestAgent::object_ = objectStore;
        auto notifierPtr = std::make_shared<StatusNotifierImpl>();
        objectStore->SetStatusNotifier(notifierPtr);
        int times = 0;
        while (times < MAX_RETRY_TIMES && notifierPtr->GetOnlineStatus() != "online")
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
            times++;
        }    

        if (notifierPtr->GetOnlineStatus() != "online")
        {
            return -1;
        }
        
        uint32_t status = object->PutDouble("salary", 100.5);
        if (status != SUCCESS) {
            return -1;
        }
        strReturnValue = "PutSuccsess";
    }
    return strReturnValue.size();
}

int DistributedTestAgent::GetItem(const std::string &strMsg, std::string &strReturnValue)
{
    GrantPermission(BUNDLENAME, DISTRIBUTED_DATASYNC);
    std::string sessionId = SESSIONID;
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(BUNDLENAME);
    if ( objectStore != nullptr)
    {
        DistributedObject *object = objectStore->CreateObject(sessionId);
        DistributedTestAgent::object_ = objectStore;

        auto watcherPtr = std::make_shared<ObjectWatcherImpl>();
        objectStore->Watch(object, watcherPtr);
        int times = 0;
        while (times < MAX_RETRY_TIMES && !watcherPtr->GetDataStatus())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(200)); 
            times++;
        }           
        std::string Getvalue ="GetItem";        
        uint32_t status = object->GetString("name", Getvalue);
        if (status != SUCCESS) {
            return -1;
        }
        objectStore->UnWatch(object);
        strReturnValue = Getvalue;
    }
    return strReturnValue.size();
}

int DistributedTestAgent::RevokeSave(const std::string &strMsg, std::string &strReturnValue)
{
    GrantPermission(BUNDLENAME, DISTRIBUTED_DATASYNC);
    std::string sessionId = SESSIONID;
    std::string putValue = "zhangsan";
    DistributedObjectStore *objectStore = DistributedObjectStore::GetInstance(BUNDLENAME);
    if (objectStore != nullptr)
    {
        DistributedObject *object = objectStore->CreateObject(sessionId);
        if (object != nullptr)
        {
            object->PutString("name", putValue);
            std::vector<ObjectStore::DeviceInfo> devices = SoftBusAdapter::GetInstance()->GetDeviceList();
            std::vector<std::string> deviceIds;
            for (auto item : devices) {
                deviceIds.push_back(item.deviceId);
            }
            std::string networkId = SoftBusAdapter::GetInstance()->ToNodeID(deviceIds[0]);
            object->Save(networkId);
            uint32_t status = object->RevokeSave();
            if (status != SUCCESS) {
                strReturnValue = "RevokeSave failed.";
                return -1;
            }
            objectStore->DeleteObject(sessionId);
        }
    }
    strReturnValue = putValue;
    return strReturnValue.size();
}

int DistributedTestAgent::DestroyObject(const std::string &strMsg, std::string &strReturnValue)
{
    if ( DistributedTestAgent::object_ != nullptr)
    {
        DistributedTestAgent::object_->DeleteObject(SESSIONID);
        DistributedTestAgent::object_ = nullptr;
        strReturnValue = "DestroyObjectDone";
    }
    return strReturnValue.size();
}

}
int main()
{
    DistributedTestAgent obj;
    if (obj.SetUp()) {
        obj.Start("agent.desc");
        obj.Join();
    } else {
        HiLog::Error(LABEL, "Init environment failed.");
    }
    if (obj.TearDown()) {
        return 0;
    } else {
        HiLog::Error(LABEL, "Clear environment failed.");
        return -1;
    }
}
