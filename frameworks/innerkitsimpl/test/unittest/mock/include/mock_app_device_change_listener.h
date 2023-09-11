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
#ifndef DATA_OBJECT_MOCK_APP_DEVICE_CHANGE_LISTENER_H
#define DATA_OBJECT_MOCK_APP_DEVICE_CHANGE_LISTENER_H

#include <gmock/gmock.h>

#include <condition_variable>
#include <memory>

#include "app_device_status_change_listener.h"
namespace OHOS {
namespace ObjectStore {
class MockAppDeviceStatusChangeListener : public AppDeviceStatusChangeListener {
public:
    MockAppDeviceStatusChangeListener()
    {
    }
    void OnDeviceChanged(const DeviceInfo &info, const DeviceChangeType &type) const override
    {
        std::unique_lock<std::mutex> lock(mutex_);
        deviceInfo_ = info;
        result_ = true;
        cond_.notify_all();
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lock(mutex_);
        if (!result_) {
            cond_.wait(lock, [this] { return result_; });
        }
    }

    bool Compare(const DeviceInfo &deviceInfo)
    {
        if (deviceInfo_.deviceId == "" && deviceInfo_.deviceName == deviceInfo.deviceName
            && deviceInfo_.deviceType == deviceInfo.deviceType) {
            return true;
        }
        return false;
    }

public:
    mutable std::mutex mutex_;
    mutable std::condition_variable cond_;
    mutable bool result_;
    mutable DeviceInfo deviceInfo_;
};
class MockAppDeviceStatusChangeListenerLow : public MockAppDeviceStatusChangeListener {
    ChangeLevelType GetChangeLevelType() const override
    {
        return ChangeLevelType::LOW;
    }
};

class MockAppDeviceStatusChangeListenerHigh : public MockAppDeviceStatusChangeListener {
public:
    ChangeLevelType GetChangeLevelType() const override
    {
        return ChangeLevelType::HIGH;
    }
};

class MockAppDeviceStatusChangeListenerMin : public MockAppDeviceStatusChangeListener {
public:
    ChangeLevelType GetChangeLevelType() const override
    {
        return ChangeLevelType::MIN;
    }
};
} // namespace ObjectStore
} // namespace OHOS

#endif // DATA_OBJECT_MOCK_APP_DEVICE_CHANGE_LISTENER_H
