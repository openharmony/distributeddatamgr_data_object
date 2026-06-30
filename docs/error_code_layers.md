# 错误码分层体系

本文档说明 data_object 组件的错误码分层和转换规则。

## 错误码架构

```
底层服务(DistributedDB/IPC) → Native层(rdb_errno.h) → JS/ETS API层(Error类)
```

**关键原则**:
1. 错误码从底层向顶层传播，不吞没
2. JS层不直接暴露C++错误码数值，使用Error类
3. 存量接口不能返回新错误码（向后兼容）

## Native层错误码

**位置**: `interfaces/innerkits/objectstore_errors.h`

**基础偏移**: `BASE_ERR_OFFSET = 1650`

| 错误码 | 值 | 说明 |
| --- | --- | --- |
| SUCCESS | 0 | 成功 |
| ERR_IPC | 1672 | IPC通信失败 |
| ERR_INVALID_ARGS | 1671 | 参数校验失败 |
| ERR_NO_PERMISSION | 1673 | 缺少权限 |
| ERR_NOMEM | 1654 | 内存分配失败 |
| ERR_DB_* | 1651-1660 | 数据库相关错误 |

## JS层错误类型

**位置**: `frameworks/jskitsimpl/include/common/object_error.h`

| Error类 | 对应Native错误码 | 业务错误码 |
| --- | --- | --- |
| PermissionError | ERR_NO_PERMISSION | 15400000 |
| ParametersType/Num | ERR_INVALID_ARGS | 401 |
| DatabaseError | ERR_DB_* | 15400001 |
| InnerError | 其他错误 | 15400099 |

## ETS层错误码

**位置**: `frameworks/ets/taihe/.../ani_error_utils.h`

| 错误码 | 值 | 说明 |
| --- | --- | --- |
| AniError_ParameterCheck | 401 | 参数校验失败 |
| AniError_ParameterError | 15400002 | 参数错误 |
| AniError_SessionJoined | 15400003 | Session已加入 |

## 新增错误码原则

### 允许

✅ Native层新增错误码（基于BASE_ERR_OFFSET偏移）
✅ 内部错误码映射到现有接口错误码

### 禁止

❌ 存量接口返回新错误码（破坏兼容性）
❌ 修改现有错误码含义
❌ JS层直接返回C++错误码数值

## 调试要点

- 检查错误码定义位置和值
- 验证跨层映射正确
- 确认存量接口错误码不变
- 添加充足日志上下文