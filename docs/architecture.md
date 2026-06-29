# 架构说明

当任务需要仓库背景、需求设计上下文或影响面分析时，读取本页。

## 领域角色

`data_object` 是 OpenHarmony 分布式数据对象组件。它提供应用侧分布式数据管理与跨设备同步能力：
分布式数据对象创建、对象属性同步、跨设备数据订阅、Asset 与属性变更分离、内存数据持久化、
Save/RevokeSave 缓存管理。

本组件不是独立桌面库。它预期放在 OpenHarmony 源码树中构建：

```text
//foundation/distributeddatamgr/data_object
```

这很重要，因为许多实现依赖 OpenHarmony 服务：ability runtime、DistributedDB、
dsoftbus、device manager、IPC/samgr、access token、hilog、hitrace、
libuv、NAPI、ANI 和 system ability 基础设施。

## 分层模型

理解本仓库时，可以按以下层次思考：

```text
ArkTS/JavaScript public API (@ohos.data.dataObject)
        |
        | NAPI dynamic binding
        v
NAPI wrapper layer (jskitsimpl)
        |
        | static binding
        v
ANI / Taihe / generated ETS+ABC layer (frameworks/ets)
        |
        v
Core distributed object semantics and entity classes (innerkitsimpl)
        |
        v
DistributedDB, SoftBus, DeviceManager, IPC/samgr, libuv
```

不是每个 API 都经过所有层。主要 JS API 以 NAPI 为主，部分高级特性通过 ANI/Taihe
静态绑定路径实现。

## 主要构建 group

`bundle.json` 声明了组件公开构建 group：

- `//foundation/distributeddatamgr/data_object/interfaces/jskits:build_module`
- `//foundation/distributeddatamgr/data_object/interfaces/innerkits:distributeddataobject_impl`

它也声明了 inner kit，例如 `distributeddataobject_impl`、相关的 C++ 库和测试目标。

## Feature Flag

feature flag 位于 `data_object.gni`：

- `data_object_feature_L1`：控制 L1 设备功能特性编译。
- `data_object_api_metrics_enabled`：当全局构建包含 api metrics 部件时自动启用。

设计改动时，要确认行为是否需要覆盖不同设备形态（L1/L2/L3），还是只面向标准 OHOS 设备构建。

## 核心架构组件

分布式数据对象由以下核心组件构成：

### 1. 对象存储层
- **FlatObjectStore**：扁平化对象存储引擎，负责数据序列化与持久化
- **DistributedObjectImpl**：分布式对象实现，封装同步逻辑
- **ClientAdaptor**：客户端适配器，处理 IPC 通信

### 2. 同步机制
- **sessionId 管理**：用于标识不同会话，实现多设备协同
- **Asset/属性变更分离**：通过 WatcherProxy 实现不同类型变更的分离处理
- **DistributedDB 集成**：底层存储引擎，提供数据持久化和跨设备同步能力

### 3. 通信层
- **SoftBusAdapter**：软总线适配器，处理设备发现与数据传输
- **ProcessCommunicatorImpl**：进程间通信实现
- **DevMgrDriver**：设备管理驱动，处理设备上下线事件

### 4. 缓存管理
- **CacheManager**：Save/RevokeSave/Resume 流程管理
- **ObjectServiceProxy**：IPC 服务代理，与 datamgr_service 通信

## 影响面分析清单

实现前先回答这些问题：

- 改动影响哪个 API 面：NAPI 动态 JS、ANI 静态 TS、C++ InnerKit，还是仅测试？
- 目标行为是在共享 C++ core 中实现，还是只在 NAPI wrapper 中实现？
- 同一个操作是否有 NAPI 和 ANI 变体需要保持一致？
- 操作涉及 sessionId、对象属性、监听器、Asset、还是 IPC 调用？
- 错误需要 `objectstore_errors.h` 标准错误码映射，还是自定义处理？
- 操作是否涉及跨设备同步、数据持久化、Asset 变更分离、还是 Save/RevokeSave？
- 哪个 GN target 是覆盖该改动的最小有效构建或测试目标？

## 依赖方向

优先保持依赖向下流动：

- NAPI wrapper 负责解析输入、绑定 class、转换错误、管理监听器生命周期。
- C++ core 文件负责可复用语义，返回标准错误码或对象状态。
- entity class 持有运行期状态，例如 `DistributedObjectImpl`、`FlatObjectStore`、
  sessionId、监听器列表。
- utility library 承载通用 C++ helper 和日志工具。

除非确实是导出时初始化行为，否则不要把业务逻辑加到 module registration 文件里。

## 分布式特性约束

### sessionId 约束
- sessionId 格式必须合法，否则会导致对象创建失败
- **NEVER** 硬编码 sessionId，**MUST** 使用 `genSessionId()` 生成
- sessionId 用于标识跨设备会话，格式错误会导致同步失败

### 属性变更约束
- 仅支持根属性修改检测
- **NEVER** 直接修改嵌套属性（如 `obj.list.push(item)`）
- **MUST** 使用整体赋值触发同步（如 `obj.list = [...obj.list]`）

### 监听器管理约束
- 监听器注册 **NEVER** 使用箭头函数
- **MUST** 使用命名函数，页面销毁时调用 `off()` 移除
- 监听器内存泄漏是常见问题

### IPC 服务约束
- Save/RevokeSave 功能依赖 datamgr_service 进程
- **NEVER** 假设服务一定可用
- **MUST** 检查返回值，处理 `ERR_IPC` 错误码，实现重试机制
