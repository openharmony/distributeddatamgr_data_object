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

#ifndef DISTRIBUTEDDATAMGR_APP_PIPE_MGR_H
#define DISTRIBUTEDDATAMGR_APP_PIPE_MGR_H

#include "app_pipe_handler.h"

namespace OHOS {
namespace ObjectStore {
class AppPipeMgr {
public:
    explicit AppPipeMgr()
    {
    }
    ~AppPipeMgr()
    {
    }
    // add DataChangeListener to watch data change;
    Status StartWatchDataChange(const AppDataChangeListener *observer, const PipeInfo &pipeInfo);

    // stop DataChangeListener to watch data change;
    Status StopWatchDataChange(const AppDataChangeListener *observer, const PipeInfo &pipeInfo);

    // Send data to other device, function will be called back after sent to notify send result.
    Status SendData(const PipeInfo &pipeInfo, const DeviceId &deviceId, const DataInfo &dataInfo, uint32_t totalLength,
        const MessageInfo &info);
    // start server
    Status Start(const PipeInfo &pipeInfo);
    // stop server
    Status Stop(const PipeInfo &pipeInfo);

    bool IsSameStartedOnPeer(const struct PipeInfo &pipeInfo, const struct DeviceId &peer);

private:
    std::mutex dataBusMapMutex_{};
    std::map<std::string, std::shared_ptr<AppPipeHandler>> dataBusMap_{};
};
} // namespace ObjectStore
} // namespace OHOS
#endif // DISTRIBUTEDDATAMGR_APP_PIPE_MGR_H
