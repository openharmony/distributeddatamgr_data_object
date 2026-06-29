# AGENTS.md

本文件是 AI Agent 处理本仓库任务时的轻量入口。先读本文件，再按任务类型只加载匹配的详细文档页。

## 阅读策略

不要一开始就读取 `docs/` 下的所有文件。

默认只读本文件。涉及需求设计或代码开发时，最多按需加载：

1. 如果影响范围不清楚，读取 `docs/architecture.md` 或 `docs/code-map.md`。
1. 读取一个与任务领域匹配的专题页。
2. 规划验证时，读取 `docs/testing.md` 。

本仓库内容较多，一次性加载全部背景知识会浪费上下文，也会降低后续实现的精度。

## 仓库定位

`distributeddatamgr_data_object` 是 OpenHarmony 分布式数据对象组件。在 OpenHarmony 源码树中的位置是：

```text
//foundation/distributeddatamgr/data_object
```

组件元信息：

- 子系统：`distributeddatamgr`
- 部件：`data_object`
- Bundle：`@ohos.data.distributedDataObject`
- 主要能力面：分布式数据对象管理、跨设备数据同步、内存数据持久化、
  Asset 与属性变更分离、Save/RevokeSave 缓存管理。

主要实现语言是 C++17，通过 NAPI 暴露到 JavaScript/ArkTS，并通过 ANI (Taihe) 支持 ETS。
本仓库同时包含 TypeScript 包装、C++ InnerKit、测试代码。

## 快速代码地图

- `bundle.json`：组件元数据、依赖列表、InnerKit 导出、测试目标。
- `data_object.gni`：共享 GN 变量，例如 `data_object_feature_L1`、
  `data_object_api_metrics_enabled`。
- `interfaces/innerkits/`：C++ InnerKit 接口，包含 `distributed_object.h`、
  `distributed_objectstore.h`、`objectstore_errors.h`、`object_types.h`。
- `interfaces/jskits/`：JavaScript API 层，是 `@ohos.data.distributedDataObject` 的主要实现区。
- `frameworks/innerkitsimpl/`：C++ 核心实现层。
  - `adaptor/`：对象存储与同步核心，包含 `FlatObjectStore`、
    `DistributedObjectImpl`、`ClientAdaptor`。
  - `communicator/`：设备通信与发现，包含 `SoftBusAdapter`、
    `ProcessCommunicatorImpl`、`DevMgrDriver`。
  - `WatcherProxy/`：Asset 与属性变更分离机制。
  - `CacheManager/`：Save/RevokeSave/Resume 缓存管理。
- `frameworks/jskitsimpl/`：JS 工具类与适配器，包含 NAPI 绑定。
- `frameworks/ets/`：ETS (Taihe) 实现，包含 ANI 静态绑定。
- `samples/distributedNotepad/`：分布式备忘录示例应用。
- `frameworks/*/test/unittest/`：C++/JS 单元测试和 mock。
- `frameworks/*/test/fuzztest/`：fuzz 测试。

## 知识路由

按任务类型决定下一步读取哪个文档：

| 任务或问题 | 读取 |
| --- | --- |
| 需要仓库背景、架构、Native 层实现 | `docs/native-implementation.md` |
| 需要理解错误码体系和分层 | `docs/error_code_layers.md` + `objectstore_errors.h` |
| 修改 JS API、NAPI 绑定、监听器 | `interfaces/jskits/` + `frameworks/jskitsimpl/` |
| 修改 C++ InnerKit、对象存储、同步机制 | `interfaces/innerkits/` + `frameworks/innerkitsimpl/adaptor/` |
| 修改设备通信、设备发现、软总线 | `frameworks/innerkitsimpl/communicator/` |
| 修改 Asset/属性变更分离机制 | `frameworks/innerkitsimpl/WatcherProxy/` |
| 修改 Save/RevokeSave 缓存管理 | `frameworks/innerkitsimpl/CacheManager/` + `ClientAdaptor/` |
| 修改 ETS (Taihe) 绑定、ANI 接口 | `frameworks/ets/taihe/` |
| 新增或修改测试、mock、fixture | `frameworks/*/test/unittest/` + `mock/` |
| 构建、选择 GN 目标或做最小验证 | `data_object.gni` + `bundle.json` |
| 修改依赖、权限、系统服务 | `bundle.json` + `objectstore_errors.h` |

## 硬约束

- 数据对象依赖 sessionId 实现多设备同步；**NEVER** 硬编码 sessionId，**MUST** 使用
  `genSessionId()` 生成随机的 sessionId。
- 仅支持根属性修改检测；**NEVER** 直接修改嵌套属性 `obj.list.push(item)`，**MUST**
  整体赋值 `obj.list = [...obj.list]` 触发同步。
- JS/ETS 接口层保持向后兼容；公共 API 表达稳定能力意图。
- 监听器注册 **NEVER** 使用箭头函数 `on('change', () => {})`，**MUST** 使用命名函数，
  页面销毁时调用 `off('change', callback)` 移除。
- IPC 服务未启动时 Save/RevokeSave 功能降级；**NEVER** 假设服务可用，**MUST** 检查返回值，
  处理 IPC 错误码 `ERR_IPC`，实现重试机制。
- 新增 GN target 时保留 OpenHarmony 组件归属：
  `subsystem_name = "distributeddatamgr"`，`part_name = "data_object"`。
- 系统 errno 风格失败应通过仓库内错误处理映射，使用 `objectstore_errors.h` 标准错误码，
  不要临时拼装错误信息。
- 不要用 public 日志占位符打印隐私数据。sessionId、对象属性、用户数据通常都是隐私数据。
- 明确管理对象生命周期和资源。所有权常由 `DistributedObjectImpl`、
  `FlatObjectStore` 表达；不要让对象在成功/失败路径中处于含混状态。
- 完整编译和运行测试依赖 OpenHarmony 源码树和产品构建环境（通常为 Linux 环境）。
  在 Windows 环境下只能进行代码审查、文本 diff 和结构验证，无法验证 OHOS 链接或设备运行行为。

### 构建验证

> **注意**：以下命令适用于 OpenHarmony Linux 构建环境。在 Windows 环境下只能进行文本和结构验证，无法执行完整编译。

| 目标 | 用途 | 命令（Linux 环境） |
| --- | --- | --- |
| `data_object` | 构建整个组件 | `./build.sh --product-name <product> --build-target data_object` |
| `data_object_test` | 构建全部测试 | `./build.sh --product-name <product> --build-target data_object_test` |
| `//foundation/distributeddatamgr/data_object/interfaces/innerkits:distributeddataobject_impl` | 构建 C++ InnerKit | `./build.sh --product-name <product> --build-target //foundation/distributeddatamgr/data_object/interfaces/innerkits:distributeddataobject_impl` |
| `//foundation/distributeddatamgr/data_object/interfaces/jskits:build_module` | 构建 JS 接口库 | `./build.sh --product-name <product> --build-target //foundation/distributeddatamgr/data_object/interfaces/jskits:build_module` |

### 任务特定验证

| 任务类型 | 验证要求 |
| --- | --- |
| 公共 API 变更 | 构建 + 跑单测 + 检查 `objectstore_errors.h` 错误码兼容性 |
| 同步机制变更 | 构建 + 跑单测 + 验证跨设备同步（如可执行） |
| 设备通信变更 | 构建 + 跑单测 + 验证设备发现与数据传输 |
| IPC 服务变更 | 构建 + 跑单测 + 验证 Save/RevokeSave 流程 |
| 错误处理变更 | 构建 + 跑单测 + 检查 `docs/testing.md` |
| 仅测试变更 | 跑变更的测试 + 至少一个附近相关测试 |

## 已知陷阱

**P1: 嵌套属性修改不触发同步**
- 数据对象仅支持根属性修改检测
- 反模式：`obj.list.push(item)`
- 正确写法：`obj.list = [...obj.list]` 触发同步
- 来源：`FlatObjectStore::Put` 路径

**P2: sessionId 格式非法导致创建失败**
- 硬编码或随意构造 sessionId 导致创建失败
- 正确写法：使用 `genSessionId()` 生成合法 sessionId
- 来源：创建分布式对象失败或同步异常问题

**P3: 监听器内存泄漏**
- 使用箭头函数注册监听器导致无法移除
- 反模式：`on('change', () => {})`
- 正确写法：使用命名函数，页面销毁时调用 `off('change', callback)` 移除
- 来源：页面销毁后回调仍被触发问题

**P4: IPC 服务未启动导致 Save 失败**
- datamgr_service 进程未启动，ObjectService 不可用
- 反模式：假设 IPC 服务一定可用
- 正确写法：检查返回值，处理 IPC 错误码 `ERR_IPC`，实现重试机制
- 来源：Save/RevokeSave 超时或失败问题

## 编码约定

- 单文件建议 ≤2000 行；超过 **MUST** 拆分
- 错误处理：**MUST** 使用 `objectstore_errors.h` 标准错误码；错误码分层详见
  `docs/error_code_layers.md`
- 日志：错误日志 **MUST** 提供充足上下文信息；高频分支 **NEVER** 打正常日志
- 命名：测试文件 `*_test.cpp` / `*JsTest.js`；Mock 文件 `mock_*.cpp/.h`；
  构建配置 `BUILD.gn`
- 导出：inner_api 仅导出 `interfaces/innerkits/` 下头文件；新增导出 **MUST**
  同步更新 `bundle.json`
- 权限：应用 **MUST** 在 `config.json` 声明
  `ohos.permission.DISTRIBUTED_DATASYNC` 权限
- 格式化：提交前 **MUST** 运行
  `clang-format --style=file -i <file>`

## 环境变量

| 变量名 | 必需 | 用途 | 来源 |
| --- | --- | --- | --- |
| `data_object_feature_L1` | 是 | 控制 L1 设备功能特性编译 | `data_object.gni` |
| `data_object_api_metrics_enabled` | 条件 | 控制 API Metrics 上报 | `data_object.gni`（依赖 `global_parts_info`） |

来源均为 `data_object.gni` declare_args()，**NEVER** 手写不一致的值。

## 文档索引

| 路径 | 类型 | 标题 | 加载场景 | 稳定性 |
| --- | --- | --- | --- | --- |
| `docs/architecture.md` | Markdown | 架构设计文档 | 理解整体架构设计 | 稳定 |
| `docs/code-map.md` | Markdown | 代码地图 | 定位文件和目录结构 | 稳定 |
| `docs/native-implementation.md` | Markdown | Native 层实现 | 跨模块改动或新服务接入时 | 稳定 |
| `docs/error_code_layers.md` | Markdown | 错误码分层体系 | 修改错误码或新增接口时 | 稳定 |
| `docs/js-api.md` | Markdown | JS API 文档 | 修改 JavaScript API 时 | 稳定 |
| `docs/synchronization-mechanism.md` | Markdown | 同步机制 | 理解跨设备同步机制 | 稳定 |
| `docs/testing.md` | Markdown | 测试指南 | 编写或运行测试时 | 稳定 |
| `docs/build-and-test.md` | Markdown | 构建与测试 | 构建配置和执行测试 | 稳定 |
| `interfaces/innerkits/objectstore_errors.h` | C++ Header | 标准错误码定义 | 修改错误处理时 | 稳定 |
| `data_object.gni` | GN Build Config | GN 编译参数与特性开关 | 修改构建配置 | 稳定 |
| `bundle.json` | Bundle Metadata | 部件声明与依赖列表 | 新增/删除依赖 | 中等 |
| `samples/distributedNotepad/` | 示例代码 | 分布式备忘录示例 | 理解 API 用法 | 稳定 |
