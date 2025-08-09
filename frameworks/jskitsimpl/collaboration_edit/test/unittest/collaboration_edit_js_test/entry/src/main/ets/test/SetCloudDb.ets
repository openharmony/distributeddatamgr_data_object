/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file expect in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

import { describe, beforeEach, afterEach, beforeAll, it, afterAll, expect, Level } from '@ohos/hypium'
import collaboration_edit from "@ohos.data.collaborationEditObject"
import common from '@ohos.app.ability.common';

import CloudDbMock from "./CloudDbMock";
import TestUtils from './TestUtils';

const TAG = "[CollaborationEdit_JsTest_SetCloudDb]"
const DOC_CONFIG_A: collaboration_edit.CollaborationConfig = {name: "doc_test_A"}
const DOC_CONFIG_B: collaboration_edit.CollaborationConfig = {name: "doc_test_B"}
let context: common.UIAbilityContext = globalThis.abilityContext;

const EDIT_UNIT_NAME = "top";
let editObject_A: collaboration_edit.CollaborationEditObject | undefined = undefined;
let editObject_B: collaboration_edit.CollaborationEditObject | undefined = undefined;

function uploadAssetHandler(config : collaboration_edit.AssetOperationConfig) : Promise<void> {
  return new Promise((resolve, reject) => {
    console.log(TAG + "upload asset handler start, path=" + config.path);
    let ret = CloudDbMock.uploadAssetTestWrap(config.path);
    console.log("uploadAsset result: " + ret);
    ret ? resolve() : reject();
    console.log(TAG + "upload asset handler end");
  });
}

function downloadAssetHandler(config : collaboration_edit.AssetOperationConfig) : Promise<void> {
  return new Promise((resolve, reject) => {
    console.log(TAG + "downloadAssetHandler start, path: " + config.path);
    resolve();
  });
}

function deleteAssetHandler(config : collaboration_edit.AssetOperationConfig) : Promise<void> {
  return new Promise((resolve, reject) => {
    console.log(TAG + "deleteAssetHandler start, path: " + config.path);
    let ret = CloudDbMock.deleteAssetTestWrap(config.path);
    ret ? resolve() : reject();
    console.log(TAG + "deleteAssetHandler end");
  });
}

function deleteLocalAssetHandler(config : collaboration_edit.AssetOperationConfig) : Promise<void> {
  return new Promise((resolve, reject) => {
    console.log(TAG + "deleteLocalAssetHandler start, path: " + config.path);
    resolve();
  });
}

function batchInsertHandler(updates: Array<collaboration_edit.EditObjectRecord>) : Promise<number> {
  return new Promise((resolve, reject) => {
    console.log(TAG + "batchInsertHandler start");
    if (updates.length == 0) {
      resolve(0);
    }
    let success_num = CloudDbMock.batchInsertTestWrap(updates);
    console.log("success num: " + success_num);
    resolve(success_num);
    console.log(TAG + "batchInsertHandler end");
  });
}

function queryHandler(queryCondition: Array<collaboration_edit.QueryCondition>) : Promise<Array<collaboration_edit.EditObjectRecord>> {
  return new Promise((resolve, reject) => {
    console.log(TAG + "queryHandler promise start");
    try {
      let res = CloudDbMock.queryTestWrap(queryCondition);
      resolve(res);
    } catch (err) {
      console.error("query err: " + err);
      reject();
    }

    console.log(TAG + "queryHandler promise end");
  });
}

function batchInsertSyncCallback(progress: collaboration_edit.ProgressDetail) : void {
  console.log("batchInsert sync callback, progress code: " + progress.code.toString());
}

function querySyncCallback_B(progress: collaboration_edit.ProgressDetail) : void {
  console.log("called query sync callback, progress code: " + progress.code.toString());
  editObject_B?.applyUpdate();
}

function querySyncCallback_A(progress: collaboration_edit.ProgressDetail) : void {
  console.log("called query sync callback, progress code: " + progress.code.toString());
  editObject_A?.applyUpdate();
}

function sync(editObject: collaboration_edit.CollaborationEditObject, syncMode: collaboration_edit.SyncMode, pCode: collaboration_edit.ProgressCode): Promise<void> {
  return new Promise((resolve, reject) => {
    const callback = (progress: collaboration_edit.ProgressDetail) => {
      console.log(TAG + "batchInsert sync callback, progress code: " + progress.code.toString());
      expect(pCode).assertEqual(progress.code);
      resolve();
    }
    editObject?.cloudSync(syncMode, callback);
  });
}

const CLOUD_DB_FUNC: collaboration_edit.CloudDB = {
  batchInsert: batchInsertHandler,
  query: queryHandler,
  downloadAsset: downloadAssetHandler,
  uploadAsset: uploadAssetHandler,
  deleteAsset: deleteAssetHandler,
  deleteLocalAsset: deleteLocalAssetHandler,
};

export default function collaborationSetCloudDbTest() {
  describe('collaborationSetCloudDbTest', () => {
    beforeAll(() => {
      console.log(TAG + "beforeAll");
    })

    beforeEach(() => {
      console.log(TAG + "beforeEach");
      try {
        editObject_A = collaboration_edit.getCollaborationEditObject(context, DOC_CONFIG_A);
        editObject_B = collaboration_edit.getCollaborationEditObject(context, DOC_CONFIG_B);
      } catch (err) {
        console.error(TAG + `get edit object failed. code:${err.code}, message:${err.message}`);
      }
    })

    afterEach(() => {
      console.log(TAG + "afterEach");
      try {
        CloudDbMock.resetEnv();
        collaboration_edit.deleteCollaborationEditObject(context, DOC_CONFIG_A);
        collaboration_edit.deleteCollaborationEditObject(context, DOC_CONFIG_B);
        console.log(TAG + "delete edit object successfully");
      } catch (err) {
        expect().assertFail();
        console.error(TAG + `delete edit object failed. code:${err.code}, message:${err.message}`);
      }
    })

    afterAll(() => {
      console.log(TAG + "afterAll");
    })

    /**
     * @tc.number CollaborationEdit_SetCloudDb_0001
     * @tc.name test cloud sync in pull mode
     * @tc.desc
     *  1. equip A insert 1 node
     *  2. equip A push to cloud
     *  3. equip B pull from cloud
     *  4. equip B reads one record
     */
    it("CollaborationEdit_SetCloudDb_001", Level.LEVEL0, async () => {
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_001 Start*****************");
      try {
        expect(editObject_A !== undefined).assertTrue();
        expect(editObject_B != undefined).assertTrue();
        editObject_A?.setCloudDB(CLOUD_DB_FUNC);
        editObject_B?.setCloudDB(CLOUD_DB_FUNC);

        let editUnit_A = editObject_A?.getEditUnit(EDIT_UNIT_NAME);
        let editUnit_B = editObject_B?.getEditUnit(EDIT_UNIT_NAME);
        let node1 = new collaboration_edit.Node("p1"); // insert xmlText
        editUnit_A?.insertNodes(0, [node1]);
        let nodeList = editUnit_A?.getChildren(0, 1);
        expect(nodeList !== undefined).assertTrue();
        expect(1).assertEqual(nodeList?.length);

        // equip A and equip B sync node
        editObject_A?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PUSH, batchInsertSyncCallback);
        await TestUtils.sleep(500);
        editObject_B?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PULL, querySyncCallback_B);
        await TestUtils.sleep(500);

        let nodes = editUnit_B?.getChildren(0, 1);
        expect(nodes !== undefined).assertTrue();
        expect(1).assertEqual(nodes?.length);
      } catch (err) {
        console.error(TAG + `CollaborationEdit_SetCloudDb_001 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_001 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_SetCloudDb_0002
     * @tc.name test cloud sync in push mode
     * @tc.desc
     *  1. equip A insert 1 node
     *  2. equip A push to cloud
     *  3. verify cloud record
     */
    it("CollaborationEdit_SetCloudDb_0002", Level.LEVEL0, async () => {
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_002 Start*****************");
      try {
        expect(editObject_A !== undefined).assertTrue();
        expect(editObject_B != undefined).assertTrue();
        editObject_A?.setCloudDB(CLOUD_DB_FUNC);
        let editUnit_A = editObject_A?.getEditUnit(EDIT_UNIT_NAME);

        // equip A edit
        let node1 = new collaboration_edit.Node("p1");
        editUnit_A?.insertNodes(0, [node1]);
        let nodeList = editUnit_A?.getChildren(0, 1);
        expect(nodeList !== undefined).assertTrue();
        expect(1).assertEqual(nodeList?.length);

        // equip A push to cloud
        console.log("equip A start cloudSync");
        editObject_A?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PUSH, batchInsertSyncCallback);
        await TestUtils.sleep(500);

        let records = CloudDbMock.getCloudRecords();
        expect(1).assertEqual(records.length);
        expect(1).assertEqual(records[0].cursor);
      } catch (err) {
        console.error(TAG + `CollaborationEdit_SetCloudDb_0002 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_002 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_SetCloudDb_0003
     * @tc.name test cloud sync with asset
     * @tc.desc
     *  1. equip A insert 1 node then set as asset
     *  2. equip A push to cloud
     *  3. equip B pull from cloud
     *  4. equip B read asset node normally
     *  5. equip B delete asset node, then sync
     *  6. equip A reads updated data
     */
    it("CollaborationEdit_SetCloudDb_0003", Level.LEVEL0, async () => {
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_003 Start*****************");
      try {
        expect(editObject_A !== undefined).assertTrue();
        expect(editObject_B != undefined).assertTrue();
        editObject_A?.setCloudDB(CLOUD_DB_FUNC);
        editObject_B?.setCloudDB(CLOUD_DB_FUNC);

        let editUnit_A = editObject_A?.getEditUnit(EDIT_UNIT_NAME);
        let editUnit_B = editObject_B?.getEditUnit(EDIT_UNIT_NAME);

        let node1 = new collaboration_edit.Node("p1");
        let node2 = new collaboration_edit.Node("p2");
        editUnit_A?.insertNodes(0, [node1, node2]);
        let nodeList = editUnit_A?.getChildren(0, 2);
        expect(nodeList !== undefined).assertTrue();
        expect(2).assertEqual(nodeList?.length);

        node1.setAsset("src", "app/equip_A/1.jpeg");
        let node_A = node1.getJsonResult();

        editObject_A?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PUSH, batchInsertSyncCallback);
        await TestUtils.sleep(500);
        editObject_B?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PULL, querySyncCallback_B);
        await TestUtils.sleep(500);

        let nodes = editUnit_B?.getChildren(0, 2);
        expect(nodes !== undefined).assertTrue();
        expect(2).assertEqual(nodes?.length);

        let node_B: string = "";
        if (nodes !== undefined) {
          node_B = nodes[0].getJsonResult()
        }
        expect(node_B).assertEqual(node_A);

        editUnit_B?.delete(0, 1);
        editObject_B?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PUSH, batchInsertSyncCallback);
        await TestUtils.sleep(500);
        editObject_A?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PULL, querySyncCallback_A);
        await TestUtils.sleep(500);

        let nodes_A = editUnit_A?.getChildren(0, 1);
        expect(1).assertEqual(nodes_A?.length);
      } catch (err) {
        console.error(TAG + `CollaborationEdit_SetCloudDb_0003 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_003 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_SetCloudDb_0004
     * @tc.name test cloud sync with SYNC_MODE_PULL_PUSH mode
     * @tc.desc
     *  1. equip A insert 1 node locally
     *  2. equip A push to cloud
     *  3. equip B insert 1 node locally
     *  4. equip B sync with SYNC_MODE_PULL_PUSH mode
     *  5. cloud contains record from both devices, and equip B has equip A's data
     */
    it("CollaborationEdit_SetCloudDb_0004", Level.LEVEL0, async () => {
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_004 Start*****************");
      try {
        expect(editObject_A !== undefined).assertTrue();
        expect(editObject_B != undefined).assertTrue();
        editObject_A?.setCloudDB(CLOUD_DB_FUNC);
        editObject_B?.setCloudDB(CLOUD_DB_FUNC);
        let editUnit_A = editObject_A?.getEditUnit(EDIT_UNIT_NAME);
        let editUnit_B = editObject_B?.getEditUnit(EDIT_UNIT_NAME);

        // equip A edit
        let node_A = new collaboration_edit.Node("p1");
        editUnit_A?.insertNodes(0, [node_A]);
        let nodeList_A = editUnit_A?.getChildren(0, 1);
        expect(nodeList_A !== undefined).assertTrue();
        expect(1).assertEqual(nodeList_A?.length);

        // equip A push to cloud
        editObject_A?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PUSH, batchInsertSyncCallback);
        await TestUtils.sleep(500);

        // equip B edit
        let node_B = new collaboration_edit.Node("p2");
        editUnit_B?.insertNodes(0, [node_B]);
        let nodeList_B = editUnit_B?.getChildren(0, 1);
        expect(nodeList_B !== undefined).assertTrue();
        expect(1).assertEqual(nodeList_B?.length);

        // equip B sync with SYNC_MODE_PULL_PUSH mode
        editObject_B?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PULL_PUSH, querySyncCallback_B);
        await TestUtils.sleep(500);

        // cloud has 2 records
        let records = CloudDbMock.getCloudRecords();
        expect(2).assertEqual(records.length);
        expect(2).assertEqual(records[1].cursor);

        // equip B has 2 nodes
        nodeList_B = editUnit_B?.getChildren(0, 2);
        expect(nodeList_B !== undefined).assertTrue();
        expect(2).assertEqual(nodeList_B?.length);
        
      } catch (err) {
        console.error(TAG + `CollaborationEdit_SetCloudDb_0004 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_004 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_SetCloudDb_0005
     * @tc.name test write update from cloud to local device
     * @tc.desc
     *  1. equip A insert 1 node locally
     *  2. equip A push to cloud
     *  3. write cloud records to equip B
     *  4. equip B contains equip A's data
     */
    it("CollaborationEdit_SetCloudDb_0005", Level.LEVEL0, async () => {
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_005 Start*****************");
      try {
        expect(editObject_A !== undefined).assertTrue();
        expect(editObject_B != undefined).assertTrue();
        editObject_A?.setCloudDB(CLOUD_DB_FUNC);
        editObject_B?.setCloudDB(CLOUD_DB_FUNC);
        let editUnit_A = editObject_A?.getEditUnit(EDIT_UNIT_NAME);
        let editUnit_B = editObject_B?.getEditUnit(EDIT_UNIT_NAME);

        // equip A insert 1 node
        let node_A = new collaboration_edit.Node("p1");
        editUnit_A?.insertNodes(0, [node_A]);
        let nodeList_A = editUnit_A?.getChildren(0, 1);
        expect(nodeList_A !== undefined).assertTrue();
        expect(1).assertEqual(nodeList_A?.length);

        // equip A push to cloud
        editObject_A?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PUSH, batchInsertSyncCallback);
        await TestUtils.sleep(500);

        // write update to equip B
        let records = CloudDbMock.getCloudRecords();
        for (let record of records) {
          editObject_B?.writeUpdate(record);
        }
        editObject_B?.applyUpdate();

        // equip B has data from device A
        let nodeList_B = editUnit_B?.getChildren(0, 1);
        expect(nodeList_B !== undefined).assertTrue();
        expect(1).assertEqual(nodeList_B?.length);

      } catch (err) {
        console.error(TAG + `CollaborationEdit_SetCloudDb_0005 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_005 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_SetCloudDb_0006
     * @tc.name test cloud sync when cloud db not set
     * @tc.desc
     *  1. equip A insert 1 node and push to cloud
     *  2. get CLOUD_NOT_SET error code in callback
     */
    it("CollaborationEdit_SetCloudDb_0006", Level.LEVEL0, async () => {
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_006 Start*****************");
      try {
        expect(editObject_A !== undefined).assertTrue();
        let editUnit_A = editObject_A?.getEditUnit(EDIT_UNIT_NAME);

        // equip A insert 1 node
        let node_A = new collaboration_edit.Node("p1");
        editUnit_A?.insertNodes(0, [node_A]);
        let nodeList_A = editUnit_A?.getChildren(0, 1);
        expect(nodeList_A !== undefined).assertTrue();
        expect(1).assertEqual(nodeList_A?.length);

        // equip A push to cloud
        if (editObject_A !== undefined) {
          await sync(editObject_A, collaboration_edit.SyncMode.SYNC_MODE_PUSH,
            collaboration_edit.ProgressCode.CLOUD_NOT_SET);
        }
      } catch (err) {
        console.error(TAG + `CollaborationEdit_SetCloudDb_0006 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_SetCloudDb_006 End*****************");
    })
  })
}
