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

import { describe, beforeEach, afterEach,beforeAll, it, afterAll, expect } from 'deccjsunit/index'
import collaboration_edit from "@ohos.data.collaborationEditObject"
import ability_featureAbility from '@ohos.ability.featureAbility'
import CloudDbMock from "./CloudDbMock";

const TAG = "[CollaborationEdit_JsTest]"
const DOC_CONFIG_A = {name: "doc_test_A"}
const DOC_CONFIG_B = {name: "doc_test_B"}
var context = ability_featureAbility.getContext();

const EDIT_UNIT_NAME = "top";
let editObject_A = undefined;
let editObject_B = undefined;

function uploadAssetHandler(config) {
  return new Promise((resolve, reject) => {
    console.log(TAG + "upload asset handler start, path=" + config.path);
    let ret = CloudDbMock.uploadAssetTestWrap(config.path);
    console.log("uploadAsset result: " + ret);
    ret ? resolve() : reject();
    console.log(TAG + "upload asset handler end");
  });
}

function downloadAssetHandler(config) {
  return new Promise((resolve, reject) => {
    console.log(TAG + "downloadAssetHandler start, path: " + config.path);
    resolve();
  });
}

function deleteAssetHandler(config) {
  return new Promise((resolve, reject) => {
    console.log(TAG + "deleteAssetHandler start, path: " + config.path);
    let ret = CloudDbMock.deleteAssetTestWrap(config.path);
    ret ? resolve() : reject();
    console.log(TAG + "deleteAssetHandler end");
  });
}

function deleteLocalAssetHandler(config) {
  return new Promise((resolve, reject) => {
    console.log(TAG + "deleteLocalAssetHandler start, path: " + config.path);
    resolve();
  });
}

function batchInsertHandler(updates) {
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

function queryHandler(queryCondition) {
  return new Promise((resolve, reject) => {
    console.log(TAG + "queryHandler promise start");
    try {
      let res = CloudDbMock.queryTestWrap(queryCondition);
      resolve(res);
    } catch (err) {
      console.log("query err: " + err);
      reject();
    }

    console.log(TAG + "queryHandler promise end");
  });
}

function batchInsertSyncCallback(progress) {
  console.log(TAG + "batchInsert sync callback, progress code: " + progress.code.toString());
}

function querySyncCallback_B(progress) {
  console.log(TAG + "called query sync callback, progress code: " + progress.code.toString());
  editObject_B?.applyUpdate();
}

function querySyncCallback_A(progress) {
  console.log(TAG + "called query sync callback, progress code: " + progress.code.toString());
  editObject_A?.applyUpdate();
}

function sync(editObject, syncMode, pCode) {
  return new Promise((resolve) => {
    const callback = (progress) => {
      console.log(TAG + "batchInsert sync callback, progress code: " + progress.code.toString());
      expect(pCode).assertEqual(progress.code);
      resolve();
    }
    editObject?.cloudSync(syncMode, callback);
  });
}

const CLOUD_DB_FUNC = {
  batchInsert: batchInsertHandler,
  query: queryHandler,
  downloadAsset: downloadAssetHandler,
  uploadAsset: uploadAssetHandler,
  deleteAsset: deleteAssetHandler,
  deleteLocalAsset: deleteLocalAssetHandler,
};

function sleep(ms) {
  return new Promise(resolve => setTimeout(resolve, ms));
}

describe('collaborationSetCloudDbTest', () => {
  beforeAll(async () => {
    console.log(TAG + "*****************SetCloudDb Test Start*****************");
  })

  beforeEach(async () => {
    try {
      editObject_A = collaboration_edit.getCollaborationEditObject(context, DOC_CONFIG_A);
      editObject_B = collaboration_edit.getCollaborationEditObject(context, DOC_CONFIG_B);
    } catch (err) {
      console.log(TAG + "get edit object failed.");
      expect().assertFail();
    }
  })

  afterEach(async () => {
    console.log(TAG + "afterEach");
    try {
      CloudDbMock.resetEnv();
      collaboration_edit.deleteCollaborationEditObject(context, DOC_CONFIG_A);
      collaboration_edit.deleteCollaborationEditObject(context, DOC_CONFIG_B);
      console.log(TAG + "delete edit object successfully");
    } catch (error) {
      console.log(TAG + "delete edit object failed");
      expect().assertFail();
    }
  })

  afterAll(async () => {
    console.log(TAG + "*****************SetCloudDb Test End*****************");
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
   it("CollaborationEdit_SetCloudDb_0001", 0, async () => {
    try {
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
      await sleep(500);
      editObject_B?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PULL, querySyncCallback_B);
      await sleep(500);

      let nodes = editUnit_B?.getChildren(0, 1);
      expect(nodes !== undefined).assertTrue();
      expect(1).assertEqual(nodes?.length);
    } catch (err) {
      console.log(TAG + "CollaborationEdit_SetCloudDb_0001 failed. err: " + err);
      expect().assertFail();
    }
  })

  /**
   * @tc.number CollaborationEdit_SetCloudDb_0002
   * @tc.name test cloud sync in push mode
   * @tc.desc 
   *  1. equip A insert 1 node
   *  2. equip A push to cloud
   *  3. verify cloud record
   */
  it("CollaborationEdit_SetCloudDb_0002", 0, async () => {
    try {
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
      await sleep(500);

      let records = CloudDbMock.getCloudRecords();
      expect(1).assertEqual(records.length);
      expect(1).assertEqual(records[0].cursor);
    } catch (err) {
      console.log(TAG + "CollaborationEdit_SetCloudDb_0002 failed. err: " + err);
      expect().assertFail();
    }
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
   it("CollaborationEdit_SetCloudDb_0003", 0, async () => {
    try {
      editObject_A?.setCloudDB(CLOUD_DB_FUNC);
      editObject_B?.setCloudDB(CLOUD_DB_FUNC);

      let editUnit_A = editObject_A?.getEditUnit(EDIT_UNIT_NAME);
      let editUnit_B = editObject_B?.getEditUnit(EDIT_UNIT_NAME);

      let node1 = new collaboration_edit.Node("p1");
      let node2 = new collaboration_edit.Node("p2"); // leave empty
      editUnit_A?.insertNodes(0, [node1, node2]);
      let nodeList = editUnit_A?.getChildren(0, 2);
      expect(nodeList !== undefined).assertTrue();
      expect(2).assertEqual(nodeList?.length);

      node1.setAsset("src", "app/equip_A/1.jpeg");
      let node_A = node1.getJsonResult();

      editObject_A?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PUSH, batchInsertSyncCallback);
      await sleep(500);
      editObject_B?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PULL, querySyncCallback_B);
      await sleep(500);

      let nodes = editUnit_B?.getChildren(0, 2);
      expect(nodes !== undefined).assertTrue();
      expect(2).assertEqual(nodes?.length);

      let node_B = nodes[0].getJsonResult();
      expect(node_B).assertEqual(node_A);

      editUnit_B?.delete(0, 1);
      editObject_B?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PUSH, batchInsertSyncCallback);
      await sleep(500);
      editObject_A?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PULL, querySyncCallback_A);
      await sleep(500);

      let nodes_A = editUnit_A?.getChildren(0, 1);
      expect(1).assertEqual(nodes_A?.length);
    } catch (err) {
      console.log(TAG + "CollaborationEdit_SetCloudDb_0003 failed. err: " + err);
      expect().assertFail();
    }
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
  it("CollaborationEdit_SetCloudDb_0004", 0, async () => {
    try {
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
      await sleep(500);

      // equip B edit
      let node_B = new collaboration_edit.Node("p2");
      editUnit_B?.insertNodes(0, [node_B]);
      let nodeList_B = editUnit_B?.getChildren(0, 1);
      expect(nodeList_B !== undefined).assertTrue();
      expect(1).assertEqual(nodeList_B?.length);

      // equip B sync with SYNC_MODE_PULL_PUSH mode
      editObject_B?.cloudSync(collaboration_edit.SyncMode.SYNC_MODE_PULL_PUSH, querySyncCallback_B);
      await sleep(500);

      // cloud has 2 records
      let records = CloudDbMock.getCloudRecords();
      expect(2).assertEqual(records.length);
      expect(2).assertEqual(records[1].cursor);

      // equip B has 2 nodes
      nodeList_B = editUnit_B?.getChildren(0, 2);
      expect(nodeList_B !== undefined).assertTrue();
      expect(2).assertEqual(nodeList_B?.length);

    } catch (err) {
      console.log(TAG + "CollaborationEdit_SetCloudDb_0004 failed. err: " + err);
      expect().assertFail();
    }
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
  it("CollaborationEdit_SetCloudDb_0005", 0, async () => {
    try {
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
      await sleep(500);

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
      console.log(TAG + "CollaborationEdit_SetCloudDb_0005 failed. err: " + err);
      expect().assertFail();
    }
  })

  /**
   * @tc.number CollaborationEdit_SetCloudDb_0006
   * @tc.name test cloud sync when cloud db not set
   * @tc.desc
   *  1. equip A insert 1 node and push to cloud
   *  2. get CLOUD_NOT_SET error code in callback
   */
  it("CollaborationEdit_SetCloudDb_0006", 0, async () => {
    try {
      let editUnit_A = editObject_A?.getEditUnit(EDIT_UNIT_NAME);

      // equip A insert 1 node
      let node_A = new collaboration_edit.Node("p1");
      editUnit_A?.insertNodes(0, [node_A]);
      let nodeList_A = editUnit_A?.getChildren(0, 1);
      expect(nodeList_A !== undefined).assertTrue();
      expect(1).assertEqual(nodeList_A?.length);

      // equip A push to cloud
      await sync(editObject_A, collaboration_edit.SyncMode.SYNC_MODE_PUSH,
        collaboration_edit.ProgressCode.CLOUD_NOT_SET);
    } catch (err) {
      console.log(TAG + "CollaborationEdit_SetCloudDb_0006 failed. err: " + err);
      expect().assertFail();
    }
  })
})
