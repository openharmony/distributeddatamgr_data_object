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
#define LOG_TAG "AniUtils"
#include "ani_utils.h"

#include "logger.h"

namespace ani_utils {
using namespace OHOS::ObjectStore;

using TaiheValueType = ::ohos::data::commonType::ValueType;

int32_t AniGetProperty(const ani_env *env, ani_object ani_obj, const char *property, std::string &result, bool optional)
{
    if (env == nullptr || ani_obj == nullptr || property == nullptr) {
        return ANI_INVALID_ARGS;
    }
    ani_object object = nullptr;
    int32_t status = AniGetProperty(env, ani_obj, property, object, optional);
    if (status == ANI_OK && object != nullptr) {
        result = AniStringUtils::ToStd(env, reinterpret_cast<ani_string>(object));
    }
    return status;
}

int32_t AniGetProperty(const ani_env *env, ani_object ani_obj, const char *property, bool &result, bool optional)
{
    if (env == nullptr || ani_obj == nullptr || property == nullptr) {
        return ANI_INVALID_ARGS;
    }
    ani_boolean ani_field_value;
    ani_status status = const_cast<ani_env*>(env)->Object_GetPropertyByName_Boolean(
        ani_obj, property, reinterpret_cast<ani_boolean*>(&ani_field_value));
    if (status != ANI_OK) {
        if (optional) {
            status = ANI_OK;
        }
        return status;
    }
    result = (bool)ani_field_value;
    return ANI_OK;
}

int32_t AniGetProperty(const ani_env *env, ani_object ani_obj, const char *property, int32_t &result, bool optional)
{
    if (env == nullptr || ani_obj == nullptr || property == nullptr) {
        return ANI_INVALID_ARGS;
    }
    ani_int ani_field_value;
    ani_status status = const_cast<ani_env*>(env)->Object_GetPropertyByName_Int(
        ani_obj, property, reinterpret_cast<ani_int*>(&ani_field_value));
    if (status != ANI_OK) {
        if (optional) {
            status = ANI_OK;
        }
        return status;
    }
    result = (int32_t)ani_field_value;
    return ANI_OK;
}

int32_t AniGetProperty(const ani_env *env, ani_object ani_obj, const char *property, uint32_t &result, bool optional)
{
    if (env == nullptr || ani_obj == nullptr || property == nullptr) {
        return ANI_INVALID_ARGS;
    }
    ani_int ani_field_value;
    ani_status status = const_cast<ani_env*>(env)->Object_GetPropertyByName_Int(
        ani_obj, property, reinterpret_cast<ani_int*>(&ani_field_value));
    if (status != ANI_OK) {
        if (optional) {
            status = ANI_OK;
        }
        return status;
    }
    result = (uint32_t)ani_field_value;
    return ANI_OK;
}

int32_t AniGetProperty(const ani_env *env, ani_object ani_obj, const char *property, ani_object &result, bool optional)
{
    if (env == nullptr || ani_obj == nullptr || property == nullptr) {
        return ANI_INVALID_ARGS;
    }
    ani_status status = const_cast<ani_env*>(env)->Object_GetPropertyByName_Ref(ani_obj, property,
        reinterpret_cast<ani_ref*>(&result));
    if (status != ANI_OK) {
        if (optional) {
            status = ANI_OK;
        }
        return status;
    }
    return ANI_OK;
}

std::string AniStringUtils::ToStd(const ani_env *env, ani_string ani_str)
{
    if (env == nullptr) {
        return std::string();
    }
    ani_size strSize = 0;
    auto status = const_cast<ani_env*>(env)->String_GetUTF8Size(ani_str, &strSize);
    if (ANI_OK != status) {
        LOG_INFO("String_GetUTF8Size failed");
        return std::string();
    }

    std::vector<char> buffer(strSize + 1);
    char *utf8Buffer = buffer.data();

    ani_size bytesWritten = 0;
    status = const_cast<ani_env*>(env)->String_GetUTF8(ani_str, utf8Buffer, strSize + 1, &bytesWritten);
    if (ANI_OK != status) {
        LOG_INFO("String_GetUTF8Size failed");
        return std::string();
    }

    utf8Buffer[bytesWritten] = '\0';
    std::string content = std::string(utf8Buffer);
    return content;
}

ani_string AniStringUtils::ToAni(const ani_env *env, const std::string& str)
{
    if (env == nullptr) {
        return nullptr;
    }
    ani_string aniStr = nullptr;
    if (ANI_OK != const_cast<ani_env*>(env)->String_NewUTF8(str.data(), str.size(), &aniStr)) {
        LOG_INFO("Unsupported ANI_VERSION_1");
        return nullptr;
    }
    return aniStr;
}

ani_status AniCreateBool(ani_env* env, bool value, ani_object& result)
{
    ani_status state;
    ani_class boolClass;
    if ((state = env->FindClass("std.core.Boolean", &boolClass)) != ANI_OK) {
        LOG_ERROR("FindClass std/core/Boolean failed, %{public}d", state);
        return state;
    }
    ani_method boolClassCtor;
    if ((state = env->Class_FindMethod(boolClass, "<ctor>", "z:", &boolClassCtor)) != ANI_OK) {
        LOG_ERROR("Class_FindMethod bool ctor failed, %{public}d", state);
        return state;
    }
    ani_boolean aniValue = value;
    if ((state = env->Object_New(boolClass, boolClassCtor, &result, aniValue)) != ANI_OK) {
        LOG_ERROR("New bool object failed, %{public}d", state);
    }
    if (state != ANI_OK) {
        result = nullptr;
    }
    return state;
}

ani_ref AniCreateByte(ani_env *env, uint8_t para)
{
    if (env == nullptr) {
        return nullptr;
    }
    static constexpr const char *className = "std.core.Byte";
    ani_class byteCls {};
    if (ANI_OK != env->FindClass(className, &byteCls)) {
        return nullptr;
    }
    ani_method ctor {};
    if (ANI_OK != env->Class_FindMethod(byteCls, "<ctor>", "b:", &ctor)) {
        return nullptr;
    }
    ani_object obj {};
    if (env->Object_New(byteCls, ctor, &obj, static_cast<ani_byte>(para)) != ANI_OK) {
        return nullptr;
    }
    return obj;
}

ani_status AniCreateInt(ani_env* env, int32_t value, ani_object& result)
{
    ani_status state;
    ani_class intClass;
    if ((state = env->FindClass("std.core.Int", &intClass)) != ANI_OK) {
        LOG_ERROR("FindClass std/core/Int failed, %{public}d", state);
        return state;
    }
    ani_method intClassCtor;
    if ((state = env->Class_FindMethod(intClass, "<ctor>", "i:", &intClassCtor)) != ANI_OK) {
        LOG_ERROR("Class_FindMethod Int ctor failed, %{public}d", state);
        return state;
    }
    ani_int aniValue = value;
    if ((state = env->Object_New(intClass, intClassCtor, &result, aniValue)) != ANI_OK) {
        LOG_ERROR("New Int object failed, %{public}d", state);
    }
    if (state != ANI_OK) {
        result = nullptr;
    }
    return state;
}

ani_status AniCreateDouble(ani_env* env, double value, ani_object& result)
{
    ani_status state;
    ani_class doubleClass;
    if ((state = env->FindClass("std.core.Double", &doubleClass)) != ANI_OK) {
        LOG_ERROR("FindClass std/core/Double failed, %{public}d", state);
        return state;
    }
    ani_method doubleClassCtor;
    if ((state = env->Class_FindMethod(doubleClass, "<ctor>", "d:", &doubleClassCtor)) != ANI_OK) {
        LOG_ERROR("Class_FindMethod Double ctor failed, %{public}d", state);
        return state;
    }
    ani_double aniValue = value;
    if ((state = env->Object_New(doubleClass, doubleClassCtor, &result, aniValue)) != ANI_OK) {
        LOG_ERROR("New Double object failed, %{public}d", state);
    }
    if (state != ANI_OK) {
        result = nullptr;
    }
    return state;
}

ani_string AniCreateString(ani_env *env, std::string const& para)
{
    if (env == nullptr) {
        return {};
    }
    ani_string ani_string_result = nullptr;
    if (env->String_NewUTF8(para.c_str(), para.size(), &ani_string_result) != ANI_OK) {
        return {};
    }
    return ani_string_result;
}

ani_method AniGetMethod(ani_env *env, ani_class cls, const char* methodName, const char* signature)
{
    if (env == nullptr) {
        return nullptr;
    }
    ani_method retMethod {};
    if (ANI_OK != env->Class_FindMethod(cls, methodName, signature, &retMethod)) {
        return nullptr;
    }
    return retMethod;
}

ani_class AniGetClass(ani_env *env, const char* className)
{
    if (env == nullptr) {
        return nullptr;
    }
    ani_class cls {};
    if (ANI_OK != env->FindClass(className, &cls)) {
        return nullptr;
    }
    return cls;
}

ani_method AniGetClassMethod(ani_env *env, const char* className, const char* methodName, const char* signature)
{
    ani_class retClass = AniGetClass(env, className);
    if (retClass == nullptr) {
        return nullptr;
    }
    ani_method retMethod {};
    if (ANI_OK != env->Class_FindMethod(retClass, methodName, signature, &retMethod)) {
        return nullptr;
    }
    return retMethod;
}

ani_object AniCreatEmptyRecord(ani_env* env, ani_method& setMethod)
{
    ani_method constructor = ani_utils::AniGetClassMethod(env, "escompat.Record", "<ctor>", ":");
    ani_method mapSetMethod = ani_utils::AniGetClassMethod(env, "escompat.Record", "$_set", nullptr);
    if (constructor == nullptr || mapSetMethod == nullptr) {
        LOG_ERROR("AniGetClassMethod escompat.Record find method failed");
        return nullptr;
    }
    ani_object ani_record_result = nullptr;
    if (ANI_OK != env->Object_New(ani_utils::AniGetClass(env, "escompat.Record"), constructor, &ani_record_result)) {
        LOG_ERROR("escompat.Record Object_New failed");
        return nullptr;
    }
    setMethod = mapSetMethod;
    return ani_record_result;
}

ani_array AniCreateEmptyAniArray(ani_env *env, uint32_t size)
{
    if (env == nullptr) {
        LOG_ERROR("create ani array env is null");
        return nullptr;
    }
    ani_ref undefinedRef = nullptr;
    if (ANI_OK != env->GetUndefined(&undefinedRef)) {
        LOG_ERROR("GetUndefined Failed.");
    }
    ani_array resultArray;
    env->Array_New(size, undefinedRef, &resultArray);
    return resultArray;
}

ani_object AniCreateArray(ani_env *env, std::vector<ani_object> const& objectArray)
{
    ani_array array = AniCreateEmptyAniArray(env, objectArray.size());
    if (array == nullptr) {
        LOG_ERROR("Create array failed");
        return nullptr;
    }
    ani_size index = 0;
    for (auto &aniItem : objectArray) {
        if (ANI_OK != env->Array_Set(array, index, aniItem)) {
            LOG_ERROR("Set array failed, index=%{public}zu", index);
            return nullptr;
        }
        index++;
    }
    return array;
}

bool AniMapSet(ani_env *env, ani_object map, ani_method mapSetMethod, const char* key, ani_ref value)
{
    if (env == nullptr || map == nullptr || mapSetMethod == nullptr
        || key == nullptr || key[0] == 0 || value == nullptr) {
        return false;
    }
    ani_ref keyref = AniCreateString(env, std::string(key));
    if (keyref == nullptr) {
        LOG_ERROR("AniCreateString failed");
        return false;
    }
    if (ANI_OK != env->Object_CallMethod_Void(map, mapSetMethod, keyref, value)) {
        LOG_ERROR("Object_CallMethod_Void failed");
        return false;
    }
    return true;
}

bool AniMapSet(ani_env *env, ani_object map, ani_method mapSetMethod, const char* key, std::string const& valueStr)
{
    if (valueStr.empty()) {
        return false;
    }
    ani_ref valueRef = AniCreateString(env, valueStr);
    if (valueRef == nullptr) {
        LOG_ERROR("AniCreateString failed");
        return false;
    }
    return AniMapSet(env, map, mapSetMethod, key, valueRef);
}

template<>
bool UnionAccessor::IsInstanceOfType<bool>()
{
    return IsInstanceOf("std.core.Boolean");
}

template<>
bool UnionAccessor::IsInstanceOfType<int>()
{
    return IsInstanceOf("std.core.Int");
}

template<>
bool UnionAccessor::IsInstanceOfType<int64_t>()
{
    return IsInstanceOf("std.core.Long");
}

template<>
bool UnionAccessor::IsInstanceOfType<double>()
{
    return IsInstanceOf("std.core.Double");
}

template<>
bool UnionAccessor::IsInstanceOfType<std::string>()
{
    return IsInstanceOf("std.core.String");
}

template<>
bool UnionAccessor::TryConvertArray<ani_ref>(std::vector<ani_ref> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        LOG_ERROR("Object_GetPropertyByName_Double length failed");
        return false;
    }
    for (int i = 0; i < int(length); i++) {
        ani_ref ref;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "i:C{std.core.Object}", &ref, (ani_int)i)) {
            LOG_ERROR("Object_GetPropertyByName_Ref failed");
            return false;
        }
        value.push_back(ref);
    }
    LOG_DEBUG("convert ref array ok.");
    return true;
}

template<>
bool UnionAccessor::TryConvertArray<bool>(std::vector<bool> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        LOG_ERROR("Object_GetPropertyByName_Double length failed");
        return false;
    }
    for (int i = 0; i < int(length); i++) {
        ani_ref ref;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "i:C{std.core.Object}", &ref, (ani_int)i)) {
            LOG_ERROR("Object_GetPropertyByName_Ref failed");
            return false;
        }
        ani_boolean val;
        if (ANI_OK != env_->Object_CallMethodByName_Boolean(static_cast<ani_object>(ref), "toBoolean", nullptr, &val)) {
            LOG_ERROR("Object_CallMethodByName_Boolean toBoolean failed");
            return false;
        }
        value.push_back(static_cast<bool>(val));
    }
    LOG_DEBUG("convert bool array ok.");
    return true;
}

template<>
bool UnionAccessor::TryConvertArray<int>(std::vector<int> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        LOG_ERROR("Object_GetPropertyByName_Double length failed");
        return false;
    }
    for (int i = 0; i < int(length); i++) {
        ani_ref ref;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "i:C{std.core.Object}", &ref, (ani_int)i)) {
            LOG_ERROR("Object_GetPropertyByName_Ref failed");
            return false;
        }
        ani_int intValue;
        if (ANI_OK != env_->Object_CallMethodByName_Int(static_cast<ani_object>(ref), "toInt", nullptr, &intValue)) {
            LOG_ERROR("Object_CallMethodByName_Int toInt failed");
            return false;
        }
        value.push_back(static_cast<int>(intValue));
    }
    LOG_DEBUG("convert int array ok.");
    return true;
}

template<>
bool UnionAccessor::TryConvertArray<double>(std::vector<double> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        LOG_ERROR("Object_GetPropertyByName_Double length failed");
        return false;
    }
    for (int i = 0; i < int(length); i++) {
        ani_ref ref;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "i:C{std.core.Object}", &ref, (ani_int)i)) {
            LOG_ERROR("Object_GetPropertyByName_Ref failed");
            return false;
        }
        ani_double val;
        if (ANI_OK != env_->Object_CallMethodByName_Double(static_cast<ani_object>(ref), "toDouble", nullptr, &val)) {
            LOG_ERROR("Object_CallMethodByName_Double toDouble failed");
            return false;
        }
        value.push_back(static_cast<double>(val));
    }
    LOG_DEBUG("convert double array ok.");
    return true;
}

template<>
bool UnionAccessor::TryConvertArray<uint8_t>(std::vector<uint8_t> &value)
{
    LOG_ERROR("TryConvertArray vector<uint8_t> 5");

    ani_ref buffer;
    if (ANI_OK != env_->Object_GetFieldByName_Ref(obj_, "buffer", &buffer)) {
        LOG_ERROR("Object_GetFieldByName_Ref failed");
        return false;
    }
    LOG_ERROR("TryConvertArray vector<uint8_t> 6");
    void* data;
    size_t size;
    if (ANI_OK != env_->ArrayBuffer_GetInfo(static_cast<ani_arraybuffer>(buffer), &data, &size)) {
        LOG_ERROR("ArrayBuffer_GetInfo failed");
        return false;
    }
    LOG_ERROR("TryConvertArray vector<uint8_t> 7");
    for (size_t i = 0; i < size; i++) {
        value.push_back(static_cast<uint8_t*>(data)[i]);
    }
    LOG_DEBUG("convert uint8 array ok.");
    return true;
}

template<>
bool UnionAccessor::TryConvertArray<float>(std::vector<float> &value)
{
    ani_ref buffer;
    if (ANI_OK != env_->Object_GetFieldByName_Ref(obj_, "buffer", &buffer)) {
        LOG_ERROR("Object_GetFieldByName_Ref failed");
        return false;
    }
    void* data;
    size_t size;
    if (ANI_OK != env_->ArrayBuffer_GetInfo(static_cast<ani_arraybuffer>(buffer), &data, &size)) {
        LOG_ERROR("ArrayBuffer_GetInfo failed");
        return false;
    }
    auto count = size / sizeof(float);
    for (size_t i = 0; i < count; i++) {
        value.push_back(static_cast<uint8_t*>(data)[i]);
    }
    LOG_DEBUG("convert float array ok.");
    return true;
}

template<>
bool UnionAccessor::TryConvertArray<std::string>(std::vector<std::string> &value)
{
    ani_double length;
    if (ANI_OK != env_->Object_GetPropertyByName_Double(obj_, "length", &length)) {
        LOG_ERROR("Object_GetPropertyByName_Double length failed");
        return false;
    }
    for (int i = 0; i < int(length); i++) {
        ani_ref ref;
        if (ANI_OK != env_->Object_CallMethodByName_Ref(obj_, "$_get", "i:C{std.core.Object}", &ref, (ani_int)i)) {
            LOG_ERROR("Object_CallMethodByName_Ref failed");
            return false;
        }
        value.push_back(AniStringUtils::ToStd(env_, static_cast<ani_string>(ref)));
    }
    LOG_DEBUG("convert string array ok.");
    return true;
}

template<>
bool UnionAccessor::TryConvert<int>(int &value)
{
    if (!IsInstanceOfType<int>()) {
        return false;
    }

    ani_int aniValue;
    auto ret = env_->Object_CallMethodByName_Int(obj_, "toInt", nullptr, &aniValue);
    if (ret != ANI_OK) {
        LOG_ERROR("toInt failed");
        return false;
    }
    value = static_cast<int>(aniValue);
    LOG_DEBUG("convert int ok.");
    return true;
}

template<>
bool UnionAccessor::TryConvert<std::monostate>(std::monostate &value)
{
    ani_boolean isNull = false;
    auto status = env_->Reference_IsNull(static_cast<ani_ref>(obj_), &isNull);
    if (ANI_OK == status) {
        if (isNull) {
            value = std::monostate();
            LOG_DEBUG("convert null ok.");
            return true;
        }
    }
    return false;
}

template<>
bool UnionAccessor::TryConvert<int64_t>(int64_t &value)
{
    return false;
}

template<>
bool UnionAccessor::TryConvert<double>(double &value)
{
    if (!IsInstanceOfType<double>()) {
        return false;
    }

    ani_double aniValue;
    auto ret = env_->Object_CallMethodByName_Double(obj_, "toDouble", nullptr, &aniValue);
    if (ret != ANI_OK) {
        LOG_ERROR("toDouble failed");
        return false;
    }
    value = static_cast<double>(aniValue);
    LOG_DEBUG("convert double ok.");
    return true;
}

template<>
bool UnionAccessor::TryConvert<std::string>(std::string &value)
{
    if (!IsInstanceOfType<std::string>()) {
        return false;
    }

    value = AniStringUtils::ToStd(env_, static_cast<ani_string>(obj_));
    LOG_DEBUG("convert string ok.");
    return true;
}

template<>
bool UnionAccessor::TryConvert<bool>(bool &value)
{
    if (!IsInstanceOfType<bool>()) {
        return false;
    }

    ani_boolean aniValue;
    auto ret = env_->Object_CallMethodByName_Boolean(obj_, "toBoolean", nullptr, &aniValue);
    if (ret != ANI_OK) {
        LOG_ERROR("toBoolean failed");
        return false;
    }
    value = static_cast<bool>(aniValue);
    LOG_DEBUG("convert bool ok.");
    return true;
}

template<>
bool UnionAccessor::TryConvert<std::vector<uint8_t>>(std::vector<uint8_t> &value)
{
    LOG_ERROR("TryConvert vector<uint8_t> 1");
    if (!IsInstanceOf("escompat.Uint8Array")) {
        return false;
    }
    LOG_ERROR("TryConvert vector<uint8_t> 2");
    return TryConvertArray(value);
}

template<>
bool UnionAccessor::TryConvert<std::vector<float>>(std::vector<float> &value)
{
    if (!IsInstanceOf("escompat.Float32Array")) {
        return false;
    }
    return TryConvertArray(value);
}

template<>
bool UnionAccessor::TryConvert<std::vector<ani_ref>>(std::vector<ani_ref> &value)
{
    if (!IsInstanceOf("escompat.Array")) {
        return false;
    }
    return TryConvertArray(value);
}

template<>
bool UnionAccessor::TryConvert<OHOS::CommonType::Asset>(OHOS::CommonType::Asset &value)
{
    std::string clsName = "std.core.Object";
    if (!IsInstanceOf(clsName)) {
        LOG_ERROR("xxxerr TryConvert 1");
        return false;
    }
    OHOS::CommonType::Asset tmp;
    if (ANI_OK != AniGetProperty(env_, obj_, "name", tmp.name)) {
        LOG_ERROR("xxxerr TryConvert 2");
        return false;
    }
    if (ANI_OK != AniGetProperty(env_, obj_, "uri", tmp.uri)) {
        LOG_ERROR("xxxerr TryConvert 3");
        return false;
    }
    if (ANI_OK != AniGetProperty(env_, obj_, "path", tmp.path)) {
        LOG_ERROR("xxxerr TryConvert 4");
        return false;
    }
    if (ANI_OK != AniGetProperty(env_, obj_, "createTime", tmp.createTime)) {
        LOG_ERROR("xxxerr TryConvert 5");
        return false;
    }
    if (ANI_OK != AniGetProperty(env_, obj_, "modifyTime", tmp.modifyTime)) {
        LOG_ERROR("xxxerr TryConvert 6");
        return false;
    }
    if (ANI_OK != AniGetProperty(env_, obj_, "size", tmp.size)) {
        LOG_ERROR("xxxxerr TryConvert 7");
        return false;
    }
    ani_int enumVal = 0;
    if (ANI_OK != AniGetProperty(env_, obj_, "status", enumVal, true)) {
        LOG_ERROR("xxxxerr TryConvert 8");
        return false;
    }
    tmp.status = static_cast<OHOS::CommonType::Asset::Status>(enumVal);
    value = tmp;
    return true;
}

template<>
bool UnionAccessor::TryConvert<std::vector<OHOS::CommonType::Asset>>(std::vector<OHOS::CommonType::Asset> &value)
{
    std::string clsName = "A{C{std.core.Object}}";
    if (!IsInstanceOf(clsName)) {
        return false;
    }
    ani_size arrayLength = 0;
    auto status = env_->Array_GetLength(static_cast<ani_array>(obj_), &arrayLength);
    if (status != ANI_OK) {
        LOG_ERROR("Array_GetLength failed");
        return false;
    }
    for (ani_size i = 0; i < arrayLength; i++) {
        ani_ref result;
        status = env_->Array_Get(static_cast<ani_array>(obj_), i, &result);
        if (status != ANI_OK) {
            LOG_ERROR("Array_Get failed");
            return false;
        }
        ani_object asset = static_cast<ani_object>(result);
        UnionAccessor sub(env_, asset);
        OHOS::CommonType::Asset val;
        auto isOk = sub.TryConvert(val);
        if (!isOk) {
            return false;
        }
        value.push_back(val);
    }
    return true;
}

bool AniGetMapItem(ani_env *env, ::taihe::map_view<::taihe::string, uintptr_t> const& taiheMap,
    const char* key, std::string& value)
{
    if (env == nullptr) {
        return false;
    }
    std::string stdKey(key);
    auto iter = taiheMap.find_item(stdKey);
    if (iter == taiheMap.end()) {
        LOG_ERROR("find_item %{public}s failed", stdKey.c_str());
        return false;
    }
    ani_object aniobj = reinterpret_cast<ani_object>(iter->second);
    UnionAccessor access(env, aniobj);
    bool result = access.TryConvert(value);
    return result;
}

bool AniGetMapItem(ani_env *env, ::taihe::map_view<::taihe::string, uintptr_t> const& taiheMap,
    const char* key, int32_t& value)
{
    if (env == nullptr) {
        return false;
    }
    std::string stdKey(key);
    auto iter = taiheMap.find_item(stdKey);
    if (iter == taiheMap.end()) {
        LOG_ERROR("find_item %{public}s failed", stdKey.c_str());
        return false;
    }
    ani_object aniobj = reinterpret_cast<ani_object>(iter->second);
    UnionAccessor access(env, aniobj);
    bool result = access.TryConvert(value);
    return result;
}

void AniExecuteFunc(ani_vm* vm, const std::function<void(ani_env*)> func)
{
    LOG_INFO("AniExecutePromise");
    if (vm == nullptr) {
        LOG_ERROR("AniExecutePromise, vm error");
        return;
    }
    ani_env *currentEnv = nullptr;
    ani_status aniResult = vm->GetEnv(ANI_VERSION_1, &currentEnv);
    if (ANI_OK == aniResult && currentEnv != nullptr) {
        LOG_INFO("AniExecutePromise, env exist");
        func(currentEnv);
        return;
    }

    ani_env* newEnv = nullptr;
    ani_options aniArgs { 0, nullptr };
    aniResult = vm->AttachCurrentThread(&aniArgs, ANI_VERSION_1, &newEnv);
    if (ANI_OK != aniResult || newEnv == nullptr) {
        LOG_ERROR("AniExecutePromise, AttachCurrentThread error");
        return;
    }
    func(newEnv);
    aniResult = vm->DetachCurrentThread();
    if (ANI_OK != aniResult) {
        LOG_ERROR("AniExecutePromise, DetachCurrentThread error");
        return;
    }
}

ani_object AniCreateAsset(ani_env *env, OHOS::CommonType::AssetValue const& asset)
{
    if (env == nullptr) {
        return {};
    }
    ani_object ani_field_name = AniCreateString(env, asset.name);
    ani_object ani_field_uri = AniCreateString(env, asset.uri);
    ani_object ani_field_path = AniCreateString(env, asset.path);
    ani_object ani_field_createTime = AniCreateString(env, asset.createTime);
    ani_object ani_field_modifyTime = AniCreateString(env, asset.modifyTime);
    ani_object ani_field_size = AniCreateString(env, asset.size);
    ani_ref ani_field_status;
    env->GetUndefined(&ani_field_status);
    if (asset.status != OHOS::CommonType::AssetValue::Status::STATUS_UNKNOWN) {
        ani_enum_item ani_enum_status = {};
        int32_t status = (int32_t)asset.status;
        if (ANI_OK == env->FindEnum("@ohos.data.commonType.commonType.AssetStatus", &ani_enum_status)) {
            env->Enum_GetEnumItemByIndex(ani_enum_status, static_cast<ani_size>(status), &ani_field_status);
        }
    }
    ani_class cls = ani_utils::AniGetClass(env,
        "@ohos.data.commonType.commonType._taihe_Asset_inner");
    ani_method method = ani_utils::AniGetClassMethod(env,
        "@ohos.data.commonType.commonType._taihe_Asset_inner", "<ctor>", nullptr);
    ani_object ani_obj = {};
    env->Object_New(cls, method, &ani_obj, ani_field_name, ani_field_uri, ani_field_path,
        ani_field_createTime, ani_field_modifyTime, ani_field_size, ani_field_status);
    return ani_obj;
}

ani_object AniCreateAssets(ani_env *env, std::vector<OHOS::CommonType::AssetValue> const& assets)
{
    if (env == nullptr) {
        return {};
    }
    std::vector<ani_object> objectArray;
    for (size_t index = 0; index < assets.size(); index++) {
        ani_object aniAsset = AniCreateAsset(env, assets[index]);
        objectArray.push_back(aniAsset);
    }
    return AniCreateArray(env, objectArray);
}

OHOS::ObjectStore::AssetBindInfo BindInfoToNative(::ohos::data::distributedDataObject::BindInfo const& taiheBindInfo)
{
    OHOS::ObjectStore::AssetBindInfo nativeBindInfo;
    nativeBindInfo.storeName = std::string(taiheBindInfo.storeName);
    nativeBindInfo.tableName = std::string(taiheBindInfo.tableName);
    nativeBindInfo.field = std::string(taiheBindInfo.field);
    nativeBindInfo.assetName = std::string(taiheBindInfo.assetName);
    nativeBindInfo.primaryKey = ValuesBucketToNative(taiheBindInfo.primaryKey);
    return nativeBindInfo;
}

} //namespace ani_utils