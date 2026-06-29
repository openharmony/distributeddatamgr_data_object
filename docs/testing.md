# 测试说明

新增或修改测试，或选择验证 target 时读取本页。

## 测试根目录

```text
frameworks/*/test
```

主要测试区域：

- `frameworks/innerkitsimpl/test`：C++ 核心实现测试。
- `frameworks/jskitsimpl/test`：JS API 测试。

主要 group：

- `frameworks/innerkitsimpl/test/unittest`：单元测试。
- `frameworks/innerkitsimpl/test/fuzztest`：模糊测试。

聚合单元测试 target：

```text
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/unittest:data_object_unittest
```

聚合 fuzz target：

```text
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/fuzztest:data_object_fuzztest
```

## C++ 单元测试布局

### `frameworks/innerkitsimpl/test/unittest`

重要目录和文件：

- `src/`：测试源码
  - `distributed_object_impl_test.cpp`：分布式对象实现测试。
  - `flat_object_store_test.cpp`：扁平对象存储测试。
  - `client_adaptor_test.cpp`：客户端适配器测试。
  - `softbus_adapter_test.cpp`：软总线适配器测试。
  - `process_communicator_test.cpp`：进程通信测试。
  - `watcher_proxy_test.cpp`：监听器代理测试。
  - `cache_manager_test.cpp`：缓存管理器测试。
  - `object_service_proxy_test.cpp`：ObjectService 代理测试。

- `mock/`：Mock 对象
  - `mock_distributed_db.*`：DistributedDB mock。
  - `mock_softbus_adapter.*`：软总线适配器 mock。
  - `mock_device_manager.*`：设备管理 mock。
  - `mock_object_service.*`：ObjectService mock。
  - `mock_ipc.*`：IPC mock。

## 主要 C++ 单元测试 target

来自 `frameworks/innerkitsimpl/test/unittest/BUILD.gn`：

- `distributed_object_impl_test`：分布式对象实现测试。
- `flat_object_store_test`：扁平对象存储测试。
- `client_adaptor_test`：客户端适配器测试。
- `communicator_test`：通信组件测试。
- `watcher_proxy_test`：监听器代理测试。
- `cache_manager_test`：缓存管理器测试。

## JS 单元测试布局

### `frameworks/jskitsimpl/test`

- `unittest/`：JS 单元测试。
  - 数据对象创建测试。
  - 属性同步测试。
  - 监听器注册与移除测试。
  - sessionId 生成测试。
  - 错误处理测试。

## Mock 策略

`frameworks/innerkitsimpl/test/unittest/mock` 包含以下 mock：

- **DistributedDB mock**：模拟数据库操作，隔离存储层。
- **SoftBusAdapter mock**：模拟软总线通信，隔离网络层。
- **DeviceManager mock**：模拟设备管理，隔离设备管理服务。
- **ObjectService mock**：模拟 IPC 服务，测试 Save/RevokeSave 流程。
- **IPC mock**：模拟进程间通信。

Mock 测试常用：

```text
private=public
protected=public
```

这是有意为之，用于让测试访问内部状态。除非测试 target 不再需要内部可见性，否则不要移除。

## Fuzz 测试

fuzz 根目录：

```text
frameworks/innerkitsimpl/test/fuzztest
```

当前 fuzz target：

- `objectstorageengine_fuzzer`：存储引擎模糊测试。
- `objectstorecreateobject_fuzzer`：创建对象模糊测试。
- `objectstoreput_fuzzer`：Put 操作模糊测试。
- `objectstoresave_fuzzer`：Save 操作模糊测试。
- `objectstorewatcher_fuzzer`：监听器模糊测试。

新增 fuzz 覆盖时，检查 OpenHarmony fuzz test 约束，并按需要加 feature/platform guard。

## 测试命名习惯

新增测试前，按操作名搜索：

```powershell
rg -n "Create|create|Put|put|Watch|watch|Save|save|Sync|sync" frameworks/*/test
```

沿用本地命名和 target 放置方式。优先在已有 operation 测试附近加窄测试，不要新增过宽泛的
测试文件。

## 测试覆盖重点

### 1. sessionId 相关测试
- sessionId 格式验证
- sessionId 生成规则
- 非法 sessionId 处理

### 2. 属性同步测试
- 根属性修改检测
- 嵌套属性修改（应不触发同步）
- 整体赋值触发同步

### 3. 监听器测试
- 监听器注册
- 监听器触发
- 监听器移除
- 内存泄漏验证

### 4. 跨设备同步测试
- 设备发现
- 数据传输
- 网络异常处理

### 5. Save/RevokeSave 测试
- Save 流程
- RevokeSave 流程
- Resume 流程
- IPC 错误处理（ERR_IPC）
- 重试机制

### 6. 错误码测试
- 验证所有错误码路径
- 错误码兼容性
- 错误信息传递

### 7. Asset 测试
- Asset 属性变更检测
- Asset 与普通属性分离
- Asset 变更通知

## TDD 流程

采用标准 TDD 流程：

1. **RED**：先写测试，**MUST** 看到失败。
2. **GREEN**：最小实现，让测试通过。
3. **IMPROVE**：重构，改善代码质量。

在实现前编写测试可以验证设计思路，确保代码可测试性。

## Mock 实现建议

创建 Mock 时：

1. **接口清晰**：Mock 应实现与真实对象相同的接口。
2. **行为可控**：提供设置预期行为和返回值的机制。
3. **状态验证**：提供验证调用次数、参数的机制。
4. **失败注入**：支持模拟失败场景，测试错误处理。

## 测试环境配置

测试依赖：

- **googletest**：C++ 测试框架。
- **gtest** + **gmock**：测试和 mock 工具。
- **libuv**：JS 测试事件循环。
- **OpenHarmony 测试框架**：系统级测试支持。

测试配置文件：

- `ohos_test.xml`：测试套件配置（位于测试资源目录）。
- 测试资源文件：位于各测试目录的 `resource/` 子目录。

## 性能测试

对于关键性能路径，考虑添加：

- **同步延迟测试**：测量跨设备同步时间。
- **内存使用测试**：测量对象创建和监听器内存占用。
- **并发测试**：多线程/多对象场景测试。

性能测试应：

- 使用真实数据规模。
- 在目标设备上运行。
- 记录基准数据。
- 包含回归检测。
