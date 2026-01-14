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
#include "ohos.data.distributedDataObject.DataObject.h"
#include <thread>

#include "anonymous.h"
#include "ani_watcher.h"
#include "ani_notifier_impl.h"
#include "ani_progress_notifier_impl.h"
#include "logger.h"
#include "objectstore_errors.h"

namespace OHOS::ObjectStore {

std::shared_ptr<OHOS::AppExecFwk::EventHandler> AsyncUtilBase::mainHandler_;
static std::once_flag g_handlerOnceFlag;

bool AsyncUtilBase::AsyncExecueInMainThread(const std::function<void()> func)
{
    if (func == nullptr) {
        return false;
    }
    std::call_once(g_handlerOnceFlag, [] {
        auto runner = OHOS::AppExecFwk::EventRunner::GetMainEventRunner();
        if (runner) {
            mainHandler_ = std::make_shared<OHOS::AppExecFwk::EventHandler>(runner);
        }
    });
    if (!mainHandler_) {
        LOG_ERROR("Failed to initialize event handler");
        return false;
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
    LOG_INFO("~AniWatcher");
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

bool AniWatcher::On(const std::string &type, VarCallbackType handler)
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

void AniWatcher::Off(const std::string &type, VarCallbackType handler)
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

EventListener *AniWatcher::Find(const std::string &type)
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

void AniWatcher::Emit(const std::string &type, const std::string &sessionId, const std::vector<std::string> &changeData)
{
    if (changeData.empty()) {
        LOG_ERROR("empty change for type %{public}s, sessionId %{public}s", type.c_str(), sessionId.c_str());
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
    AsyncUtilBase::AsyncExecueInMainThread([ sharedThis ] {
        std::unique_lock<std::shared_mutex> cacheLock(sharedThis->cachedChangeEventMutex_);
        ani_env *aniEnv = taihe::get_env();
        if (aniEnv == nullptr) {
            LOG_ERROR("aniEnv null, clear");
            sharedThis->cachedChangeEventList_.clear();
            return;
        }
        for (auto &itemPtr : sharedThis->cachedChangeEventList_) {
            if (itemPtr == nullptr) {
                continue;
            }
            auto taiheStringList = ::taihe::array<::taihe::string>(
                ::taihe::copy_data_t{}, itemPtr->changeData_.data(), itemPtr->changeData_.size());
            auto taiheSessionId = ::taihe::string(itemPtr->sessionId_);
            itemPtr->callback_(taiheSessionId, taiheStringList);
        }
        sharedThis->cachedChangeEventList_.clear();
    });
}

void AniWatcher::Emit(
    const std::string &type, const std::string &sessionId, const std::string &networkId, const std::string &status)
{
    if (sessionId.empty() || networkId.empty()) {
        LOG_ERROR("empty %{public}s  %{public}s", sessionId.c_str(), Anonymous::Change(networkId).c_str());
        return;
    }
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

    auto sharedThis = shared_from_this();
    AsyncUtilBase::AsyncExecueInMainThread([sharedThis] {
        std::unique_lock<std::shared_mutex> cacheLock(sharedThis->cachedStatusEventMutex_);
        ani_env *aniEnv = taihe::get_env();
        if (aniEnv == nullptr) {
            LOG_ERROR("aniEnv null, clear");
            sharedThis->cachedStatusEventList_.clear();
            return;
        }
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
    });
}

void AniWatcher::Emit(
    const std::string &type, const std::string &sessionId, const int32_t progress)
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

    auto sharedThis = shared_from_this();
    AsyncUtilBase::AsyncExecueInMainThread([sharedThis] {
        std::unique_lock<std::shared_mutex> cacheLock(sharedThis->cachedProgressEventMutex_);
        ani_env *aniEnv = taihe::get_env();
        if (aniEnv == nullptr) {
            LOG_ERROR("aniEnv null, clear");
            sharedThis->cachedProgressEventList_.clear();
            return;
        }
        for (auto &itemPtr : sharedThis->cachedProgressEventList_) {
            if (itemPtr == nullptr) {
                continue;
            }
            auto taiheSessionId = ::taihe::string(itemPtr->sessionId_);
            itemPtr->callback_(taiheSessionId, itemPtr->progress_);
        }
        sharedThis->cachedProgressEventList_.clear();
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
    std::shared_ptr<AniWatcher> lockedWatcher = watcher_.lock();
    if (lockedWatcher) {
        lockedWatcher->Emit(EVENT_CHANGE, sessionid, changedData);
    } else {
        LOG_ERROR("watcher expired");
    }
}

WatcherImpl::~WatcherImpl()
{
    LOG_INFO("~WatcherImpl");
}

ChangeEventListener::ChangeEventListener(
    std::weak_ptr<AniWatcher> watcher, DistributedObjectStore *objectStore, DistributedObject *object)
    : objectStore_(objectStore), object_(object), watcher_(watcher)
{
}

bool ChangeEventListener::Add(VarCallbackType handler)
{
    if (!isWatched_ && object_ != nullptr) {
        std::shared_ptr<WatcherImpl> watcher = std::make_shared<WatcherImpl>(watcher_);
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

StatusEventListener::StatusEventListener(std::weak_ptr<AniWatcher> watcher, const std::string &sessionId)
    : watcher_(watcher), sessionId_(sessionId)
{
    notifyImpl_ = std::make_shared<AniNotifierImpl>();
    DistributedObjectStore *storeInstance = DistributedObjectStore::GetInstance();
    if (storeInstance != nullptr) {
        auto ret = storeInstance->SetStatusNotifier(notifyImpl_);
        if (ret != SUCCESS) {
            LOG_ERROR("SetStatusNotifier %{public}d error", ret);
        } else {
            LOG_INFO("SetStatusNotifier success");
        }
    }
}

bool StatusEventListener::Add(VarCallbackType handler)
{
    LOG_INFO("Add status watch %{public}s", sessionId_.c_str());
    notifyImpl_->AddWatcher(sessionId_, watcher_);
    return EventListener::Add(handler);
}

bool StatusEventListener::Del(VarCallbackType handler)
{
    if (EventListener::Del(handler)) {
        LOG_INFO("Del status watch %{public}s", sessionId_.c_str());
        notifyImpl_->DelWatcher(sessionId_);
        return true;
    }
    return false;
}

void StatusEventListener::Clear()
{
    notifyImpl_->DelWatcher(sessionId_);
    EventListener::Clear();
}

ProgressEventListener::ProgressEventListener(std::weak_ptr<AniWatcher> watcher, const std::string &sessionId)
    : watcher_(watcher), sessionId_(sessionId)
{
    progressNotifyImpl_ = std::make_shared<AniProgressNotifierImpl>();
    DistributedObjectStore *storeInstance = DistributedObjectStore::GetInstance();
    if (storeInstance != nullptr) {
        auto ret = storeInstance->SetProgressNotifier(progressNotifyImpl_);
        if (ret != SUCCESS) {
            LOG_ERROR("SetProgressNotifier %{public}d error", ret);
        } else {
            LOG_INFO("SetProgressNotifier success");
        }
    }
}

bool ProgressEventListener::Add(VarCallbackType handler)
{
    LOG_INFO("Add progress watch %{public}s", Anonymous::Change(sessionId_).c_str());
    progressNotifyImpl_->AddWatcher(sessionId_, watcher_);
    return EventListener::Add(handler);
}

bool ProgressEventListener::Del(VarCallbackType handler)
{
    if (EventListener::Del(handler)) {
        LOG_INFO("Del progress watch %{public}s", Anonymous::Change(sessionId_).c_str());
        progressNotifyImpl_->DelWatcher(sessionId_);
        return true;
    }
    return false;
}

void ProgressEventListener::Clear()
{
    progressNotifyImpl_->DelWatcher(sessionId_);
    EventListener::Clear();
}

AniWatcher::ChangeArgs::ChangeArgs(
    const AniChangeCallbackType callback, const std::string &sessionId, const std::vector<std::string> &changeData)
    : callback_(callback), sessionId_(sessionId), changeData_(changeData)
{
}
AniWatcher::StatusArgs::StatusArgs(const AniStatusCallbackType callback,
    const std::string &sessionId, const std::string &networkId, const std::string &status)
    : callback_(callback), sessionId_(sessionId), networkId_(networkId), status_(status)
{
}
AniWatcher::ProgressArgs::ProgressArgs(
    const AniProgressCallbackType callback, const std::string &sessionId, const int32_t progress)
    : callback_(callback), sessionId_(sessionId), progress_(progress)
{
}

} // namespace OHOS::ObjectStore
