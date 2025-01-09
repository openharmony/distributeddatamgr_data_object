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

#ifndef COLLABORATION_EDIT_ABSTRACT_TYPE_H
#define COLLABORATION_EDIT_ABSTRACT_TYPE_H

#include "db_store.h"
#include "rd_adapter.h"

namespace OHOS::CollaborationEdit {
class AbstractType {
public:
    AbstractType();
    virtual ~AbstractType() {}

    void SetDBStore(std::shared_ptr<DBStore> dbStore);
    std::shared_ptr<DBStore> GetDBStore();
    void SetTableName(std::string name);
    std::string GetTableName();
    void SetID(std::optional<ID> id);
    std::optional<ID> GetID();
    std::shared_ptr<RdAdapter> GetAdapter();

    virtual int32_t GetLength()
    {
        return 0;
    }

private:
    std::shared_ptr<RdAdapter> adapter_;
};
} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_ABSTRACT_TYPE_H
