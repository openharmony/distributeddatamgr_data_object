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

#ifndef COLLABORATION_EDIT_RD_ADAPTER_H
#define COLLABORATION_EDIT_RD_ADAPTER_H

#include "db_store.h"
#include "grd_type_export.h"
#include "rd_utils.h"

namespace OHOS::CollaborationEdit {
static constexpr const uint8_t NUMBER_OF_CHARS_IN_LABEL_PREFIX = 2;
const uint8_t LABEL_FRAGMENT = 5; //  See kernel struct DmElementContentType

struct ID {
    ID(const std::string deviceId, uint64_t clock)
    {
        this->deviceId = deviceId;
        this->clock = clock;
    }
    std::string deviceId;
    uint64_t clock;
};

class RdAdapter {
public:
    RdAdapter();
    ~RdAdapter();
    void SetDBStore(std::shared_ptr<DBStore> dbStore);
    std::shared_ptr<DBStore> GetDBStore();
    void SetTableName(std::string name);
    std::string GetTableName();
    void SetID(std::optional<ID> id);
    std::optional<ID> GetID();

    std::pair<int32_t, std::optional<ID>> InsertNode(uint32_t index, std::string nodeName);
    std::pair<int32_t, std::optional<ID>> InsertText(uint32_t index);
    int32_t DeleteChildren(uint32_t index, uint32_t length);
    std::pair<int32_t, std::string> GetChildren(uint32_t index, uint32_t length);
    std::pair<int32_t, std::string> GetJsonString();
    int32_t SetAttribute(const std::string &attributeName, const std::string &attributeValue, uint32_t flags);
    int32_t RemoveAttrribute(const std::string &attributeName);
    std::pair<int32_t, std::string> GetAttributes();

    int32_t TextInsert(uint32_t index, const std::string &content, const std::string &formatStr);
    int32_t TextDelete(uint32_t index, uint32_t length);
    int32_t TextFormat(uint32_t index, uint32_t length, const std::string &formatStr);
    std::pair<int32_t, uint32_t> GetTextLength();
    std::pair<int32_t, std::string> ReadDeltaText(const std::string &snapshot, const std::string &snapshotPerv);
    std::pair<int32_t, std::string> ReadStringText();

    int32_t CreateUndoManager(uint64_t captureTimeout);
    int32_t CloseUndoManager();
    int32_t Undo();
    int32_t Redo();

private:
    std::shared_ptr<DBStore> dbStore_;
    std::string tableName_;
    std::optional<ID> id_;
};
} // namespace OHOS::CollaborationEdit
#endif // COLLABORATION_EDIT_RD_ADAPTER_H
