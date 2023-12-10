/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

/* SAID:1301 */
/* FeatureSystem: object_service */
#ifndef DISTRIBUTEDDATA_OBJECT_STORE_IPC_INTERFACE_H
#define DISTRIBUTEDDATA_OBJECT_STORE_IPC_INTERFACE_H

namespace OHOS::DistributedObject {
namespace ObjectStoreService {
enum class ObjectServiceInterfaceCode {
    OBJECTSTORE_SAVE,
    OBJECTSTORE_REVOKE_SAVE,
    OBJECTSTORE_RETRIEVE,
    OBJECTSTORE_REGISTER_OBSERVER,
    OBJECTSTORE_UNREGISTER_OBSERVER,
    OBJECTSTORE_ON_ASSET_CHANGED,
    OBJECTSTORE_BIND_ASSET_STORE,
    OBJECTSTORE_DELETE_SNAPSHOT,
    OBJECTSTORE_SERVICE_CMD_MAX
};

enum class KvStoreServiceInterfaceCode {
    GET_FEATURE_INTERFACE = 0,
    REGISTERCLIENTDEATHOBSERVER,
};
}
} // namespace OHOS

#endif // DISTRIBUTEDDATA_OBJECT_STORE_IPC_INTERFACE_H