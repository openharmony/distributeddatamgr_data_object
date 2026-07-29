# AGENTS.md

本文件定义 `data_object` 组件的 Agent 工作规则。

## 1. 适用范围与优先级

- **适用范围**: `//foundation/distributeddatamgr/data_object`
- **子系统**: `distributeddatamgr`
- **Bundle**: `@ohos.data.distributedDataObject`
- **优先级**: 用户指令优先于本文件。原则：**代码优先、证据优先、不臆造**。

## 2. 快速构建与测试

### 构建

```bash
# 构建 data_object 组件（Linux 环境，在 OpenHarmony 根目录执行）
./build.sh --product-name rk3568 --build-target data_object

# 构建测试
./build.sh --product-name rk3568 --build-target data_object_test
```

### 运行测试

```bash
# 单元测试位置（替换为实际测试二进制路径）
./out/rk3568/tests/distributeddatamgr/data_object/unittest/<test_binary>

# 运行单个测试用例
./out/rk3568/tests/distributeddatamgr/data_object/unittest/<test_binary> --gtest_filter=<TestCaseName>
```

### 构建产物

主输出目录: `out/rk3568/foundation/distributeddatamgr/data_object/`

关键产物:
- `libdistributed_object_inner.z.so`: Native 内部库
- `libdistributed_data_object.z.so`: JS/ETS 桥接库
- `*.z.so`: 其他组件库

## 3. 知识库

### 3.1 先查阅文档

**必须：编辑代码前，先阅读相关文档。**

入口文档:
- `docs/native-implementation.md`: Native层实现、sessionId机制、同步流程
- `docs/error_code_layers.md`: 错误码分层、新增错误码原则

### 3.2 任务路由规则

| 任务类型 | 读取文档 | 触发关键词 |
| --- | --- | --- |
| Native实现、sessionId、同步机制 | `docs/native-implementation.md` | sessionId, FlatObjectStore, SoftBusAdapter, DistributedDB, sync |
| 错误码、新增错误码 | `docs/error_code_layers.md` | error code, ERR_IPC, ERR_INVALID_ARGS, error mapping |
| JS/ETS API、NAPI绑定 | `docs/native-implementation.md` + 检查 `interfaces/jskits/` | @ohos.data.distributedDataObject, NAPI, ETS, callback |
| 缓存、Save/RevokeSave | `docs/native-implementation.md` → CacheManager章节 | CacheManager, Save, RevokeSave, persistence |
| Asset类型、变更通知 | `docs/native-implementation.md` → WatcherProxy章节 | Asset, WatcherProxy, ObjectWatcher, change detection |

### 3.3 路径路由规则

| 路径 | 编辑前需读取 |
| --- | --- |
| `frameworks/innerkitsimpl/adaptor/` | `docs/native-implementation.md` → FlatObjectStore, DistributedObjectImpl |
| `frameworks/innerkitsimpl/communicator/` | `docs/native-implementation.md` → SoftBusAdapter, 设备通信 |
| `frameworks/innerkitsimpl/WatcherProxy/` | `docs/native-implementation.md` → WatcherProxy, Asset变更检测 |
| `frameworks/innerkitsimpl/CacheManager/` | `docs/native-implementation.md` → CacheManager, Save/RevokeSave |
| `frameworks/jskitsimpl/` | `docs/native-implementation.md` → JS API章节; 检查NAPI模式 |
| `interfaces/jskits/` | 现有API签名和d.ts文件 |
| `interfaces/innerkits/` | `docs/error_code_layers.md` → Native层错误码 |

### 3.4 词汇路由规则

| 术语 | 读取文档 |
| --- | --- |
| sessionId | `docs/native-implementation.md` → sessionId机制 |
| FlatObjectStore | `docs/native-implementation.md` → FlatObjectStore章节 |
| DistributedDB | `docs/native-implementation.md` → 同步机制 |
| SoftBusAdapter | `docs/native-implementation.md` → 设备通信层 |
| Asset | `docs/native-implementation.md` → WatcherProxy章节 |
| CacheManager | `docs/native-implementation.md` → 缓存管理章节 |
| WatcherProxy | `docs/native-implementation.md` → 变更检测章节 |
| ERR_IPC, ERR_INVALID_ARGS | `docs/error_code_layers.md` → Native层错误码 |

## 4. 编辑前声明

**编辑任何代码前，必须声明：**

1. **任务类别**: 任务类型（API变更、Bug修复、新功能、重构等）
2. **已读文档**: 读取了哪些文档，发现了什么约束
3. **影响范围**: 影响哪些API面（NAPI/ANI/InnerKit/测试）

如果无法确定任务类别或影响范围，**先询问用户**再继续。

## 5. 项目地图

| 目录 | 职责 |
| --- | --- |
| `interfaces/jskits/` | JS公共API：对象创建、会话管理、监听器、持久化（SDK 8/9+兼容） |
| `interfaces/innerkits/` | C++ InnerKit接口定义 |
| `frameworks/jskitsimpl/` | NAPI绑定、监听器管理、类型转换 |
| `frameworks/ets/taihe/` | ETS (ANI) 实现 |
| `frameworks/innerkitsimpl/adaptor/` | 核心：FlatObjectStore、DistributedObjectImpl、ClientAdaptor |
| `frameworks/innerkitsimpl/communicator/` | 设备通信：SoftBusAdapter、DevManager、ProcessCommunicator |
| `frameworks/innerkitsimpl/WatcherProxy/` | Asset/属性变更分离、通知路由 |
| `frameworks/innerkitsimpl/CacheManager/` | Save/RevokeSave流程、缓存策略 |
| `frameworks/innerkitsimpl/common/` | 通用工具类 |

### 任务到路径映射

| 任务 | 从这里开始 |
| --- | --- |
| 新增对象类型 | `frameworks/innerkitsimpl/adaptor/distributed_object_impl.*` |
| 修改同步行为 | `frameworks/innerkitsimpl/adaptor/flat_object_store.*` |
| 修复IPC问题 | `frameworks/innerkitsimpl/adaptor/client_adaptor.*` |
| 新增/修改监听器 | `frameworks/innerkitsimpl/WatcherProxy/` |
| 修改设备通信 | `frameworks/innerkitsimpl/communicator/softbus_adapter.*` |
| 新增JS API | `frameworks/jskitsimpl/`; 在 `interfaces/jskits/` 定义 |
| 新增ETS API | `frameworks/ets/taihe/` |
| 新增错误码 | `interfaces/innerkits/objectstore_errors.h`; 在 `docs/error_code_layers.md` 映射 |

## 6. 硬性边界

### 禁止事项（无用户确认时）

- **修改公共API签名/语义/错误码**：`interfaces/jskits/` 或 `interfaces/innerkits/` 下的API
- **修改sessionId生成逻辑**：未查阅 `docs/native-implementation.md` → sessionId机制时
- **修改同步协议或数据格式**：会破坏跨设备兼容性
- **修改现有API的错误码值**：会破坏兼容性
- **添加其他系统模块依赖**：未经用户批准
- **移除或修改监听器回调签名**：未检查所有调用方时
- **执行破坏性命令**：如 `rm -rf`、`git reset --hard`

### 需先询问

- 任何公共API变更（签名、语义、错误码）
- 新增错误码（先查阅 `docs/error_code_layers.md`）
- 修改同步行为或数据格式
- 在 `BUILD.gn` 或 `bundle.json` 添加新依赖
- 修改IPC通信逻辑
- 影响sessionId机制的变更

## 7. 已知陷阱

- **仅根属性同步**：只有根属性变更触发同步。嵌套属性变更（`object.user.age = 25`）不会同步。需整体赋值（`object.user = {...object.user}`）。
- **IPC错误处理**：IPC调用可能返回 ERR_IPC。必须在 `CacheManager` 中处理重试。
- **监听器生命周期**：使用 `shared_ptr`/`weak_ptr` 避免循环引用。页面销毁时必须移除监听器。
- **线程安全**：对象操作需要互斥锁保护。SessionId 创建后只读。
- **错误码兼容性**：存量API不能返回新错误码。内部错误需映射到现有错误码。
- **日志中的敏感数据**：日志打印前使用 `Anonymous::Change()` 脱敏。

## 8. 核心实现细节

### 关键类

| 类 | 位置 | 职责 |
| --- | --- | --- |
| `DistributedObjectImpl` | `adaptor/distributed_object_impl.*` | 入口类，持有sessionId，管理监听器 |
| `FlatObjectStore` | `adaptor/flat_object_store.*` | 扁平存储引擎，序列化，DistributedDB交互 |
| `ClientAdaptor` | `adaptor/client_adaptor.*` | IPC通信，ObjectService代理 |
| `SoftBusAdapter` | `communicator/softbus_adapter.*` | 设备发现，数据传输 |
| `CacheManager` | `CacheManager/cache_manager.*` | Save/RevokeSave流程，缓存状态 |
| `WatcherProxy` | `WatcherProxy/` | Asset/属性变更分离 |

### 依赖流向

```
NAPI/ETS → C++ core → Entity class → Utility
```

- NAPI/ETS：解析输入、转换错误、管理监听器
- C++ core：可复用逻辑、标准错误码
- Entity：持有运行状态（sessionId、监听器）
- Utility：通用工具、日志

## 9. 验证闭环

### 最小检查项

报告完成前，必须：

1. **构建通过**: `./build.sh --product-name rk3568 --build-target data_object`
2. **无新增编译警告**: 修改的文件无警告
3. **API兼容性**: 公共API签名未变更（或已批准变更）
4. **错误码映射**: 新增错误码按 `docs/error_code_layers.md` 正确映射

### 任务特定验证

| 任务类型 | 额外验证 |
| --- | --- |
| API变更 | 检查d.ts和C++头文件一致性 |
| 同步机制 | 跨设备测试；验证sessionId唯一性 |
| 错误处理 | 验证错误码正确传播 |
| 监听器变更 | 测试回调生命周期，无内存泄漏 |

### 完成定义

任务完成需满足：

1. ✅ 代码变更编译无警告
2. ✅ 相关测试通过（如有测试）
3. ✅ 公共API兼容性已验证
4. ✅ 无禁止性变更引入
5. ✅ 代码遵循现有模式和命名

### 最终响应

报告完成时包含：
- 变更摘要（文件、行数）
- 执行的验证
- 后续需要的事项（如需手动测试）

### 后备方案

如果验证命令无法执行（如Windows环境）：

1. 说明限制
2. 描述应验证的内容
3. 建议用户在Linux环境执行验证