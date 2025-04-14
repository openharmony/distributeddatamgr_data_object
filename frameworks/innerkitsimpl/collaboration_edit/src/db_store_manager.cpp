/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "DBStoreManager"

#include "db_store_manager.h"

#include <dirent.h>
#include <securec.h>
#include <sys/stat.h>

#include "db_common.h"
#include "log_print.h"
#include "rd_utils.h"
#include "cloud_db_proxy.h"

namespace OHOS::CollaborationEdit {
GRD_ThreadPoolT DBStoreManager::threadPool_ = {reinterpret_cast<GRD_ScheduleFunc>(DBStoreManager::Schedule)};
std::shared_ptr<ExecutorPool> DBStoreManager::executorPool_ = nullptr;
std::shared_ptr<DBStoreMgrThreadPool> DBStoreManager::executors_ = nullptr;
constexpr size_t MAX_THREADS_SIZE = 12;
constexpr size_t MIN_THREADS_SIZE = 5;

DBStoreManager &DBStoreManager::GetInstance()
{
    static DBStoreManager manager;
    return manager;
}

DBStoreManager::DBStoreManager()
{}

DBStoreManager::~DBStoreManager()
{}

std::shared_ptr<DBStore> DBStoreManager::GetDBStore(const DBStoreConfig &config)
{
    std::lock_guard<std::mutex> lock(mutex_);
    auto it = storeCache_.find(config.GetName());
    if (it != storeCache_.end()) {
        return it->second;
    }
    std::shared_ptr<DBStore> dbStore = OpenDBStore(config);
    if (dbStore != nullptr) {
        storeCache_[config.GetName()] = dbStore;
    }
    return dbStore;
}

std::shared_ptr<DBStore> DBStoreManager::OpenDBStore(const DBStoreConfig &dbConfig)
{
    std::string dbFilePath = dbConfig.GetPath();
    std::string name = dbConfig.GetName();
    LOG_DEBUG("[OpenDBStore] start");
    // create database and oplog directory if not exists
    if (access(dbFilePath.c_str(), F_OK) != 0) {
        int ret = DBCommon::CreateDirectory(dbFilePath);
        if (ret != 0) {
            LOG_ERROR("[OpenDBStore] Create base dir go wrong, ret = %{public}d", ret);
            return nullptr;
        }
        // creating oplog dir here is to provide group permission for other processes access
        std::string opLogDir = dbFilePath + "/" + name + OP_LOG_DIR;
        ret = DBCommon::CreateDirectory(opLogDir);
        if (ret != 0) {
            LOG_ERROR("[OpenDBStore] Create oplog dir go wrong, ret = %{public}d", ret);
            return nullptr;
        }
    }

    GRD_DB *db = nullptr;
    std::string dbFile = dbFilePath + "/" + name;
    std::string config = R"({"sharedModeEnable":1,"redoFlushByTrx":1})";
    int32_t errCode = RdUtils::RdDbOpen(dbFile.c_str(), config.c_str(), GRD_DB_OPEN_CREATE, &db);
    if (errCode != GRD_OK || db == nullptr) {
        LOG_ERROR("[OpenDBStore] open db go wrong, err = %{public}d", errCode);
        return nullptr;
    }
    LOG_INFO("[OpenDBStore] open db successfully");
    std::shared_ptr<DBStore> dbStore = std::make_shared<DBStore>(db, name);
    return dbStore;
}

int DBStoreManager::DeleteDBStore(const DBStoreConfig &config)
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::string dbPath = config.GetPath();
    if (dbPath.empty()) {
        LOG_ERROR("[DeleteDBStore] empty path");
        return -1;
    }
    auto it = storeCache_.find(config.GetName());
    if (it != storeCache_.end()) {
        std::shared_ptr<DBStore> dbStorePtr = it->second;
        int32_t errCode = RdUtils::RdDbClose(dbStorePtr->GetDB(), GRD_DB_CLOSE);
        if (errCode != GRD_OK) {
            LOG_ERROR("[DeleteDBStore] db close go wrong, err = %{public}d", errCode);
            return -1;
        }
        dbStorePtr->FreeCloudDB();
    }
    int ret = RemoveDir(dbPath.c_str());
    if (ret != 0) {
        LOG_ERROR("[DeleteDBStore] remove dir go wrong");
        return ret;
    }
    LOG_INFO("[DeleteDBStore] delete db successfully");
    storeCache_.erase(config.GetName());
    return 0;
}

int DBStoreManager::RemoveDir(const char *dir)
{
    if (access(dir, F_OK) != 0) {
        return 0;
    }

    struct stat dirStat;
    if (stat(dir, &dirStat) < 0) {
        LOG_ERROR("[RemoveDir] get dir stat go wrong");
        return -1;
    }

    char dirName[PATH_MAX];
    DIR *dirPtr = nullptr;
    struct dirent *dr = nullptr;
    if (S_ISREG(dirStat.st_mode)) {  // normal file
        remove(dir);
    } else if (S_ISDIR(dirStat.st_mode)) {
        dirPtr = opendir(dir);
        while ((dr = readdir(dirPtr)) != nullptr) {
            // ignore . and ..
            if ((strcmp(".", dr->d_name) == 0) || (strcmp("..", dr->d_name) == 0)) {
                continue;
            }
            if (sprintf_s(dirName, PATH_MAX, "%s/%s", dir, dr->d_name) <= 0) {
                LOG_ERROR("[RemoveDir] dirName too long.");
                closedir(dirPtr);
                return -1;
            }
            RemoveDir(dirName);
        }
        closedir(dirPtr);
        rmdir(dir);  // remove empty dir
    } else {
        LOG_ERROR("[RemoveDir] unknown file type, st mode %{public}d", dirStat.st_mode);
    }
    return 0;
}

int DBStoreManager::SetCloudDb(std::shared_ptr<DBStore> dbStore, NapiCloudDb *napiCloudDb)
{
    AssetLoaderT *assetLoader = (AssetLoaderT *)malloc(sizeof(AssetLoaderT));
    if (assetLoader == nullptr) {
        LOG_ERROR("[SetCloudDb] malloc AssetLoaderT go wrong");
        return -1;
    }
    assetLoader->downloadAsset = CloudDbProxy::DownloadAsset;
    assetLoader->uploadAsset = CloudDbProxy::UploadAsset;
    assetLoader->deleteAsset = CloudDbProxy::DeleteAsset;
    assetLoader->deleteLocalAsset = CloudDbProxy::DeleteLocalAsset;

    CloudDbProxy *cloudDbProxy = new (std::nothrow) CloudDbProxy();
    if (cloudDbProxy == nullptr) {
        LOG_ERROR("[SetCloudDb] create cloudDbProxy go wrong");
        free(assetLoader);
        return -1;
    }
    cloudDbProxy->SetNapiCloudDb(napiCloudDb);

    GRD_ICloudDBT *cloudDB = (GRD_ICloudDBT *)malloc(sizeof(GRD_ICloudDBT));
    if (cloudDB == nullptr) {
        LOG_ERROR("[SetCloudDb] malloc ICloudDBT go wrong");
        free(assetLoader);
        delete cloudDbProxy;
        return -1;
    }
    cloudDB->assetLoader = assetLoader;
    cloudDB->cloudDB = reinterpret_cast<void *>(cloudDbProxy);
    cloudDB->batchInsert = CloudDbProxy::BatchInsert;
    cloudDB->query = CloudDbProxy::Query;
    cloudDB->sendAwarenessData = CloudDbProxy::SendAwarenessData;
    cloudDB->heartBeat = CloudDbProxy::HeartBeat;
    cloudDB->lock = CloudDbProxy::Lock;
    cloudDB->unLock = CloudDbProxy::UnLock;
    cloudDB->close = CloudDbProxy::Close;

    int32_t errCode = RdUtils::RdSetCloudDb(dbStore->GetDB(), cloudDB);
    if (errCode != GRD_OK) {
        LOG_ERROR("[SetCloudDb] RdSetCloudDb go wrong, errCode = %{public}d", errCode);
        free(assetLoader);
        free(cloudDB);
        delete cloudDbProxy;
        return -1;
    }
    dbStore->SetCloudDbProxy(cloudDbProxy, cloudDB);
    return 0;
}

void DBStoreManager::Schedule(void *func, void *param)
{
    if (executors_ == nullptr) {
        LOG_ERROR("executors_ is nullptr.");
        return;
    }
    executors_->Execute([func, param]() {
        void (*funcPtr)(void *) = reinterpret_cast<void (*)(void *)>(func);
        funcPtr(param);
    });
}

void DBStoreManager::InitThreadPool()
{
    std::lock_guard<std::mutex> lock(threadPoolMutex_);
    if (executorPool_ == nullptr) {
        executorPool_ = std::make_shared<ExecutorPool>(MAX_THREADS_SIZE, MIN_THREADS_SIZE);
    }
    if (executors_ == nullptr) {
        executors_ = std::make_shared<DbThreadPool>(DBStoreManager::executorPool_);
        LOG_INFO("init thread pool success");
    }
}
} // namespace OHOS::CollaborationEdit
