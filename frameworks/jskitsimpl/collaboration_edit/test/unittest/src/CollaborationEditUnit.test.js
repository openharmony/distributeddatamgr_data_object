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

    /**
     * @tc.number CollaborationEdit_EditUnit_0001
     * @tc.name getEditUnit by invalid empty name
     * @tc.desc 
     *  1. getEditUnit by empty input string
     *  2. check 401 error code
     */
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
    
    /**
     * @tc.number CollaborationEdit_EditUnit_0002
     * @tc.name EditUnit.insertNodes by null array
     * @tc.desc 
     *  1. check EditUnit.getName
     *  2. insert null node array and check 401 error code
     */
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
 
    /**
     * @tc.number CollaborationEdit_EditUnit_0003
     * @tc.name Normal test case of methods in EditUnit
     * @tc.desc 
     *  1. construct node list
     *  2. EditUnit.insertNodes
     *  3. check the id of inserted nodes
     *  4. EditUnit.getChildren and check result
     *  5. EditUnit.getJsonResult and check result
     */
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

    /**
     * @tc.number CollaborationEdit_EditUnit_0004
     * @tc.name EditUnit.getChildren by invalid input
     * @tc.desc 
     *  1. start is negative
     *  2. end is negative
     *  3. start is greater than end
     */
    it("CollaborationEdit_EditUnit_0004", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0004 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      
      let errCode = "";
      try {
        editUnit.getChildren(-1, 2);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      errCode = "";
      try {
        editUnit.getChildren(0, -1);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      errCode = "";
      try {
        editUnit.getChildren(1, 0);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      console.log(TAG + "*****************CollaborationEdit_EditUnit_0004 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_EditUnit_0005
     * @tc.name EditUnit.delete by invalid input
     * @tc.desc 
     *  1. index is negative
     *  2. length is negative or zero
     */
    it("CollaborationEdit_EditUnit_0005", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0005 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      
      let errCode = "";
      try {
        editUnit.delete(-1, 2);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      errCode = "";
      try {
        editUnit.delete(0, 0);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      console.log(TAG + "*****************CollaborationEdit_EditUnit_0005 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_EditUnit_0006
     * @tc.name EditUnit.insertNodes by invalid index
     * @tc.desc 
     *  1. index is invalid and check 401 error code
     */
    it("CollaborationEdit_EditUnit_0006", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0006 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      
      let errCode = "";
      try {
        editUnit.insertNodes(-1, []);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0006 End*****************");
    })
})
