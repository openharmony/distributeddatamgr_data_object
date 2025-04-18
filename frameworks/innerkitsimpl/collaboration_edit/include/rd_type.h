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

#ifndef COLLABORATION_EDIT_RD_TYPE_H
#define COLLABORATION_EDIT_RD_TYPE_H

namespace OHOS::CollaborationEdit {
enum SyncMode {
    PUSH,
    PULL,
    PULL_PUSH,
};

enum Predicate {
    EQUAL_TO,
    NOT_EQUAL_TO,
    GREATER_THAN,
    LESS_THAN,
    GREATER_THAN_OR_EQUAL_TO,
    LESS_THAN_OR_EQUAL_TO,
};

enum ProgressCode {
    CLOUD_SYNC_SUCCESS,
    CLOUD_NOT_SET,
    SYNC_INTERNAL_ERROR,
    SYNC_EXTERNAL_ERROR,
};
} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_RD_TYPE_H
