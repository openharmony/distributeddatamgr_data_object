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

#include "db_thread_pool.h"

namespace OHOS::CollaborationEdit {
DbThreadPool::DbThreadPool(std::shared_ptr<ExecutorPool> executors) : executors_(executors)
{}

DbThreadPool::~DbThreadPool()
{
    executors_ = nullptr;
}

TaskId DbThreadPool::Execute(const Task &task)
{
    return executors_->Execute(task);
}
}  // namespace OHOS::CollaborationEdit
