

# 分布式对象用户手册（小型设备）

分布式数据对象管理框架是一款面向对象的内存数据管理框架，向应用开发者提供内存对象的创建、查询、删除、修改、订阅等基本数据对象的管理能力，同时具备分布式能力，满足超级终端场景下，相同应用多设备间的数据对象协同需求。 

## 基本概念

分布式数据对象管理框架的两个主要组件： 

 •     数据对象：数据对象模型类的实例，用于存储应用程序的运行时数据。

 •     数据对象仓库：数据对象的管理类，通过数据对象仓库的接口完成数据对象的插入、查找、删除、订阅等操作。 

##支持的数据类型


| 类型名称    | 类型描述                                  |
| ----------- | ----------------------------------------- |
| int32_t     | 32位整型，对应大型设备int类型             |
| int16_t     | 16位整型，对应大型设备short类型           |
| int64_t     | 封装长整型                                |
| char        | 字符型，对应大型设备char类型              |
| float       | 单精度浮点型，对应大型设备float类型       |
| double      | 双精度浮点型，对应大型设备double类型      |
| boolean     | 布尔型，对应大型设备boolean类型           |
| std::string | 字符串型,对应大型设备java.lang.String类型 |
| int8_t      | 8位整型，对应大型设备byte类型             |
## 开发指导
### 接口说明
•	对象仓库管理
distributed_objectstore_manager.h中OHOS::ObjectStore::DistributedObjectStoreManager用于创建，销毁分布式对象仓库。

| 接口名称                                                     | 描述                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| static DistributedObjectStoreManager* GetInstance()          | 获取DistributedObjectStoreManager实例                        |
| DistributedObjectStore *Create(const std::string &bundleName, const std::string &sessionId) | 创建分布式对象仓库<br>bundleName和大型设备hap包的bundleName一致<br>sessionId和大型设备构建分布式对象仓库时的sessionId一致 |
| uint32_t Destroy(const std::string &bundleName, const std::string &sessionId) | 销毁分布式对象仓库<br>bundleName和大型设备hap包的bundleName一致<br>sessionId和大型设备构建分布式对象仓库时的sessionId一致 |

•	对象仓库
distributed_objectstore.h中OHOS::ObjectStore::DistributedObjectStore用于创建，同步分布式对象仓库。

| 接口名称                                                     | 描述                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| DistributedObject *CreateObject(const std::string &classPath, const std::string &key) | 创建分布式对象<br>classPath和大型设备创建对象时class的路径相同，如com.testobject.dosdemo1.model.Student<br>key和大型设备创建分布式对象时key相同，同一个class中可以标识一个对象（一个class可以创建多个对象） |
| uint32_t Sync(DistributedObject *object)                     | 同步分布式对象                                               |
| void Close()                                                 | 关闭仓库，无需调用，在DistributedObjectStoreManager.destroy时自动调用 |
| uint32_t Watch(DistributedObject *object, std::shared_ptr<ObjectWatcher> objectWatcher) | 监听object变化                            |
| uint32_t UnWatch(DistributedObject *object)                  | 删除监听                                                     |
| uint32_t Get(const std::string &objectId, DistributedObject *object) | 根据objectId获取对象                                 |
| uint32_t Delete(DistributedObject *object)                   | 删除分布式对象                                               |

ObjectWatcher用于监听对象变化
| 接口名称                               | 描述        |
| ------------------------------------- | ----------- |
| void OnChanged(const std::string &id) | 对象变更回调 |
| void OnDeleted(const std::string &id) | 对象删除回调 |

•	对象

distributed_object.h中OHOS::ObjectStore::DistributedObject用于管理对象属性。

| 接口名称                                                     | 描述                                                         |
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| uint32_t PutChar(const std::string &key, char value)         | 增加或更新类型为char的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是刷新的内容 |
| uint32_t PutInt(const std::string &key, int32_t value)       | 增加或更新类型为int32_t的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是刷新的内容 |
| uint32_t PutShort(const std::string &key, int16_t value)     | 增加或更新类型为int16_t的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是刷新的内容 |
| uint32_t PutLong(const std::string &key, int64_t value)      | 增加或更新类型为int64_t的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是刷新的内容 |
| uint32_t PutFloat(const std::string &key, float value)       | 增加或更新类型为float的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是刷新的内容 |
| uint32_t PutDouble(const std::string &key, double value)     | 增加或更新类型为double的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是刷新的内容 |
| uint32_t PutBoolean(const std::string &key, bool value)      | 增加或更新类型为bool的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是刷新的内容 |
| uint32_t PutString(const std::string &key, const std::string &value) | 增加或更新类型为std::string的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是刷新的内容 |
| uint32_t PutByte(const std::string &key, const int8_t &value) | 获取类型为int8_t的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是刷新的内容 |
| uint32_t GetChar(const std::string &key, char &value)        | 获取类型为char的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是获取的内容 |
| uint32_t GetInt(const std::string &key, int32_t &value)      | 获取类型为int32_t的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是获取的内容 |
| uint32_t GetShort(const std::string &key, int16_t &value)    | 获取类型为int16_t的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是获取的内容 |
| uint32_t GetLong(const std::string &key, int64_t &value)     | 获取类型为int64_t的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是获取的内容 |
| uint32_t GetFloat(const std::string &key, float &value)      | 获取类型为float的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是获取的内容 |
| uint32_t GetDouble(const std::string &key, double &value)    | 获取类型为double的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是获取的内容 |
| uint32_t GetBoolean(const std::string &key, bool &value)     | 获取类型为bool的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是获取的内容 |
| uint32_t GetString(const std::string &key, std::string &value) | 获取类型为std::string的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是获取的内容 |
| uint32_t GetByte(const std::string &key, int8_t &value)      | 获取类型为int8_t的属性<br>key 标识属性名，和大型设备侧用@Field标识的对象名对应<br>value是获取的内容 |

| 接口名称                                    | 描述          |
| ------------------------------------------- | -----------  |
| uint32_t GetObjectId(std::string &objectId) | 获取objectId |

### 开发步骤
 #### 1.配置工程
  配置build.gn文件

•    在public_deps节点中添加以下配置：

```
public_deps = [
      "//foundation/distributeddatamgr/objectstore:objectstore_sdk"
    ]

```

•    设置用c++版本为c++17：

```
static_library("objectstoremgr") {
  configs -= [ "//build/lite/config:language_cpp" ]
   cflags = [ "-fPIC" ]
   cflags_cc = cflags
   cflags_cc += [ "-std=c++17" ]
  configs += [ ":objectStore_config" ]
```
  #### 2.获取分布式对象仓库管理类
```c
DistributedObjectStoreManager *ptr = DistributedObjectStoreManager::GetInstance();
if (ptr == nullptr) {
	return 0;
}
  ```
 #### 3.创建对象仓库
```c
DistributedObjectStore *store = ptr->Create(BUNDLE_NAME, SESSION_ID);
TEST_ASSERT_TRUE(store != nullptr);
```
#### 4.创建数据对象

```c
 DistributedObject *object = store->CreateObject("com.testobject.dosdemo1.model.Student", "key1");
 TEST_ASSERT_TRUE(object != nullptr);
```
#### 5.修改数据对象到本地
```c
//更新name属性的值为 myTestName
int ret = object->PutString("name", "myTestName");
TEST_ASSERT_TRUE(ret == 0);
ret = object->PutInt("age", 18);
TEST_ASSERT_TRUE(ret == 0);
ret = object->PutLong("testLong", 1);
TEST_ASSERT_TRUE(ret == 0);
ret = object->PutDouble("testDouble", 1.234);
TEST_ASSERT_TRUE(ret == 0);
ret = object->PutFloat("testFloat", 1.23456);
TEST_ASSERT_TRUE(ret == 0);
ret = object->PutShort("testShort", 2);
TEST_ASSERT_TRUE(ret == 0);
ret = object->PutBoolean("testBoolean", false);
TEST_ASSERT_TRUE(ret == 0);
ret = object->PutChar("testChar", 'a');
TEST_ASSERT_TRUE(ret == 0);
```
#### 6.同步数据对象到手机

```c
store->Sync(object);
```

  #### 7.获取对端数据对象

```c
//获取"name"对应的std::string类型的值
std::string stringVal;
ret = object->GetString("name", stringVal);
TEST_ASSERT_TRUE(ret == 0);
//获取"age"对应的int32_t类型的值
int32_t intVal;
ret = object->GetInt("age", intVal);
TEST_ASSERT_TRUE(ret == 0);
//获取"testChar"对应的char类型的值                
char charVal;
ret = object->GetChar("testChar", charVal);
TEST_ASSERT_TRUE(ret == 0);
//获取"testShort"对应的int16_t类型的值
int16_t shortVal;
ret = object->GetShort("testShort", shortVal);               
//获取"testLong"对应的int64_t类型的值
int64_t longVal;
ret = object->GetLong("testLong", longVal);
TEST_ASSERT_TRUE(ret == 0);
//获取"testFloat"对应的float类型的值
float floatVal;
ret = object->GetFloat("testFloat", floatVal);
TEST_ASSERT_TRUE(ret == 0);
//获取"testDouble"对应的double类型的值
double doubleVal;
ret = object->GetDouble("testDouble", doubleVal);
TEST_ASSERT_TRUE(ret == 0);
//获取"testBoolean"对应的bool类型的值
bool boolVal;
ret = object->GetBoolean("testBoolean", boolVal);
TEST_ASSERT_TRUE(ret == 0);
```

  #### 8.实现ObjectWatcher
```c
class TestObjectWatcher:public ObjectWatcher{

public:
    TestObjectWatcher();
    void OnChanged(const std::string &id) override;
    void OnDeleted(const std::string &id) override;
};

TestObjectWatcher::TestObjectWatcher() {}
void TestObjectWatcher::OnChanged(const std::string &id)
{
  //数据变更时获取到数据对象
  ret = store->Get(id, object);
  TEST_ASSERT_TRUE(ret == 0);
  //开发者自实现数据变更回调
  ...
}

void TestObjectWatcher::OnDeleted(const std::string &id)
{
  //开发者自实现删除回调
  ...
}
```

  #### 9.监听/取消监听数据对象
在实现ObjectWatcher之后，可以对数据对象进行监听，取消监听后，数据对象改变时不会回调OnChanged接口。
```c
//监听数据对象
ret = store->Watch(object, watcher);
TEST_ASSERT_TRUE(ret == 0);
//取消监听数据对象
ret = store->UnWatch(object);
TEST_ASSERT_TRUE(ret == 0);
```

  #### 10.删除数据对象
数据对象调用delete接口后，数据对象仓库会将其删除。
```c
ret = store->Delete(object);
TEST_ASSERT_TRUE(ret == 0);
```

  #### 11.关闭数据对象仓库
数据对象仓库使用完毕后，需要调用Destroy接口进行关闭，关闭后所有的数据对象都会被销毁。
```c
ptr->Destroy(BUNDLE_NAME, SESSION_ID);
```