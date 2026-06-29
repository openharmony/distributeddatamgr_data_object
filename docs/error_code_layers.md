# 错误码分层体系

本文档描述 distributeddatamgr_data_object 组件中错误码的定义、分层和转换规则。

## 概述

data_object 组件使用分层错误码体系，不同层次定义和维护各自的错误码：

```
┌─────────────────────────────────────────────┐
│           JS/ETS API 层                      │
│  (业务错误码: 15400000+ / 401)              │
└─────────────────────────────────────────────┘
                    ↓ 映射
┌─────────────────────────────────────────────┐
│         C++ InnerKit 层                      │
│  (核心错误码: 1650 + offset)                 │
└─────────────────────────────────────────────┘
                    ↓ 包装
┌─────────────────────────────────────────────┐
│        JS 工具层 (object_error.h)           │
│  (Error 类层次结构)                         │
└─────────────────────────────────────────────┘
                    ↓ 转换
┌─────────────────────────────────────────────┐
│         底层服务层                           │
│  (DistributedDB / IPC 错误)                │
└─────────────────────────────────────────────┘
```

## C++ 核心错误码（Native 层）

**定义位置**: `interfaces/innerkits/objectstore_errors.h`

### 基础偏移量

```cpp
constexpr uint32_t BASE_ERR_OFFSET = 1650;
```

### 核心错误码列表

| 错误码 | 值 | 说明 | 使用场景 |
| --- | --- | --- | --- |
| `SUCCESS` | 0 | 执行成功 | 所有操作成功返回 |
| `ERR_DB_SET_PROCESS` | 1651 | 数据库设置失败 | 分布式 DB 初始化失败 |
| `ERR_EXIST` | 1652 | 已存在 | 对象/数据已存在 |
| `ERR_DATA_LEN` | 1653 | 数据长度非法 | 数据超长或格式错误 |
| `ERR_NOMEM` | 1654 | 内存分配失败 | new/malloc 失败 |
| `ERR_DB_NOT_INIT` | 1655 | 数据库未初始化 | DB 未初始化就访问 |
| `ERR_DB_GETKV_FAIL` | 1656 | KV 存取失败 | KVStore 操作失败 |
| `ERR_DB_NOT_EXIST` | 1657 | 数据库不存在 | DB 文件缺失 |
| `ERR_DB_GET_FAIL` | 1658 | 获取数据失败 | 读操作失败 |
| `ERR_DB_ENTRY_FAIL` | 1659 | 数据库条目失败 | Entry 操作失败 |
| `ERR_CLOSE_STORAGE` | 1660 | 关闭存储失败 | 关闭 DB 失败 |
| `ERR_NULL_OBJECT` | 1661 | 对象为空 | 空指针检查 |
| `ERR_REGISTER` | 1662 | 注册失败 | 监听器注册失败 |
| `ERR_NULL_OBJECTSTORE` | 1663 | ObjectStore 为空 | 空指针检查 |
| `ERR_GET_OBJECT` | 1664 | 获取对象失败 | 对象查询失败 |
| `ERR_NO_OBSERVER` | 1665 | 未注册 | 监听器未注册 |
| `ERR_UNREGISTER` | 1666 | 注销失败 | 监听器注销失败 |
| `ERR_SINGLE_DEVICE` | 1667 | 仅单设备 | 单设备环境 |
| `ERR_NULL_PTR` | 1668 | 空指针 | 通用空指针检查 |
| `ERR_PROCESSING` | 1669 | 处理失败 | 通用处理失败 |
| `ERR_RESULTSET` | 1670 | ResultSet 失败 | 查询结果集错误 |
| `ERR_INVALID_ARGS` | 1671 | 参数错误 | 参数校验失败 |
| `ERR_IPC` | 1672 | IPC 错误 | 进程间通信失败 |
| `ERR_NO_PERMISSION` | 1673 | 无权限 | 缺少 DATA_SYNC 权限 |

### 错误码使用规则

1. **新增错误码**：在 `BASE_ERR_OFFSET + 23` 之后递增定义
2. **错误码传播**：从底层向顶层传播，使用标准错误码
3. **错误码兼容性**：公共 API 错误码不能修改或删除
4. **文档更新**：新增错误码时更新本文档

### 典型使用示例

```cpp
#include "objectstore_errors.h"

// 返回成功
return SUCCESS;

// 返回 IPC 错误
if (ipcResult != IPC_OK) {
    return ERR_IPC;
}

// 参数校验失败
if (sessionId.empty()) {
    return ERR_INVALID_ARGS;
}
```

## JS 工具层错误类型

**定义位置**: `frameworks/jskitsimpl/include/common/object_error.h`

### Error 类层次结构

```
Error (抽象基类)
├── ParametersType      - 参数类型错误
├── ParametersNum       - 参数数量错误
├── PermissionError     - 权限错误
├── DatabaseError       - 数据库错误
├── InnerError          - 内部错误
└── DeviceNotSupportedError - 设备不支持错误
```

### 错误类说明

| 类 | 使用场景 | 映射到核心错误码 |
| --- | --- | --- |
| `Error` | 错误基类，定义 GetCode()/GetMessage() | - |
| `ParametersType` | JS 参数类型不匹配 | `ERR_INVALID_ARGS` |
| `ParametersNum` | JS 参数数量不正确 | `ERR_INVALID_ARGS` |
| `PermissionError` | 缺少必要权限 | `ERR_NO_PERMISSION` |
| `DatabaseError` | 数据库操作失败 | `ERR_DB_*` 系列 |
| `InnerError` | 内部处理失败 | `ERR_PROCESSING` |
| `DeviceNotSupportedError` | 设备不支持 | `ERR_SINGLE_DEVICE` |

### JS 错误码常量

```cpp
namespace OHOS::ObjectStore {
    static constexpr int EXCEPTION_INNER = 0;
}
```

### 使用示例

```cpp
#include "object_error.h"

// 参数类型错误
throw ParametersType("sessionId", "string");

// 参数数量错误
throw ParametersNum("at least 1");

// 权限错误
throw PermissionError();
```

### 使用规则

1. **类别优先**：先选择类别基码，再考虑具体错误码
2. **跨层转换**：错误码不应直接暴露到 JS API

## ETS/Taihe 层错误码

**定义位置**: `frameworks/ets/taihe/ohos.data.distributedDataObject/include/ani_error_utils.h`

### AniError 枚举

| 错误码 | 值 | 说明 |
| --- | --- | --- |
| `AniError_ParameterCheck` | 401 | 参数校验失败 |
| `AniError_ParameterError` | 15400002 | 参数错误（业务错误码） |
| `AniError_SessionJoined` | 15400003 | Session 已加入 |

### 使用工具函数

```cpp
void ThrowError(const char* message);                      // 抛出默认错误
void ThrowError(int32_t code, const std::string &message);  // 抛出带错误码
ani_object ToBusinessError(ani_env *env, int32_t code, const std::string &message); // 转换为业务错误对象
```

### 使用示例

```cpp
#include "ani_error_utils.h"

// 抛出参数校验错误
ThrowError(AniError_ParameterCheck, "Parameter check failed");

// 抛出业务错误
ThrowError(AniError_SessionJoined, "Session already joined");
```

## 错误码转换规则

### Native → JS 转换

```
C++ 错误码 → JS Error 类型
┌─────────────────────┬─────────────────────────────┐
│ ERR_NO_PERMISSION   │ → PermissionError           │
│ ERR_INVALID_ARGS    │ → ParametersType/Num        │
│ ERR_DB_*            │ → DatabaseError             │
│ ERR_*               │ → InnerError                │
└─────────────────────┴─────────────────────────────┘
```

### JS → 业务错误码转换

```
JS Error → OpenHarmony 业务错误码
┌─────────────────────┬─────────────────────────────┐
│ PermissionError     │ → 15400000 (无权限)          │
│ ParameterError      │ → 401 (参数错误)             │
│ DatabaseError       │ → 15400001 (数据库错误)       │
│ InnerError          │ → 15400099 (内部错误)         │
└─────────────────────┴─────────────────────────────┘
```

## 错误处理最佳实践

### C++ 层

1. **始终返回标准错误码**：使用 `objectstore_errors.h` 中定义的常量
2. **错误码向上传播**：不要在中间层吞没错误
3. **详细日志**：错误发生时记录足够上下文

```cpp
uint32_t FlatObjectStore::Put(const std::string &key, const std::value &value)
{
    if (key.empty()) {
        LOG_ERROR("Empty key in Put");
        return ERR_INVALID_ARGS;
    }
    // ... 实现代码
    return SUCCESS;
}
```

### JS 层

1. **使用 Error 类层次**：不要直接抛出数字错误码
2. **错误消息国际化**：提供清晰的错误描述
3. **保持兼容性**：公共 API 的错误类型不能轻易改变

```javascript
// 正确做法
if (!sessionId || typeof sessionId !== 'string') {
    throw new BusinessError(401, 'Invalid sessionId type');
}

// 错误做法 - 不要这样做
if (error) {
    return 1651; // 不要直接返回 C++ 错误码
}
```

### ETS 层

1. **使用 AniErrorUtils**：通过工具函数抛出错误
2. **转换为业务错误对象**：使用 ToBusinessError

```cpp
if (!CheckSessionId(sessionId)) {
    ThrowError(AniError_ParameterCheck, "Invalid sessionId format");
}
```

## 错误码扩展指南

### 新增 Native 错误码

1. 在 `objectstore_errors.h` 中添加定义：
   ```cpp
   /**
    * @brief 新功能失败描述
    */
   constexpr uint32_t ERR_NEW_FEATURE = BASE_ERR_OFFSET + 24;
   ```

2. 在代码中使用：
   ```cpp
   return ERR_NEW_FEATURE;
   ```

3. 更新本文档的"核心错误码列表"表格

### 新增 JS Error 类型

1. 在 `object_error.h` 中定义类：
   ```cpp
   class NewFeatureError : public Error {
   public:
       NewFeatureError() = default;
       std::string GetMessage() override;
       int GetCode() override;
   };
   ```

2. 在对应的 `.cpp` 文件中实现

3. 在 JS 绑定中注册错误类型

## 相关文件速查

| 文件 | 路径 | 用途 |
| --- | --- | --- |
| 核心错误码定义 | `interfaces/innerkits/objectstore_errors.h` | C++ 核心层错误码 |
| JS Error 类 | `frameworks/jskitsimpl/include/common/object_error.h` | JS 层错误类型 |
| ETS 错误工具 | `frameworks/ets/taihe/.../ani_error_utils.h` | ETS 层错误处理 |
