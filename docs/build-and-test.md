# 构建与测试命令

选择最小验证方式时读取本页。

## 环境边界

本仓库是 OpenHarmony 组件。完整编译和运行验证需要 OpenHarmony 源码树，并且本仓库位于：

```text
foundation/distributeddatamgr/data_object
```

当前独立 Windows checkout 可以验证文本、文件结构、链接和 diff，但不能单独证明 OHOS GN
链接或设备运行行为。

## 最小文本验证

在本仓库根目录运行：

```powershell
git status --short
git diff -- AGENTS.md docs/
rg -n "TO[D]O|TB[D]|PLACE[H]OLDER" AGENTS.md docs/
rg -n "docs/" AGENTS.md
```

代码改动还应搜索是否误用 public 日志打印隐私数据：

```powershell
rg -n "HILOG[DIWE].*%\\{public\\}.*(sessionId|object|property|data)" interfaces frameworks
```

这是启发式检查，不能替代码审查。

## 构建入口

组件级构建契约以 `bundle.json` 为准。重要 group：

```text
//foundation/distributeddatamgr/data_object/interfaces/jskits:build_module
//foundation/distributeddatamgr/data_object/interfaces/innerkits:distributeddataobject_impl
```

重要测试 group：

```text
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/unittest:data_object_unittest
```

## 常见 OHOS 构建形式

具体 product name 取决于工作区。进入 OpenHarmony 源码根后，先使用该工作区惯用的产品选择，
再构建覆盖改动的最小 target。

常见形式：

```bash
./build.sh --product-name <product> --build-target //foundation/distributeddatamgr/data_object/interfaces/jskits:build_module
./build.sh --product-name <product> --build-target //foundation/distributeddatamgr/data_object/interfaces/innerkits:distributeddataobject_impl
./build.sh --product-name <product> --build-target //foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/unittest:data_object_unittest
```

如果工作区使用 `hb`，等价模式是：

```bash
hb set
hb build --target //foundation/distributeddatamgr/data_object/interfaces/jskits:build_module
hb build --target //foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/unittest:data_object_unittest
```

优先使用当前 OHOS workspace 已标准化的构建前端。

## 小 target 选择

选择最小有意义 target：

- JS API 改动：
  `//foundation/distributeddatamgr/data_object/interfaces/jskits:build_module`
- C++ InnerKit 改动：
  `//foundation/distributeddatamgr/data_object/interfaces/innerkits:distributeddataobject_impl`
- 对象存储核心改动：
  `//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/adaptor:flat_object_store`
- 设备通信改动：
  `//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/communicator:communicator`
- WatcherProxy 改动：
  `//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/WatcherProxy:watcher_proxy`
- CacheManager 改动：
  `//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/CacheManager:cache_manager`
- ETS/Taihe 改动：
  `//foundation/distributeddatamgr/data_object/frameworks/ets/taihe:ani_data_object`

## 小测试 target 选择

`frameworks/innerkitsimpl/test/unittest` 下的 target：

```text
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/unittest:distributed_object_impl_test
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/unittest:flat_object_store_test
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/unittest:client_adaptor_test
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/unittest:communicator_test
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/unittest:watcher_proxy_test
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/unittest:cache_manager_test
```

JS 测试：

```text
//foundation/distributeddatamgr/data_object/frameworks/jskitsimpl/test/unittest:js_data_object_test
```

Fuzz 测试：

```text
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/fuzztest:objectstorageengine_fuzzer
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/fuzztest:objectstorecreateobject_fuzzer
//foundation/distributeddatamgr/data_object/frameworks/innerkitsimpl/test/fuzztest:objectstoreput_fuzzer
```

## Feature Flag 控制

部分 target 和测试受 feature flag 控制：

- `data_object_feature_L1`：控制 L1 设备特性编译
- `data_object_api_metrics_enabled`：控制 API Metrics 上报

验证涉及 feature flag 的改动时，需要测试两种配置。

## Mock 与测试支撑

测试 Mock 位于：

```text
frameworks/innerkitsimpl/test/unittest/mock/
```

常用 Mock：

- `mock_distributed_db.*`：DistributedDB mock
- `mock_softbus_adapter.*`：软总线适配器 mock
- `mock_device_manager.*`：设备管理 mock
- `mock_object_service.*`：ObjectService mock
- `mock_ipc.*`：IPC mock

Mock 测试常用：

```text
private=public
protected=public
```

这是有意为之，用于让测试访问内部状态。除非测试 target 不再需要内部可见性，否则不要移除。

## 验证报告要求

报告验证时要明确：

- 运行了哪些命令。
- 环境是独立 Windows checkout 还是完整 OHOS 源码树。
- 如果无法运行完整构建，明确说明，并给出下一个 OHOS 环境应运行的最小 target。
- 涉及跨设备同步的功能，说明是否进行了跨设备验证或说明了验证限制。

## 常见验证场景

| 改动类型 | 最小验证 target | 额外验证 |
| --- | --- | --- |
| JS API 变更 | `build_module` + `js_data_object_test` | 检查 `objectstore_errors.h` 兼容性 |
| 对象存储变更 | `flat_object_store` + `flat_object_store_test` | 验证数据持久化 |
| 设备通信变更 | `communicator` + `communicator_test` | 验证设备发现与传输 |
| 监听器变更 | `watcher_proxy` + `watcher_proxy_test` | 验证监听器注册/移除 |
| Save/RevokeSave 变更 | `cache_manager` + `cache_manager_test` | 验证 IPC 错误处理 |
| 错误码变更 | 任一相关 target | 检查 `docs/error_code_layers.md` |
| sessionId 生成变更 | `distributed_object_impl` + 对应测试 | 验证 sessionId 格式 |
