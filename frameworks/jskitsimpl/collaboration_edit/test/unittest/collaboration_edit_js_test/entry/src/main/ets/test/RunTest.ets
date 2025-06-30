/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
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

import { describe, beforeEach, afterEach, beforeAll, it, afterAll, expect, Level } from '@ohos/hypium'
import common from '@ohos.app.ability.common';
import commonType from '@ohos.data.commonType';
import DataObject from '@ohos.data.distributedDataObject';
import abilityDelegatorRegistry from '@ohos.app.ability.abilityDelegatorRegistry';
import UIAbility from '@ohos.app.ability.UIAbility';


const TAG = "[CollaborationEdit_JsTest_Text]"
let context: common.Context = globalThis.abilityContext;


class Note {
  title: string | undefined
  text: string | undefined
  attachment: commonType.Asset | undefined

  constructor(title: string | undefined, text: string | undefined,
    attachment: commonType.Asset | undefined) {
    this.title = title;
    this.text = text;
    this.attachment = attachment;
  }
}

let attachment: commonType.Asset = {
  name: 'test_img.jpg',
  uri: 'file://com.example.myapplication/data/storage/el2/distributedfiles/dir/test_img.jpg',
  path: '/dir/test_img.jpg',
  createTime: '2024-01-02 10:00:00',
  modifyTime: '2024-01-02 10:00:00',
  size: '5',
  status: commonType.AssetStatus.ASSET_NORMAL
}

const bindInfo: DataObject.BindInfo = {
  storeName: 'notepad',
  tableName: 'note_t',
  primaryKey: {
    'uuid': '00000000-0000-0000-0000-000000000000'
  },
  field: 'attachment',
  assetName: attachment.name as string
}

let note: Note = new Note('test', 'test', attachment);

function statusCallBack(sessionId: string, networkId: string, status: string) {
}

function changeCallBack(sessionId: string, fields: Array<string>) {
}

function progressCallBack(sessionId: string, progress: number) {
}

let sleep = (ms:number) => {
  return new Promise<ESObject>(resolve => setTimeout(resolve, ms));
}
export default function collaborationrunTextTest() {
  describe('collaborationrunTextTest', () => {
    beforeAll(async(done:Function) => {

      let abilityDelegator:abilityDelegatorRegistry.AbilityDelegator = abilityDelegatorRegistry.getAbilityDelegator()
      try{
      abilityDelegator.addAbilityMonitor({
        abilityName: "EntryAbility",
        moduleName:"entry",
        onAbilityCreate: (abilitys : UIAbility) : void => {
          context = abilitys.context;
          // hilog.info(domain, tag, '%{public}s', 'onAbilityCreate end');

        },
      }, (err) : void => {

      });
      } catch (err){
      }

      abilityDelegator.executeShellCommand("aa start -a EntryAbility -b com.example.myapplication")
      await sleep(2000)

      done();
    })

    beforeEach(() => {

    })

    afterEach(() => {
    })

    afterAll(() => {
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0001
     * @tc.name      : Monitor data changes 
     * @tc.desc      : Test the interface function for monitoring data changes
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0001', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("change", changeCallBack);
        expect(true).assertTrue()

      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })


    /**
     * @tc.number    : CollaborationEdit_Text_0002
     * @tc.name      : Monitor data changes 
     * @tc.desc      : Test the interface function for monitoring data changes, callback is closure
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */


    it('CollaborationEdit_Text_0002', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("change", () => {
        });
        expect(true).assertTrue()
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0003
     * @tc.name      : Remove monitoring of data changes
     * @tc.desc      : Test the interface function for remove monitoring of data changes
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0003', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("change",changeCallBack);
        g_object.off("change", changeCallBack);
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0004
     * @tc.name      : Remove monitoring of data changes
     * @tc.desc      : Test the interface function for remove of all data changes
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0004', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("change",changeCallBack);
        g_object.off("change");
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0005
     * @tc.name      : Monitor data object online/offline 
     * @tc.desc      : Test the interface function for monitoring of data object online/offline 
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0005', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("status", statusCallBack);
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0006
     * @tc.name      : Monitor data object online/offline 
     * @tc.desc      : Test the interface function for monitoring of data object online/offline , no callback
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0006', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("status", null);
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })


    /**
     * @tc.number    : CollaborationEdit_Text_0007
     * @tc.name      : Remove monitor of data object online/offline 
     * @tc.desc      : Test the interface function for removing monitor of data object online/offline
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */


    it('CollaborationEdit_Text_0007', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("status",statusCallBack);
        g_object.off("status", statusCallBack);
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0008
     * @tc.name      : Remove monitor of data object online/offline 
     * @tc.desc      : Test the interface function for removing monitor of all data object online/offline
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0008', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("status",statusCallBack);
        g_object.off("status");
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0010
     * @tc.name      : Set a single asset
     * @tc.desc      : Test the interface function for setting a single asset, property parameter is null
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0010', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        let uri = "file:1.txt";
        g_object.setAsset(null, uri);
      } catch (err) {
        expect(err.code == 15400002).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0011
     * @tc.name      : Set a single asset
     * @tc.desc      : Test the interface function for setting a single asset, property parameter is empty string
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0011', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        let uri = "file:1.txt";
        g_object.setAsset("", uri);
      } catch (err) {
        expect(err.code == 15400002).assertEqual(true);
      }
    })


    /**
     * @tc.number    : CollaborationEdit_Text_0012
     * @tc.name      : Set a single asset
     * @tc.desc      : Test the interface function for setting a single asset, uri is null
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0012', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        let uri = "";
        g_object.setAsset("file", uri);
      } catch (err) {
        expect(err.code == 15400002).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0014
     * @tc.name      : Set a single asset
     * @tc.desc      : Test the interface function for setting asset array, property parameter is null
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0014', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        let uris = ["file:1.txt"];
        g_object.setAssets(null, uris);
      } catch (err) {
        expect(err.code == 15400002).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0015
     * @tc.name      : Set a single asset
     * @tc.desc      : Test the interface function for setting asset array, property parameter is empty string
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0015', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        let uris = ["file:1.txt"];
        g_object.setAssets("", uris);
      } catch (err) {
        expect(err.code == 15400002).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0016
     * @tc.name      : Set a single asset
     * @tc.desc      : Test the interface function for setting asset array, asset array is empty
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0016', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.setAssets("file", []);
      } catch (err) {
        expect(err.code == 15400002).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0017
     * @tc.name      : Set a single asset
     * @tc.desc      : Test the interface function for setting asset array, number of asset more than 50
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0017', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        const uris: string[] = [];
        for (let index = 0; index < 55; index++) {
          let uri = `file${index + 1}:${index + 1}.txt`;
          uris.push(uri);
        }
        g_object.setAssets("file", uris);
      } catch (err) {
        expect(err.code == 15400002).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0017
     * @tc.name      : Set a single asset
     * @tc.desc      : Test the interface function for setting asset array, apart of assets are invalid
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0018', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        let uris = ["file:1.txt", null];
        g_object.setAssets("file", uris);
      } catch (err) {
        expect(err.code == 15400002).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0019
     * @tc.name      : Monitor asset progress
     * @tc.desc      : Test the interface function for monitoring asset progress
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0019', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("progressChanged", progressCallBack);
        expect(true).assertTrue()

      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })


    /**
     * @tc.number    : CollaborationEdit_Text_0020
     * @tc.name      : Monitor asset progress
     * @tc.desc      : Test the interface function for monitoring asset progress, callback is closure
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */


    it('CollaborationEdit_Text_0020', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("progressChanged", () => {
        });
        expect(true).assertTrue()
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0021
     * @tc.name      : Remove monitoring of asset progress
     * @tc.desc      : Test the interface function for remove monitoring of asset progress
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0021', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("progressChanged",progressCallBack);
        g_object.off("progressChanged", progressCallBack);
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0022
     * @tc.name      : Remove monitoring of asset progress
     * @tc.desc      : Test the interface function for remove of all asset progress
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0022', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("progressChanged",progressCallBack);
        g_object.off("progressChanged");
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0023
     * @tc.name      : Monitor data object online/offline 
     * @tc.desc      : Test the interface function for monitoring of data object online/offline 
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0023', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("progressChanged", progressCallBack);
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0024
     * @tc.name      : Monitor data object online/offline 
     * @tc.desc      : Test the interface function for monitoring of data object online/offline , no callback
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0024', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("progressChanged", null);
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })


    /**
     * @tc.number    : CollaborationEdit_Text_0025
     * @tc.name      : Remove monitor of data object online/offline 
     * @tc.desc      : Test the interface function for removing monitor of data object online/offline
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */


    it('CollaborationEdit_Text_0025', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("progressChanged",progressCallBack);
        g_object.off("progressChanged", progressCallBack);
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0026
     * @tc.name      : Remove monitor of data object online/offline 
     * @tc.desc      : Test the interface function for removing monitor of all data object online/offline
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0026', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("progressChanged",progressCallBack);
        g_object.off("progressChanged");
        expect(true).assertTrue();
      } catch (err) {
        expect(err.code == 401).assertEqual(true);
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0027
     * @tc.name      : Remove monitor of data object
     * @tc.desc      : Test the interface function for removing monitor of all data object
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0027', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("progressChanged",()=>{});
        g_object.on("progressChanged",progressCallBack);
        g_object.off("progressChanged",()=>{});
      } catch (err) {
        expect().assertFail();
      }
    })

    /**
     * @tc.number    : CollaborationEdit_Text_0028
     * @tc.name      : Remove monitor of data object
     * @tc.desc      : Test the interface function for removing monitor of all data object
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0028', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("progressChanged",progressCallBack);
        g_object.on("progressChanged",progressCallBack);
        g_object.off("progressChanged",()=>{});
      } catch (err) {
        expect().assertFail();
      }
    })

  /**
     * @tc.number    : CollaborationEdit_Text_0029
     * @tc.name      : Remove monitor of data object
     * @tc.desc      : Test the interface function for removing monitor of all data object
     * @tc.size      : MEDIUM
     * @tc.type      : Function
     * @tc.level     : Level 1
     */

    it('CollaborationEdit_Text_0029', Level.LEVEL1, () => {
      try {
        let g_object: DataObject.DataObject = DataObject.create(context, note);
        g_object.on("progressChanged",progressCallBack);
        g_object.on("progressChanged",progressCallBack);
        g_object.off("progressChanged",undefined);
      } catch (err) {
        expect().assertFail();
      }
    })
  })
}