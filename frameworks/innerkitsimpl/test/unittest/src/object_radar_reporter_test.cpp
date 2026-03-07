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

#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <chrono>

#include "object_radar_reporter.h"

using namespace testing::ext;
using namespace OHOS::ObjectStore;
using namespace std;

namespace {
class ObjectRadarReporterTest : public testing::Test {
public:
    static void SetUpTestCase(void);
    static void TearDownTestCase(void);
    void SetUp();
    void TearDown();
};

void ObjectRadarReporterTest::SetUpTestCase(void)
{
}

void ObjectRadarReporterTest::TearDownTestCase(void)
{
}

void ObjectRadarReporterTest::SetUp(void)
{
}

void ObjectRadarReporterTest::TearDown(void)
{
}

/**
 * @tc.name: ReportStateFinished_001
 * @tc.desc: Test ReportStateFinished with CREATE scene INIT_STORE stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_001, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_002
 * @tc.desc: Test ReportStateFinished with CREATE scene CREATE_TABLE stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_002, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::CREATE_TABLE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_003
 * @tc.desc: Test ReportStateFinished with CREATE scene RESTORE stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_003, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::RESTORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_004
 * @tc.desc: Test ReportStateFinished with CREATE scene TRANSFER stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_004, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::TRANSFER;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_005
 * @tc.desc: Test ReportStateFinished with SAVE scene SAVE_TO_SERVICE stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_005, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_006
 * @tc.desc: Test ReportStateFinished with SAVE scene SAVE_TO_STORE stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_006, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_007
 * @tc.desc: Test ReportStateFinished with SAVE scene PUSH_ASSETS stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_007, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::PUSH_ASSETS;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_008
 * @tc.desc: Test ReportStateFinished with SAVE scene SYNC_DATA stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_008, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SYNC_DATA;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_009
 * @tc.desc: Test ReportStateFinished with DATA_RESTORE scene DATA_RECV stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_009, TestSize.Level1)
{
    std::string func = "RestoreData";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_010
 * @tc.desc: Test ReportStateFinished with DATA_RESTORE scene ASSETS_RECV stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_010, TestSize.Level1)
{
    std::string func = "RestoreData";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::ASSETS_RECV;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_011
 * @tc.desc: Test ReportStateFinished with DATA_RESTORE scene NOTIFY stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_011, TestSize.Level1)
{
    std::string func = "RestoreData";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::NOTIFY;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_012
 * @tc.desc: Test ReportStateFinished with RADAR_FAILED result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_012, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_013
 * @tc.desc: Test ReportStateFinished with CANCELLED result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_013, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::CANCELLED;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_014
 * @tc.desc: Test ReportStateFinished with IDLE result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_014, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::IDLE;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_015
 * @tc.desc: Test ReportStateFinished with empty function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_015, TestSize.Level1)
{
    std::string func = "";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_016
 * @tc.desc: Test ReportStateFinished with long function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_016, TestSize.Level1)
{
    std::string func(256, 'a');
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_017
 * @tc.desc: Test ReportStateFinished with special characters in function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_017, TestSize.Level1)
{
    std::string func = "Test@Func#With$Special%Chars&*()";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_018
 * @tc.desc: Test ReportStateFinished with unicode characters in function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_018, TestSize.Level1)
{
    std::string func = "Test函数名测试";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateFinished(func, scene, stage, stageRes, state);
}

/**
 * @tc.name: ReportStateFinished_019
 * @tc.desc: Test ReportStateFinished multiple calls in sequence
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_019, TestSize.Level1)
{
    for (int i = 0; i < 10; i++) {
        std::string func = "TestFunc_" + std::to_string(i);
        RadarReporter::ReportStateFinished(func, BizScene::CREATE, CreateStage::INIT_STORE,
            StageRes::RADAR_SUCCESS, BizState::FINISHED);
    }
}

/**
 * @tc.name: ReportStateFinished_020
 * @tc.desc: Test ReportStateFinished with all combinations
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateFinished_020, TestSize.Level1)
{
    std::vector<int32_t> scenes = {BizScene::CREATE, BizScene::SAVE, BizScene::DATA_RESTORE};
    std::vector<int32_t> stageResults = {StageRes::IDLE, StageRes::RADAR_SUCCESS, 
        StageRes::RADAR_FAILED, StageRes::CANCELLED};
    
    for (auto scene : scenes) {
        for (auto stageRes : stageResults) {
            RadarReporter::ReportStateFinished("TestFunc", scene, 1, stageRes, BizState::FINISHED);
        }
    }
}

/**
 * @tc.name: ReportStage_001
 * @tc.desc: Test ReportStage with CREATE scene INIT_STORE stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_001, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_002
 * @tc.desc: Test ReportStage with CREATE scene CREATE_TABLE stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_002, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::CREATE_TABLE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_003
 * @tc.desc: Test ReportStage with CREATE scene RESTORE stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_003, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::RESTORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_004
 * @tc.desc: Test ReportStage with CREATE scene TRANSFER stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_004, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::TRANSFER;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_005
 * @tc.desc: Test ReportStage with SAVE scene SAVE_TO_SERVICE stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_005, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_006
 * @tc.desc: Test ReportStage with SAVE scene SAVE_TO_STORE stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_006, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_007
 * @tc.desc: Test ReportStage with SAVE scene PUSH_ASSETS stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_007, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::PUSH_ASSETS;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_008
 * @tc.desc: Test ReportStage with SAVE scene SYNC_DATA stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_008, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SYNC_DATA;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_009
 * @tc.desc: Test ReportStage with DATA_RESTORE scene DATA_RECV stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_009, TestSize.Level1)
{
    std::string func = "RestoreData";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_010
 * @tc.desc: Test ReportStage with DATA_RESTORE scene ASSETS_RECV stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_010, TestSize.Level1)
{
    std::string func = "RestoreData";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::ASSETS_RECV;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_011
 * @tc.desc: Test ReportStage with DATA_RESTORE scene NOTIFY stage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_011, TestSize.Level1)
{
    std::string func = "RestoreData";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::NOTIFY;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_012
 * @tc.desc: Test ReportStage with RADAR_FAILED result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_012, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_013
 * @tc.desc: Test ReportStage with CANCELLED result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_013, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::CANCELLED;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_014
 * @tc.desc: Test ReportStage with IDLE result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_014, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::IDLE;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_015
 * @tc.desc: Test ReportStage with empty function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_015, TestSize.Level1)
{
    std::string func = "";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_016
 * @tc.desc: Test ReportStage with long function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_016, TestSize.Level1)
{
    std::string func(512, 'b');
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_017
 * @tc.desc: Test ReportStage with special characters
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_017, TestSize.Level1)
{
    std::string func = "Func@#$%^&*()_+-={}[]|\\:;\"'<>,.?/~`";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    
    RadarReporter::ReportStage(func, scene, stage, stageRes);
}

/**
 * @tc.name: ReportStage_018
 * @tc.desc: Test ReportStage multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_018, TestSize.Level1)
{
    for (int i = 0; i < 20; i++) {
        std::string func = "Stage_" + std::to_string(i);
        RadarReporter::ReportStage(func, BizScene::CREATE, i % 4 + 1, StageRes::RADAR_SUCCESS);
    }
}

/**
 * @tc.name: ReportStage_019
 * @tc.desc: Test ReportStage with all CREATE stages
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_019, TestSize.Level1)
{
    std::string func = "TestAllCreateStages";
    int32_t scene = BizScene::CREATE;
    
    RadarReporter::ReportStage(func, scene, CreateStage::INIT_STORE, StageRes::RADAR_SUCCESS);
    RadarReporter::ReportStage(func, scene, CreateStage::CREATE_TABLE, StageRes::RADAR_SUCCESS);
    RadarReporter::ReportStage(func, scene, CreateStage::RESTORE, StageRes::RADAR_SUCCESS);
    RadarReporter::ReportStage(func, scene, CreateStage::TRANSFER, StageRes::RADAR_SUCCESS);
}

/**
 * @tc.name: ReportStage_020
 * @tc.desc: Test ReportStage with all SAVE stages
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStage_020, TestSize.Level1)
{
    std::string func = "TestAllSaveStages";
    int32_t scene = BizScene::SAVE;
    
    RadarReporter::ReportStage(func, scene, SaveStage::SAVE_TO_SERVICE, StageRes::RADAR_SUCCESS);
    RadarReporter::ReportStage(func, scene, SaveStage::SAVE_TO_STORE, StageRes::RADAR_SUCCESS);
    RadarReporter::ReportStage(func, scene, SaveStage::PUSH_ASSETS, StageRes::RADAR_SUCCESS);
    RadarReporter::ReportStage(func, scene, SaveStage::SYNC_DATA, StageRes::RADAR_SUCCESS);
}

/**
 * @tc.name: ReportStateStart_001
 * @tc.desc: Test ReportStateStart with CREATE scene
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_001, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "com.example.myapplication";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_002
 * @tc.desc: Test ReportStateStart with SAVE scene
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_002, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "com.example.testapp";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_003
 * @tc.desc: Test ReportStateStart with DATA_RESTORE scene
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_003, TestSize.Level1)
{
    std::string func = "RestoreData";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "com.example.restoreapp";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_004
 * @tc.desc: Test ReportStateStart with empty appCaller
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_004, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_005
 * @tc.desc: Test ReportStateStart with long appCaller
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_005, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller(256, 'c');
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_006
 * @tc.desc: Test ReportStateStart with special characters in appCaller
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_006, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "com.example@app#test$service";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_007
 * @tc.desc: Test ReportStateStart with RADAR_FAILED result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_007, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t state = BizState::START;
    std::string appCaller = "com.example.test";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_008
 * @tc.desc: Test ReportStateStart with CANCELLED result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_008, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::CANCELLED;
    int32_t state = BizState::START;
    std::string appCaller = "com.example.test";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_009
 * @tc.desc: Test ReportStateStart with IDLE result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_009, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::IDLE;
    int32_t state = BizState::START;
    std::string appCaller = "com.example.test";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_010
 * @tc.desc: Test ReportStateStart with empty function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_010, TestSize.Level1)
{
    std::string func = "";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "com.example.test";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_011
 * @tc.desc: Test ReportStateStart with long function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_011, TestSize.Level1)
{
    std::string func(384, 'd');
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "com.example.test";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_012
 * @tc.desc: Test ReportStateStart with all CREATE stages
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_012, TestSize.Level1)
{
    std::string func = "TestFunc";
    std::string appCaller = "com.example.test";
    int32_t scene = BizScene::CREATE;
    int32_t state = BizState::START;
    
    RadarReporter::ReportStateStart(func, scene, CreateStage::INIT_STORE, StageRes::RADAR_SUCCESS, state, appCaller);
    RadarReporter::ReportStateStart(func, scene, CreateStage::CREATE_TABLE, StageRes::RADAR_SUCCESS, state, appCaller);
    RadarReporter::ReportStateStart(func, scene, CreateStage::RESTORE, StageRes::RADAR_SUCCESS, state, appCaller);
    RadarReporter::ReportStateStart(func, scene, CreateStage::TRANSFER, StageRes::RADAR_SUCCESS, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_013
 * @tc.desc: Test ReportStateStart with all SAVE stages
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_013, TestSize.Level1)
{
    std::string func = "TestFunc";
    std::string appCaller = "com.example.test";
    int32_t scene = BizScene::SAVE;
    int32_t state = BizState::START;
    
    RadarReporter::ReportStateStart(func, scene, SaveStage::SAVE_TO_SERVICE, StageRes::RADAR_SUCCESS, state, appCaller);
    RadarReporter::ReportStateStart(func, scene, SaveStage::SAVE_TO_STORE, StageRes::RADAR_SUCCESS, state, appCaller);
    RadarReporter::ReportStateStart(func, scene, SaveStage::PUSH_ASSETS, StageRes::RADAR_SUCCESS, state, appCaller);
    RadarReporter::ReportStateStart(func, scene, SaveStage::SYNC_DATA, StageRes::RADAR_SUCCESS, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_014
 * @tc.desc: Test ReportStateStart with all DATA_RESTORE stages
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_014, TestSize.Level1)
{
    std::string func = "TestFunc";
    std::string appCaller = "com.example.test";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t state = BizState::START;
    
    RadarReporter::ReportStateStart(func, scene, DataRestoreStage::DATA_RECV, StageRes::RADAR_SUCCESS, state, appCaller);
    RadarReporter::ReportStateStart(func, scene, DataRestoreStage::ASSETS_RECV, StageRes::RADAR_SUCCESS, state, appCaller);
    RadarReporter::ReportStateStart(func, scene, DataRestoreStage::NOTIFY, StageRes::RADAR_SUCCESS, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_015
 * @tc.desc: Test ReportStateStart multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_015, TestSize.Level1)
{
    for (int i = 0; i < 15; i++) {
        std::string func = "StartFunc_" + std::to_string(i);
        std::string appCaller = "com.example.app" + std::to_string(i);
        RadarReporter::ReportStateStart(func, BizScene::CREATE, i % 4 + 1, 
            StageRes::RADAR_SUCCESS, BizState::START, appCaller);
    }
}

/**
 * @tc.name: ReportStateStart_016
 * @tc.desc: Test ReportStateStart with unicode in appCaller
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_016, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "com.示例.测试.应用";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_017
 * @tc.desc: Test ReportStateStart with whitespace in appCaller
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_017, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "  com.example.test  ";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_018
 * @tc.desc: Test ReportStateStart with newline in appCaller
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_018, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "com.example.test\nwith\nnewlines";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_019
 * @tc.desc: Test ReportStateStart with path in appCaller
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_019, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "/data/service/el2/public/com.example.test";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateStart_020
 * @tc.desc: Test ReportStateStart with URL in appCaller
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateStart_020, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_SUCCESS;
    int32_t state = BizState::START;
    std::string appCaller = "https://example.com/path?query=value";
    
    RadarReporter::ReportStateStart(func, scene, stage, stageRes, state, appCaller);
}

/**
 * @tc.name: ReportStateError_001
 * @tc.desc: Test ReportStateError with DUPLICATE_CREATE error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_001, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::DUPLICATE_CREATE;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_002
 * @tc.desc: Test ReportStateError with NO_MEMORY error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_002, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::CREATE_TABLE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::NO_MEMORY;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_003
 * @tc.desc: Test ReportStateError with SA_DIED error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_003, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::RESTORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::SA_DIED;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_004
 * @tc.desc: Test ReportStateError with TIMEOUT error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_004, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::TRANSFER;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::TIMEOUT;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_005
 * @tc.desc: Test ReportStateError with IPC_ERROR
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_005, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::IPC_ERROR;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_006
 * @tc.desc: Test ReportStateError with NO_PERMISSION error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_006, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::NO_PERMISSION;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_007
 * @tc.desc: Test ReportStateError with GETKV_FAILED error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_007, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::PUSH_ASSETS;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::GETKV_FAILED;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_008
 * @tc.desc: Test ReportStateError with DB_NOT_INIT error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_008, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SYNC_DATA;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::DB_NOT_INIT;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_009
 * @tc.desc: Test ReportStateError with DATA_RESTORE scene
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_009, TestSize.Level1)
{
    std::string func = "RestoreData";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::TIMEOUT;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_010
 * @tc.desc: Test ReportStateError with empty function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_010, TestSize.Level1)
{
    std::string func = "";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::NO_MEMORY;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_011
 * @tc.desc: Test ReportStateError with long function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_011, TestSize.Level1)
{
    std::string func(300, 'e');
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::SA_DIED;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_012
 * @tc.desc: Test ReportStateError with special characters
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_012, TestSize.Level1)
{
    std::string func = "Error@Func#With$Special%Chars";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::IPC_ERROR;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_013
 * @tc.desc: Test ReportStateError with all error codes
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_013, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, ErrorCode::DUPLICATE_CREATE, state);
    RadarReporter::ReportStateError(func, scene, stage, stageRes, ErrorCode::NO_MEMORY, state);
    RadarReporter::ReportStateError(func, scene, stage, stageRes, ErrorCode::SA_DIED, state);
    RadarReporter::ReportStateError(func, scene, stage, stageRes, ErrorCode::TIMEOUT, state);
    RadarReporter::ReportStateError(func, scene, stage, stageRes, ErrorCode::IPC_ERROR, state);
    RadarReporter::ReportStateError(func, scene, stage, stageRes, ErrorCode::NO_PERMISSION, state);
    RadarReporter::ReportStateError(func, scene, stage, stageRes, ErrorCode::GETKV_FAILED, state);
    RadarReporter::ReportStateError(func, scene, stage, stageRes, ErrorCode::DB_NOT_INIT, state);
}

/**
 * @tc.name: ReportStateError_014
 * @tc.desc: Test ReportStateError with CANCELLED result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_014, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::CANCELLED;
    int32_t errCode = ErrorCode::TIMEOUT;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_015
 * @tc.desc: Test ReportStateError with IDLE result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_015, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::IDLE;
    int32_t errCode = ErrorCode::NO_MEMORY;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_016
 * @tc.desc: Test ReportStateError multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_016, TestSize.Level1)
{
    for (int i = 0; i < 8; i++) {
        std::string func = "ErrorFunc_" + std::to_string(i);
        int32_t errCode = ErrorCode::OFFSET + i;
        RadarReporter::ReportStateError(func, BizScene::CREATE, CreateStage::INIT_STORE,
            StageRes::RADAR_FAILED, errCode, BizState::FINISHED);
    }
}

/**
 * @tc.name: ReportStateError_017
 * @tc.desc: Test ReportStateError with negative error code
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_017, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = -1;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_018
 * @tc.desc: Test ReportStateError with zero error code
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_018, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = 0;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_019
 * @tc.desc: Test ReportStateError with large error code
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_019, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = 999999;
    int32_t state = BizState::FINISHED;
    
    RadarReporter::ReportStateError(func, scene, stage, stageRes, errCode, state);
}

/**
 * @tc.name: ReportStateError_020
 * @tc.desc: Test ReportStateError with all scenes and error codes
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStateError_020, TestSize.Level1)
{
    std::vector<int32_t> scenes = {BizScene::CREATE, BizScene::SAVE, BizScene::DATA_RESTORE};
    std::vector<int32_t> errorCodes = {ErrorCode::DUPLICATE_CREATE, ErrorCode::NO_MEMORY,
        ErrorCode::SA_DIED, ErrorCode::TIMEOUT, ErrorCode::IPC_ERROR,
        ErrorCode::NO_PERMISSION, ErrorCode::GETKV_FAILED, ErrorCode::DB_NOT_INIT};
    
    for (auto scene : scenes) {
        for (auto errCode : errorCodes) {
            RadarReporter::ReportStateError("TestFunc", scene, 1, StageRes::RADAR_FAILED, errCode, BizState::FINISHED);
        }
    }
}

/**
 * @tc.name: ReportStageError_001
 * @tc.desc: Test ReportStageError with CREATE scene DUPLICATE_CREATE error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_001, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::DUPLICATE_CREATE;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_002
 * @tc.desc: Test ReportStageError with CREATE scene NO_MEMORY error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_002, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::CREATE_TABLE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::NO_MEMORY;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_003
 * @tc.desc: Test ReportStageError with CREATE scene SA_DIED error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_003, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::RESTORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::SA_DIED;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_004
 * @tc.desc: Test ReportStageError with CREATE scene TIMEOUT error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_004, TestSize.Level1)
{
    std::string func = "CreateObject";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::TRANSFER;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::TIMEOUT;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_005
 * @tc.desc: Test ReportStageError with SAVE scene IPC_ERROR
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_005, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::IPC_ERROR;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_006
 * @tc.desc: Test ReportStageError with SAVE scene NO_PERMISSION error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_006, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::NO_PERMISSION;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_007
 * @tc.desc: Test ReportStageError with SAVE scene GETKV_FAILED error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_007, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::PUSH_ASSETS;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::GETKV_FAILED;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_008
 * @tc.desc: Test ReportStageError with SAVE scene DB_NOT_INIT error
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_008, TestSize.Level1)
{
    std::string func = "SaveObject";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SYNC_DATA;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::DB_NOT_INIT;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_009
 * @tc.desc: Test ReportStageError with DATA_RESTORE scene
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_009, TestSize.Level1)
{
    std::string func = "RestoreData";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::TIMEOUT;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_010
 * @tc.desc: Test ReportStageError with empty function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_010, TestSize.Level1)
{
    std::string func = "";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::NO_MEMORY;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_011
 * @tc.desc: Test ReportStageError with long function name
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_011, TestSize.Level1)
{
    std::string func(350, 'f');
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::SA_DIED;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_012
 * @tc.desc: Test ReportStageError with special characters
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_012, TestSize.Level1)
{
    std::string func = "Stage@Error#Func$With%Special";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = ErrorCode::IPC_ERROR;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_013
 * @tc.desc: Test ReportStageError with all error codes
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_013, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, ErrorCode::DUPLICATE_CREATE);
    RadarReporter::ReportStageError(func, scene, stage, stageRes, ErrorCode::NO_MEMORY);
    RadarReporter::ReportStageError(func, scene, stage, stageRes, ErrorCode::SA_DIED);
    RadarReporter::ReportStageError(func, scene, stage, stageRes, ErrorCode::TIMEOUT);
    RadarReporter::ReportStageError(func, scene, stage, stageRes, ErrorCode::IPC_ERROR);
    RadarReporter::ReportStageError(func, scene, stage, stageRes, ErrorCode::NO_PERMISSION);
    RadarReporter::ReportStageError(func, scene, stage, stageRes, ErrorCode::GETKV_FAILED);
    RadarReporter::ReportStageError(func, scene, stage, stageRes, ErrorCode::DB_NOT_INIT);
}

/**
 * @tc.name: ReportStageError_014
 * @tc.desc: Test ReportStageError with CANCELLED result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_014, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::CANCELLED;
    int32_t errCode = ErrorCode::TIMEOUT;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_015
 * @tc.desc: Test ReportStageError with IDLE result
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_015, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::IDLE;
    int32_t errCode = ErrorCode::NO_MEMORY;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_016
 * @tc.desc: Test ReportStageError multiple calls
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_016, TestSize.Level1)
{
    for (int i = 0; i < 8; i++) {
        std::string func = "StageError_" + std::to_string(i);
        int32_t errCode = ErrorCode::OFFSET + i;
        RadarReporter::ReportStageError(func, BizScene::CREATE, CreateStage::INIT_STORE,
            StageRes::RADAR_FAILED, errCode);
    }
}

/**
 * @tc.name: ReportStageError_017
 * @tc.desc: Test ReportStageError with negative error code
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_017, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::CREATE;
    int32_t stage = CreateStage::INIT_STORE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = -100;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_018
 * @tc.desc: Test ReportStageError with zero error code
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_018, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::SAVE;
    int32_t stage = SaveStage::SAVE_TO_SERVICE;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = 0;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_019
 * @tc.desc: Test ReportStageError with large error code
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_019, TestSize.Level1)
{
    std::string func = "TestFunc";
    int32_t scene = BizScene::DATA_RESTORE;
    int32_t stage = DataRestoreStage::DATA_RECV;
    int32_t stageRes = StageRes::RADAR_FAILED;
    int32_t errCode = 1000000;
    
    RadarReporter::ReportStageError(func, scene, stage, stageRes, errCode);
}

/**
 * @tc.name: ReportStageError_020
 * @tc.desc: Test ReportStageError with all scenes and stages
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ReportStageError_020, TestSize.Level1)
{
    std::vector<std::pair<int32_t, int32_t>> sceneStages = {
        {BizScene::CREATE, CreateStage::INIT_STORE},
        {BizScene::CREATE, CreateStage::CREATE_TABLE},
        {BizScene::SAVE, SaveStage::SAVE_TO_SERVICE},
        {BizScene::SAVE, SaveStage::SAVE_TO_STORE},
        {BizScene::DATA_RESTORE, DataRestoreStage::DATA_RECV},
        {BizScene::DATA_RESTORE, DataRestoreStage::ASSETS_RECV}
    };
    
    for (auto& p : sceneStages) {
        RadarReporter::ReportStageError("TestFunc", p.first, p.second,
            StageRes::RADAR_FAILED, ErrorCode::TIMEOUT);
    }
}

/**
 * @tc.name: ConcurrentTest_001
 * @tc.desc: Test concurrent calls to ReportStateFinished
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ConcurrentTest_001, TestSize.Level1)
{
    const int threadCount = 10;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < threadCount; i++) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 10; j++) {
                std::string func = "ConcurrentFunc_" + std::to_string(i) + "_" + std::to_string(j);
                RadarReporter::ReportStateFinished(func, BizScene::CREATE, CreateStage::INIT_STORE,
                    StageRes::RADAR_SUCCESS, BizState::FINISHED);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

/**
 * @tc.name: ConcurrentTest_002
 * @tc.desc: Test concurrent calls to ReportStage
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ConcurrentTest_002, TestSize.Level1)
{
    const int threadCount = 10;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < threadCount; i++) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 10; j++) {
                std::string func = "ConcurrentStage_" + std::to_string(i) + "_" + std::to_string(j);
                RadarReporter::ReportStage(func, BizScene::SAVE, SaveStage::SAVE_TO_SERVICE,
                    StageRes::RADAR_SUCCESS);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

/**
 * @tc.name: ConcurrentTest_003
 * @tc.desc: Test concurrent calls to ReportStateStart
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ConcurrentTest_003, TestSize.Level1)
{
    const int threadCount = 10;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < threadCount; i++) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 10; j++) {
                std::string func = "ConcurrentStart_" + std::to_string(i) + "_" + std::to_string(j);
                std::string appCaller = "com.concurrent.app" + std::to_string(i);
                RadarReporter::ReportStateStart(func, BizScene::DATA_RESTORE, DataRestoreStage::DATA_RECV,
                    StageRes::RADAR_SUCCESS, BizState::START, appCaller);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

/**
 * @tc.name: ConcurrentTest_004
 * @tc.desc: Test concurrent calls to ReportStateError
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ConcurrentTest_004, TestSize.Level1)
{
    const int threadCount = 10;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < threadCount; i++) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 10; j++) {
                std::string func = "ConcurrentError_" + std::to_string(i) + "_" + std::to_string(j);
                int32_t errCode = ErrorCode::OFFSET + (j % 8);
                RadarReporter::ReportStateError(func, BizScene::CREATE, CreateStage::INIT_STORE,
                    StageRes::RADAR_FAILED, errCode, BizState::FINISHED);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}

/**
 * @tc.name: ConcurrentTest_005
 * @tc.desc: Test concurrent calls to ReportStageError
 * @tc.type: FUNC
 */
HWTEST_F(ObjectRadarReporterTest, ConcurrentTest_005, TestSize.Level1)
{
    const int threadCount = 10;
    std::vector<std::thread> threads;
    
    for (int i = 0; i < threadCount; i++) {
        threads.emplace_back([i]() {
            for (int j = 0; j < 10; j++) {
                std::string func = "ConcurrentStageError_" + std::to_string(i) + "_" + std::to_string(j);
                int32_t errCode = ErrorCode::OFFSET + (j % 8);
                RadarReporter::ReportStageError(func, BizScene::SAVE, SaveStage::SAVE_TO_SERVICE,
                    StageRes::RADAR_FAILED, errCode);
            }
        });
    }
    
    for (auto& thread : threads) {
        thread.join();
    }
}
}
