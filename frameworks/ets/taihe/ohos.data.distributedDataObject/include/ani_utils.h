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
#ifndef OHOS_ANI_UTILS_H
#define OHOS_ANI_UTILS_H

#include <memory>
#include <map>
#include <string>
#include <vector>
#include "taihe/runtime.hpp"
#include "object_types.h"
#include "ohos.data.distributedDataObject.proj.hpp"
#include "ohos.data.distributedDataObject.impl.hpp"

namespace AniUtils {

int32_t AniGetProperty(ani_env *env, ani_object ani_obj, const char *property, std::string &result,
    bool optional = false);
int32_t AniGetProperty(ani_env *env, ani_object ani_obj, const char *property, bool &result,
    bool optional = false);
int32_t AniGetProperty(ani_env *env, ani_object ani_obj, const char *property, int32_t &result,
    bool optional = false);
int32_t AniGetProperty(ani_env *env, ani_object ani_obj, const char *property, uint32_t &result,
    bool optional = false);
int32_t AniGetProperty(ani_env *env, ani_object ani_obj, const char *property, ani_object &result,
    bool optional = false);

ani_status AniCreateBool(ani_env* env, bool value, ani_object& result);
ani_ref AniCreateByte(ani_env *env, uint8_t para);
ani_status AniCreateInt(ani_env* env, int32_t value, ani_object& result);
ani_status AniCreateDouble(ani_env* env, double value, ani_object& result);
ani_string AniCreateString(ani_env *env, const std::string &para);

ani_object AniCreateArray(ani_env *env, const std::vector<ani_object> &objectArray);
ani_object AniCreatEmptyRecord(ani_env* env, ani_method& setMethod);

ani_method AniGetMethod(ani_env *env, ani_class cls, const char* methodName, const char* signature);
ani_class AniGetClass(ani_env *env, const char* className);
ani_method AniGetClassMethod(ani_env *env, const char* className, const char* methodName, const char* signature);
ani_field AniFindClassField(ani_env *env, ani_class cls, char const *name);

void AniExecuteFunc(ani_vm* vm, const std::function<void(ani_env*)> func);

class AniStringUtils {
public:
    static std::string ToStd(ani_env *env, ani_string ani_str);
    static ani_string ToAni(ani_env *env, const std::string& str);
};

class UnionAccessor {
public:
    UnionAccessor(ani_env *env, ani_object &obj);

    bool IsInstanceOf(const std::string& className);

    template<typename T>
    bool IsInstanceOfType();

    bool TryConvertToNumber(double &value);

    template<typename T>
    bool TryConvert(T &value);

    template<typename T>
    bool TryConvertArray(std::vector<T> &value);

private:
    ani_env *env_ = nullptr;
    ani_object obj_ = nullptr;
};

ani_object AniCreateProxyAsset(ani_env *env, const std::string &externalKey, const OHOS::CommonType::AssetValue &asset);
ani_object AniCreateAsset(ani_env *env, const OHOS::CommonType::AssetValue &asset);
ani_object AniCreateAssets(ani_env *env, const std::vector<OHOS::CommonType::AssetValue> &assets);
OHOS::ObjectStore::AssetBindInfo BindInfoToNative(const ::ohos::data::distributedDataObject::BindInfo &taiheBindInfo);
uint32_t TaiheStatusToNative(::ohos::data::commonType::AssetStatus status);
::taihe::array<::ohos::data::commonType::Asset> AssetsToTaihe(const std::vector<OHOS::CommonType::AssetValue> &values);

} //namespace AniUtils
#endif

