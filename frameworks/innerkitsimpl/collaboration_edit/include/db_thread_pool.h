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

#ifndef COLLABORATION_EDIT_DB_THREAD_POOL_H
#define COLLABORATION_EDIT_DB_THREAD_POOL_H

#include <thread>

#include "executor_pool.h"

namespace OHOS::CollaborationEdit {
using Task = std::function<void()>;
using TaskId = uint64_t;
using Duration = std::chrono::steady_clock::duration;

class DBStoreMgrThreadPool {
public:
    DBStoreMgrThreadPool() = default;
    virtual ~DBStoreMgrThreadPool() = default;
 
    virtual TaskId Execute(const Task &task) = 0;
};

class DbThreadPool : public CollaborationEdit::DBStoreMgrThreadPool {
public:
    DbThreadPool() = default;
    explicit DbThreadPool(std::shared_ptr<ExecutorPool> executors);
    ~DbThreadPool() override;
    TaskId Execute(const Task &task) override;

private:
    std::shared_ptr<ExecutorPool> executors_ = nullptr;
};

} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_DB_THREAD_POOL_H
