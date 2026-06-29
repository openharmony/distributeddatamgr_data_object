# 代码地图

当需要在设计或编码前定位文件时，读取本页。

## 根目录文件

- `bundle.json`：组件定义、syscap、feature flag、依赖、构建 group、inner kit 和测试目标。
- `data_object.gni`：共享 GN 路径和 build arg。
- `README_zh.md`：当前中文功能概览和约束。
- `AGENTS.md`：AI 轻量路由入口。
- `OAT.xml`：开源合规性元数据。

## `interfaces/innerkits`

这是 C++ InnerKit 接口定义区。

关键构建文件：

```text
interfaces/innerkits/BUILD.gn
```

关键 target：

- `distributeddataobject_impl`：C++ InnerKit 库。

源码布局：

- `distributed_object.h`：分布式对象核心接口定义。
- `distributed_objectstore.h`：分布式对象存储接口定义。
- `objectstore_errors.h`：统一错误码定义。
- `object_types.h`：数据类型定义和枚举。

## `interfaces/jskits`

这是 JS API 主实现区。

关键构建文件：

```text
interfaces/jskits/BUILD.gn
```

关键 target：

- `build_module`：聚合 JS 模块构建目标。
- `distributed_object`：`@ohos.data.dataObject` 模块。

源码布局：

- `distributed_object.js`：JS API 主要入口和实现。
- `napi/`：NAPI 绑定实现。
- `common/`：JS 工具类和适配器。
- `adaptor/`：JS 适配器实现。

## `frameworks/innerkitsimpl`

这是 C++ 核心实现层。

### `adaptor/` - 对象存储与同步核心

关键文件：

- `flat_object_store.*`：扁平对象存储引擎实现。
- `distributed_object_impl.*`：分布式对象核心实现。
- `distributed_object_store_impl.*`：对象存储实现。
- `flat_object_storage_engine.*`：存储引擎实现。
- `client_adaptor.*`：客户端适配器，处理 IPC 通信。

职责：
- 对象生命周期管理
- 数据持久化
- 跨设备同步协调
- 与 DistributedDB 交互

### `communicator/` - 设备通信与发现

关键文件：

- `softbus_adapter.*`：软总线适配器。
- `process_communicator_impl.*`：进程间通信实现。
- `dev_manager.*` / `dev_mgr_driver.*`：设备管理驱动。

职责：
- 设备发现与上下线感知
- 设备间数据传输
- NetworkId/UUID 转换
- 软总线通信封装

### `WatcherProxy/` - Asset/属性变更分离

关键文件：

- `asset_change_timer.*`：Asset 变更定时器。
- `object_watcher.*`：对象监听器。

职责：
- Asset 类型属性变更检测
- 普通属性变更检测
- 变更事件分离与路由

### `CacheManager/` - 缓存管理

关键文件：

- `cache_manager.*` / `cache_manager_proxy.*`：缓存管理器。
- 相关 Save/RevokeSave/Resume 流程实现。

职责：
- Save/RevokeSave/Resume 状态管理
- 与 ObjectService 的 IPC 通信
- 缓存策略实现

### `common/` - 通用组件

关键文件：

- `common_types.*`：通用类型定义。
- `logger.*`：日志封装。
- `macro.h`：宏定义。
- `object_utils.*`：对象工具类。

### `object_service_proxy.*` / `object_callback_stub.*`

- IPC 代理和回调存根。

## `frameworks/jskitsimpl`

这是 JS 工具类与适配器层。

- `adaptor/`：JS 适配器实现。
- `common/`：JS 通用工具类（JSUtil、UvQueue 等）。
- `napi/`：NAPI 绑定实现。

## `frameworks/ets`

这是 ETS (Taihe) 实现层。

关键目录：

- `taihe/ohos.data.distributedDataObject/`：Taihe 静态绑定实现。
  - `include/`：ETS 接口头文件（`ani_dataobject_session.h`、`ani_watcher.h` 等）。
  - `src/`：ETS 实现源码（`ani_constructor.cpp`、`ani_dataobject_session.cpp` 等）。
  - `idl/`：接口定义语言文件。

## `samples`

示例应用目录。

- `distributedNotepad/`：分布式备忘录示例应用。
  - `entry/src/main/js/MainAbility/`：主要实现。
  - `entry/src/main/js/MainAbility/model/DistributedDataModel.js`：数据模型示例。
  - `ReadMe.md`：示例说明。

## `frameworks/*/test`

测试目录。

### `frameworks/innerkitsimpl/test`

- `unittest/`：单元测试。
  - `src/`：测试源码（`distributed_object_impl_test.cpp`、
    `flat_object_store_test.cpp`、`client_adaptor_test.cpp` 等）。
  - `mock/`：Mock 对象（`mock_*.cpp`/`mock_*.h`）。
- `fuzztest/`：模糊测试。
  - `objectstorageengine_fuzzer/`：存储引擎模糊测试。
  - `objectstorecreateobject_fuzzer/`：创建对象模糊测试。
  - `objectstoreput_fuzzer/`：Put 操作模糊测试。

### `frameworks/jskitsimpl/test`

- `unittest/`：JS 单元测试。

## 关键文件路径速查

| 功能 | 主要文件位置 |
| --- | --- |
| 对象创建 | `frameworks/innerkitsimpl/adaptor/distributed_object_store_impl.*` |
| 数据存储 | `frameworks/innerkitsimpl/adaptor/flat_object_store.*` |
| sessionId 生成 | `frameworks/innerkitsimpl/adaptor/` 或 `common/` |
| 设备通信 | `frameworks/innerkitsimpl/communicator/softbus_adapter.*` |
| 监听器管理 | `frameworks/innerkitsimpl/WatcherProxy/object_watcher.*` |
| Save/RevokeSave | `frameworks/innerkitsimpl/CacheManager/cache_manager.*` |
| JS API 入口 | `interfaces/jskits/distributed_object.js` |
| NAPI 绑定 | `interfaces/jskits/napi/` 或 `frameworks/jskitsimpl/napi/` |
| 错误码定义 | `interfaces/innerkits/objectstore_errors.h` |
