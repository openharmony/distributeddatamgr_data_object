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

#ifndef OBJECT_STORE_LOGGER_H
#define OBJECT_STORE_LOGGER_H
#include <memory>
#ifdef HILOG_ENABLE
#include "hilog/log.h"
namespace OHOS::ObjectStore {
static inline OHOS::HiviewDFX::HiLogLabel LogLabel()
{
    return { LOG_CORE, 0xD001652, "ObjectStore-x" };
}

#define LOG_DEBUG(fmt, ...)                                                                 \
    do {                                                                                    \
        auto lable = LogLabel();                                                            \
        if (HiLogIsLoggable(lable.domain, lable.tag, LogLevel::LOG_DEBUG)) {                \
            ((void)HILOG_IMPL(lable.type, LogLevel::LOG_DEBUG, lable.domain, lable.tag,     \
                "%{public}d: %{public}s " fmt " ", __LINE__, __FUNCTION__, ##__VA_ARGS__)); \
        }                                                                                   \
    } while (0)

#define LOG_INFO(fmt, ...)                                                                  \
    do {                                                                                    \
        auto lable = LogLabel();                                                            \
        if (HiLogIsLoggable(lable.domain, lable.tag, LogLevel::LOG_INFO)) {                 \
            ((void)HILOG_IMPL(lable.type, LogLevel::LOG_INFO, lable.domain, lable.tag,      \
                "%{public}d: %{public}s " fmt " ", __LINE__, __FUNCTION__, ##__VA_ARGS__)); \
        }                                                                                   \
    } while (0)

#define LOG_WARN(fmt, ...)                                                                  \
    do {                                                                                    \
        auto lable = LogLabel();                                                            \
        if (HiLogIsLoggable(lable.domain, lable.tag, LogLevel::LOG_WARN)) {                 \
            ((void)HILOG_IMPL(lable.type, LogLevel::LOG_WARN, lable.domain, lable.tag,      \
                "%{public}d: %{public}s " fmt " ", __LINE__, __FUNCTION__, ##__VA_ARGS__)); \
        }                                                                                   \
    } while (0)

#define LOG_ERROR(fmt, ...)                                                                 \
    do {                                                                                    \
        auto lable = LogLabel();                                                            \
        if (HiLogIsLoggable(lable.domain, lable.tag, LogLevel::LOG_ERROR)) {                \
            ((void)HILOG_IMPL(lable.type, LogLevel::LOG_ERROR, lable.domain, lable.tag,     \
                "%{public}d: %{public}s " fmt " ", __LINE__, __FUNCTION__, ##__VA_ARGS__)); \
        }                                                                                   \
    } while (0)

#define LOG_FATAL(fmt, ...)                                                                           \
    do {                                                                                    \
        auto lable = LogLabel();                                                            \
        if (HiLogIsLoggable(lable.domain, lable.tag, LogLevel::LOG_FATAL)) {                \
            ((void)HILOG_IMPL(lable.type, LogLevel::LOG_FATAL, lable.domain, lable.tag,     \
                "%{public}d: %{public}s " fmt " ", __LINE__, __FUNCTION__, ##__VA_ARGS__)); \
        }                                                                                   \
    } while (0)

} // namespace OHOS::ObjectStore
#else
#include <stdio.h>
#include <stdlib.h>

#define LOG_DEBUG(fmt, ...) \
    printf("[D][ObjectStore]%s:%d %s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_ERROR(fmt, ...) \
    printf("[E][ObjectStore]%s:%d %s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_INFO(fmt, ...) \
    printf("[I][ObjectStore]%s:%d %s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define LOG_WARN(fmt, ...) \
    printf("[W][ObjectStore]%s:%d %s: " fmt "\n", __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#endif // #ifdef HILOG_ENABLE
#endif // OBJECT_STORE_LOGGER_H
