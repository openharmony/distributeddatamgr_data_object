

# Distributed Object User Manual (Small-System Devices)

The distributed data object management framework is an object-oriented in-memory data management framework. It provides application developers with basic data object management capabilities, such as creating, querying, deleting, modifying, and subscribing to in-memory objects. Featuring the distributed capability, this framework supports data object collaboration for the same application between multiple devices that form a Super Device.

## Basic Concepts

The distributed data object management framework consists of the following components:

- Data object: an instance of the data object model class used to store the runtime data of an application.

- Data object store: a data object management class used to insert, query, delete, and subscribe to data objects by using the data object store APIs.


## Supported Data Types

| Type| Description|
| ----------- | ----------------------------------------- |
| int32_t     | 32-bit integer, corresponding to the **int** type of large-system devices.|
| int16_t     | 16-bit integer, corresponding to the **short** type of large-system devices.|
| int64_t     | Long integer.|
| char        | Character type, corresponding to the **char** type of large-system devices.|
| float       | Single-precision floating-point, corresponding to the **float** type of large-system devices.|
| double      | Double-precision floating point, corresponding to the **double** type of large-system devices.|
| boolean     | Boolean type, corresponding to the **boolean** type of large-system devices.|
| std::string | String, corresponding to the **java.lang.String** type of large-system devices.|
| int8_t      | 8-bit integer, corresponding to the **byte** type of large-system devices.|
## Development Guides
### Available APIs
- **OHOS::ObjectStore::DistributedObjectStoreManager** in the **distributed_objectstore_manager.h** file is used to create and destroy a distributed data object store.

| API| Description|
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| static DistributedObjectStoreManager* GetInstance()          | Obtains a **DistributedObjectStoreManager** instance.|
| DistributedObjectStore *Create(const std::string &bundleName, const std::string &sessionId) | Creates a distributed data object store. <br>**bundleName** must be the same as **bundleName** of the HAP package for the large-system device. <br>**sessionId** must be the same as the **sessionId** used when the large-system device creates the distributed data object store.|
| uint32_t Destroy(const std::string &bundleName, const std::string &sessionId) | Destroys a distributed data object store. <br>**bundleName** must be the same as **bundleName** of the HAP package for the large-system device. <br>**sessionId** must be the same as the **sessionId** used when the large-system device creates the distributed data object store.|

- **OHOS::ObjectStore::DistributedObjectStore** in the **distributed_objectstore.h** file is used to create and synchronize a distributed data object.

| API| Description|
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| DistributedObject *CreateObject(const std::string &classPath, const std::string &key) | Creates a distributed data object. <br>**classPath** must be the same as the **class** path when the large-system device creates the distributed data object, for example, **com.testobject.dosdemo1.model.Student**. <br>**key** must be the same as the **key** used when the large-system device creates the distributed data object. A class can have multiple objects, and **key** uniquely identifies an object in the class.|
| uint32_t Sync(DistributedObject *object)                     | Synchronizes a distributed data object.|
| void Close()                                                 | Closes this distributed data object store. This API is automatically called in **DistributedObjectStoreManager.destroy**.|
| uint32_t Watch(DistributedObject *object, std::shared_ptr<ObjectWatcher> objectWatcher) | Listens for object changes.|
| uint32_t UnWatch(DistributedObject *object)                  | Cancels object listening. |
| uint32_t Get(const std::string &objectId, DistributedObject *object) | Obtains an object based on **objectId**.|
| uint32_t Delete(DistributedObject *object)                   | Deletes a distributed data object.|

- **ObjectWatcher** is used to listen for object changes.

| API| Description|
| ------------------------------------- | ----------- |
| void OnChanged(const std::string &id) | Callback invoked when an object changes.|
| void OnDeleted(const std::string &id) | Callback invoked when an object is deleted.|

- **OHOS::ObjectStore::DistributedObject** in the **distributed_object.h** file is used to manage object attributes.

| API| Description|
| ------------------------------------------------------------ | ------------------------------------------------------------ |
| uint32_t PutChar(const std::string &key, char value)         | Adds or updates an attribute of the **char** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the new value of the attribute.|
| uint32_t PutInt(const std::string &key, int32_t value)       | Adds or updates an attribute of the **int32_t** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the new value of the attribute.|
| uint32_t PutShort(const std::string &key, int16_t value)     | Adds or updates an attribute of the **int16_t** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the new value of the attribute.|
| uint32_t PutLong(const std::string &key, int64_t value)      | Adds or updates an attribute of the **int64_t** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the new value of the attribute.|
| uint32_t PutFloat(const std::string &key, float value)       | Adds or updates an attribute of the **float** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the new value of the attribute.|
| uint32_t PutDouble(const std::string &key, double value)     | Adds or updates an attribute of the **double** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the new value of the attribute.|
| uint32_t PutBoolean(const std::string &key, bool value)      | Adds or updates an attribute of the **bool** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the new value of the attribute.|
| uint32_t PutString(const std::string &key, const std::string &value) | Adds or updates an attribute of the **std::string** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the new value of the attribute.|
| uint32_t PutByte(const std::string &key, const int8_t &value) | Adds or updates an attribute of the **int8_t** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the new value of the attribute.|
| uint32_t GetChar(const std::string &key, char &value)        | Obtains an attribute of the **char** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the attribute value obtained.|
| uint32_t GetInt(const std::string &key, int32_t &value)      | Obtains an attribute of the **int32_t** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the attribute obtained.|
| uint32_t GetShort(const std::string &key, int16_t &value)    | Obtains an attribute of the **int16_t** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the attribute obtained.|
| uint32_t GetLong(const std::string &key, int64_t &value)     | Obtains an attribute of the **int64_t** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the attribute obtained.|
| uint32_t GetFloat(const std::string &key, float &value)      | Obtains an attribute of the **float** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the attribute obtained.|
| uint32_t GetDouble(const std::string &key, double &value)    | Obtains an attribute of the **double** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the attribute obtained.|
| uint32_t GetBoolean(const std::string &key, bool &value)     | Obtains an attribute of the **bool** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the attribute obtained.|
| uint32_t GetString(const std::string &key, std::string &value) | Obtains an attribute of the **std::string** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the attribute obtained.|
| uint32_t GetByte(const std::string &key, int8_t &value)      | Obtains an attribute of the **int8_t** type. <br>**key** indicates the attribute name, which corresponds to the object name identified by **@Field** on the large-system device. <br>**value** indicates the attribute obtained.|

| API| Description|
| ------------------------------------------- | -----------  |
| uint32_t GetObjectId(std::string &objectId) | Obtains **objectId**.|

### How to Develop
#### 1. Create a project.
  Configure the **build.gn** file.

  a) Add the following configuration to the **public_deps** node:

```
public_deps = [
      "//foundation/distributeddatamgr/objectstore:objectstore_sdk"
    ]

```

  b) Set the C++ version to **C++17**.

```
static_library("objectstoremgr") {
  configs -= [ "//build/lite/config:language_cpp" ]
   cflags = [ "-fPIC" ]
   cflags_cc = cflags
   cflags_cc += [ "-std=c++17" ]
  configs += [ ":objectStore_config" ]
```
#### 2. Obtain a **DistributedObjectStoreManager** instance.
```c
DistributedObjectStoreManager *ptr = DistributedObjectStoreManager::GetInstance();
if (ptr == nullptr) {
	return 0;
}
```
#### 3. Create a distributed data object store.
```c
DistributedObjectStore *store = ptr->Create(BUNDLE_NAME, SESSION_ID);
TEST_ASSERT_TRUE(store != nullptr);
```
#### 4. Create a distributed data object.

```c
 DistributedObject *object = store->CreateObject("com.testobject.dosdemo1.model.Student", "key1");
 TEST_ASSERT_TRUE(object != nullptr);
```
#### 5. Modify the data object at the local device.
```c
// Change the value of the **name** attribute to **myTestName**.
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
#### 6. Synchronize the data object to a mobile phone.

```c
store->Sync(object);
```

#### 7. Obtain the peer data object.

```c
// Obtain the value of **name**, which is of the **std::string** type.
std::string stringVal;
ret = object->GetString("name", stringVal);
TEST_ASSERT_TRUE(ret == 0);
// Obtain the value of **age**, which is of the **int32_t** type.
int32_t intVal;
ret = object->GetInt("age", intVal);
TEST_ASSERT_TRUE(ret == 0);
// Obtain the value of **testChar**, which is of the **char** type.
char charVal;
ret = object->GetChar("testChar", charVal);
TEST_ASSERT_TRUE(ret == 0);
// Obtain the value of **testShort**, which is of the **int16_t** type.
int16_t shortVal;
ret = object->GetShort("testShort", shortVal);               
// Obtain the value of **testLong**, which is of the **int64_t** type.
int64_t longVal;
ret = object->GetLong("testLong", longVal);
TEST_ASSERT_TRUE(ret == 0);
// Obtain the value of **testFloat**, which is of the **float** type.
float floatVal;
ret = object->GetFloat("testFloat", floatVal);
TEST_ASSERT_TRUE(ret == 0);
// Obtain the value of **testDouble**, which is of the **double** type.
double doubleVal;
ret = object->GetDouble("testDouble", doubleVal);
TEST_ASSERT_TRUE(ret == 0);
// Obtain the value of **testBoolean**, which is of the **bool** type.
bool boolVal;
ret = object->GetBoolean("testBoolean", boolVal);
TEST_ASSERT_TRUE(ret == 0);
```

#### 8. Implement **ObjectWatcher**.
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
  // Obtain the data object when the data changes.
  ret = store->Get(id, object);
  TEST_ASSERT_TRUE(ret == 0);
  // Callback invoked when data changes. You should implement this callback.
  ...
}

void TestObjectWatcher::OnDeleted(const std::string &id)
{
  // Callback invoked when data is deleted. You should implement this callback.
  ...
}
```

#### 9. Listen for data objects and cancel the listening.
After **ObjectWatcher** is implemented, you can listen for data objects. After listening is canceled, **OnChanged** will not be called back when a data object changes.
```c
// Listen for the data object.
ret = store->Watch(object, watcher);
TEST_ASSERT_TRUE(ret == 0);
// Cancel the listening of the data object.
ret = store->UnWatch(object);
TEST_ASSERT_TRUE(ret == 0);
```

#### 10. Delete the data object.
After **delete()** is called, the distributed data object store deletes the data object.
```c
ret = store->Delete(object);
TEST_ASSERT_TRUE(ret == 0);
```

#### 11. Close the distributed data object store.
After using the distributed data object store, call **Destroy()** to close it. After the store is closed, all data objects in the store will be destroyed.
```c
ptr->Destroy(BUNDLE_NAME, SESSION_ID);
```
