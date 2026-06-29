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

### FlatObjectStore

**位置**：`frameworks/innerkitsimpl/adaptor/flat_object_store.*`

**职责**：
- 扁平化对象存储引擎
- 数据序列化与反序列化
- 与 DistributedDB 交互
- 对象生命周期管理

**关键接口**：
- `Put()`：保存或更新对象属性。
- `Get()`：获取对象属性。
- `Delete()`：删除对象。
- `Watch()`：注册属性变更监听。
- `Unwatch()`：取消监听。

**约束**：
- 仅支持根属性修改检测。
- 嵌套属性修改不触发同步。
- 所有数据变更必须通过 `Put()` 接口。

### DistributedObjectImpl

**位置**：`frameworks/innerkitsimpl/adaptor/distributed_object_impl.*`

**职责**：
- 分布式对象实现
- 封装同步逻辑
- 管理 sessionId

**关键接口**：
- `SetSessionId()`：设置会话标识。
- `Put()`：更新对象属性。
- `Get()`：获取对象属性。
- `Save()`：持久化对象。
- `RevokeSave()`：撤销持久化。
- `On()` / `Off()`：监听器管理。

**状态管理**：
- sessionId 状态
- 同步状态
- 持久化状态
- 监听器列表

### ClientAdaptor

**位置**：`frameworks/innerkitsimpl/adaptor/client_adaptor.*`

**职责**：
- 客户端适配器
- IPC 通信管理
- ObjectService 代理获取
- Save/RevokeSave 流程协调

**关键接口**：
- `GetObjectService()`：获取 ObjectService 代理。
- `SaveObject()`：保存对象到服务端。
- `RevokeSaveObject()`：撤销保存。
- `ResumeObject()`：恢复对象。

**错误处理**：
- IPC 服务不可用（ERR_IPC）。
- 超时处理。
- 重试机制。

## 设备通信层

### SoftBusAdapter

**位置**：`frameworks/innerkitsimpl/communicator/softbus_adapter.*`

**职责**：
- 软总线适配器
- 设备发现
- 数据传输
- 连接管理

**关键接口**：
- `GetRemoteDevices()`：获取远程设备列表。
- `SendData()`：发送数据到远程设备。
- `RegisterCallback()`：注册接收回调。
- `UnregisterCallback()`：取消注册。

**依赖**：
- `dsoftbus`：软总线服务。
- `device_manager`：设备管理服务。

### ProcessCommunicatorImpl

**位置**：`frameworks/innerkitsimpl/communicator/process_communicator_impl.*`

**职责**：
- 进程间通信实现
- 设备间消息路由
- 数据序列化传输

**关键接口**：
- `SendMessage()`：发送消息。
- `RegisterMessageHandler()`：注册消息处理器。
- `GetDeviceId()`：获取设备标识。

### DevMgrDriver

**位置**：`frameworks/innerkitsimpl/communicator/dev_mgr_driver.*`

**职责**：
- 设备管理驱动
- 设备上下线感知
- NetworkId/UUID 转换

**关键接口**：
- `OnDeviceOnline()`：设备上线处理。
- `OnDeviceOffline()`：设备下线处理。
- `GetNetworkId()`：获取 NetworkId。

## 变更检测与通知

### WatcherProxy

**位置**：`frameworks/innerkitsimpl/WatcherProxy/`

**职责**：
- Asset 与属性变更分离
- 变更事件路由
- 变更通知分发

**关键组件**：
- `ObjectWatcher`：对象监听器。
- `AssetChangeTimer`：Asset 变更定时器。

**分离机制**：
- Asset 类型属性：特殊处理路径。
- 普通属性：标准同步路径。
- 变更合并与去重。

## 缓存管理层

### CacheManager

**位置**：`frameworks/innerkitsimpl/CacheManager/cache_manager.*`

**职责**：
- Save/RevokeSave/Resume 流程管理
- 缓存策略
- 与 ObjectService 的 IPC 通信

**关键接口**：
- `Save()`：保存对象到缓存。
- `RevokeSave()`：撤销保存。
- `Resume()`：恢复对象。
- `GetCacheStatus()`：获取缓存状态。

**状态机**：
- 正常状态：数据在内存中。
- Saved 状态：数据已持久化。
- Revoked 状态：持久化已撤销。

**依赖**：
- `ObjectServiceProxy`：IPC 服务代理。
- `samgr`：系统 Ability 管理。

## 错误处理

### 标准错误码

**位置**：`interfaces/innerkits/objectstore_errors.h`

**错误码分层**：详见 `docs/error_code_layers.md`

**常见错误**：
- `ERR_OK`：成功。
- `ERR_IPC`：IPC 服务错误。
- `ERR_INVALID_SESSIONID`：sessionId 格式错误。
- `ERR_PERMISSION_DENIED`：权限不足。
- `ERR_DEVICE_OFFLINE`：设备离线。
- `ERR_SYNC_FAILED`：同步失败。

### 错误处理策略

**原则**：
1. 统一使用标准错误码。
2. 错误信息提供充足上下文。
3. 区分可恢复和不可恢复错误。
4. 实现适当的重试机制。

**实现模式**：
```cpp
auto result = operation();
if (result != ERR_OK) {
    HILOG_ERROR("Operation failed: %{public}d", result);
    return result;
}
```

## 资源管理

### 对象生命周期

**创建阶段**：
1. 验证 sessionId 格式。
2. 初始化 FlatObjectStore。
3. 注册设备监听。
4. 初始化同步状态。

**运行阶段**：
1. 属性变更检测。
2. 同步到 DistributedDB。
3. 通知远程设备。
4. 触发本地监听器。

**销毁阶段**：
1. 取消所有监听器。
2. 注销设备监听。
3. 清理缓存。
4. 释放资源。

### 监听器管理

**注册流程**：
1. 验证监听器有效性。
2. 注册到 WatcherProxy。
3. 建立变更映射。
4. 返回监听器 ID。

**移除流程**：
1. 查找监听器 ID。
2. 从 WatcherProxy 移除。
3. 清理关联资源。
4. 验证移除成功。

**内存管理**：
- 使用 shared_ptr/weak_ptr 管理生命周期。
- 避免循环引用。
- 监听器持有对象引用需谨慎。

## 数据持久化

### Save 流程

1. **客户端发起**：
   - 调用 `DistributedObjectImpl::Save()`。
   - 参数验证（sessionId、对象状态）。

2. **缓存管理**：
   - CacheManager 检查缓存状态。
   - 准备持久化数据。

3. **IPC 调用**：
   - 通过 ObjectServiceProxy 调用服务端。
   - 处理超时和错误。

4. **结果处理**：
   - 成功：更新本地状态。
   - 失败：返回错误码，可能重试。

### RevokeSave 流程

1. **客户端发起**：
   - 调用 `DistributedObjectImpl::RevokeSave()`。

2. **IPC 调用**：
   - 通知服务端撤销持久化。

3. **结果处理**：
   - 成功：更新本地状态。
   - 失败：处理错误（可能已撤销）。

## 线程安全

**并发控制**：
- 对象操作需线程安全。
- 使用 mutex/lock 保护共享状态。
- 避免死锁（注意锁顺序）。

**异步操作**：
- libuv 事件循环集成。
- 异步回调调度。
- 线程池使用。

**注意事项**：
- sessionId 是只读的，创建后不应修改。
- 监听器列表修改需加锁。
- FlatObjectStore 的操作可能并发调用。

## 性能考虑

**优化策略**：
- 批量操作：合并多个属性变更。
- 延迟同步：设置合理的同步间隔。
- 缓存策略：合理使用 Save/RevokeSave。
- 网络优化：压缩数据，减少传输量。

**性能指标**：
- 对象创建时间。
- 属性同步延迟。
- 内存占用（对象数量 × 属性数量）。
- 网络流量。

## 调试支持

**日志策略**：
- 错误日志：ERROR 级别，包含详细上下文。
- 关键操作：INFO 级别，记录主要流程。
- 调试日志：DEBUG 级别，默认关闭。
- **隐私数据使用 private 占位符**。

**诊断工具**：
- HiTrace：链路追踪。
- HiSysEvent：事件上报。
- 性能统计：API metrics（如果启用）。

**常见问题排查**：
- 同步失败：检查网络、设备状态、权限。
- 内存泄漏：检查监听器是否正确移除。
- 性能问题：检查同步频率、对象数量。
