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
import {describe, beforeAll, beforeEach, afterEach, afterAll, it, expect} from 'deccjsunit/index'
import distributedObject from '@ohos.data.distributedDataObject';

var baseLine = 500; //0.5 second
const TAG = "OBJECTSTORE_TEST";

function changeCallback(sessionId, changeData) {
    console.info("get init change111" + sessionId + " " + changeData);
    if (changeData != null && changeData != undefined) {
        changeData.forEach(element => {
            console.info(TAG + "data changed !" + element);
        });
    }
    console.info(TAG + "get init change111 end" + sessionId + " " + changeData);
}

function changeCallback2(sessionId, changeData) {
    console.info("get init change222" + sessionId + " " + changeData);
    if (changeData != null && changeData != undefined) {
        changeData.forEach(element => {
            console.info(TAG + "data changed !" + element);
        });
    }
    console.info(TAG + "get init change222 end" + sessionId + " " + changeData);
}

function statusCallback1(sessionId, networkId, status) {
    console.info(TAG + "test init change111" + sessionId);
    this.response += "\nstatus changed " + sessionId + " " + status + " " + networkId;
}

function statusCallback2(sessionId, networkId, status) {
    console.info(TAG + "test init change222" + sessionId);
    this.response += "\nstatus changed " + sessionId + " " + status + " " + networkId;
}

function statusCallback3(sessionId, networkId, status) {
    console.info(TAG + "test init change333" + sessionId);
    this.response += "\nstatus changed " + sessionId + " " + status + " " + networkId;
}

describe('objectStoreTest', function () {
    beforeAll(function () {
        console.info(TAG + 'beforeAll')
    })

    beforeEach(function () {
        console.info(TAG + 'beforeEach')
    })

    afterEach(function () {
        console.info(TAG + 'afterEach')
    })

    afterAll(function () {
        console.info(TAG + 'afterAll')
    })

    console.log(TAG + "*************Unit Test Begin*************");


    /**
     * @tc.name: testOn001
     * @tc.desc: object join session and on,object can receive callback when data has been changed
     * @tc.type: FUNC
     * @tc.require: I4H3LS
     */
    it('testOn001', 0, function (done) {
        console.log(TAG + "************* testOn001 start *************");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.setSessionId("session1");
        if (g_object != undefined && g_object != null) {
            expect("session1" == g_object.__sessionId);
        } else {
            console.log(TAG + "testOn001 joinSession failed");
        }
        console.info(TAG + " start call watch change");
        g_object.on("change", changeCallback);
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack1";
            g_object.age = 19;
            g_object.isVis = true;
            expect(g_object.name == "jack1");
            expect(g_object.age == 19);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }

        done()
        console.log(TAG + "************* testOn001 end *************");
    })

    /**
     * @tc.name: testOn002
     * @tc.desc object join session and no on,obejct can not receive callback when data has been changed
     * @tc.type: FUNC
     * @tc.require: I4H3LS
     */
    it('testOn002', 0, function (done) {
        console.log(TAG + "************* testOn002 start *************");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.setSessionId("session2");
        if (g_object != undefined && g_object != null) {
            expect("session2" == g_object.__sessionId);
        } else {
            console.log(TAG + "testOn002 joinSession failed");
        }
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack1";
            g_object.age = 19;
            g_object.isVis = true;
            expect(g_object.name == "jack1");
            expect(g_object.age == 19);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }

        done()
        console.log(TAG + "************* testOn002 end *************");
    })

    /**
     * @tc.name: testOn003
     * @tc.desc: object join session and on,then object change data twice,object can receive two callbacks when data has been changed
     * @tc.type: FUNC
     * @tc.require: I4H3LS
     */
    it('testOn003', 0, function (done) {
        console.log(TAG + "************* testOn003 start *************");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.setSessionId("session3");
        if (g_object != undefined && g_object != null) {
            expect("session3" == g_object.__sessionId);
        } else {
            console.log(TAG + "testOn003 joinSession failed");
        }
        g_object.on("change", changeCallback);
        console.info(TAG + " start call watch change");
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack1";
            g_object.age = 19;
            g_object.isVis = true;
            expect(g_object.name == "jack1");
            expect(g_object.age == 19);
            g_object.name = "jack2";
            g_object.age = 20;
            g_object.isVis = false;
            expect(g_object.name == "jack2");
            expect(g_object.age == 20);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }

        done()
        console.log(TAG + "************* testOn003 end *************");
    })

    /**
     * @tc.name: testOn004
     * @tc.desc object join session and on,then object do not change data,object can not receive callbacks
     * @tc.type: FUNC
     * @tc.require: I4H3LS
     */
    it('testOn004', 0, function (done) {
        console.log(TAG + "************* testOn004 start *************");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.setSessionId("session4");
        if (g_object != undefined && g_object != null) {
            expect("session4" == g_object.__sessionId);
        } else {
            console.log(TAG + "testOn004 joinSession failed");
        }
        g_object.on("change", changeCallback);
        console.info(TAG + " start call watch change");
        console.info(TAG + " end call watch change");

        done()
        console.log(TAG + "************* testOn004 end *************");
    })

    /**
     * @tc.name testOff001
     * @tc.desc object join session and on&off,object can not receive callback after off
     * @tc.type: FUNC
     * @tc.require: I4H3LS
     */
    it('testOff001', 0, function (done) {
        console.log(TAG + "************* testOff001 start *************");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.setSessionId("session5");
        if (g_object != undefined && g_object != null) {
            expect("session5" == g_object.__sessionId);
        } else {
            console.log(TAG + "testOff001 joinSession failed");
        }
        g_object.on("change", changeCallback);
        console.info(TAG + " start call watch change");
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack1";
            g_object.age = 19;
            g_object.isVis = true;
            expect(g_object.name == "jack1");
            expect(g_object.age == 19);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }
        g_object.off("change");
        console.info(TAG + " end call watch change");
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack2";
            g_object.age = 20;
            g_object.isVis = false;
            expect(g_object.name == "jack2");
            expect(g_object.age == 20);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }

        done()
        console.log(TAG + "************* testOff001 end *************");
    })

    /**
     * @tc.name:testOff002
     * @tc.desc object join session and off,object can not receive callback
     * @tc.type: FUNC
     * @tc.require: I4H3LS
     */
    it('testOff002', 0, function (done) {
        console.log(TAG + "************* testOff002 start *************");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.setSessionId("session6");
        if (g_object != undefined && g_object != null) {
            expect("session6" == g_object.__sessionId);
        } else {
            console.log(TAG + "testOff002 joinSession failed");
        }
        g_object.off("change");
        console.info(TAG + " end call watch change");
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack1";
            g_object.age = 19;
            g_object.isVis = true;
            expect(g_object.name == "jack1");
            expect(g_object.age == 19);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }

        done()
        console.log(TAG + "************* testOff002 end *************");
    })

    /**
     * @tc.name: testMultiObjectOff001
     * @tc.desc: two objects join session and on&off,then two objects can not receive callbacks
     * @tc.type: FUNC
     * @tc.require: I4H3LS
     */
    it('testMultiObjectOff001', 0, function (done) {
        console.log(TAG + "************* testMultiObjectOff001 start *************");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.setSessionId("session8");
        if (g_object != undefined && g_object != null) {
            expect("session8" == g_object.__sessionId);
        } else {
            console.log(TAG + "testMultiObjectOn002 joinSession failed");
        }
        var test_object = distributedObject.createDistributedObject({ name: "Eric", age: 81, isVis: true });
        test_object.setSessionId("testSession2");
        if (test_object != undefined && test_object != null) {
            expect("testSession2" == test_object.__sessionId);
        } else {
            console.log(TAG + "testMultiObjectOn002 joinSession failed");
        }
        console.log(TAG + " start call watch change")
        g_object.on("change", changeCallback);
        test_object.on("change", changeCallback2);
        console.info(TAG + " watch success");
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack1";
            g_object.age = 19;
            g_object.isVis = true;
            expect(g_object.name == "jack1");
            expect(g_object.age == 19);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }
        if (test_object != undefined && test_object != null) {
            test_object.name = "jack2";
            test_object.age = 20;
            test_object.isVis = false;
            expect(test_object.name == "jack2");
            expect(test_object.age == 20);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }
        g_object.off("change");
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack3";
            g_object.age = 21;
            g_object.isVis = false;
            expect(g_object.name == "jack3");
            expect(g_object.age == 21);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }
        test_object.off("change");
        if (test_object != undefined && test_object != null) {
            test_object.name = "jack4";
            test_object.age = 22;
            test_object.isVis = true;
            expect(test_object.name == "jack4");
            expect(test_object.age == 22);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }

        done()
        console.log(TAG + "************* testMultiObjectOff001 end *************");
    })

    /**
     * @tc.name: testChangeSession001
     * @tc.desc: objects join session and on,then change sessionId
     * @tc.type: FUNC
     * @tc.require: I4H3LS
     */
    it('testChangeSession001', 0, function (done) {
        console.log(TAG + "************* testChangeSession001 start *************");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.setSessionId("session9");
        if (g_object != undefined && g_object != null) {
            expect("session9" == g_object.__sessionId);
        } else {
            console.log(TAG + "testChangeSession001 joinSession session9 failed");
        }
        g_object.on("change", changeCallback);
        console.info(TAG + " start call watch change");
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack1";
            g_object.age = 19;
            g_object.isVis = true;
            expect(g_object.name == "jack1");
            expect(g_object.age == 19);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }
        g_object.setSessionId("session10");
        if (g_object != undefined && g_object != null) {
            expect("session10" == g_object.__sessionId);
        } else {
            console.log(TAG + "testChangeSession001 joinSession session10 failed");
        }
        if (g_object != undefined && g_object != null) {
            g_object.name = "jack2";
            g_object.age = 20;
            g_object.isVis = false;
            expect(g_object.name == "jack2");
            expect(g_object.age == 20);
            console.info(TAG + " set data success!");
        } else {
            console.info(TAG + " object is null,set name fail");
        }

        done()
        console.log(TAG + "************* testChangeSession001 end *************");
    })

    /**
     * @tc.name: testUndefinedType001
     * @tc.desc: object use undefined type,can not join session
     * @tc.type: FUNC
     * @tc.require: I4H3LS
     */
    it('testUndefinedType001', 0, function (done) {
        console.log(TAG + "************* testUndefinedType001 start *************");
        var undefined_object = distributedObject.createDistributedObject({ name: undefined, age: undefined, isVis: undefined });
        try {
            undefined_object.setSessionId("session11");
            if (undefined_object != undefined && undefined_object != null) {
                expect("session11" == undefined_object.__sessionId);
            } else {
                console.log(TAG + "testChangeSession001 joinSession session11 failed");
            }
        } catch (error) {
            console.error(TAG + error);
        }

        done()
        console.log(TAG + "************* testUndefinedType001 end *************");
    })

    /**
     * @tc.name: testGenSessionId001
     * @tc.desc: object generate random sessionId
     * @tc.type: FUNC
     * @tc.require: I4H3LS
     */
    it('testGenSessionId001', 0, function (done) {
        console.log(TAG + "************* testGenSessionId001 start *************");
        var sessionId = distributedObject.genSessionId();
        expect(sessionId != null && sessionId.length > 0 && typeof (sessionId) == 'string');

        done()
        console.log(TAG + "************* testGenSessionId001 end *************");
    })

    /**
     * @tc.name: testGenSessionId002
     * @tc.desc: object generate 2 random sessionId and not equal
     * @tc.type: FUNC
     * @tc.require: I4H3LS
     */
    it('testGenSessionId002', 0, function (done) {
        console.log(TAG + "************* testGenSessionId001 start *************");
        var sessionId1 = distributedObject.genSessionId();
        var sessionId2 = distributedObject.genSessionId();
        expect(sessionId1 != sessionId2);

        done()
        console.log(TAG + "************* testGenSessionId002 end *************");
    })

    /**
     * @tc.name: testOnStatus001
     * @tc.desc: object set a listener to watch another object online/offline
     * @tc.type: FUNC
     * @tc.require: I4H3M8
     */
    it('testOnStatus001', 0, function (done) {
        console.log(TAG + "************* testOnStatus001 start *************");
        console.log(TAG + "start watch status");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.on("status", statusCallback1);
        console.log(TAG + "watch success");

        done()
        console.log(TAG + "************* testOnStatus001 end *************");
    })

    /**
     * @tc.name: testOnStatus002
     * @tc.desc: object set several listener and can unset specified listener
     * @tc.type: FUNC
     * @tc.require: I4H3M8
     */
    it('testOnStatus002', 0, function (done) {
        console.log(TAG + "************* testOnStatus001 start *************");
        console.log(TAG + "start watch status");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.on("status", statusCallback1);
        g_object.on("status", statusCallback2);
        g_object.on("status", statusCallback3);
        console.log(TAG + "watch success");
        console.log(TAG + "start call unwatch status");
        g_object.off("status", statusCallback1);
        console.log(TAG + "unwatch success");

        done()
        console.log(TAG + "************* testOnStatus002 end *************");
    })

    /**
     * @tc.name: testOnStatus003
     * @tc.desc: object set several listener and can unWatch all watcher
     * @tc.type: FUNC
     * @tc.require: I4H3M8
     */
    it('testOnStatus002', 0, function (done) {
        console.log(TAG + "************* testOnStatus001 start *************");
        console.log(TAG + "start watch status");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.on("status", statusCallback1);
        g_object.on("status", statusCallback2);
        g_object.on("status", statusCallback3);
        console.log(TAG + "watch success");
        console.log(TAG + "start call unwatch status");
        g_object.off("status");
        console.log(TAG + "unwatch success");

        done()
        console.log(TAG + "************* testOnStatus003 end *************");
    })

    /**
     * @tc.name: testComplex001
     * @tc.desc: object can get/set complex data
     * @tc.type: FUNC
     * @tc.require: I4H3M8
     */
    it('testComplex001', 0, function (done) {
        console.log(TAG + "************* testComplex001 start *************");
        var complex_object = distributedObject.createDistributedObject({
            name: undefined,
            age: undefined,
            parent: undefined,
            list: undefined
        });
        complex_object.setSessionId("session12");
        if (complex_object != undefined && complex_object != null) {
            expect("session12" == complex_object.__sessionId);
        } else {
            console.log(TAG + "testOnComplex001 joinSession session12 failed");
        }
        complex_object.name = "jack";
        complex_object.age = 19;
        complex_object.isVis = false;
        complex_object.parent = { mother: "jack mom", father: "jack Dad" };
        complex_object.list = [{ mother: "jack mom" }, { father: "jack Dad" }];
        expect(complex_object.name == "jack");
        expect(complex_object.age == 19);
        expect(complex_object.parent == { mother: "jack1 mom", father: "jack1 Dad" });
        expect(complex_object.list == [{ mother: "jack1 mom", father: "jack1 Dad" }]);

        done()
        console.log(TAG + "************* testComplex001 end *************");
    })

    /**
     * @tc.name: testMaxSize001
     * @tc.desc: object can get/set data under 4MB size
     * @tc.type: FUNC
     * @tc.require: I4H3M8
     */
    it('testMaxSize001', 0, function (done) {
        console.log(TAG + "************* testMaxSize001 start *************");
        var g_object = distributedObject.createDistributedObject({ name: "Amy", age: 18, isVis: false });
        g_object.setSessionId("session13");
        if (g_object != undefined && g_object != null) {
            expect("session13" == g_object.__sessionId);
        } else {
            console.log(TAG + "testMaxSize001 joinSession session13 failed");
        }
        //maxString = 32byte
        var maxString = "12345678123456781234567812345678".repeat(131072);
        if (g_object != undefined && g_object != null) {
            g_object.name = maxString;
            g_object.age = 42;
            g_object.isVis = false;
            expect(g_object.name == maxString);
            console.log(TAG + "get/set maxSize string success");
        } else {
            console.info(TAG + " object is null,set name fail");
        }

        done()
        console.log(TAG + "************* testMaxSize001 end *************");
    })

    /**
     * @tc.name: testPerformance001
     * @tc.desc: performanceTest for set/get data
     * @tc.type: FUNC
     * @tc.require: I4H3M8
     */
    it('testPerformance001', 0, function (done) {
        console.log(TAG + "************* testPerformance001 start *************");
        var complex_object = distributedObject.createDistributedObject({
            name: undefined,
            age: undefined,
            parent: undefined,
            list: undefined
        });
        var st1;
        var totalTime = 0;
        var setSessionIdTime = 0;
        var setTime = 0;
        var onTime = 0;
        var offTime = 0;
        for (var i = 0;i < 100; i++) {
            st1 = Date.now();
            complex_object.setSessionId("session14");
            setSessionIdTime += Date.now() - st1;
            if (complex_object != undefined && complex_object != null) {
                expect("session14" == complex_object.__sessionId);
            } else {
                console.log(TAG + "testPerformance001 joinSession session14 failed");
            }
            console.info(TAG + " start call watch change");
            st1 = Date.now();
            complex_object.on("change", changeCallback);
            onTime += Date.now() - st1;
            console.info(TAG + "on change success");
            st1 = Date.now();
            complex_object.name = "jack2";
            complex_object.age = 20;
            complex_object.isVis = false;
            complex_object.parent = { mother: "jack1 mom1", father: "jack1 Dad1" };
            complex_object.list = [{ mother: "jack1 mom1" }, { father: "jack1 Dad1" }];
            setTime += Date.now() - st1;
            expect(complex_object.name == "jack2");
            expect(complex_object.age == 20);
            expect(complex_object.parent == { mother: "jack1 mom1", father: "jack1 Dad1" });
            expect(complex_object.list == [{ mother: "jack1 mom1", father: "jack1 Dad1" }]);

            console.log(TAG + "start unWatch change");
            st1 = Date.now();
            complex_object.off("change");
            offTime += Date.now() - st1;
            totalTime += setSessionIdTime;
            totalTime += setTime;
            totalTime += onTime;
            totalTime += offTime;
            console.log(TAG + "end unWatch success");
        }
        console.log(TAG + "totalTime = " + (totalTime / 100));
        expect(totalTime < baseLine);
        done()
        console.log(TAG + "************* testPerformance001 end *************");
    })

    console.log(TAG + "*************Unit Test End*************");
})

