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

const TAG = "[CollaborationEdit_JsTest_EditUnit]"
const DOC_CONFIG: collaboration_edit.CollaborationConfig = {name: "doc_test"}
const EDIT_UNIT_NAME: string = "top"
let context: common.UIAbilityContext = globalThis.abilityContext;
let editObject: collaboration_edit.CollaborationEditObject | undefined = undefined;
let editUnit: collaboration_edit.EditUnit | undefined = undefined;

export default function collaborationEditUnitTest() {
  describe('collaborationEditUnitTest', () => {
    beforeAll(() => {
      console.log(TAG + "beforeAll");
    })

    beforeEach(() => {
      console.log(TAG + "beforeEach");
      try {
        editObject = collaboration_edit.getCollaborationEditObject(context, DOC_CONFIG);
        editUnit = editObject.getEditUnit(EDIT_UNIT_NAME);
      } catch (err) {
        console.error(TAG + "get edit object failed.");
      }
    })

    afterEach(() => {
      console.log(TAG + "afterEach");
      try {
        collaboration_edit.deleteCollaborationEditObject(context, DOC_CONFIG);
        console.log(TAG + "delete edit object successfully");
      } catch (err) {
        console.error(TAG + `delete edit object failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
    })

    afterAll(() => {
      console.log(TAG + "afterAll");
    })

    /**
     * @tc.number CollaborationEdit_EditUnit_0001
     * @tc.name getEditUnit by invalid empty name
     * @tc.desc
     *  1. getEditUnit by empty input string
     *  2. check 401 error code
     */
    it("CollaborationEdit_EditUnit_0001", Level.LEVEL0, async () => {
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0001 Start*****************");
      expect(editObject !== undefined).assertTrue();
      let editUnit: collaboration_edit.EditUnit | undefined;
      let errCode: string = "";
      try {
        editUnit = editObject?.getEditUnit("");
      } catch (err) {
        errCode = err.code;
      }
      expect(editUnit).assertUndefined();
      expect("401").assertEqual(errCode);
    })

    /**
     * @tc.number CollaborationEdit_EditUnit_0002
     * @tc.name EditUnit.insertNodes by null array
     * @tc.desc
     *  1. check EditUnit.getName
     *  2. insert null node array and check 401 error code
     */
    it("CollaborationEdit_EditUnit_0002", Level.LEVEL0, async () => {
      console.log(TAG + "*****************CollaborationEdit_EditUnit_0002 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      expect(EDIT_UNIT_NAME).assertEqual(editUnit?.getName());
      let errCode: string = "";
      try {
        editUnit?.insertNodes(0, null);
      } catch (err) {
        console.error(TAG + `insert node failed. code:${err.code}, message:${err.message}`);
        errCode = err.code;
      }
      expect("401").assertEqual(errCode);
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
    it("CollaborationEdit_EditUnit_0003", Level.LEVEL0, async () => {
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
        console.error(TAG + `CollaborationEdit_EditUnit_0003 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
    })
  })
}
