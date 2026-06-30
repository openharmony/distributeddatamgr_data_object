# Native 层实现说明

处理 C++ InnerKit 和核心实现相关任务时读取本页。

## 心智模型

Native 层是分布式数据对象的核心实现，负责对象管理、数据存储、跨设备同步、设备通信等
关键功能。

主入口：

```text
frameworks/innerkitsimpl/
```

主要组件：

- **adaptor/**：对象存储与同步核心。
- **communicator/**：设备通信与发现。
- **WatcherProxy/**：Asset/属性变更分离。
- **CacheManager/**：Save/RevokeSave 缓存管理。
- **common/**：通用组件和工具类。

## 核心组件架构

| 组件 | 位置 | 职责 | 关键接口 |
| --- | --- | --- | --- |
| **FlatObjectStore** | `adaptor/flat_object_store.*` | 扁平化存储引擎、序列化、与 DistributedDB 交互 | `Put()`, `Get()`, `Delete()`, `Watch()` |
| **DistributedObjectImpl** | `adaptor/distributed_object_impl.*` | 分布式对象实现、同步逻辑、sessionId 管理 | `SetSessionId()`, `Save()`, `RevokeSave()`, `On/Off()` |
| **ClientAdaptor** | `adaptor/client_adaptor.*` | IPC 通信管理、ObjectService 代理获取 | `GetObjectService()`, `SaveObject()`, `RevokeSaveObject()` |
| **CacheManager** | `CacheManager/cache_manager.*` | Save/RevokeSave 流程、缓存策略 | `Save()`, `RevokeSave()`, `GetCacheStatus()` |
| **WatcherProxy** | `WatcherProxy/` | Asset/属性变更分离、变更通知路由 | `ObjectWatcher`, `AssetChangeTimer` |

**核心约束**：
- 仅支持根属性修改检测，嵌套属性修改不触发同步
- 所有数据变更必须通过 `Put()` 接口
- IPC 服务不可用时需处理 ERR_IPC，实现重试机制

## sessionId 机制

**sessionId 是同步的核心标识符：**

- **生成**：使用 `genSessionId()` 生成唯一 sessionId。
- **格式**：字符串类型，固定长度，全局唯一（考虑时间、设备、随机因素）。
- **约束**：**MUST** 使用 `genSessionId()` 生成，**NEVER** 硬编码或随意构造。

**作用**：
1. 会话标识：标识一个分布式对象实例。
2. 设备关联：相同 sessionId 在不同设备上对应同一对象。
3. 同步路由：通过 sessionId 确定数据同步目标。
4. 生命周期：随对象创建而生成，销毁而失效。

## 设备通信层

关键组件：
- **SoftBusAdapter**：软总线适配器，设备发现与数据传输。
- **ProcessCommunicatorImpl**：进程间通信实现。
- **DevMgrDriver**：设备管理驱动，设备上下线感知。

**设备发现流程**：
1. SoftBusAdapter 扫描附近设备。
2. DevMgrDriver 设备上下线感知。
3. 建立设备信任关系。
4. 获取设备 NetworkId/UUID。

**设备状态**：
- 在线：设备可通信，数据实时同步。
- 离线：设备不可通信，变更缓存。
- 信任：设备已通过认证。

## 变更检测与通知

关键组件：
- **WatcherProxy**：Asset 与属性变更分离。
- **ObjectWatcher**：对象监听器。
- **AssetChangeTimer**：Asset 变更定时器。

**根属性限制**：
- 仅支持根属性修改检测（`object.name = "test"` ✅）。
- 嵌套属性修改不触发同步（`object.user.age = 25` ❌）。
- 整体赋值可触发同步（`object.list = [...object.list]` ✅）。

## 数据同步流程

### 本地变更检测

1. 属性修改：`object.name = "new name"`（根属性修改）。
2. 变更检测：FlatObjectStore 检测属性变更，验证是否为根属性。
3. 触发同步：调用 DistributedDB 接口。

### 本地持久化

1. FlatObjectStore 调用 DistributedDB 接口。
2. 数据以 sessionId 为键存储。
3. 序列化对象属性。
4. DistributedDB 自动同步到其他设备。

### 远程设备接收

1. SoftBusAdapter 接收数据。
2. DistributedDB 更新本地存储。
3. FlatObjectStore 更新对象属性。
4. 触发本地监听器。

## 同步冲突处理

**冲突场景**：
- 并发修改：设备 A 和 B 同时修改同一属性。
- 网络分区：网络中断后各设备独立修改，恢复后合并。

**解决策略**：
- DistributedDB 使用 Last-Write-Wins 策略（基于时间戳或版本号）。
- 部分场景支持应用自定义冲突解决。
- 应用层面：避免频繁修改同一属性，使用 Save/RevokeSave 保证重要数据。

## 同步性能优化

**批量同步**：合并短时间内的多次变更，设置同步间隔（如 100ms）。

**增量同步**：仅传输变更数据，对比新旧对象状态，减少网络流量。

**压缩**：大对象压缩后传输，Asset 类型自动压缩。

## 同步限制

**网络要求**：设备在同一局域网，软总线服务正常。

**数据大小限制**：单个对象、属性值、Asset 文件大小有上限。建议：避免存储大对象，大文件使用 Asset 类型。

**设备数量限制**：同时在线设备数量、同步组大小有上限。建议：避免过多设备同步。

## 缓存管理与持久化

**CacheManager 位置**：`frameworks/innerkitsimpl/CacheManager/cache_manager.*`

**Save 流程**：
1. 调用 `DistributedObjectImpl::Save()` → CacheManager 检查状态
2. 通过 ObjectServiceProxy IPC 调用服务端
3. 成功：更新本地状态；失败：返回 ERR_IPC，重试

**RevokeSave 流程**：调用服务端撤销持久化，更新本地状态。

**状态机**：正常（内存）→ Saved（持久化）→ Revoked（撤销）

**依赖**：ObjectServiceProxy（IPC 代理）、samgr（系统 Ability 管理）

## 错误处理

详见 `docs/error_code_layers.md`

**原则**：统一使用标准错误码，错误信息提供上下文，错误码向上传播不吞没。

## 资源管理

**对象生命周期**：
- 创建：验证 sessionId → 初始化 FlatObjectStore → 注册设备监听
- 运行：属性变更检测 → 同步 DistributedDB → 通知远程/本地监听器
- 销毁：取消监听器 → 注销设备监听 → 清理缓存 → 释放资源

**监听器管理**：使用 shared_ptr/weak_ptr 管理生命周期，避免循环引用，页面销毁时必须移除。

## 线程安全

- 对象操作需线程安全，使用 mutex/lock 保护共享状态
- sessionId 创建后只读，不应修改
- 监听器列表修改需加锁，避免死锁（注意锁顺序）
- 使用 libuv 异步回调，不阻塞主线程

## 调试支持

- 日志：ERROR 级别含上下文，隐私数据用 private 占位符
- 工具：HiTrace（链路追踪）、HiSysEvent（事件上报）
- 常见问题：同步失败→检查网络/权限；内存泄漏→检查监听器移除

## 指导原则

**错误码使用**: 详见 `docs/error_code_layers.md`

### 允许的场景

✅ 监听器生命周期管理（shared_ptr/weak_ptr）
✅ 线程安全访问（mutex/lock）
✅ IPC 错误处理和重试

### 禁止的场景

❌ 监听器循环引用（使用 weak_ptr）
❌ 不加锁访问共享状态
❌ 忽略 IPC 错误
❌ 阻塞主线程（使用异步操作）

## 总结

Native 层是分布式数据对象的核心实现。

**关键原则**:
1. **错误处理**: 使用标准错误码，向上传播
2. **线程安全**: mutex/lock 保护共享状态
3. **资源管理**: shared_ptr/weak_ptr，避免循环引用
4. **IPC 处理**: 检查 ERR_IPC，实现重试
5. **异步操作**: 不阻塞主线程

**实现要点**: 对象线程安全、监听器正确管理、日志含上下文、隐私用 private 占位符
