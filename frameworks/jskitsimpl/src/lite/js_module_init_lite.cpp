/*
 * Copyright (c) 2026 Huawei Device Co., Ltd.
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

#include "js_common.h"
#include "js_distributedobject_lite.h"

using namespace OHOS::ObjectStore;

static napi_value DistributedDataObjectExport(napi_env env, napi_value exports)
{
    napi_status status;
    static napi_property_descriptor desc[] = {
        DECLARE_NAPI_FUNCTION("create", JSDistributedObjectWatchLite::JSDistributedObjectLite),
        DECLARE_NAPI_FUNCTION("genSessionId", JSDistributedObjectWatchLite::JSDistributedObjectLite),
        DECLARE_NAPI_FUNCTION("setSessionId", JSDistributedObjectWatchLite::JSDistributedObjectLite),
        DECLARE_NAPI_FUNCTION("on", JSDistributedObjectWatchLite::JSDistributedObjectLite),
        DECLARE_NAPI_FUNCTION("off", JSDistributedObjectWatchLite::JSDistributedObjectLite),
        DECLARE_NAPI_FUNCTION("save", JSDistributedObjectWatchLite::JSDistributedObjectLite),
        DECLARE_NAPI_FUNCTION("revokeSave", JSDistributedObjectWatchLite::JSDistributedObjectLite),
        DECLARE_NAPI_FUNCTION("bindAssetStore", JSDistributedObjectWatchLite::JSDistributedObjectLite),
        DECLARE_NAPI_FUNCTION("setAsset", JSDistributedObjectWatchLite::JSDistributedObjectLite),
        DECLARE_NAPI_FUNCTION("setAssets", JSDistributedObjectWatchLite::JSDistributedObjectLite),
    };

    status = napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}

// storage module define
static napi_module storageModule = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = DistributedDataObjectExport,
    .nm_modname = "data.distributedDataObject",
    .nm_priv = ((void *)0),
    .reserved = { 0 },
};

// distributeddataobject module register
static __attribute__((constructor)) void RegisterModule()
{
    napi_module_register(&storageModule);
}