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

#ifndef BYTES_UTILS_H
#define BYTES_UTILS_H

#include <initializer_list>

#include "bytes.h"
#include "logger.h"
#include "objectstore_errors.h"

namespace OHOS::ObjectStore {
class BytesUtils final {
public:
    BytesUtils() = delete;
    ~BytesUtils() = delete;

    static void PutNum(void *val, uint32_t offset, uint32_t valLen, Bytes &data)
    {
        uint32_t len = valLen + offset;
        if (len > sizeof(data.front()) * data.size()) {
            data.resize(len);
        }

        for (uint32_t i = 0; i < valLen; i++) {
            // 8 bit = 1 byte
            data[offset + i] = *(static_cast<uint8_t *>(val) + valLen - i - 1);
        }
    }

    static uint32_t GetNum(Bytes &data, uint32_t offset, void *val, uint32_t valLen)
    {
        uint8_t *value = static_cast<uint8_t *>(val);
        uint32_t len = offset + valLen;
        uint32_t dataLen = data.size();
        if (dataLen < len) {
            LOG_ERROR("GetNum data.size() %{public}d, offset %{public}d, valLen %{public}d", dataLen, offset, valLen);
            return ERR_DATA_LEN;
        }
        for (uint32_t i = 0; i < valLen; i++) {
            value[i] = data[len - 1 - i];
        }
        return SUCCESS;
    }
};
} // namespace OHOS::ObjectStore
#endif // BYTES_UTILS_H
