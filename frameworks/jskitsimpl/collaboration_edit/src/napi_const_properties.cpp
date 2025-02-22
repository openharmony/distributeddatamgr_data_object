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

#define LOG_TAG "NapiConstProperties"

#include "napi_const_properties.h"

#include "js_utils.h"
#include "napi_error_utils.h"
#include "napi_utils.h"
#include "rd_type.h"

namespace OHOS::CollaborationEdit {
static napi_value ExportSyncMode(napi_env env)
{
    napi_value syncMode = nullptr;
    napi_status status = napi_create_object(env, &syncMode);
    ASSERT(status == napi_ok, "create syncMode object go wrong.", nullptr);
    napi_set_named_property(env, syncMode, "SYNC_MODE_PUSH",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(SyncMode::PUSH)));
    napi_set_named_property(env, syncMode, "SYNC_MODE_PULL",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(SyncMode::PULL)));
    napi_set_named_property(env, syncMode, "SYNC_MODE_PULL_PUSH",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(SyncMode::PULL_PUSH)));
    napi_object_freeze(env, syncMode);
    return syncMode;
}

static napi_value ExportPredicate(napi_env env)
{
    napi_value predicate = nullptr;
    napi_status status = napi_create_object(env, &predicate);
    ASSERT(status == napi_ok, "create predicate object go wrong.", nullptr);
    napi_set_named_property(env, predicate, "EQUAL_TO",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(Predicate::EQUAL_TO)));
    napi_set_named_property(env, predicate, "NOT_EQUAL_TO",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(Predicate::NOT_EQUAL_TO)));
    napi_set_named_property(env, predicate, "GREATER_THAN",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(Predicate::GREATER_THAN)));
    napi_set_named_property(env, predicate, "LESS_THAN",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(Predicate::LESS_THAN)));
    napi_set_named_property(env, predicate, "GREATER_THAN_OR_EQUAL_TO",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(Predicate::GREATER_THAN_OR_EQUAL_TO)));
    napi_set_named_property(env, predicate, "GREATER_THAN_OR_EQUAL_TO",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(Predicate::LESS_THAN_OR_EQUAL_TO)));
    napi_object_freeze(env, predicate);
    return predicate;
}

static napi_value ExportProgressCode(napi_env env)
{
    napi_value progressCode = nullptr;
    napi_status status = napi_create_object(env, &progressCode);
    ASSERT(status == napi_ok, "create progressCode object go wrong.", nullptr);
    napi_set_named_property(env, progressCode, "CLOUD_SYNC_SUCCESS",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(ProgressCode::CLOUD_SYNC_SUCCESS)));
    napi_set_named_property(env, progressCode, "CLOUD_NOT_SET",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(ProgressCode::CLOUD_NOT_SET)));
    napi_set_named_property(env, progressCode, "INTERNAL_ERROR",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(ProgressCode::SYNC_INTERNAL_ERROR)));
    napi_set_named_property(env, progressCode, "EXTERNAL_ERROR",
        JSUtils::Convert2JSValue(env, static_cast<int32_t>(ProgressCode::SYNC_EXTERNAL_ERROR)));
    napi_object_freeze(env, progressCode);
    return progressCode;
}

napi_status InitConstProperties(napi_env env, napi_value exports)
{
    const napi_property_descriptor properties[] = {
        DECLARE_NAPI_PROPERTY("SyncMode", ExportSyncMode(env)),
        DECLARE_NAPI_PROPERTY("Predicate", ExportPredicate(env)),
        DECLARE_NAPI_PROPERTY("ProgressCode", ExportProgressCode(env)),
    };

    size_t count = sizeof(properties) / sizeof(properties[0]);
    return napi_define_properties(env, exports, count, properties);
}
} // namespace OHOS::CollaborationEdit
