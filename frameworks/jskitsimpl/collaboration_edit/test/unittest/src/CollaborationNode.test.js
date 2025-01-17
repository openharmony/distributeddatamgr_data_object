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
 
describe('collaborationNodeTest', () => {
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
        console.log(TAG + "delete edit object failed. err: %s", err.message);
      } catch (err) {
        console.log(TAG + "delete edit object failed");
        expect().assertFail();
      }
    })
 
    afterAll(async () => {
        console.log(TAG + "afterAll");
    })
 
    it("CollaborationEdit_Node_0001", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_Node_0001 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      try {
        let node1 = new collaboration_edit.Node("p1");
        editUnit?.insertNodes(0, [node1]);
        node1.setAttributes({"align": "left", "width": 36, "italics": true});
        let attrs = node1.getAttributes();
        expect(attrs["align"]).assertEqual("left");
        expect(attrs["width"]).assertEqual("36");
        expect(attrs["italics"]).assertEqual("true");
        let jsonStr = node1.getJsonResult();
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
        console.log(TAG + "CollaborationEdit_Node_0001 failed. err: %s", err);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_Node_0001 End*****************");
    })
 
    it("CollaborationEdit_Node_0002", 0, async () => {
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
        let nodeList = node1.getChildren(0, 2);
        expect(nodeList !== undefined).assertTrue();
        expect(2).assertEqual(nodeList.length);
        if (nodeList !== undefined) {
          expect(nodeList[0].getId().clock).assertEqual(1);
          expect(nodeList[1].getId().clock).assertEqual(2);
        }
        let jsonStr = node1.getJsonResult();
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
        console.log(TAG + "CollaborationEdit_Node_0002 failed. err: %s", err);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_Node_0002 End*****************");
    })
 
    it("CollaborationEdit_Node_0003", 0, async () => {
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
        let nodeList = node1?.getChildren(0, 2);
        expect(nodeList !== undefined).assertTrue();
        expect(2).assertEqual(nodeList?.length);
        if (nodeList !== undefined) {
          expect(nodeList[0].getId().clock).assertEqual(1);
          expect(nodeList[1].getId().clock).assertEqual(2);
        }
        let jsonStr = node1?.getJsonResult();
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
        console.log(TAG + "CollaborationEdit_Node_0003 failed. err: %s", err);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_Node_0003 End*****************");
    })

    it("CollaborationEdit_Node_0004", 0, async () => {
        console.log(TAG + "*****************CollaborationEdit_Node_0004 Start*****************");
        expect(editUnit !== undefined).assertTrue();
        let errCode = "";
        let nodeList = undefined;
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
      
    it("CollaborationEdit_Node_0005", 0, async () => {
        console.log(TAG + "*****************CollaborationEdit_Node_0005 Start*****************");
        let node = new collaboration_edit.Node("p1");
        let errCode = "";
        let id = undefined;
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
})
