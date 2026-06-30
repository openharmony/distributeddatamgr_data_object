# AGENTS.md

本文件是 AI Agent 处理本仓库任务的轻量入口。先读本文件，再按任务类型加载匹配文档。

## 阅读策略

默认只读本文件。涉及代码开发时按任务类型加载专题文档（见知识路由表）。

## 仓库定位

`distributeddatamgr_data_object` 是 OpenHarmony 分布式数据对象组件。

```text
//foundation/distributeddatamgr/data_object
```

- 子系统：`distributeddatamgr`
- 部件：`data_object`
- Bundle：`@ohos.data.distributedDataObject`
- 主要能力：分布式数据对象管理、跨设备同步、内存持久化、Save/RevokeSave

## 分层模型

| 层级 | 组件 | 路径 |
| --- | --- | --- |
| 公共 API | @ohos.data.dataObject | interfaces/jskits/ |
| NAPI 绑定 | NAPI wrapper | frameworks/jskitsimpl/ |
| ETS 绑定 | ANI/Taihe | frameworks/ets/ |
| 核心实现 | C++ InnerKit | frameworks/innerkitsimpl/ |
| 底层服务 | DistributedDB/SoftBus/IPC | 外部依赖 |

## 快速代码地图

| 目录 | 职责 |
| --- | --- |
| `interfaces/innerkits/` | C++ InnerKit 接口定义 |
| `interfaces/jskits/` | JavaScript API 主实现 |
| `frameworks/innerkitsimpl/adaptor/` | 对象存储与同步核心 |
| `frameworks/innerkitsimpl/communicator/` | 设备通信与发现 |
| `frameworks/innerkitsimpl/WatcherProxy/` | Asset/属性变更分离 |
| `frameworks/innerkitsimpl/CacheManager/` | Save/RevokeSave 缓存管理 |
| `frameworks/jskitsimpl/` | JS 工具类与 NAPI 绑定 |
| `frameworks/ets/taihe/` | ETS (Taihe) 实现 |

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

## 硬约束

- **sessionId**: MUST 使用 `genSessionId()` 生成，NEVER 硬编码
- **根属性限制**: 仅支持根属性修改检测，嵌套属性需整体赋值
- **监听器**: NEVER 使用箭头函数，MUST 使用命名函数并移除
- **IPC**: NEVER 假设服务可用，MUST 检查 ERR_IPC 并重试
- **向后兼容**: JS/ETS 接口层 NEVER 破坏兼容性
- **错误码**: MUST 使用 `objectstore_errors.h` 标准错误码
- **隐私**: NEVER 用 public 占位符打印 sessionId/对象属性

## 已知陷阱

| 问题 | 反模式 | 正确做法 |
| --- | --- | --- |
| 嵌套属性不触发同步 | `obj.list.push(item)` | `obj.list = [...obj.list]` |
| sessionId 格式非法 | 硬编码 sessionId | 使用 `genSessionId()` |
| 监听器内存泄漏 | `on('change', () => {})` | 命名函数 + `off()` 移除 |
| Save 失败 | 假设 IPC 可用 | 检查 ERR_IPC + 重试 |

## 构建验证

> Linux 环境。Windows 仅文本验证。

| 目标 | 命令 |
| --- | --- |
| 构建组件 | `./build.sh --product-name <product> --build-target data_object` |
| 构建测试 | `./build.sh --product-name <product> --build-target data_object_test` |

## 文档索引

| 文档 | 内容 |
| --- | --- |
| `docs/native-implementation.md` | Native 实现、sessionId、同步、性能优化 |
| `docs/error_code_layers.md` | 错误码分层体系 |