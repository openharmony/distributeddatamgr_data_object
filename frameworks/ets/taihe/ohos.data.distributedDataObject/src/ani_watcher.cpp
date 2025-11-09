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

#include "ohos.data.distributedDataObject.ani.hpp"
#include "ohos.data.distributedDataObject.DataObject.hpp"
#include <thread>

#include "anonymous.h"
#include "ani_notifier_impl.h"
#include "ani_progress_notifier_impl.h"
#include "ani_watcher.h"
#include "logger.h"
#include "objectstore_errors.h"

namespace OHOS::ObjectStore {

static const int32_t ANI_SCOPE_SIZE = 16;
std::shared_ptr<OHOS::AppExecFwk::EventHandler> AsyncUtilBase::mainHandler_;

bool AsyncUtilBase::AsyncExecueInMainThread(const std::function<void()> func)
{
    if (func == nullptr) {
        return false;
    }
    if (!mainHandler_) {
        std::shared_ptr<OHOS::AppExecFwk::EventRunner> runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
        if (!runner) {
            LOG_ERROR("GetMainEventRunner failed");
            return false;
        }
        mainHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
    }
    mainHandler_->PostTask(func, "", 0, OHOS::AppExecFwk::EventQueue::Priority::IMMEDIATE, {});
    return true;
}

bool AsyncUtilBase::AsyncExecue(const std::function<void()> func)
{
    std::thread t(func);
    t.detach();
    return true;
}

AniWatcher::AniWatcher(DistributedObjectStore *objectStore, DistributedObject *object)
{
}

AniWatcher::~AniWatcher()
{
    if (changeEventListener_) {
        delete changeEventListener_;
        changeEventListener_ = nullptr;
    }
    if (statusEventListener_) {
        delete statusEventListener_;
        statusEventListener_ = nullptr;
    }
    if (progressEventListener_) {
        delete progressEventListener_;
        progressEventListener_ = nullptr;
    }
}

bool AniWatcher::On(std::string type, VarCallbackType handler)
{
    if (std::holds_alternative<std::monostate>(handler)) {
        LOG_WARN("VarCallbackType empty");
        return false;
    }
    EventListener *listener = Find(type);
    if (listener == nullptr) {
        LOG_ERROR("listener null, type %{public}s", type.c_str());
        return false;
    }
    return listener->Add(handler);
}

void AniWatcher::Off(std::string type, VarCallbackType handler)
{
    EventListener *listener = Find(type);
    if (listener == nullptr) {
        LOG_ERROR("error type %{public}s", type.c_str());
        return;
    }
    if (std::holds_alternative<std::monostate>(handler)) {
        listener->Clear();
    } else {
        listener->Del(handler);
    }
}

EventListener *AniWatcher::Find(std::string type)
{
    if (type.empty()) {
        LOG_ERROR("type null");
        return nullptr;
    }
    if (EVENT_CHANGE == type) {
        return changeEventListener_;
    }
    if (EVENT_STATUS == type) {
        return statusEventListener_;
    }
    if (EVENT_PROGRESS == type) {
        return progressEventListener_;
    }
    return nullptr;
}

void AniWatcher::Emit(std::string type, const std::string &sessionId, const std::vector<std::string> &changeData)
{
    if (changeData.empty()) {
        LOG_ERROR("empty change");
        return;
    }
    LOG_INFO("start %{public}s, %{public}s", sessionId.c_str(), changeData.at(0).c_str());
    EventListener *listener = Find(type);
    if (listener == nullptr) {
        LOG_ERROR("error type %{public}s", type.c_str());
        return;
    }

    {
        std::unique_lock<std::shared_mutex> cacheLock(cachedChangeEventMutex_);
        for (EventHandler *handler = listener->handlers_; handler != nullptr; handler = handler->next) {
            AniChangeCallbackType* pval = std::get_if<AniChangeCallbackType>(&handler->callbackVar);
            if (pval != nullptr) {
                auto changeArgs = std::make_shared<ChangeArgs>(*pval, sessionId, changeData);
                cachedChangeEventList_.emplace_back(changeArgs);
            }
        }
    }
    auto sharedThis = shared_from_this();
    AsyncUtilBase::AsyncExecueInMainThread( [ sharedThis ] {
        LOG_INFO("AniWatcher::Emit, change, AsyncExecueInMainThread!");
        ani_env *aniEnv = taihe::get_env();
        if (aniEnv == nullptr) {
            LOG_ERROR("aniEnv null, clear");
            sharedThis->cachedChangeEventList_.clear();
            return;
        }
        ani_status localScopeStatus = aniEnv->CreateLocalScope(ANI_SCOPE_SIZE);
        if (localScopeStatus != ANI_OK) {
            LOG_WARN("Failed to creat local scope");
        }
        std::unique_lock<std::shared_mutex> cacheLock(sharedThis->cachedChangeEventMutex_);
        for (auto &itemPtr : sharedThis->cachedChangeEventList_) {
            if (itemPtr == nullptr) {
                continue;
            }
            auto taiheStringList = ::taihe::array<::taihe::string>(
                ::taihe::copy_data_t{}, itemPtr->changeData_.data(), itemPtr->changeData_.size());
            auto taiheSessionId = ::taihe::string(itemPtr->sessionId_);
            LOG_INFO("AniWatcher::Emit, change, AsyncExecueInMainThread! exec");
            itemPtr->callback_(taiheSessionId, taiheStringList);
        }
        sharedThis->cachedChangeEventList_.clear();

        if (localScopeStatus == ANI_OK) {
            aniEnv->DestroyLocalScope();
        }
    });
}

void AniWatcher::Emit(
    std::string type, const std::string &sessionId, const std::string &networkId, const std::string &status)
{
    LOG_INFO("xxx AniWatcher::Emit 1");

    if (sessionId.empty() || networkId.empty()) {
        LOG_ERROR("empty %{public}s  %{public}s", sessionId.c_str(), Anonymous::Change(networkId).c_str());
        return;
    }
    LOG_INFO("xxx AniWatcher::Emit 2");
    LOG_INFO("status change %{public}s  %{public}s", sessionId.c_str(), Anonymous::Change(networkId).c_str());
    EventListener *listener = Find(type);
    if (listener == nullptr) {
        LOG_ERROR("error type %{public}s", type.c_str());
        return;
    }

    {
        std::unique_lock<std::shared_mutex> cacheLock(cachedStatusEventMutex_);
        for (EventHandler *handler = listener->handlers_; handler != nullptr; handler = handler->next) {
            AniStatusCallbackType* pval = std::get_if<AniStatusCallbackType>(&handler->callbackVar);
            if (pval != nullptr) {
                auto statusArgs = std::make_shared<StatusArgs>(*pval, sessionId, networkId, status);
                cachedStatusEventList_.emplace_back(statusArgs);
            }
        }
    }

    LOG_INFO("xxx AniWatcher::Emit AsyncExecueInMainThread");
    auto sharedThis = shared_from_this();
    AsyncUtilBase::AsyncExecueInMainThread([sharedThis] {
        LOG_INFO("AniWatcher::Emit, status, AsyncExecueInMainThread!");
        ani_env *aniEnv = taihe::get_env();
        if (aniEnv == nullptr) {
            LOG_ERROR("aniEnv null, clear");
            sharedThis->cachedStatusEventList_.clear();
            return;
        }
        ani_status localScopeStatus = aniEnv->CreateLocalScope(ANI_SCOPE_SIZE);
        if (localScopeStatus != ANI_OK) {
            LOG_WARN("Failed to creat local scope");
        }
        std::unique_lock<std::shared_mutex> cacheLock(sharedThis->cachedStatusEventMutex_);
        for (auto &itemPtr : sharedThis->cachedStatusEventList_) {
            if (itemPtr == nullptr) {
                continue;
            }
            auto taiheSessionId = ::taihe::string(itemPtr->sessionId_);
            auto taiheNetworkId = ::taihe::string(itemPtr->networkId_);
            auto taiheStatus = ::taihe::string(itemPtr->status_);
            itemPtr->callback_(taiheSessionId, taiheNetworkId, taiheStatus);
        }
        sharedThis->cachedStatusEventList_.clear();

        if (localScopeStatus == ANI_OK) {
            aniEnv->DestroyLocalScope();
        }
    });
}

void AniWatcher::Emit(
    std::string type, const std::string &sessionId, const int32_t progress)
{
    if (sessionId.empty()) {
        LOG_ERROR("empty %{public}s", sessionId.c_str());
        return;
    }
    LOG_INFO("progress change %{public}s  %{public}d", sessionId.c_str(), progress);
    EventListener *listener = Find(type);
    if (listener == nullptr) {
        LOG_ERROR("error type %{public}s", type.c_str());
        return;
    }

    {
        std::unique_lock<std::shared_mutex> cacheLock(cachedProgressEventMutex_);
        for (EventHandler *handler = listener->handlers_; handler != nullptr; handler = handler->next) {
            AniProgressCallbackType* pval = std::get_if<AniProgressCallbackType>(&handler->callbackVar);
            if (pval != nullptr) {
                auto progressArgs = std::make_shared<ProgressArgs>(*pval, sessionId, progress);
                cachedProgressEventList_.emplace_back(progressArgs);
            }
        }
    }

    LOG_INFO("xxx AniWatcher::Emit AsyncExecueInMainThread");
    auto sharedThis = shared_from_this();
    AsyncUtilBase::AsyncExecueInMainThread([sharedThis] {
        LOG_INFO("AniWatcher::Emit, status, AsyncExecueInMainThread!");
        ani_env *aniEnv = taihe::get_env();
        if (aniEnv == nullptr) {
            LOG_ERROR("aniEnv null, clear");
            sharedThis->cachedProgressEventList_.clear();
            return;
        }
        ani_status localScopeStatus = aniEnv->CreateLocalScope(ANI_SCOPE_SIZE);
        if (localScopeStatus != ANI_OK) {
            LOG_WARN("Failed to creat local scope");
        }
        std::unique_lock<std::shared_mutex> cacheLock(sharedThis->cachedProgressEventMutex_);
        for (auto &itemPtr : sharedThis->cachedProgressEventList_) {
            if (itemPtr == nullptr) {
                continue;
            }
            auto taiheSessionId = ::taihe::string(itemPtr->sessionId_);
            itemPtr->callback_(taiheSessionId, itemPtr->progress_);
        }
        sharedThis->cachedProgressEventList_.clear();

        if (localScopeStatus == ANI_OK) {
            aniEnv->DestroyLocalScope();
        }
    });
}

bool AniWatcher::IsEmpty()
{
    if (changeEventListener_->IsEmpty() && statusEventListener_->IsEmpty() && progressEventListener_->IsEmpty()) {
        return true;
    }
    return false;
}

void AniWatcher::SetListener(ChangeEventListener *changeEventListener, StatusEventListener *statusEventListener,
    ProgressEventListener *progressEventListener)
{
    if (changeEventListener_ == nullptr) {
        changeEventListener_ = changeEventListener;
    }
    if (statusEventListener_ == nullptr) {
        statusEventListener_ = statusEventListener;
    }
    if (progressEventListener_ == nullptr) {
        progressEventListener_ = progressEventListener;
    }
}

EventHandler *EventListener::Find(VarCallbackType handler)
{
    std::lock_guard<std::recursive_mutex> autoLock(mutex_);
    EventHandler *result = nullptr;
    for (EventHandler *i = handlers_; i != nullptr; i = i->next) {
        bool isEquals = DataObjectImpl::IsSameTaiheCallback(handler, i->callbackVar);
        if (isEquals) {
            result = i;
            break;
        }
    }
    return result;
}

void EventListener::Clear()
{
    std::lock_guard<std::recursive_mutex> autoLock(mutex_);
    for (EventHandler *i = handlers_; i != nullptr; i = handlers_) {
        handlers_ = i->next;
        i->callbackVar = std::monostate();
        delete i;
    }
}

bool EventListener::Del(VarCallbackType handler)
{
    std::lock_guard<std::recursive_mutex> autoLock(mutex_);
    EventHandler *temp = nullptr;
    for (EventHandler *i = handlers_; i != nullptr;) {
        bool isEquals = false;
        if (!std::holds_alternative<std::monostate>(i->callbackVar)) {
            isEquals = DataObjectImpl::IsSameTaiheCallback(handler, i->callbackVar);
        }
        if (isEquals) {
            EventHandler *delData = i;
            i = i->next;
            if (temp == nullptr) {
                handlers_ = delData->next;
            } else {
                temp->next = delData->next;
            }
            delData->callbackVar = std::monostate();
            delete delData;
        } else {
            temp = i;
            i = i->next;
        }
    }
    return handlers_ == nullptr;
}

bool EventListener::Add(VarCallbackType handler)
{
    if (std::holds_alternative<std::monostate>(handler)) {
        LOG_WARN("EventListener::Add, VarCallbackType empty");
        return false;
    }

    std::lock_guard<std::recursive_mutex> autoLock(mutex_);
    if (Find(handler) != nullptr) {
        LOG_ERROR("has added,return");
        return false;
    }

    if (handlers_ == nullptr) {
        handlers_ = new (std::nothrow) EventHandler();
        if (handlers_ == nullptr) {
            LOG_ERROR("EventListener::Add no memory for EventHandler malloc!");
            return false;
        }
        handlers_->next = nullptr;
    } else {
        auto temp = new (std::nothrow) EventHandler();
        if (temp == nullptr) {
            LOG_ERROR("EventListener::Add no memory for EventHandler malloc!");
            return false;
        }
        temp->next = handlers_;
        handlers_ = temp;
    }
    handlers_->callbackVar = handler;
    return true;
}

void WatcherImpl::OnChanged(const std::string &sessionid, const std::vector<std::string> &changedData)
{
    LOG_INFO("xxx WatcherImpl OnChanged 1");
    std::shared_ptr<AniWatcher> lockedWatcher = watcher_.lock();
    if (lockedWatcher) {
        lockedWatcher->Emit(EVENT_CHANGE, sessionid, changedData);
    } else {
        LOG_ERROR("watcher expired");
    }
}

WatcherImpl::~WatcherImpl()
{
    LOG_ERROR("destroy");
}

bool ChangeEventListener::Add(VarCallbackType handler)
{
    LOG_INFO("xxx ChangeEventListener::Add 1");

    if (!isWatched_ && object_ != nullptr) {
        std::shared_ptr<WatcherImpl> watcher = std::make_shared<WatcherImpl>(watcher_);

        LOG_INFO("xxx ChangeEventListener::Add objectStore_->Watch");
        uint32_t ret = objectStore_->Watch(object_, watcher);
        if (ret != SUCCESS) {
            LOG_ERROR("Watch %{public}s error", object_->GetSessionId().c_str());
        } else {
            LOG_INFO("Watch %{public}s success", object_->GetSessionId().c_str());
            isWatched_ = true;
        }
    }
    return EventListener::Add(handler);
}

bool ChangeEventListener::Del(VarCallbackType handler)
{
    bool isEmpty = EventListener::Del(handler);
    if (isEmpty && isWatched_ && object_ != nullptr) {
        uint32_t ret = objectStore_->UnWatch(object_);
        if (ret != SUCCESS) {
            LOG_ERROR("UnWatch %{public}s error", object_->GetSessionId().c_str());
        } else {
            LOG_INFO("UnWatch %{public}s success", object_->GetSessionId().c_str());
            isWatched_ = false;
        }
    }
    return isEmpty;
}

void ChangeEventListener::Clear()
{
    EventListener::Clear();
    if (isWatched_ && object_ != nullptr) {
        uint32_t ret = objectStore_->UnWatch(object_);
        if (ret != SUCCESS) {
            LOG_ERROR("UnWatch %{public}s error", object_->GetSessionId().c_str());
        } else {
            LOG_INFO("UnWatch %{public}s success", object_->GetSessionId().c_str());
            isWatched_ = false;
        }
    }
}

ChangeEventListener::ChangeEventListener(
    std::weak_ptr<AniWatcher> watcher, DistributedObjectStore *objectStore, DistributedObject *object)
    : objectStore_(objectStore), object_(object), watcher_(watcher)
{
}

bool StatusEventListener::Add(VarCallbackType handler)
{
    LOG_INFO("Add status watch %{public}s", sessionId_.c_str());
    AniNotifierImpl::GetInstance()->AddWatcher(sessionId_, watcher_);
    return EventListener::Add(handler);
}

bool StatusEventListener::Del(VarCallbackType handler)
{
    if (EventListener::Del(handler)) {
        LOG_INFO("Del status watch %{public}s", sessionId_.c_str());
        AniNotifierImpl::GetInstance()->DelWatcher(sessionId_);
        return true;
    }
    return false;
}

void StatusEventListener::Clear()
{
    AniNotifierImpl::GetInstance()->DelWatcher(sessionId_);
    EventListener::Clear();
}

StatusEventListener::StatusEventListener(std::weak_ptr<AniWatcher> watcher, const std::string &sessionId)
    : watcher_(watcher), sessionId_(sessionId)
{
}

bool ProgressEventListener::Add(VarCallbackType handler)
{
    LOG_INFO("Add progress watch %{public}s", Anonymous::Change(sessionId_).c_str());
    AniProgressNotifierImpl::GetInstance()->AddWatcher(sessionId_, watcher_);
    return EventListener::Add(handler);
}

bool ProgressEventListener::Del(VarCallbackType handler)
{
    if (EventListener::Del(handler)) {
        LOG_INFO("Del progress watch %{public}s", Anonymous::Change(sessionId_).c_str());
        AniProgressNotifierImpl::GetInstance()->DelWatcher(sessionId_);
        return true;
    }
    return false;
}

void ProgressEventListener::Clear()
{
    AniProgressNotifierImpl::GetInstance()->DelWatcher(sessionId_);
    EventListener::Clear();
}

ProgressEventListener::ProgressEventListener(std::weak_ptr<AniWatcher> watcher, const std::string &sessionId)
    : watcher_(watcher), sessionId_(sessionId)
{
}

AniWatcher::ChangeArgs::ChangeArgs(
    const AniChangeCallbackType callback, const std::string &sessionId, const std::vector<std::string> &changeData)
    : callback_(callback), sessionId_(sessionId), changeData_(changeData)
{
}
AniWatcher::StatusArgs::StatusArgs(
    const AniStatusCallbackType callback, const std::string &sessionId, const std::string &networkId, const std::string &status)
    : callback_(callback), sessionId_(sessionId), networkId_(networkId), status_(status)
{
}
AniWatcher::ProgressArgs::ProgressArgs(
    const AniProgressCallbackType callback, const std::string &sessionId, const int32_t progress)
    : callback_(callback), sessionId_(sessionId), progress_(progress)
{
}

} // namespace OHOS::ObjectStore
