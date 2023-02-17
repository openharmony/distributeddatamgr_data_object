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
#ifndef OBJECTSTORE_ERRORS_H
#define OBJECTSTORE_ERRORS_H

#include <stdint.h>

namespace OHOS::ObjectStore {
constexpr uint32_t BASE_ERR_OFFSET = 1650;

/* module defined errors */
/**
 * @brief Executed successfully.
 */
constexpr uint32_t SUCCESS = 0;

/**
 * @brief Database setup failure.
 */
constexpr uint32_t ERR_DB_SET_PROCESS = BASE_ERR_OFFSET + 1;

/**
 * @brief Already exit.
 */
constexpr uint32_t ERR_EXIST = BASE_ERR_OFFSET + 2;

/**
 * @brief Illegal data length.
 */
constexpr uint32_t ERR_DATA_LEN = BASE_ERR_OFFSET + 3;

/**
 * @brief Memory allocation failure.
 */
constexpr uint32_t ERR_NOMEM = BASE_ERR_OFFSET + 4;

/**
 * @brief Database is not initialized.
 */
constexpr uint32_t ERR_DB_NOT_INIT = BASE_ERR_OFFSET + 5;

/**
 * @brief Kvstore error.
 */
constexpr uint32_t ERR_DB_GETKV_FAIL = BASE_ERR_OFFSET + 6;

/**
 * @brief Database not exist.
 */
constexpr uint32_t ERR_DB_NOT_EXIST = BASE_ERR_OFFSET + 7;

/**
 * @brief Get database data failure.
 */
constexpr uint32_t ERR_DB_GET_FAIL = BASE_ERR_OFFSET + 8;

/**
 * @brief Get database entry failure.
 */
constexpr uint32_t ERR_DB_ENTRY_FAIL = BASE_ERR_OFFSET + 9;

/**
 * @brief Close database failure.
 */
constexpr uint32_t ERR_CLOSE_STORAGE = BASE_ERR_OFFSET + 10;

/**
 * @brief The object is null.
 */
constexpr uint32_t ERR_NULL_OBJECT = BASE_ERR_OFFSET + 11;

/**
 * @brief registration failed.
 */
constexpr uint32_t ERR_REGISTER = BASE_ERR_OFFSET + 12;

/**
 * @brief The object store is null.
 */
constexpr uint32_t ERR_NULL_OBJECTSTORE = BASE_ERR_OFFSET + 13;

/**
 * @brief Get the object failure.
 */
constexpr uint32_t ERR_GET_OBJECT = BASE_ERR_OFFSET + 14;

/**
 * @brief Unregistered.
 */
constexpr uint32_t ERR_NO_OBSERVER = BASE_ERR_OFFSET + 15;

/**
 * @brief Deregistration failure.
 */
constexpr uint32_t ERR_UNRIGSTER = BASE_ERR_OFFSET + 16;

/**
 * @brief Only one device.
 */
constexpr uint32_t ERR_SINGLE_DEVICE = BASE_ERR_OFFSET + 17;

/**
 * @brief The pointer is null.
 */
constexpr uint32_t ERR_NULL_PTR = BASE_ERR_OFFSET + 18;

/**
 * @brief Processing failure.
 */
constexpr uint32_t ERR_PROCESSING = BASE_ERR_OFFSET + 19;

/**
 * @brief ResultSet processing failure.
 */
constexpr uint32_t ERR_RESULTSET = BASE_ERR_OFFSET + 20;

/**
 * @brief Input parameters error.
 */
constexpr uint32_t ERR_INVALID_ARGS = BASE_ERR_OFFSET + 21;

/**
 * @brief Ipc error.
 */
constexpr uint32_t ERR_IPC = BASE_ERR_OFFSET + 22;
} // namespace OHOS::ObjectStore

#endif
