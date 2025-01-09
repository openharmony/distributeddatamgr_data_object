/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifndef COLLABORATION_EDIT_NAPI_ERRNO
#define COLLABORATION_EDIT_NAPI_ERRNO

namespace OHOS::CollaborationEdit {
constexpr const int OK = 0;  // used for internal method
constexpr const int ERR = -1;
static constexpr const int EDIT_ERROR_OFFSET = 15410000;

enum Status : int32_t {
    SUCCESS = 0,
    INVALID_ARGUMENT = 401,
    INTERNAL_ERROR = EDIT_ERROR_OFFSET,
    UNSUPPORTED_OPERATION = EDIT_ERROR_OFFSET + 1, // 15410001
    INDEX_OUT_OF_RANGE, // 15410002
    DB_ERROR, // 15410003
};
} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_NAPI_ERRNO
