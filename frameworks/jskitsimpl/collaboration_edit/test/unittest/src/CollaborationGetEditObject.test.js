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
 
var context = ability_featureAbility.getContext()
 
describe('collaborationGetEditObjectTest', () => {
    beforeAll(async () => {
        console.log(TAG + "beforeAll");
    })
 
    beforeEach(async () => {
      console.log(TAG + "beforeEach");
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
     * @tc.number CollaborationEdit_GetEditObject_0001
     * @tc.name Normal case of getCollaborationEditObject
     * @tc.desc 
     *  1. get collaboration edit object correctly
     *  2. delete edit object by empty name, then check 401 code
     */
    it("CollaborationEdit_GetEditObject_0001", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_GetEditObject_0001 Start*****************");
      let editObject = undefined;
      try {
        editObject = collaboration_edit.getCollaborationEditObject(context, DOC_CONFIG);
        expect(editObject !== undefined).assertTrue();
      } catch (err) {
        console.log(TAG + "CollaborationEdit_GetEditObject_0001 failed.");
        expect().assertFail();
      }

      let errCode = "";
      try {
        collaboration_edit.deleteCollaborationEditObject(context, {name: ""});
      } catch (err) {
        errCode = err.code;
      }
      expect(errCode).assertEqual("401");
      console.log(TAG + "*****************CollaborationEdit_GetEditObject_0001 End*****************");
    })
    
    /**
     * @tc.number CollaborationEdit_GetEditObject_0002
     * @tc.name getCollaborationEditObject by null context
     * @tc.desc 
     *  1. get collaboration edit object by null context
     *  2. check 401 error code
     */
    it("CollaborationEdit_GetEditObject_0002", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_GetEditObject_0002 Start*****************");
      let editObject = undefined;
      let errCode = "";
      try {
        editObject = collaboration_edit.getCollaborationEditObject(null, DOC_CONFIG);
      } catch (err) {
        errCode = err.code;
      }
      expect(editObject).assertUndefined();
      expect("401").assertEqual(errCode);
      console.log(TAG + "*****************CollaborationEdit_GetEditObject_0002 End*****************");
    })
    
    /**
     * @tc.number CollaborationEdit_GetEditObject_0003
     * @tc.name getCollaborationEditObject by empty name
     * @tc.desc 
     *  1. get collaboration edit object by empty name
     *  2. check 401 error code
     */
    it("CollaborationEdit_GetEditObject_0003", 0, async () => {
      console.log(TAG + "*****************CollaborationEdit_GetEditObject_0003 Start*****************");
      let editObject = undefined;
      let errCode = "";
      try {
        editObject = collaboration_edit.getCollaborationEditObject(context, {name: ""});
      } catch (err) {
        errCode = err.code;
      }
      expect(editObject).assertUndefined();
      expect("401").assertEqual(errCode);
      console.log(TAG + "*****************CollaborationEdit_GetEditObject_0003 End*****************");
    })
})
