# JavaScript API 说明

处理 `@ohos.data.dataObject` JS API 相关任务时读取本页。

## 心智模型

`@ohos.data.dataObject` 是分布式数据对象的 JavaScript API 实现。它提供面向对象的内存数据管理
和跨设备同步能力。

主入口：

```text
interfaces/jskits/distributed_object.js
frameworks/jskitsimpl/
```

JS API 通过 NAPI 暴露到 JavaScript/ArkTS 层，底层由 C++ 核心实现支撑。

## 导出结构

模块导出：

- 通过 `interfaces/jskits/distributed_object.js` 导出主要 API。
- 通过 NAPI 绑定导出类和方法。
- 部分高级特性通过 ANI/Taihe 静态绑定暴露。

主要导出内容：

- `createDistributedObject(sessionId)`：创建分布式对象。
- `DistributedObject` 类：分布式对象实例。
- 事件监听：`on('change')`、`off('change')`。
- `save()` / `revokeSave()`：持久化相关方法。
- `sessionId` 属性：会话标识。

## 核心数据流

```
JavaScript Application
        |
        v
@ohos.data.dataObject (JS API)
        |
        v
NAPI Binding (jskitsimpl/napi/)
        |
        v
C++ Core Implementation (innerkitsimpl/adaptor/)
        |
        v
DistributedDB / SoftBus / DeviceManager
```

## 异步编程模型

JS API 主要使用 Promise 和 callback 模型：

- **Sync 模式**：部分简单操作支持同步调用。
- **Promise 模式**：异步操作返回 Promise。
- **Callback 模式**：支持 callback 风格，部分操作兼容。
- **事件模式**：通过 `on()`/`off()` 注册事件监听器。

常用 helper：

- `NFuncArg`：参数数量和参数访问。
- `NVal`：NAPI value 转换和类型检查。
- `NAsyncWorkPromise`：Promise 调度。
- `NAsyncWorkCallback`：Callback 调度。
- `NError`：错误码到 JS error 的转换。

## sessionId 管理

**sessionId 是分布式对象的关键标识符：**

- **格式**：字符串，必须符合特定格式要求。
- **用途**：标识不同会话，实现多设备数据同步。
- **生成**：**MUST** 使用 `genSessionId()` 生成，**NEVER** 硬编码。
- **验证**：创建对象时会验证 sessionId 格式。

**约束：**
- 格式非法会导致对象创建失败。
- 相同 sessionId 在同一设备上对应同一对象实例。
- 不同设备上相同 sessionId 的对象会自动同步。

## 对象属性操作

### 属性设置

```javascript
let object = distributedObject.create(sessionId);
object.name = "test";
object.count = 100;
```

**约束：**
- 仅支持根属性修改检测。
- **NEVER** 直接修改嵌套属性（如 `obj.list.push(item)`）。
- **MUST** 使用整体赋值触发同步（如 `obj.list = [...obj.list]`）。

### 属性读取

```javascript
let name = object.name;
let count = object.count;
```

## 监听器管理

### 注册监听器

```javascript
function onChange(sessionId, fields) {
    console.log('Object changed:', sessionId, fields);
}

object.on('change', onChange);
```

**约束：**
- **NEVER** 使用箭头函数注册监听器（如 `on('change', () => {})`）。
- **MUST** 使用命名函数，以便后续移除。
- 监听器内存泄漏是常见问题。

### 移除监听器

```javascript
object.off('change', onChange);
```

**最佳实践：**
- 页面销毁时必须移除所有监听器。
- 使用命名函数引用，确保能正确移除。
- 避免重复注册相同监听器。

## 持久化操作

### Save 操作

```javascript
object.save().then(() => {
    console.log('Save succeeded');
}).catch((error) => {
    console.error('Save failed:', error.code);
});
```

**约束：**
- 依赖 datamgr_service 进程（ObjectService）。
- **NEVER** 假设服务一定可用。
- **MUST** 检查返回值，处理 `ERR_IPC` 错误码。
- **MUST** 实现重试机制。

### RevokeSave 操作

```javascript
object.revokeSave().then(() => {
    console.log('RevokeSave succeeded');
}).catch((error) => {
    console.error('RevokeSave failed:', error.code);
});
```

## 错误处理

所有异步操作都可能失败，必须进行错误处理：

```javascript
object.save().catch((error) => {
    // error.code: 错误码
    // error.message: 错误信息
    console.error('Operation failed:', error.code, error.message);
});
```

**错误码来源：**
- `interfaces/innerkits/objectstore_errors.h`
- 标准错误码分层：`docs/error_code_layers.md`

**常见错误：**
- 格式错误的 sessionId。
- IPC 服务不可用（ERR_IPC）。
- 权限不足（需要 DISTRIBUTED_DATASYNC 权限）。
- 网络异常或设备离线。

## NAPI/ANI 一致性

部分操作同时有 NAPI 和 ANI 实现。语义改动时同时搜索：

```powershell
rg -n "CreateDistributedObject|Save|RevokeSave|On|Off" frameworks/jskitsimpl frameworks/ets
```

如果 `frameworks/jskitsimpl/test/unittest` 和 `frameworks/ets/test` 都有相关测试，
尽量同时更新或运行两类 target。

## 隐私与日志

**隐私规则：**
- sessionId、对象属性、用户数据都是隐私数据。
- 除固定枚举、errno、非敏感诊断值外，日志中应使用 private 占位符。
- **NEVER** 使用 public 占位符打印用户数据。

```cpp
// 正确
HILOG_INFO(LOG_CORE, "SessionId: %{private}s", sessionId.c_str());

// 错误
HILOG_INFO(LOG_CORE, "SessionId: %{public}s", sessionId.c_str());
```

## 常用本地搜索

```powershell
rg -n "createDistributedObject|save|revokeSave|on\(|off\(" interfaces/jskits frameworks/jskitsimpl
rg -n "sessionId|DISTRIBUTED_DATASYNC|ERR_IPC" interfaces/jskits frameworks/jskitsimpl
rg -n "HILOG[DIWE]|%\\{public\\}|%\\{private\\}" frameworks/jskitsimpl
```

## JS API 目标路由

常见改动按以下路径查找：

- 对象创建：`createDistributedObject`、`DistributedObject` 构造。
- 属性操作：属性 getter/setter 实现。
- 监听器：`on()`、`off()`、事件触发逻辑。
- 持久化：`save()`、`revokeSave()`、`resume()`。
- 错误处理：NError 转换、错误码映射。
- sessionId 处理：`genSessionId()`、格式验证。

## 权限要求

应用必须在 `config.json` 中声明权限：

```json
{
    "requestPermissions": [
        {
            "name": "ohos.permission.DISTRIBUTED_DATASYNC",
            "reason": "需要使用分布式数据同步"
        }
    ]
}
```

**约束：**
- 没有权限的应用无法创建分布式对象。
- 权限检查在对象创建时进行。
- 运行时权限撤销会影响同步功能。
