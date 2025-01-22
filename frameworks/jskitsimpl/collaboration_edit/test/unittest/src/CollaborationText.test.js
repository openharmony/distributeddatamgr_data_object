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
 
describe('collaborationTextTest', () => {
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
     * @tc.number CollaborationEdit_Text_0001
     * @tc.name Normal test case of methods in Text
     * @tc.desc 
     *  1. construct a node and insert it into edit unit
     *  2. construct a text and insert it into the node
     *  3. insert strings into text
     *  4. check result by getPlainText and getJsonResult
     *  5. format some characters and check result by getJsonResult
     *  6. delete some characters and check result by getPlainText and getJsonResult
     */
    it("CollaborationEdit_Text_0001", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_Text_0001 Start*****************");
      expect(editUnit !== undefined).assertTrue();
      try {
        let node = new collaboration_edit.Node("p1");
        editUnit?.insertNodes(0, [node]);
 
        // insert Text
        let text = new collaboration_edit.Text();
        node.insertTexts(0, [text]);
        expect(text.getId() !== undefined).assertTrue();
        expect(text.getId().clock).assertEqual(1);
 
        // insert string into text
        text.insert(0, "abc");
        text.insert(3, "def", {"color":"red", "isBold":true});
        let plainText = text.getPlainText();
        expect(plainText).assertEqual("abcdef");
        let jsonStr = text.getJsonResult();
        expect(jsonStr).assertEqual("[{\"insert\":\"abc\"},{\"insert\":\"def\",\"attributes\":{\"color\":\"red\",\"isBold\":\"true\"}}]");
 
        // format text
        text.format(1, 2, {"font-size": 12});
        jsonStr = text.getJsonResult();
        console.log(TAG + "json str = %s", jsonStr);
        expect(jsonStr).assertEqual("[{\"insert\":\"a\"},{\"insert\":\"bc\",\"attributes\":{\"font-size\":\"12\"}},{\"insert\":\"def\",\"attributes\":{\"color\":\"red\",\"isBold\":\"true\"}}]");
 
        // delete
        text.delete(2, 3);
        plainText = text.getPlainText();
        expect(plainText).assertEqual("abf");
        jsonStr = text.getJsonResult();
        expect(jsonStr).assertEqual("[{\"insert\":\"a\"},{\"insert\":\"b\",\"attributes\":{\"font-size\":\"12\"}},{\"insert\":\"f\",\"attributes\":{\"color\":\"red\",\"isBold\":\"true\"}}]");
      } catch (err) {
        console.log(TAG + "CollaborationEdit_Text_0001 failed. err: %s", err);
        expect().assertFail();
      }
      console.log(TAG + "*****************CollaborationEdit_Text_0001 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_Text_0002
     * @tc.name Invalid operation if the text is not inserted
     * @tc.desc
     *  1. construct a text
     *  2. call getId/insert of the text
     *  3. check the invalid operation error code
     */
    it("CollaborationEdit_Text_0002", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_Text_0002 Start*****************");
      let text = new collaboration_edit.Text();
      let errCode = "";
      let id = undefined;
      try {
        id = text.getId();
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("15410001");
      expect(id).assertUndefined();
  
      errCode = "";
      try {
        text.insert(0, "abc");
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("15410001");
      console.log(TAG + "*****************CollaborationEdit_Text_0002 End*****************");
    })

    /**
     * @tc.number CollaborationEdit_Text_0003
     * @tc.name Invalid index input when call insert/delete/format
     * @tc.desc 
     *  1. construct a node and insert it into edit unit
     *  2. construct a text and insert it into the node
     *  3. insert string if the index is negative
     *  4. delete if the index is negative
     *  5. delete if the length is negative or zero
     *  6. format if the index is negative
     *  7. format if the length is negative or zero
     */
    it("CollaborationEdit_Text_0003", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_Text_0003 Start*****************");
      let text = undefined;
      try {
        let node = new collaboration_edit.Node("p1");
        editUnit?.insertNodes(0, [node]);
        text = new collaboration_edit.Text();
        node.insertTexts(0, [text]);
      } catch (err) {
        console.log(TAG + "CollaborationEdit_Text_0003 failed, err: %s", err);
        expect().assertFail();
      }
      let errCode = "";
      try {
        text.insert(-1, "abc");
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");
  
      errCode = "";
      try {
        text.delete(-1, 1);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      errCode = "";
      try {
        text.delete(0, 0);
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      errCode = "";
      try {
        text.format(-1, 1, {"color":"red"});
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");

      errCode = "";
      try {
        text.format(0, 0, {"color":"red"});
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");
      console.log(TAG + "*****************CollaborationEdit_Text_0003 End*****************");
    })
})
