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

const TAG = "[CollaborationEdit_JsTest_Node]"
const DOC_CONFIG: collaboration_edit.CollaborationConfig = {name: "doc_test"}
const EDIT_UNIT_NAME: string = "top"
let context: common.UIAbilityContext = globalThis.abilityContext;
let editObject: collaboration_edit.CollaborationEditObject | undefined = undefined;
let editUnit: collaboration_edit.EditUnit | undefined = undefined;

export default function collaborationNodeTest() {
  describe('collaborationNodeTest', () => {
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
     * @tc.number CollaborationEdit_Node_0001
     * @tc.name Normal case of attribute-related methods
     * @tc.desc
     *  1. construct a node list and insert them into edit unit
     *  2. set different attributes
     *  3. get attributes and check result
     *  4. get json result and check result
     *  5. remove some attributes
     *  6. again check result by getAttributes and getJsonResult
     */
    it("CollaborationEdit_Node_0001", Level.LEVEL0, async () => {
      console.log(TAG + "*****************CollaborationEdit_Node_0001 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      try {
        let node1 = new collaboration_edit.Node("p1");
        editUnit?.insertNodes(0, [node1]);
        node1.setAttributes({"align": "left", "width": 36, "italics": true});
        let attrs: collaboration_edit.AttributesRecord = node1.getAttributes();
        expect(attrs["align"]).assertEqual("left");
        expect(attrs["width"]).assertEqual("36");
        expect(attrs["italics"]).assertEqual("true");
        let jsonStr: string = node1.getJsonResult();
        expect(jsonStr !== undefined).assertTrue();
        expect(jsonStr).assertEqual("{\"array\":[{\"ele\":{\"name\":\"p1\",\"attr\":{\"align\":\"left\",\"italics\":\"true\",\"width\":\"36\"},\"children\":[]}}]}");
        node1.removeAttributes(["align", "italics"]);
        attrs = node1.getAttributes();
        expect(attrs["align"]).assertUndefined();
        expect(attrs["width"]).assertEqual("36");
        expect(attrs["italics"]).assertUndefined();
        jsonStr = node1.getJsonResult();
        expect(jsonStr !== undefined).assertTrue();
        expect(jsonStr).assertEqual("{\"array\":[{\"ele\":{\"name\":\"p1\",\"attr\":{\"width\":\"36\"},\"children\":[]}}]}");
      } catch (err) {
        console.error(TAG + `CollaborationEdit_Node_0001 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_Node_0001 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_Node_0002
     * @tc.name Normal case of insertTexts and delete
     * @tc.desc
     *  1. construct a node and insert it into edit unit
     *  2. construct a text list and insert them into the node
     *  3. check the unique id of the texts
     *  4. getChildren and check unique id of the result list
     *  5. getJsonResult and check the result
     *  6. delete the first child
     *  7. again check result by getChildren and getJsonResult
     */
    it("CollaborationEdit_Node_0002", Level.LEVEL0, () => {
      console.log(TAG + "*****************CollaborationEdit_Node_0002 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      try {
        let node1 = new collaboration_edit.Node("p1");
        editUnit?.insertNodes(0, [node1]);

        // insert Texts
        let text1 = new collaboration_edit.Text();
        let text2 = new collaboration_edit.Text();
        node1.insertTexts(0, [text1, text2]);
        expect(text1.getId() !== undefined).assertTrue();
        expect(text2.getId() !== undefined).assertTrue();
        expect(text1.getId().clock).assertEqual(1);
        expect(text2.getId().clock).assertEqual(2);
        let nodeList: Array<collaboration_edit.Node | collaboration_edit.Text> | undefined = node1.getChildren(0, 2);
        expect(nodeList !== undefined).assertTrue();
        expect(2).assertEqual(nodeList.length);
        if (nodeList !== undefined) {
          expect(nodeList[0].getId().clock).assertEqual(1);
          expect(nodeList[1].getId().clock).assertEqual(2);
        }
        let jsonStr: string | undefined = node1.getJsonResult();
        expect(jsonStr !== undefined).assertTrue();
        expect(jsonStr).assertEqual("{\"array\":[{\"ele\":{\"name\":\"p1\",\"attr\":{},\"children\":[{\"text\":\"[]\"},{\"text\":\"[]\"}]}}]}");

        // delete first child
        node1.delete(0, 1);
        nodeList = node1.getChildren(0, 2);  // if end out of length, return all
        expect(1).assertEqual(nodeList.length);
        expect(nodeList[0].getId().clock).assertEqual(2);
        jsonStr = node1.getJsonResult();
        expect(jsonStr).assertEqual("{\"array\":[{\"ele\":{\"name\":\"p1\",\"attr\":{},\"children\":[{\"text\":\"[]\"}]}}]}");
      } catch (err) {
        console.error(TAG + `CollaborationEdit_Node_0002 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_Node_0002 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_Node_0003
     * @tc.name Normal case of insertNode and delete
     * @tc.desc
     *  1. construct a node p1 and insert it into edit unit
     *  2. construct a node list and insert them into the node p1
     *  3. check the unique id of the node list
     *  4. getChildren and check unique id of the result list
     *  5. getJsonResult and check the result
     *  6. delete the second child
     *  7. again check result by getChildren and getJsonResult
     */
    it("CollaborationEdit_Node_0003", Level.LEVEL0, () => {
      console.log(TAG + "*****************CollaborationEdit_Node_0003 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      try {
        let node1 = new collaboration_edit.Node("p1");
        editUnit?.insertNodes(0, [node1]);

        // insert Nodes
        let node2 = new collaboration_edit.Node("p2");
        let node3 = new collaboration_edit.Node("p3");
        node1?.insertNodes(0, [node2, node3]);
        expect(node2.getId() !== undefined).assertTrue();
        expect(node3.getId() != undefined).assertTrue();
        expect(node2.getId().clock).assertEqual(1);
        expect(node3.getId().clock).assertEqual(2);
        let nodeList: Array<collaboration_edit.Node | collaboration_edit.Text> | undefined = node1?.getChildren(0, 2);
        expect(nodeList !== undefined).assertTrue();
        expect(2).assertEqual(nodeList?.length);
        if (nodeList !== undefined) {
          expect(nodeList[0].getId().clock).assertEqual(1);
          expect(nodeList[1].getId().clock).assertEqual(2);
        }
        let jsonStr: string | undefined = node1?.getJsonResult();
        expect(jsonStr !== undefined).assertTrue();
        expect(jsonStr).assertEqual("{\"array\":[{\"ele\":{\"name\":\"p1\",\"attr\":{},\"children\":[{\"ele\":{\"name\":\"p2\",\"attr\":{},\"children\":[]}},{\"ele\":{\"name\":\"p3\",\"attr\":{},\"children\":[]}}]}}]}");

        // delete second child
        node1.delete(1, 1);
        nodeList = node1.getChildren(0, 2);  // if end out of length, return all
        expect(1).assertEqual(nodeList.length);
        expect(nodeList[0].getId().clock).assertEqual(1);
        jsonStr = node1.getJsonResult();
        expect(jsonStr).assertEqual("{\"array\":[{\"ele\":{\"name\":\"p1\",\"attr\":{},\"children\":[{\"ele\":{\"name\":\"p2\",\"attr\":{},\"children\":[]}}]}}]}");
      } catch (err) {
        console.error(TAG + `CollaborationEdit_Node_0003 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }

      console.log(TAG + "*****************CollaborationEdit_Node_0003 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_Node_0004
     * @tc.name test getChildren by index out of range
     * @tc.desc
     *  1. construct a node list and insert them into edit unit
     *  2. construct a text list and insert them into the node
     *  3. check the unique id of the texts
     *  4. getChildren if start and end are out of range
     *  5. check error code
     */
    it("CollaborationEdit_Node_0004", Level.LEVEL0, () => {
      console.log(TAG + "*****************CollaborationEdit_Node_0004 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      let errCode: string = "";
      let nodeList: Array<collaboration_edit.Node | collaboration_edit.Text> | undefined;
      try {
        let node1 = new collaboration_edit.Node("p1");
        editUnit?.insertNodes(0, [node1]);

        // insert Texts
        let text1 = new collaboration_edit.Text();
        let text2 = new collaboration_edit.Text();
        node1.insertTexts(0, [text1, text2]);
        expect(text1.getId() !== undefined).assertTrue();
        expect(text2.getId() !== undefined).assertTrue();
        expect(text1.getId().clock).assertEqual(1);
        expect(text2.getId().clock).assertEqual(2);
        nodeList = node1.getChildren(2, 4);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("15410002");
      expect(nodeList).assertUndefined();
      console.log(TAG + "*****************CollaborationEdit_Node_0004 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_Node_0005
     * @tc.name Invalid operation if node is not inserted
     * @tc.desc
     *  1. construct a node
     *  2. call getId/setAttributes of the node
     *  3. check the invalid operation error code
     */
    it("CollaborationEdit_Node_0005", Level.LEVEL0, () => {
      console.log(TAG + "*****************CollaborationEdit_Node_0005 Start*****************");
      let node = new collaboration_edit.Node("p1");
      let errCode: string = "";
      let id: collaboration_edit.UniqueId | undefined;
      try {
        id = node.getId();
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("15410001");
      expect(id).assertUndefined();

      errCode = "";
      try {
        node.setAttributes({
          "align": "left"
        });
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("15410001");
      console.log(TAG + "*****************CollaborationEdit_Node_0005 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_Node_0006
     * @tc.name Invalid index input when call insertXXXs/delete/getChildren
     * @tc.desc
     *  1. insertNodes - index is negative
     *  2. insertTexts - index is negative
     *  3. delete - index is negative
     *  4. delete - length is negative or zero
     *  5. getChildren - start is negative
     *  6. getChildren - start is greater than end
     */
    it("CollaborationEdit_Node_0006", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_Node_0006 Start*****************");
      let node: collaboration_edit.Node | undefined = undefined;
      try {
        node = new collaboration_edit.Node("p1");
        editUnit?.insertNodes(0, [node]);
      } catch (err) {
        console.error(TAG + `CollaborationEdit_Node_0006 insertNodes failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      expect(node !== undefined).assertTrue();
      let errCode = "";
      try {
        node?.insertNodes(-1, []);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      errCode = "";
      try {
        node?.insertTexts(-1, []);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      errCode = "";
      try {
        node?.delete(-1, 1);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      errCode = "";
      try {
        node?.delete(0, 0);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      errCode = "";
      try {
        node?.getChildren(-1, 1);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      errCode = "";
      try {
        node?.getChildren(1, 0);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");
      console.log(TAG + "*****************CollaborationEdit_Node_0006 End*****************");
    })

    it("CollaborationEdit_Node_0007", Level.LEVEL0, () => {
      console.log(TAG + "*****************CollaborationEdit_Node_0007 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      try {
        let node1 = new collaboration_edit.Node("p1");
        editUnit?.insertNodes(0, [node1]);

        // insert Texts
        let text1 = new collaboration_edit.Text();
        node1.insertTexts(0, [text1]);
        text1.insert(0,'你好');
        expect(text1.getId() !== undefined).assertTrue();
        expect(text1.getId().clock).assertEqual(1);
        let relPos = editUnit?.getRelativePos(2);
        expect(relPos !== undefined).assertTrue();
        if (relPos !== undefined) {
          if (relPos.parentId != null) {
            expect(relPos.parentId.clock === 1).assertTrue();
          }
          if (relPos.id != null) {
            expect(relPos.id.clock === 3).assertTrue();
          }
          expect(relPos.pos === 0).assertTrue();
        }
      } catch (err) {
        console.error(TAG + `CollaborationEdit_Node_0007 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_Node_0007 End*****************");
    })

    it("CollaborationEdit_Node_0008", Level.LEVEL0, () => {
      console.log(TAG + "*****************CollaborationEdit_Node_0008 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      try {
        let node1 = new collaboration_edit.Node("p1");
        editUnit?.insertNodes(0, [node1]);

        let relPos = editUnit?.getRelativePos(2);
        expect(relPos !== undefined).assertTrue();
        if (relPos !== undefined) {
          expect(relPos.parentName != null).assertTrue();
          if (relPos.parentName != null) {
            expect(relPos.parentName === "top").assertTrue();
          }
          expect(relPos.pos === 0).assertTrue();
        }
      } catch (err) {
        console.error(TAG + `CollaborationEdit_Node_0008 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_Node_0008 End*****************");
    })

    it("CollaborationEdit_Node_0009", Level.LEVEL0, () => {
      console.log(TAG + "*****************CollaborationEdit_Node_0009 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      try {
        let node1 = new collaboration_edit.Node("p1");
        editUnit?.insertNodes(0, [node1]);

        // insert Texts
        let text1 = new collaboration_edit.Text();
        node1.insertTexts(0, [text1]);
        text1.insert(0,'followRedone');
        expect(text1.getId() !== undefined).assertTrue();
        expect(text1.getId().clock).assertEqual(1);

        let node2 = new collaboration_edit.Node("p2");
        editUnit?.insertNodes(1, [node2]);

        let pos11 = editUnit?.getRelativePos(2);
        console.log("pos11 :" + JSON.stringify(pos11));
        let pos = editUnit?.getAbsolutePos({parentId: null, parentName:"top", id:null, pos:0});
        expect(pos === 16).assertTrue();

        let localId = editObject?.getLocalId();
        expect(localId !== undefined).assertTrue();
        console.log("localId :" + localId);
        if (localId !== undefined) {
          pos = editUnit?.getAbsolutePos({parentId: {id:localId, clock:1}, parentName:null, id:{id:localId, clock:4}, pos:0});
          expect(pos === 3).assertTrue();

          pos = editUnit?.getAbsolutePos({parentId: {id:localId, clock:14}, parentName:null, id:null, pos:0});
          expect(pos === 15).assertTrue();
        }
      } catch (err) {
        console.error(TAG + `CollaborationEdit_Node_0009 failed. code:${err.code}, message:${err.message}`);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_Node_0009 End*****************");
    })
  })
}
