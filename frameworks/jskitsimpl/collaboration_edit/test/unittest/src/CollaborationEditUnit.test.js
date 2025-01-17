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
 
import { describe, beforeEach, afterEach, beforeAll, afterAll, it, expect } from 'deccjsunit/index'
import collaboration_edit from "@ohos.data.collaborationEditObject"
import ability_featureAbility from '@ohos.ability.featureAbility'
 
const TAG = "[CollaborationEdit_JsTest]"
const DOC_CONFIG = {name: "doc_test"}
const EDIT_UNIT_NAME = "top"
 
var context = ability_featureAbility.getContext()
var editObject = undefined;
var editUnit = undefined;
 
describe('collaborationEditUnitTest', () => {
    beforeAll(async () => {
      console.log(TAG + "beforeAll");
    })
 
    beforeEach(async () => {
      console.log(TAG + "beforeEach");
      try {
        editObject = collaboration_edit.getCollaborationEditObject(context, DOC_CONFIG);
        editUnit = editObject.getEditUnit(EDIT_UNIT_NAME);
      } catch (err) {
        console.log(TAG + "get edit object failed. err: %s", err.message);
      }
    })
 
    afterEach(async () => {
      console.log(TAG + "afterEach");
      try {
        collaboration_edit.deleteCollaborationEditObject(context, DOC_CONFIG);
        console.log(TAG + "delete edit object successfully");
      } catch (err) {
        console.log(TAG + "delete edit object failed. err: %s", err.message);
        expect().assertFail();
      }
    })
 
    afterAll(async () => {
      console.log(TAG + "afterAll");
    })
 
    it("CollaborationEdit_EditUnit_0001", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0001 Start*****************");
      let editUnit = undefined;
      let errCode = "";
      try {
        editUnit = editObject?.getEditUnit("");
      } catch (err) {
        errCode = err.code;
      }
      expect(editUnit).assertUndefined();
      expect("401").assertEqual(errCode);
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0001 End*****************");
    })
 
    it("CollaborationEdit_EditUnit_0002", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0002 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      expect(EDIT_UNIT_NAME).assertEqual(editUnit?.getName());
      let errCode = "";
      try {
        editUnit?.insertNodes(0, null);
      } catch (err) {
        console.log(TAG + "insert node failed. err: %s", err);
        errCode = err.code;
      }
      expect("401").assertEqual(errCode);
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0002 End*****************");
    })
 
 
    it("CollaborationEdit_EditUnit_0003", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0003 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      try {
        let node1 = new collaboration_edit.Node("p1");
        let node2 = new collaboration_edit.Node("p2");
        editUnit?.insertNodes(0, [node1, node2]);
        expect(node1.getId() !== undefined).assertTrue();
        expect(node2.getId() !== undefined).assertTrue();
        expect(node1.getId().clock).assertEqual(0);
        expect(node2.getId().clock).assertEqual(1);
        let nodeList = editUnit?.getChildren(0, 2);
        expect(nodeList !== undefined).assertTrue();
        expect(2).assertEqual(nodeList?.length);
        if (nodeList !== undefined) {
          expect(nodeList[0].getId().clock).assertEqual(0);
          expect(nodeList[1].getId().clock).assertEqual(1);
        }
        let jsonStr = editUnit?.getJsonResult();
        expect(jsonStr !== undefined).assertTrue();
        expect(jsonStr).assertEqual("{\"array\":[{\"ele\":{\"name\":\"p1\",\"attr\":{},\"children\":[]}},{\"ele\":{\"name\":\"p2\",\"attr\":{},\"children\":[]}}]}");
      } catch (err) {
        console.log(TAG + "CollaborationEdit_EditUnit_0003 failed. err: %s", err);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0003 End*****************");
    })
})
