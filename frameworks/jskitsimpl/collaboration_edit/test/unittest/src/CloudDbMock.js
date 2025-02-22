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

const QRY_CURSOR_IDX = 0;
const QRY_EQUIP_ID_IDX = 1;

export default class CloudDbMock {
  static g_batchUploadIndex = 0;
  static g_cursor = 0;
  static g_records = new Array();
  static g_assets = new Array();

  static batchInsertTestWrap(records) {
    for (let record of records) {
      record.cursor = ++CloudDbMock.g_cursor;
    }
    CloudDbMock.g_records = CloudDbMock.g_records.concat(records);
    return records.length;
  }

  static queryTestWrap(conditions) {
    let cursor = conditions[QRY_CURSOR_IDX].fieldValue;
    let equipId = conditions[QRY_EQUIP_ID_IDX].fieldValue;

    const result = CloudDbMock.g_records.filter((item) => item.cursor > cursor).filter((item) => item.id != equipId);
    return result;
  }

  static uploadAssetTestWrap(path) {
    CloudDbMock.g_assets.push(path);
    return true;
  }

  static deleteAssetTestWrap(path) {
    let index = CloudDbMock.g_assets.indexOf(path);
    if (index < 0) {
      console.log("no matching asset");
      return false;
    }
    CloudDbMock.g_assets.splice(index, 1);
    return true;
  }

  static deleteLocalAssetTestWrap() {
    return true;
  }

  static getCloudRecords() {
    return CloudDbMock.g_records;
  }

  static getCloudAssets() {
    return CloudDbMock.g_assets;
  }

  static resetEnv() {
    CloudDbMock.g_records = new Array();
    CloudDbMock.g_assets = new Array();
    CloudDbMock.g_cursor = 0;
  }
}