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

#define LOG_TAG "RdAdapter"

#include "rd_adapter.h"

#include "log_print.h"

namespace OHOS::CollaborationEdit {
RdAdapter::RdAdapter()
{}

RdAdapter::~RdAdapter()
{}

void RdAdapter::SetDBStore(std::shared_ptr<DBStore> dbStore)
{
    this->dbStore_ = dbStore;
}

std::shared_ptr<DBStore> RdAdapter::GetDBStore()
{
    return this->dbStore_;
}

void RdAdapter::SetTableName(std::string name)
{
    this->tableName_ = name;
}

std::string RdAdapter::GetTableName()
{
    return this->tableName_;
}

void RdAdapter::SetID(std::optional<ID> id)
{
    this->id_ = id;
}

std::optional<ID> RdAdapter::GetID()
{
    return this->id_;
}

std::pair<int32_t, std::optional<ID>> RdAdapter::InsertNode(uint32_t index, std::string nodeName)
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };
    GRD_DocNodeInfo nodeInfo = {
        .type = GRD_XML_ELEMENT_TYPE,
        .content = nodeName.c_str(),
    };

    GRD_ElementIdT *outElementId = nullptr;
    int32_t errCode = RdUtils::RdInsertElements(this->dbStore_->GetDB(), &position, index, &nodeInfo, &outElementId);
    if (errCode != GRD_OK || outElementId == nullptr) {
        LOG_ERROR("InsertElements go wrong, errCode = %{public}d", errCode);
        return std::make_pair(TransferToNapiErrNo(errCode), std::nullopt);
    }
    ID id(std::string(outElementId->equipId), outElementId->incrClock);
    RdUtils::RdFreeElementId(outElementId);
    return std::make_pair(errCode, id);
}

std::pair<int32_t, std::optional<ID>> RdAdapter::InsertText(uint32_t index)
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };
    GRD_DocNodeInfo nodeInfo = {
        .type = GRD_XML_TEXT_TYPE,
        .content = nullptr,
    };

    GRD_ElementIdT *outElementId = nullptr;
    int32_t errCode = RdUtils::RdInsertElements(this->dbStore_->GetDB(), &position, index, &nodeInfo, &outElementId);
    if (errCode != GRD_OK || outElementId == nullptr) {
        LOG_ERROR("InsertElements go wrong, errCode = %{public}d", errCode);
        return std::make_pair(TransferToNapiErrNo(errCode), std::nullopt);
    }
    ID id(std::string(outElementId->equipId), outElementId->incrClock);
    RdUtils::RdFreeElementId(outElementId);
    return std::make_pair(errCode, id);
}

int32_t RdAdapter::DeleteChildren(uint32_t index, uint32_t length)
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }

    int32_t errCode = RdUtils::RdDeleteElements(this->dbStore_->GetDB(), this->tableName_.c_str(), elementId, index,
        length);
    if (errCode != GRD_OK) {
        LOG_ERROR("DeleteElements go wrong, errCode = %{public}d", errCode);
    }
    return TransferToNapiErrNo(errCode);
}

std::pair<int32_t, std::string> RdAdapter::GetChildren(uint32_t index, uint32_t length)
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };

    char *respXml = nullptr;
    int32_t errCode = RdUtils::RdGetElements(this->dbStore_->GetDB(), &position, index, length, &respXml);
    if (errCode != GRD_OK) {
        LOG_ERROR("RdGetElements go wrong, errCode = %{public}d", errCode);
        return std::make_pair(TransferToNapiErrNo(errCode), "");
    }
    std::string retString(respXml == nullptr ? "" : respXml);
    (void)RdUtils::RdFreeValue(respXml);
    return std::make_pair(errCode, retString);
}

std::pair<int32_t, std::string> RdAdapter::GetJsonString()
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }

    char *replyJson = nullptr;
    int32_t errCode = RdUtils::RdFragmentToString(this->dbStore_->GetDB(), this->tableName_.c_str(), elementId,
        &replyJson);
    if (errCode != GRD_OK) {
        LOG_ERROR("RdFragmentToString go wrong, errCode = %{public}d", errCode);
        return std::make_pair(TransferToNapiErrNo(errCode), "");
    }
    std::string retString(replyJson == nullptr ? "" : replyJson);
    (void)RdUtils::RdFreeValue(replyJson);
    return std::make_pair(errCode, retString);
}

int32_t RdAdapter::SetAttribute(const std::string &attributeName, const std::string &attributeValue, uint32_t flags)
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };
    int32_t errCode = RdUtils::RdElementSetAttribute(this->dbStore_->GetDB(), &position, attributeName.c_str(),
        attributeValue.c_str(), flags);
    if (errCode != GRD_OK) {
        LOG_ERROR("RdElementSetAttribute go wrong, errCode = %{public}d", errCode);
    }
    return TransferToNapiErrNo(errCode);
}

int32_t RdAdapter::RemoveAttrribute(const std::string &attributeName)
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };
    int32_t errCode = RdUtils::RdElementRemoveAttribute(this->dbStore_->GetDB(), &position, attributeName.c_str());
    if (errCode != GRD_OK) {
        LOG_ERROR("RdElementRemoveAttribute go wrong, errCode = %{public}d", errCode);
    }
    return TransferToNapiErrNo(errCode);
}

std::pair<int32_t, std::string> RdAdapter::GetAttributes()
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };

    char *fullAttributes = nullptr;
    int32_t errCode = RdUtils::RdElementGetAttributes(this->dbStore_->GetDB(), &position, &fullAttributes);
    if (errCode != GRD_OK) {
        LOG_ERROR("RdElementGetAttributes go wrong, errCode = %{public}d", errCode);
        return std::make_pair(TransferToNapiErrNo(errCode), "");
    }
    std::string attrsString(fullAttributes == nullptr ? "" : fullAttributes);
    (void)RdUtils::RdFreeValue(fullAttributes);
    return std::make_pair(errCode, attrsString);
}

int32_t RdAdapter::TextInsert(uint32_t index, const std::string &content, const std::string &formatStr)
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };
    int32_t errCode = RdUtils::RdTextInsert(this->dbStore_->GetDB(), &position, index, content.c_str(),
        formatStr.empty() ? nullptr : formatStr.c_str());
    if (errCode != GRD_OK) {
        LOG_ERROR("RdTextInsert go wrong, errCode = %{public}d", errCode);
    }
    return TransferToNapiErrNo(errCode);
}

int32_t RdAdapter::TextDelete(uint32_t index, uint32_t length)
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };
    int32_t errCode = RdUtils::RdTextDelete(this->dbStore_->GetDB(), &position, index, length);
    if (errCode != GRD_OK) {
        LOG_ERROR("RdTextDelete go wrong, errCode = %{public}d", errCode);
    }
    return TransferToNapiErrNo(errCode);
}

int32_t RdAdapter::TextFormat(uint32_t index, uint32_t length, const std::string &formatStr)
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };
    int32_t errCode = RdUtils::RdTextFormat(this->dbStore_->GetDB(), &position, index, length,
        formatStr.c_str());
    if (errCode != GRD_OK) {
        LOG_ERROR("RdTextFormat go wrong, errCode = %{public}d", errCode);
    }
    return TransferToNapiErrNo(errCode);
}

std::pair<int32_t, uint32_t> RdAdapter::GetTextLength()
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };
    uint32_t textLength = 0;
    int32_t errCode = RdUtils::RdTextGetLength(this->dbStore_->GetDB(), &position, &textLength);
    if (errCode != GRD_OK) {
        LOG_ERROR("RdTextGetLength go wrong, errCode = %{public}d", errCode);
    }
    return std::make_pair(TransferToNapiErrNo(errCode), textLength);
}

std::pair<int32_t, std::string> RdAdapter::ReadDeltaText(const std::string &snapshot, const std::string &snapshotPrev)
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };

    char *deltaText = nullptr;
    const char *snapshotPtr = snapshot.empty() ? nullptr : snapshot.c_str();
    const char *snapshotPrevPtr = snapshotPrev.empty() ? nullptr : snapshotPrev.c_str();
    int32_t errCode = RdUtils::RdTextReadInDeltaMode(this->dbStore_->GetDB(), &position, snapshotPtr,
        snapshotPrevPtr, &deltaText);
    if (errCode != GRD_OK) {
        LOG_ERROR("RdTextReadInDeltaMode go wrong, errCode = %{public}d", errCode);
        return std::make_pair(TransferToNapiErrNo(errCode), "");
    }
    std::string deltaString(deltaText == nullptr ? "" : deltaText);
    (void)RdUtils::RdFreeValue(deltaText);
    return std::make_pair(errCode, deltaString);
}

std::pair<int32_t, std::string> RdAdapter::ReadStringText()
{
    GRD_ElementIdT *elementId = nullptr;
    GRD_ElementIdT tempId;
    if (this->id_.has_value()) {
        tempId.equipId = this->id_.value().deviceId.c_str();
        tempId.incrClock = this->id_.value().clock;
        elementId = &tempId;
    }
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = elementId,
    };

    char *text = nullptr;
    int32_t errCode = RdUtils::RdTextReadInStrMode(this->dbStore_->GetDB(), &position, &text);
    if (errCode != GRD_OK) {
        LOG_ERROR("RdTextReadInStrMode go wrong, errCode = %{public}d", errCode);
        return std::make_pair(TransferToNapiErrNo(errCode), "");
    }
    std::string str(text == nullptr ? "" : text);
    (void)RdUtils::RdFreeValue(text);
    return std::make_pair(errCode, str);
}

int32_t RdAdapter::CreateUndoManager(uint64_t captureTimeout)
{
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = nullptr,
    };

    GRD_UndoParamT undoParam = {
        .captureTimeout = captureTimeout,
    };

    int32_t errCode = RdUtils::RdDocUndoManager(this->dbStore_->GetDB(), &position, &undoParam);
    if (errCode != GRD_OK) {
        LOG_ERROR("RdDocUndoManager go wrong, errCode = %{public}d", errCode);
    }
    return TransferToNapiErrNo(errCode);
}

int32_t RdAdapter::Undo()
{
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = nullptr,
    };

    char *modify = nullptr;
    int32_t errCode = RdUtils::RdDocUndo(this->GetDBStore()->GetDB(), &position, &modify);
    if (errCode != GRD_OK) {
        LOG_ERROR("RdDocUndo go wrong, errCode = %{public}d", errCode);
    }
    (void)RdUtils::RdFreeValue(modify);
    return TransferToNapiErrNo(errCode);
}

int32_t RdAdapter::Redo()
{
    GRD_XmlOpPositionT position = {
        .tableName = this->tableName_.c_str(),
        .elementId = nullptr,
    };

    char *modify = nullptr;
    int32_t errCode = RdUtils::RdDocRedo(this->GetDBStore()->GetDB(), &position, &modify);
    if (errCode != GRD_OK) {
        LOG_ERROR("RdDocRedo go wrong, errCode = %{public}d", errCode);
    }
    (void)RdUtils::RdFreeValue(modify);
    return TransferToNapiErrNo(errCode);
}

int32_t RdAdapter::TransferToNapiErrNo(int32_t originNo)
{
    auto it = g_errMap.find(originNo);
    if (it == g_errMap.end()) {
        return Status::DB_ERROR;
    }
    return it->second;
}

} // namespace OHOS::CollaborationEdit
