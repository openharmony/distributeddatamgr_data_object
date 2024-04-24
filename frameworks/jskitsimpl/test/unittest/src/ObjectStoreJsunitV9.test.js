/*
 * Copyright (C) 2022 Huawei Device Co., Ltd.
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
import {afterAll, afterEach, beforeAll, beforeEach, describe, expect, it} from 'deccjsunit/index';
import distributedObject from '@ohos.data.distributedDataObject';
import commonType from '@ohos.data.commonType';
import abilityAccessCtrl from '@ohos.abilityAccessCtrl';
import featureAbility from '@ohos.ability.featureAbility';
import bundle from '@ohos.bundle';
var context;
const TAG = "OBJECTSTORE_TEST";
function changeCallback(sessionId, changeData) {
    console.info("changeCallback start");
    console.info(TAG + "sessionId:" + " " + sessionId);
    if (changeData != null && changeData != undefined) {
        changeData.forEach(element => {
            console.info(TAG + "data changed !" + element);
        });
    }
    console.info("changeCallback end");
}

function statusCallback1(sessionId, networkId, status) {
    console.info(TAG + "statusCallback1" + " " + sessionId);
    this.response += "\nstatus changed " + sessionId + " " + status + " " + networkId;
}

function statusCallback2(sessionId, networkId, status) {
    console.info(TAG + "statusCallback2" + " " + sessionId);
    this.response += "\nstatus changed " + sessionId + " " + status + " " + networkId;
}

const PERMISSION_USER_SET = 1;
const PERMISSION_USER_NAME = "ohos.permission.DISTRIBUTED_DATASYNC";
const CATCH_ERR = -1;
var tokenID = undefined;
async function grantPerm() {
    console.info("====grant Permission start====");
    var appInfo = await bundle.getApplicationInfo('com.OpenHarmony.app.test', 0, 100);
    tokenID = appInfo.accessTokenId;
    console.info("accessTokenId" + appInfo.accessTokenId + " bundleName:" + appInfo.bundleName);
    var atManager = abilityAccessCtrl.createAtManager();
    var result = await atManager.grantUserGrantedPermission(tokenID, PERMISSION_USER_NAME, PERMISSION_USER_SET);
    console.info("tokenId" + tokenID + " result:" + result);
    console.info("====grant Permission end====");
}
describe('objectStoreTest', function () {
    beforeAll(async function (done) {
        await grantPerm();
        done();
    })

    beforeEach(function () {
        console.info(TAG + 'beforeEach');
        context = featureAbility.getContext();
    })

    afterEach(function () {
        console.info(TAG + 'afterEach')
    })

    afterAll(function () {
        console.info(TAG + 'afterAll')
    })

    console.log(TAG + "*************Unit Test Begin*************");

    /**
     * @tc.name: V9testsetSessionId001
     * @tc.desc: object join session and on,object can receive callback when data has been changed
     * @tc.type: Function
     * @tc.number: V9testcreate001
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testcreate001', 0, function () {
        console.log(TAG + "************* V9testcreate001 start *************");
        var g_object;
        try {
            g_object = distributedObject.create(123, {name: "Amy", age: 18, isVis: false});
        } catch (error) {
            console.info(error.code + error.message);
            expect(error.code == 401).assertEqual(true);
            expect(error.message == "Parameter error. The type of 'context' must be 'Context'.").assertEqual(true);
        }
        try {
            g_object = distributedObject.create(context, 123);
        } catch (error) {
            console.info(error.code + error.message);
            expect(error.code == 401).assertEqual(true);
            expect(error.message == "Parameter error. The type of 'source' must be 'object'.").assertEqual(true);
        }
        g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("123456").then((data) => {
            console.info(TAG + "V9testcreate001");
            console.info(TAG + data);
        }).catch((error) => {
            console.info(TAG + error);
        });
        console.log(TAG + "************* V9testcreate001 end *************");
        g_object.setSessionId((error, data) => {
            console.info(TAG + error + "," + data);
        });
    })

    /**
     * @tc.name: V9testsetSessionId001
     * @tc.desc: object join session and on,object can receive callback when data has been changed
     * @tc.type: Function
     * @tc.number: V9testsetSessionId001
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testsetSessionId001', 0, function () {
        console.log(TAG + "************* V9testsetSessionId001 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        try {
            g_object.setSessionId(123).then((data) => {
                console.info(TAG + "setSessionId test");
                console.info(TAG + data);
            }).catch((err) => {
                console.info(TAG + err.code + err.message);
            });
        } catch (error) {
            expect(error.code == 401).assertEqual(true);
            expect(error.message == "Parameter error. The type of 'sessionId' must be 'string'.").assertEqual(true);
        }
        console.log(TAG + "************* V9testsetSessionId001 end *************");
        g_object.setSessionId((error, data) => {
            console.info(TAG + error + "," + data);
        });
    })

    /**
     * @tc.name: V9testsetSessionId002
     * @tc.desc: object join session and on,object can receive callback when data has been changed
     * @tc.type: Function
     * @tc.number: V9testsetSessionId002
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testsetSessionId002', 0, function () {
        console.log(TAG + "************* V9testsetSessionId002 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("session1");
        expect("session1" == g_object.__sessionId).assertEqual(true);
        g_object.setSessionId("session1").then(() => {
            console.info(TAG + "setSessionId test");
        }).catch((error) => {
            expect(error.code == 15400001).assertEqual(true);
            expect(error.message == "create table failed").assertEqual(true);
        });
        console.log(TAG + "************* V9testsetSessionId002 end *************");
        g_object.setSessionId((error, data) => {
            console.info(TAG + error + "," + data);
        });
    })

    /**
     * @tc.name: V9testsetSessionId003
     * @tc.desc: object join session and on,object can receive callback when data has been changed
     * @tc.type: Function
     * @tc.number: V9testsetSessionId003
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testsetSessionId003', 0, function () {
        console.log(TAG + "************* V9testsetSessionId003 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("123456").then((data) => {
            console.info(TAG + "setSessionId test");
            console.info(TAG + data);
        }).catch((err) => {
            console.info(TAG + err.code + err.message);
        });
        console.log(TAG + "************* V9testsetSessionId003 end *************");
        g_object.setSessionId((error, data) => {
            console.info(TAG + error + "," + data);
        });
    })

    /**
     * @tc.name: V9testsetSessionId004
     * @tc.desc: object join session and on,object can receive callback when data has been changed
     * @tc.type: Function
     * @tc.number: V9testsetSessionId004
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testsetSessionId004', 0, function () {
        console.log(TAG + "************* V9testsetSessionId004 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("123456", (error, data) => {
            console.info(TAG + error + "," + data);
            console.info(TAG + "setSessionId test");
        });
        g_object.setSessionId("", (error, data) => {
            console.info(TAG + error + "," + data);
        });
        try {
            g_object.setSessionId(1234, (error, data) => {
                console.info(TAG + error + "," + data);
                console.info(TAG + "setSessionId test");
            });
        } catch (error) {
            console.log(error.code + error.message);
            expect(error.code == 401).assertEqual(true);
            expect(error.message == "Parameter error. The type of 'sessionId' must be 'string'.").assertEqual(true);
        }
        console.log(TAG + "************* V9testsetSessionId004 end *************");
        g_object.setSessionId("", (error, data) => {
            console.info(TAG + error + "," + data);
        });
    })


    /**
     * @tc.name: V9testOn001
     * @tc.desc: object join session and on,object can receive callback when data has been changed
     * @tc.type: Function
     * @tc.number: V9testOn001
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testOn001', 0, function () {
        console.log(TAG + "************* V9testOn001 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("session1").then(() => {
            console.info("join session");
        }).catch((error) => {
            console.info(TAG + error.code + error.message);
        });
        expect("session1" == g_object.__sessionId).assertEqual(true);
        console.info(TAG + " start call watch change");
        g_object.on("change", function (sessionId, changeData) {
            console.info("V9testOn001 callback start.");
            if (changeData != null && changeData != undefined) {
                changeData.forEach(element => {
                    console.info(TAG + "data changed !" + element);
                });
            }
            console.info("V9testOn001 callback end.");
        });

        if (g_object != undefined && g_object != null) {
            g_object.name = "jack1";
            g_object.age = 19;
            g_object.isVis = true;
            expect(g_object.name == "jack1").assertEqual(true);
            expect(g_object.age == 19).assertEqual(true);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }

        console.log(TAG + "************* V9testOn001 end *************");
        g_object.setSessionId("", (error, data) => {
            console.info(TAG + error + "," + data);
        });
    })

    /**
     * @tc.name: V9testOn002
     * @tc.desc: object join session and on,then object change data twice,object can receive two callbacks when data has been changed
     * @tc.type: Function
     * @tc.number: V9testOn002
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testOn002', 0, function () {
        console.log(TAG + "************* V9testOn002 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("session1");
        expect("session1" == g_object.__sessionId).assertEqual(true);
        console.info(TAG + " start call watch change");
        try {
            g_object.on(123, function (sessionId, changeData) {
                console.info("V9testOn002 callback start.");
                console.info(TAG + sessionId);
                if (changeData != null && changeData != undefined) {
                    changeData.forEach(element => {
                        console.info(TAG + "data changed !" + element);
                    });
                }
                console.info("V9testOn002 callback end.");
            });

        } catch (error) {
            expect(error.code == 401).assertEqual(true);
            expect(error.message == "Parameter error. The type of 'type' must be 'string'.").assertEqual(true);
        }
        console.log(TAG + "************* V9testOn002 end *************");
        g_object.setSessionId("", (error, data) => {
            console.info(TAG + error + "," + data);
        });
    })

    /**
     * @tc.name: testOn003
     * @tc.desc object join session and on,then object do not change data,object can not receive callbacks
     * @tc.type: Function
     * @tc.number: testOn003
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testOn003', 0, function () {
        console.log(TAG + "************* V9testOn003 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("session1").then(() => {
            console.info("join session");
        }).catch((error) => {
            console.info(TAG + error.code + error.message);
        });
        expect("session1" == g_object.__sessionId).assertEqual(true);
        console.info(TAG + " start call watch change");
        try {
            g_object.on("error", function (sessionId, changeData) {
                console.info("V9testOn003 callback start.");
                if (changeData != null && changeData != undefined) {
                    changeData.forEach(element => {
                        console.info(TAG + "data changed !" + element);
                    });
                }
                console.info("V9testOn003 callback end.");
            });
        } catch (error) {
            expect(error != undefined).assertEqual(true);
        }
        console.log(TAG + "************* V9testOn003 end *************");
        g_object.setSessionId("", (error, data) => {
            console.info(TAG + error + "," + data);
        });
    })

    /**
     * @tc.name V9testOff001
     * @tc.desc object join session and on&off,object can not receive callback after off
     * @tc.type: Function
     * @tc.number: V9testOff001
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testOff001', 0, function () {
        console.log(TAG + "************* V9testOff001 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("session5").then(() => {
            console.info("join session");
        }).catch((error) => {
            console.info(TAG + error.code + error.message);
        });
        expect("session5" == g_object.__sessionId).assertEqual(true);

        g_object.on("change", changeCallback);
        console.info(TAG + " start call watch change");
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack1";
            g_object.age = 19;
            g_object.isVis = true;
            expect(g_object.name == "jack1").assertEqual(true);
            expect(g_object.age == 19).assertEqual(true);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }
        g_object.off("change", changeCallback);
        console.info(TAG + " end call watch change");
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack2";
            g_object.age = 20;
            g_object.isVis = false;
            expect(g_object.name == "jack2").assertEqual(true);
            expect(g_object.age == 20).assertEqual(true);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }
        console.log(TAG + "************* V9testOff001 end *************");
        g_object.setSessionId((error, data) => {
            console.info(TAG + error + "," + data);
        });
    })

    /**
     * @tc.name: V9testOff002
     * @tc.desc object join session and off,object can not receive callback
     * @tc.type: Function
     * @tc.number: V9testOff002
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testOff002', 0, function () {
        console.log(TAG + "************* V9testOff002 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("session6").then(() => {
            console.info("join session");
        }).catch((error) => {
            console.info(TAG + error.code + error.message);
        });
        expect("session6" == g_object.__sessionId).assertEqual(true);
        try {
            g_object.off(123);
        } catch (error) {
            expect(error.code == 401).assertEqual(true);
            expect(error.message == "Parameter error. The type of 'type' must be 'string'.").assertEqual(true);
        }
        console.info(TAG + " end call watch change");
        console.log(TAG + "************* V9testOff002 end *************");
        g_object.setSessionId().then((data) => {
            console.info(TAG + data);
            console.info(TAG + "setSessionId test");
        }).catch((error) => {
            console.info(TAG + error.code + error.message);
        });
    })

    /**
     * @tc.name: V9testOnStatus001
     * @tc.desc: object set a listener to watch another object online/offline
     * @tc.type: Function
     * @tc.number: V9testOnStatus001
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testOnStatus001', 0, function () {
        console.log(TAG + "************* V9testOnStatus001 start *************");
        console.log(TAG + "start watch status");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        try {
            g_object.on("status", null);
        } catch (error) {
            expect(error.code == 401).assertEqual(true);
            expect(error.message == "Parameter error. The type of 'callback' must be 'function'.").assertEqual(true);
        }
        console.log(TAG + "watch success");
        console.log(TAG + "************* V9testOnStatus001 end *************");
        g_object.setSessionId("").then((data) => {
            console.info(TAG + data);
            console.info(TAG + "setSessionId test");
        }).catch((error) => {
            console.info(TAG + error.code + error.message);
        });
    })

    /**
     * @tc.name: V9testOnStatus002
     * @tc.desc: object set several listener and can unWatch all watcher
     * @tc.type: Function
     * @tc.number: V9testOnStatus002
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testOnStatus002', 0, function () {
        console.log(TAG + "************* V9testOnStatus002 start *************");
        console.log(TAG + "start watch status");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        expect(g_object.name == "Amy").assertEqual(true);
        g_object.on("status", statusCallback1);
        console.log(TAG + "watch success");
        console.log(TAG + "start call unwatch status");
        g_object.off("status");
        console.log(TAG + "unwatch success");
        console.log(TAG + "************* V9testOnStatus002 end *************");
        g_object.setSessionId().then(() => {
            console.info("leave session");
        }).catch((error) => {
            console.info(TAG + error.code + error.message);
        });
    })

    /**
     * @tc.name: V9testSave001
     * @tc.desc: test save local
     * @tc.type: Function
     * @tc.number: V9testSave001
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testSave001', 0, async function (done) {
        console.log(TAG + "************* V9testSave001 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);

        g_object.setSessionId("mySession1").then(() => {
            console.info("join session");
        }).catch((error) => {
            console.info(TAG + error.code + error.message);
        });
        expect("mySession1" == g_object.__sessionId).assertEqual(true);

        g_object.save("local").then((ret) => {
            expect(ret.sessionId == "mySession1").assertEqual(true);
            expect(ret.version == g_object.__version).assertEqual(true);
            expect(ret.deviceId == "local").assertEqual(true);
            done();

            g_object.setSessionId("");
            g_object.name = undefined;
            g_object.age = undefined;
            g_object.isVis = undefined;
            g_object.setSessionId("mySession1");

            expect(g_object.name == "Amy").assertEqual(true);
            expect(g_object.age == 18).assertEqual(true);
            expect(g_object.isVis == false).assertEqual(true);
        }).catch((err) => {
            expect("801").assertEqual(err.code.toString());
            done();
        });
        console.log(TAG + "************* V9testSave001 end *************");
    })

    /**
     * @tc.name: V9testSave002
     * @tc.desc: test save local
     * @tc.type: Function
     * @tc.number: V9testSave002
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testSave002', 0, async function (done) {
        console.log(TAG + "************* V9testSave002 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);

        g_object.setSessionId("mySession2");
        expect("mySession2" == g_object.__sessionId).assertEqual(true);

        g_object.save("local", (err, result) => {
            if (err) {
                expect("801").assertEqual(err.code.toString());
                done();
                return;
            }
            expect(result.sessionId == "mySession2").assertEqual(true);
            expect(result.version == g_object.__version).assertEqual(true);
            expect(result.deviceId == "local").assertEqual(true);

            g_object.setSessionId("");
            g_object.name = undefined;
            g_object.age = undefined;
            g_object.isVis = undefined;
            g_object.setSessionId("mySession2");

            expect(g_object.name == "Amy").assertEqual(true);
            expect(g_object.age == 18).assertEqual(true);
            expect(g_object.isVis == false).assertEqual(true);
            done();
        });
        console.log(TAG + "************* V9testSave002 end *************");
    })

    /**
     * @tc.name: V9testSave003
     * @tc.desc: test save local
     * @tc.type: Function
     * @tc.number: V9testSave003
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testSave003', 0, async function () {
        console.log(TAG + "************* V9testSave003 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);

        g_object.setSessionId("mySession3").then(() => {
            console.info("join session");
        }).catch((error) => {
            console.info(TAG + error.code + error.message);
        });
        expect("mySession3" == g_object.__sessionId).assertEqual(true);
        try {
            g_object.save(1234).then((result) => {
                expect(result.sessionId == "mySession3").assertEqual(true);
                expect(result.version == g_object.__version).assertEqual(true);
                expect(result.deviceId == "local").assertEqual(true);
            })
        } catch (error) {
            expect(error.message == "Parameter error. The type of 'deviceId' must be 'string'.").assertEqual(true);
        }
        g_object.save("errorDeviceId").then((result) => {
            expect(result.sessionId == "mySession3").assertEqual(true);
            expect(result.version == g_object.__version).assertEqual(true);
            expect(result.deviceId == "local").assertEqual(true);
        }).catch((error) => {
            expect(error != undefined).assertEqual(true);
        });

        try {
            g_object.save("local", 123);
        } catch (error) {
            expect(error.code == 401).assertEqual(true);
            expect(error.message == "Parameter error. The type of 'callback' must be 'function'.").assertEqual(true);
        }
        console.log(TAG + "************* V9testSave003 end *************");
        g_object.setSessionId().then(() => {
            console.info("leave session");
        }).catch((error) => {
            console.info(TAG + error.code + error.message);
        });
    })

    /**
     * @tc.name: V9testRevokeSave001
     * @tc.desc: test RevokeSave
     * @tc.type: Function
     * @tc.number: V9testRevokeSave001
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testRevokeSave001', 0, async function (done) {
        console.log(TAG + "************* V9testRevokeSave001 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);

        g_object.setSessionId("mySession4");
        expect("mySession4" == g_object.__sessionId).assertEqual(true);

        g_object.save("local", (err, result) => {
            if (err) {
                expect("801").assertEqual(err.code.toString());
                done();
                return;
            }
            expect(result.sessionId == "mySession4").assertEqual(true);
            expect(result.version == g_object.__version).assertEqual(true);
            expect(result.deviceId == "local").assertEqual(true);
            g_object.revokeSave((err, result) => {
                if (err) {
                    expect("801").assertEqual(err.code.toString());
                    done();
                    return;
                }
                expect("mySession4" == result.sessionId).assertEqual(true);
                g_object.setSessionId("");
                g_object.name = undefined;
                g_object.age = undefined;
                g_object.isVis = undefined;
                g_object.setSessionId("mySession4");

                expect(g_object.name == undefined).assertEqual(true);
                expect(g_object.age == undefined).assertEqual(true);
                expect(g_object.isVis == undefined).assertEqual(true);
                done();
            })
        });
        console.log(TAG + "************* V9testRevokeSave001 end *************");
    })

    /**
     * @tc.name: V9testRevokeSave002
     * @tc.desc: test save local
     * @tc.type: Function
     * @tc.require:
     * @tc.number: V9testRevokeSave002
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testRevokeSave002', 0, async function () {
        console.log(TAG + "************* V9testRevokeSave002 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object != undefined).assertEqual(true);

        g_object.setSessionId("mySession5");
        expect("mySession5" == g_object.__sessionId.toString()).assertEqual(true);

        let result = await g_object.save("local").catch((err)=> {
            expect("801").assertEqual(err.code.toString());
            return CATCH_ERR;
        });
        if (result === CATCH_ERR) {
            return;
        }

        expect(result.sessionId.toString() == "mySession5").assertEqual(true);
        expect(result.version.toString() == g_object.__version.toString()).assertEqual(true);
        expect(result.deviceId.toString() == "local").assertEqual(true);

        result = await g_object.revokeSave().catch((err)=> {
            expect("801").assertEqual(err.code.toString());
            return CATCH_ERR;
        });

        if (result === CATCH_ERR) {
            return;
        }
        g_object.setSessionId("");
        g_object.name = undefined;
        g_object.age = undefined;
        g_object.isVis = undefined;
        g_object.setSessionId("mySession5");

        expect(g_object.name == undefined).assertEqual(true);
        expect(g_object.age == undefined).assertEqual(true);
        expect(g_object.isVis == undefined).assertEqual(true);

        console.log(TAG + "************* V9testRevokeSave002 end *************");
    })

    /**
     * @tc.name: V9testRevokeSave003
     * @tc.desc: test RevokeSave
     * @tc.type: Function
     * @tc.number: V9testRevokeSave003
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testRevokeSave003', 0, async function () {
        console.log(TAG + "************* V9testRevokeSave003 start *************");
        var g_object = distributedObject.create(context, {name: "Amy", age: 18, isVis: false});
        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("mySession6").then(() => {
            console.info("join session");
        }).catch((error) => {
            console.info(TAG + error.code + error.message);
        });
        expect("mySession6" == g_object.__sessionId).assertEqual(true);
        let result = await g_object.save("local").catch((err) => {
            expect("801").assertEqual(err.code.toString());
            return CATCH_ERR;
        });
        if (result === CATCH_ERR) {
            return;
        }
        expect(result.sessionId == "mySession6").assertEqual(true);
        expect(result.version == g_object.__version).assertEqual(true);
        expect(result.deviceId == "local").assertEqual(true);
        try {
            g_object.revokeSave(123).then((result) => {
                expect(result.sessionId == "mySession6").assertEqual(true);
            }).catch((err) => {
                console.log(err.code + err.message);
            });
        } catch (error) {
            console.info(error.code + error.message);
            expect("401").assertEqual(error.code.toString());
        }
        console.log(TAG + "************* V9testRevokeSave003 end *************");
    })

    /**
     * @tc.name: V9testbindAssetCommunType001
     * @tc.desc: Test CommType Enum Value
     * @tc.type: Function
     * @tc.number: V9testbindAssetCommunType001
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testbindAssetCommunType001', 0, function (done) {
        console.log(TAG + "************* V9testbindAssetCommunType001 start *************");
        expect(commonType.AssetStatus.ASSET_NORMAL == 1).assertTrue();
        expect(commonType.AssetStatus.ASSET_INSERT == 2).assertTrue();
        expect(commonType.AssetStatus.ASSET_UPDATE == 3).assertTrue();
        expect(commonType.AssetStatus.ASSET_DELETE == 4).assertTrue();
        expect(commonType.AssetStatus.ASSET_ABNORMAL == 5).assertTrue();
        expect(commonType.AssetStatus.ASSET_DOWNLOADING == 6).assertTrue();
        done();
    })
    
    /**
     * @tc.name: V9testsetSessionId001
     * @tc.desc: Test bindAssetStore
     * @tc.type: Function
     * @tc.number: V9testbindAssetStore001
	 * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testbindAssetStore001', 0, function (done) {
        console.log(TAG + "************* V9testbindAssetStore001 start *************");
        let g_object = distributedObject.create(context, {
            title:"bindAssettest-title",
            content:"bindAssettest-content",
            mark:"no mark",
            asset001:
            {
                status:0,
                name:"1.txt",
                uri:"file://com.example.myapp/data/dir/1.txt",
                path:"/dir/1.txt",
                createTime: "2023/11/30 19:48:00",
                modifyTime: "2023/11/30 20:10:00",
                size:"1",
            }
        });

        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("sessionBA1");

        let asset1 = {
            name: "name1",
            uri: "uri1",
            createTime: "createTime1",
            modifyTime: "modifyTime1",
            size: "size1",
            path: "path1",
            status: commonType.AssetStatus.ASSET_NORMAL,
        }

        let asset2 = {
            name: "name2",
            uri: "uri2",
            createTime: "createTime2",
            modifyTime: "modifyTime2",
            size: "size2",
            path: "path2",
            status: commonType.AssetStatus.ASSET_UPDATE,
        }

        let asset3 = {
            name: "name3",
            uri: "uri3",
            createTime: "createTime3",
            modifyTime: "modifyTime3",
            size: "size3",
            path: "path3",
        }

        let result = new Uint8Array(8);
        for (let i = 0; i < 8; i++) {
          result[i] = 1;
        }

        let arr = [asset1, asset2];
        let bindInfo  = {
            storeName:"store1",
            tableName:"table1",
            primaryKey:{
                "data1": 123, 
                "data2": arr, 
                "data3": asset3, 
                "data4": 101.5, 
                "data5": result, 
                "data6": false, 
                "data7": null, 
                "data8": "test1"},
            field:"field1",
            assetName:"asset1"
        }
        
        try {
            g_object.bindAssetStore("asset001", bindInfo, (err, data)=>{
                expect(err == undefined).assertEqual(true);
                done();
            });
        } catch (error) {
            console.info(`V9testbindAssetStore001 err is: ${error.code} and msg is: ${error.message}`);
            expect(null).assertFail();
            done();
        }
    })

    /**
     * @tc.name: V9testsetSessionId002
     * @tc.desc: Test bindAssetStore with invalid args
     * @tc.type: Function
     * @tc.number: V9testsetSessionId002
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testbindAssetStore002', 0, function (done) {
        console.log(TAG + "************* V9testbindAssetStore002 start *************");
        let g_object = distributedObject.create(context, {
            title:"bindAssettest-title",
            content:"bindAssettest-content",
            mark:"no mark",
            asset002:
            {
                status:0,
                name:"1.txt",
                uri:"file://com.example.myapp/data/dir/1.txt",
                path:"/dir/1.txt",
                createTime: "2023/11/30 19:48:00",
                modifyTime: "2023/11/30 20:10:00",
                size:"1",
            }
        });

        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("sessionBA2");
        
        let bindInfo  = {
            storeName: undefined,
            tableName: "table1",
            primaryKey: {"data1": 123},
            field: "field1",
            assetName: "asset1"
        }
        
        try {
            g_object.bindAssetStore("asset002", bindInfo, (err, data)=>{
                expect(null).assertFail();
                done();
            });
        } catch (error) {
            console.info(`V9testbindAssetStore002 err is: ${error.code} and msg is: ${error.message}`);
            expect(error.code == 401).assertTrue();
            done();
        }
    })

    /**
     * @tc.name: V9testbindAssetStore003
     * @tc.desc: Test bindAssetStore with invalid args
     * @tc.type: Function
     * @tc.number: V9testbindAssetStore003
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testbindAssetStore003', 0, function (done) {
        console.log(TAG + "************* V9testbindAssetStore003 start *************");
        let g_object = distributedObject.create(context, {
            title:"bindAssettest-title",
            content:"bindAssettest-content",
            mark:"no mark",
            asset003:
            {
                status:0,
                name:"1.txt",
                uri:"file://com.example.myapp/data/dir/1.txt",
                path:"/dir/1.txt",
                createTime: "2023/11/30 19:48:00",
                modifyTime: "2023/11/30 20:10:00",
                size:"1",
            }
        });
        
        expect(g_object == undefined).assertEqual(false);
        g_object.setSessionId("sessionBA3");
        let bindInfo  = {
            storeName:"name1",
            tableName: null,
            primaryKey:{"data1": 123},
            field:"field1",
            assetName:"asset1"
        }
        
        try {
            g_object.bindAssetStore("asset003", bindInfo, (err, data)=>{
                expect(null).assertFail();
                done();
            });
        } catch (error) {
            console.info(`V9testbindAssetStore003 err is: ${error.code} and msg is: ${error.message}`);
            expect(error.code == 401).assertTrue();
            done();
        }
    })

    /**
     * @tc.name: V9testcreate002
     * @tc.desc: object with asset create distributed data object
     * @tc.type: Function
     * @tc.number: V9testcreate002
     * @tc.size: MediumTest
     * @tc.level: Level 2
     */
    it('V9testcreate002', 0, function () {
        console.log(TAG + "************* V9testcreate002 start *************");
        var g_object;
        const attachment = {
            status: 0,
            name: "1.txt",
            uri: "file://com.example.myapplication/data/storage/el2/distributedfiles/dir/1.txt",
            path: "/dir/1.txt",
            createTime: "2023-11-26 10:00:00",
            modifyTime: "2023-11-26 10:00:00",
            size: "1"
        };
        try {
            g_object = distributedObject.create(context, {
                title: "my note",
                content: "It's a nice day today.",
                mark: false,
                attachment
            });
        } catch (error) {
            console.info(error.code + error.message);
        }
        expect(g_object === undefined).assertEqual(false);

        g_object.setSessionId("123456").then((data) => {
            console.info(TAG + "V9testcreate002");
            console.info(TAG + data);
        }).catch((error) => {
            console.info(TAG + error);
        });

        try {
            g_object.attachment = {};
        } catch (error) {
            expect(error.code === 401).assertEqual(true);
            expect(error.message === "cannot set attachment by non Asset type data").assertEqual(true);
        }

        g_object.setSessionId("").then((data) => {
            console.info(TAG + "V9testcreate002");
            console.info(TAG + data);
        }).catch((error) => {
            console.info(TAG + error);
        });

        console.log(TAG + "************* V9testcreate002 end *************");
        g_object.setSessionId((error, data) => {
            console.info(TAG + error + "," + data);
        });
    })
    console.log(TAG + "*************Unit Test End*************");
})