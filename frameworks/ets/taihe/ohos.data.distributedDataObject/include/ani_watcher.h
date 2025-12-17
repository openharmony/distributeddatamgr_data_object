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
#ifndef JSWATCHER_H
#define JSWATCHER_H

#include <variant>
#include <optional>
#include <mutex>
#include <shared_mutex>
#include "distributed_objectstore.h"
#include <ani.h>
#include "event_handler.h"
#include "event_runner.h"

#include "taihe/runtime.hpp"
#include "taihe/callback.hpp"
#include "taihe/array.hpp"

using AniChangeCallbackType = ::taihe::callback<void(::taihe::string_view, ::taihe::array_view<::taihe::string>)>;
using AniStatusCallbackType = ::taihe::callback<void(::taihe::string_view, ::taihe::string_view, ::taihe::string_view)>;
using AniProgressCallbackType = ::taihe::callback<void(::taihe::string_view, int32_t)>;

using VarCallbackType = std::variant<std::monostate,
    AniChangeCallbackType, AniStatusCallbackType, AniProgressCallbackType>;

constexpr const char* EVENT_CHANGE = "change";
constexpr const char* EVENT_STATUS = "status";
constexpr const char* EVENT_PROGRESS = "progressChanged";

namespace OHOS::ObjectStore {
class AniWatcher;

class AsyncUtilBase {
public:
    AsyncUtilBase() = default;
    virtual ~AsyncUtilBase() {}
    static bool AsyncExecue(const std::function<void()> func);
    static bool AsyncExecueInMainThread(const std::function<void()> func);

protected:
    std::mutex jsCallbackMutex_;
    static std::shared_ptr<OHOS::AppExecFwk::EventHandler> mainHandler_;
};

struct EventHandler {
    VarCallbackType callbackVar;
    EventHandler *next = nullptr;
};

class EventListener {
public:
    EventListener()
    {
    }

    virtual ~EventListener()
    {
    }

    bool IsEmpty()
    {
        return handlers_ == nullptr;
    }

    void Lock()
    {
        mutex_.lock();
    }

    void Unlock()
    {
        mutex_.unlock();
    }

    virtual bool Add(VarCallbackType handler);

    virtual bool Del(VarCallbackType handler);

    virtual void Clear();

    EventHandler *Find(VarCallbackType handler);
    EventHandler *handlers_ = nullptr;
    std::recursive_mutex mutex_;
};

class ChangeEventListener : public EventListener {
public:
    ChangeEventListener(
        std::weak_ptr<AniWatcher> watcher, DistributedObjectStore *objectStore, DistributedObject *object);

    bool Add(VarCallbackType handler) override;

    bool Del(VarCallbackType handler) override;

    void Clear() override;
private:
    bool isWatched_{false};
    DistributedObjectStore *objectStore_{nullptr};
    DistributedObject *object_{nullptr};
    std::weak_ptr<AniWatcher> watcher_{};
};

class StatusEventListener : public EventListener {
public:
    StatusEventListener(std::weak_ptr<AniWatcher> watcher, const std::string &sessionId);
    bool Add(VarCallbackType handler) override;

    bool Del(VarCallbackType handler) override;

    void Clear() override;
private:
    std::weak_ptr<AniWatcher> watcher_{};
    std::string sessionId_;
};

class ProgressEventListener : public EventListener {
public:
    ProgressEventListener(std::weak_ptr<AniWatcher> watcher, const std::string &sessionId);
    bool Add(VarCallbackType handler) override;

    bool Del(VarCallbackType handler) override;

    void Clear() override;

private:
    std::weak_ptr<AniWatcher> watcher_{};
    std::string sessionId_;
};

class AniWatcher : public std::enable_shared_from_this<AniWatcher> {
public:
    AniWatcher(DistributedObjectStore *objectStore, DistributedObject *object);
    ~AniWatcher();

    bool On(const std::string &type, VarCallbackType handler);

    void Off(const std::string &type, VarCallbackType optHandler);

    void Emit(const std::string &type, const std::string &sessionId, const std::vector<std::string> &changeData);

    void Emit(const std::string &type, const std::string &sessionId,
        const std::string &networkId, const std::string &status);

    void Emit(const std::string &type, const std::string &sessionId, const int32_t progress);

    bool IsEmpty();

    void SetListener(ChangeEventListener *changeEventListener, StatusEventListener *statusEventListener,
        ProgressEventListener *progressEventListener);

private:
    class ChangeArgs {
        public:
            explicit ChangeArgs(const AniChangeCallbackType callback,
                const std::string &sessionId, const std::vector<std::string> &changeData);
            AniChangeCallbackType callback_;
            const std::string sessionId_;
            const std::vector<std::string> changeData_;
    };
    class StatusArgs {
        public:
            explicit StatusArgs(const AniStatusCallbackType callback,
                const std::string &sessionId, const std::string &networkId, const std::string &status);
            AniStatusCallbackType callback_;
            const std::string sessionId_;
            const std::string networkId_;
            const std::string status_;
    };
    class ProgressArgs {
        public:
            explicit ProgressArgs(const AniProgressCallbackType callback,
                const std::string &sessionId, const int32_t progress);
            AniProgressCallbackType callback_;
            const std::string sessionId_;
            const int32_t progress_;
    };

    EventListener *Find(const std::string &type);

    ChangeEventListener* changeEventListener_ = nullptr;
    StatusEventListener* statusEventListener_ = nullptr;
    ProgressEventListener* progressEventListener_ = nullptr;

    std::vector<std::shared_ptr<ChangeArgs>> cachedChangeEventList_;
    std::shared_mutex cachedChangeEventMutex_{};
    std::vector<std::shared_ptr<StatusArgs>> cachedStatusEventList_;
    std::shared_mutex cachedStatusEventMutex_{};
    std::vector<std::shared_ptr<ProgressArgs>> cachedProgressEventList_;
    std::shared_mutex cachedProgressEventMutex_{};
};

class WatcherImpl : public ObjectWatcher {
public:
    explicit WatcherImpl(std::weak_ptr<AniWatcher> watcher) : watcher_(watcher)
    {
    }
    virtual ~WatcherImpl();
    void OnChanged(const std::string &sessionid, const std::vector<std::string> &changedData) override;

private:
    std::weak_ptr<AniWatcher> watcher_{};
};
} // namespace OHOS::ObjectStore

#endif // ANI_WATCHER_H
