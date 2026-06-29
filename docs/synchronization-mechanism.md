# 同步机制说明

处理跨设备数据同步相关任务时读取本页。

## 心智模型

分布式数据对象的核心能力是跨设备数据同步。同步机制基于 DistributedDB 和软总线实现，
确保多设备间数据一致性。

主入口：

```text
frameworks/innerkitsimpl/adaptor/ (FlatObjectStore)
frameworks/innerkitsimpl/communicator/ (SoftBusAdapter)
```

## 同步架构

```
应用修改对象属性
        |
        v
FlatObjectStore::Put() (检测变更)
        |
        v
DistributedDB (持久化 + 同步)
        |
        v
SoftBusAdapter (设备间传输)
        |
        v
远程设备接收更新
        |
        v
触发远程对象监听器
```

## sessionId 机制

**sessionId 是同步的核心标识符：**

### 生成规则

**位置**：`frameworks/innerkitsimpl/adaptor/` 或 `common/`

```cpp
std::string genSessionId() {
    // 生成唯一 sessionId
    // 格式要求：特定长度和字符集
}
```

**格式要求**：
- 字符串类型
- 固定长度或符合特定模式
- 全局唯一性（考虑时间、设备、随机因素）

### 作用

1. **会话标识**：标识一个分布式对象实例。
2. **设备关联**：相同 sessionId 在不同设备上对应同一对象。
3. **同步路由**：通过 sessionId 确定数据同步目标。
4. **生命周期**：随对象创建而生成，销毁而失效。

**约束**：
- **MUST** 使用 `genSessionId()` 生成。
- **NEVER** 硬编码 sessionId。
- **NEVER** 随意构造 sessionId。

## 数据同步流程

### 本地变更检测

1. **属性修改**：
   ```javascript
   object.name = "new name";  // 根属性修改
   ```

2. **变更检测**：
   - FlatObjectStore 检测到属性变更。
   - 验证是否为根属性。
   - 触发同步流程。

3. **嵌套属性处理**：
   ```javascript
   object.list.push(item);  // 不触发同步
   object.list = [...object.list, item];  // 触发同步
   ```

### 本地持久化

1. **DistributedDB 存储**：
   - FlatObjectStore 调用 DistributedDB 接口。
   - 数据以 sessionId 为键存储。
   - 序列化对象属性。

2. **DistributedDB 同步**：
   - DistributedDB 自动同步到其他设备。
   - 使用软总线传输数据。
   - 处理网络异常和冲突。

### 远程设备接收

1. **数据接收**：
   - 远程设备通过 SoftBusAdapter 接收数据。
   - DistributedDB 更新本地存储。

2. **对象更新**：
   - FlatObjectStore 更新对象属性。
   - 触发本地监听器。
   - 应用收到变更通知。

## 同步冲突处理

### 冲突场景

1. **并发修改**：
   - 设备 A 和 B 同时修改同一属性。
   - 网络延迟导致顺序不确定。

2. **网络分区**：
   - 设备间网络中断。
   - 各设备独立修改。
   - 网络恢复后数据合并。

### 冲突解决策略

**DistributedDB 层面**：
- 使用最后写入胜出（Last-Write-Wins）策略。
- 基于时间戳或版本号。
- 部分场景支持应用自定义冲突解决。

**应用层面**：
- 避免频繁修改同一属性。
- 使用 Save/RevokeSave 保证重要数据。
- 合理设计数据结构。

## 变更检测机制

### FlatObjectStore 变更检测

**位置**：`frameworks/innerkitsimpl/adaptor/flat_object_store.*`

**原理**：
- 拦截属性设置操作。
- 对比新旧值。
- 判断是否需要同步。

**实现**：
```cpp
void FlatObjectStore::Put(const std::string& key, const napi_value& value) {
    // 1. 序列化新值
    // 2. 对比旧值
    // 3. 如果不同，触发同步
    // 4. 更新本地存储
}
```

### 根属性限制

**仅支持根属性修改检测**：
- `object.name = "test"` ✅
- `object.user.age = 25` ❌
- `object.list.push(item)` ❌
- `object.list = [...object.list]` ✅

**原因**：
- 扁平化存储模型。
- 嵌套路径追踪复杂度高。
- 性能考虑。

### WatcherProxy 分离机制

**位置**：`frameworks/innerkitsimpl/WatcherProxy/`

**职责**：
- 分离 Asset 和普通属性变更。
- 路由不同类型的变更。

**Asset 特殊处理**：
- Asset 可能是大文件。
- 延迟加载和传输。
- 单独的变更通知路径。

## 设备发现与连接

### 设备发现

**位置**：`frameworks/innerkitsimpl/communicator/`

**流程**：
1. SoftBusAdapter 扫描附近设备。
2. DevMgrDriver 设备上下线感知。
3. 建立设备信任关系。
4. 获取设备 NetworkId/UUID。

### 连接管理

**建立连接**：
1. 设备发现后，SoftBus 建立连接。
2. 验证设备权限和信任关系。
3. 注册数据接收回调。
4. 开始数据同步。

**连接断开**：
1. 检测设备离线。
2. 暂停该设备的数据同步。
3. 缓存变更（网络恢复后同步）。
4. 触发离线事件。

### 设备状态

**设备状态管理**：
- 在线：设备可通信，数据实时同步。
- 离线：设备不可通信，变更缓存。
- 信任：设备已通过认证。
- 未知：设备状态未确定。

## 同步性能优化

### 批量同步

**策略**：
- 合并短时间内的多次变更。
- 设置同步间隔（如 100ms）。
- 减少网络传输次数。

**实现**：
```cpp
void FlatObjectStore::ScheduleSync() {
    // 延迟执行同步
    // 合并期间的所有变更
}
```

### 增量同步

**仅传输变更数据**：
- 对比新旧对象状态。
- 只传输修改的属性。
- 减少网络流量。

### 压缩

**数据压缩**：
- 大对象压缩后传输。
- Asset 类型自动压缩。
- 选择合适的压缩算法。

## 同步可靠性

### 重试机制

**场景**：
- 网络不稳定。
- 设备临时离线。
- 服务端暂时不可用。

**策略**：
- 指数退避重试。
- 最大重试次数限制。
- 超时控制。

### 断点续传

**Asset 传输**：
- 支持大文件分块传输。
- 记录传输进度。
- 断点恢复传输。

### 数据一致性

**保证**：
- 最终一致性模型。
- 不保证强一致性。
- 可能出现短暂不一致。

## 同步限制

### 网络要求

**条件**：
- 设备在同一局域网。
- 软总线服务正常。
- 网络质量满足要求。

**影响**：
- 网络差：同步延迟增加。
- 网络中断：同步暂停。
- 网络恢复：同步继续。

### 数据大小限制

**限制**：
- 单个对象大小限制。
- 单个属性值大小限制。
- Asset 文件大小限制。

**建议**：
- 避免存储大对象。
- 大文件使用 Asset 类型。
- 考虑分页或分块。

### 设备数量限制

**限制**：
- 同时在线设备数量。
- 同步组大小。

**建议**：
- 避免过多设备同步。
- 合理设计同步组。

## 同步调试

### 日志追踪

**关键日志点**：
- 对象创建（sessionId 生成）。
- 属性变更（变更内容）。
- 同步开始/结束（时间戳）。
- 错误和重试（原因和次数）。

**日志格式**：
```cpp
HILOG_INFO(LOG_CORE, "Sync started: sessionId=%{private}s, fields=%{public}s",
    sessionId.c_str(), fields.c_str());
```

### 性能监控

**指标**：
- 同步延迟（变更到远程接收）。
- 同步成功率。
- 网络流量。
- 设备在线率。

**工具**：
- HiTrace：链路追踪。
- HiSysEvent：事件上报。
- API metrics：API 调用统计。

### 问题排查

**同步失败**：
1. 检查网络连接。
2. 验证设备信任关系。
3. 确认权限配置。
4. 查看 DistributedDB 日志。

**同步延迟**：
1. 检查网络质量。
2. 查看同步间隔设置。
3. 分析数据大小。
4. 评估设备性能。

**数据不一致**：
1. 检查冲突解决策略。
2. 验证变更检测逻辑。
3. 确认嵌套属性处理。
4. 查看监听器触发时机。
