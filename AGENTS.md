# AGENTS.md

本文件是 AI Agent 处理本仓库任务的轻量入口。先读本文件，再按任务类型加载匹配文档。

## 阅读策略

默认只读本文件。涉及代码开发时按任务类型加载专题文档（见知识路由）。

## 仓库定位

`distributeddatamgr_data_object` 是 OpenHarmony 分布式数据对象组件。

```text
//foundation/distributeddatamgr/data_object
```

- 子系统：`distributeddatamgr`
- 部件：`data_object`
- Bundle：`@ohos.data.distributedDataObject`

## 核心能力

| 能力 | 说明 | 关键实现 |
| --- | --- | --- |
| 对象管理 | sessionId 唯一标识分布式对象，单例模式管理生命周期 | `DistributedObjectStoreImpl` |
| 跨设备同步 | 基于 DistributedDB + SoftBus，AUTO_SYNC 模式自动同步 | `FlatObjectStorageEngine`, `SoftBusAdapter` |
| 内存持久化 | KvStore 扁平存储（p_前缀+key），类型前缀序列化 | `FlatObjectStore` |
| Save/RevokeSave | 异步 IPC 调用持久化服务，BlockData 等待（超时5秒） | `CacheManager`, `ObjectServiceProxy` |

## 代码地图

| 目录 | 职责 |
| --- | --- |
| `interfaces/jskits/` | JS 公共 API：对象创建/会话管理/监听/持久化（SDK 8/9+ 兼容） |
| `interfaces/innerkits/` | C++ InnerKit 接口定义 |
| `frameworks/jskitsimpl/` | NAPI 绑定、监听器管理、类型转换 |
| `frameworks/ets/taihe/` | ETS (ANI) 实现 |
| `frameworks/innerkitsimpl/adaptor/` | 核心实现：FlatObjectStore、CacheManager、WatcherProxy（Asset/属性变更分离） |
| `frameworks/innerkitsimpl/communicator/` | 设备通信：SoftBusAdapter、DevManager、ProcessCommunicator |

## 知识路由

| 任务类型 | 读取文档 |
| --- | --- |
| Native 实现、sessionId、同步机制 | `docs/native-implementation.md` |
| 错误码体系、新增错误码 | `docs/error_code_layers.md` |
| JS API、NAPI 绑定 | `docs/native-implementation.md` → JS API 层补充 |

## 影响面分析

改动前需确认：
- 影响哪个 API 面：NAPI/ANI/InnerKit/测试
- 是否需保持 NAPI/ANI 变体一致
- 涉及 sessionId/属性/监听器/Asset/IPC？
- 使用标准错误码映射？
- 最小 GN target？

## 依赖方向

保持向下流动：NAPI → C++ core → entity class → utility

- NAPI：解析输入、转换错误、管理监听器
- C++ core：可复用语义、标准错误码
- Entity：持有运行状态（sessionId、监听器）
- Utility：通用 helper 和日志

## 已知陷阱

- IPC 调用需处理 ERR_IPC，支持重试
- JS/ETS 接口保持向后兼容
- 错误码统一用 `objectstore_errors.h` 定义
- 日志打印敏感信息应使用 `Anonymous::Change()` 脱敏

## 构建验证

> Linux 环境。Windows 仅文本验证。

| 目标 | 命令 |
| --- | --- |
| 构建组件 | `./build.sh --product-name <product> --build-target data_object` |
| 构建测试 | `./build.sh --product-name <product> --build-target data_object_test` |
