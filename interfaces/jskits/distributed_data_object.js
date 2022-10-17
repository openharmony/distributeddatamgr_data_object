/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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
 
const distributedObject = requireInternal("data.distributedDataObject");
const SESSION_ID = "__sessionId";
const VERSION = "__version";
const COMPLEX_TYPE = "[COMPLEX]";
const STRING_TYPE = "[STRING]";
const NULL_TYPE = "[NULL]"
const JS_ERROR = 1;

class Distributed {
    constructor(obj) {
        this.__proxy = obj;
        Object.keys(obj).forEach(key => {
            Object.defineProperty(this, key, {
                enumerable: true,
                configurable: true,
                get: function () {
                    return this.__proxy[key];
                },
                set: function (newValue) {
                    this[VERSION]++;
                    this.__proxy[key] = newValue;
                }
            });
        });
        Object.defineProperty(this, SESSION_ID, {
            enumerable: true,
            configurable: true,
            get: function () {
                return this.__proxy[SESSION_ID];
            },
            set: function (newValue) {
                this.__proxy[SESSION_ID] = newValue;
            }
        });
        this.__objectId = randomNum();
        this[VERSION] = 8;
        console.info("constructor success ");
    }

    setSessionId(sessionId) {
        if (sessionId == null || sessionId == "") {
            leaveSession(this.__version, this.__proxy);
            return false;
        }
        if (this.__proxy[SESSION_ID] == sessionId) {
            console.info("same session has joined " + sessionId);
            return true;
        }
        leaveSession(this.__version, this.__proxy);
        let object = joinSession(this.__version, this.__proxy, this.__objectId, sessionId);
        if (object != null) {
            this.__proxy = object;
            return true;
        }
        return false;
    }

    on(type, callback) {
        onWatch(this.__version, type, this.__proxy, callback);
        distributedObject.recordCallback(this.__version, type, this.__objectId, callback);
    }

    off(type, callback) {
        offWatch(this.__version, type, this.__proxy, callback);
        if (callback != undefined || callback != null) {
            distributedObject.deleteCallback(this.__version, type, this.__objectId, callback);
        } else {
            distributedObject.deleteCallback(this.__version, type, this.__objectId);
        }
    }

    save(deviceId, callback) {
        if (this.__proxy[SESSION_ID] == null || this.__proxy[SESSION_ID] == "") {
            console.info("not join a session, can not do save");
            return JS_ERROR;
        }
        return this.__proxy.save(deviceId, this[VERSION], callback);
    }

    revokeSave(callback) {
        if (this.__proxy[SESSION_ID] == null || this.__proxy[SESSION_ID] == "") {
            console.info("not join a session, can not do revoke save");
            return JS_ERROR;
        }
        return this.__proxy.revokeSave(callback);
    }

    __proxy;
    __objectId;
    __version;
}

function randomNum() {
    return distributedObject.sequenceNum();
}

function newDistributed(obj) {
    console.info("start newDistributed");
    if (obj == null) {
        console.error("object is null");
        return null;
    }
    return new Distributed(obj);
}

function joinSession(version, obj, objectId, sessionId, context) {
    console.info("start joinSession " + sessionId);
    if (obj == null || sessionId == null || sessionId == "") {
        console.error("object is null");
        return null;
    }

    let object = null;
    if (context != undefined || context != null) {
        object = distributedObject.createObjectSync(version, sessionId, objectId, context);
    } else {
        object = distributedObject.createObjectSync(version, sessionId, objectId);
    }

    if (object == null) {
        console.error("create fail");
        return null;
    }
    Object.keys(obj).forEach(key => {
        console.info("start define " + key);
        Object.defineProperty(object, key, {
            enumerable: true,
            configurable: true,
            get: function () {
                console.info("start get " + key);
                let result = object.get(key);
                console.info("get " + result);
                if (typeof result == "string") {
                    if (result.startsWith(STRING_TYPE)) {
                        result = result.substr(STRING_TYPE.length);
                    } else if (result.startsWith(COMPLEX_TYPE)) {
                        result = JSON.parse(result.substr(COMPLEX_TYPE.length))
                    } else if (result.startsWith(NULL_TYPE)) {
                        result = null;
                    } else {
                        console.error("error type " + result);
                    }
                }
                console.info("get " + result + " success");
                return result;
            },
            set: function (newValue) {
                console.info("start set " + key + " " + newValue);
                if (typeof newValue == "object") {
                    let value = COMPLEX_TYPE + JSON.stringify(newValue);
                    object.put(key, value);
                    console.info("set " + key + " " + value);
                } else if (typeof newValue == "string") {
                    let value = STRING_TYPE + newValue;
                    object.put(key, value);
                    console.info("set " + key + " " + value);
                } else if (newValue === null) {
                    let value = NULL_TYPE;
                    object.put(key, value);
                    console.info("set " + key + " " + value);
                } else {
                    object.put(key, newValue);
                    console.info("set " + key + " " + newValue);
                }
            }
        });
        if (obj[key] != undefined) {
            object[key] = obj[key];
        }
    });

    Object.defineProperty(object, SESSION_ID, {
        value: sessionId,
        configurable: true,
    });
    return object;
}

function leaveSession(version, obj) {
    console.info("start leaveSession");
    if (obj == null || obj[SESSION_ID] == null || obj[SESSION_ID] == "") {
        console.warn("object is null");
        return;
    }
    Object.keys(obj).forEach(key => {
        Object.defineProperty(obj, key, {
            value: obj[key],
            configurable: true,
            writable: true,
            enumerable: true,
        });
    });
    // disconnect,delete object
    distributedObject.destroyObjectSync(version, obj);
    delete obj[SESSION_ID];
}

function onWatch(version, type, obj, callback) {
    console.info("start on " + obj[SESSION_ID]);
    if (obj[SESSION_ID] != null && obj[SESSION_ID] != undefined && obj[SESSION_ID].length > 0) {
        distributedObject.on(version, type, obj, callback);
    }
}

function offWatch(version, type, obj, callback = undefined) {
    console.info("start off " + obj[SESSION_ID] + " " + callback);
    if (obj[SESSION_ID] != null && obj[SESSION_ID] != undefined && obj[SESSION_ID].length > 0) {
        if (callback != undefined || callback != null) {
            distributedObject.off(version, type, obj, callback);
        } else {
            distributedObject.off(version, type, obj);
        }

    }
}

function newDistributedV9(obj, context) {
    console.info("start newDistributed");
    if (obj == null) {
        console.error("object is null");
        return null;
    }
    return new DistributedV9(obj, context);
}

class DistributedV9 {

    constructor(obj, context) {
        this.__context = context;
        this.__proxy = obj;
        Object.keys(obj).forEach(key => {
            Object.defineProperty(this, key, {
                enumerable: true,
                configurable: true,
                get: function () {
                    return this.__proxy[key];
                },
                set: function (newValue) {
                    this[VERSION]++;
                    this.__proxy[key] = newValue;
                }
            });
        });
        Object.defineProperty(this, SESSION_ID, {
            enumerable: true,
            configurable: true,
            get: function () {
                return this.__proxy[SESSION_ID];
            },
            set: function (newValue) {
                this.__proxy[SESSION_ID] = newValue;
            }
        });
        this.__objectId = randomNum();
        this[VERSION] = 9;
        console.info("constructor success ");
    }

    setSessionId(sessionId, callback) {
        if (sessionId == null || sessionId == "") {
            leaveSession(this.__version, this.__proxy);
            if (typeof callback == "function") {
                callback()
            } else {
                return Promise.reject();
            }
        }
        if (this.__proxy[SESSION_ID] == sessionId) {
            console.info("same session has joined " + sessionId);
            if (typeof callback == "function") {
                callback()
            } else {
                return Promise.resolve();
            }
        }
        leaveSession(this.__version, this.__proxy);
        let object = joinSession(this.__version, this.__proxy, this.__objectId, sessionId);
        if (object != null) {
            this.__proxy = object;
            if (typeof callback == "function") {
                callback()
            } else {
                return Promise.resolve();
            }
        } else {
            if (typeof callback == "function") {
                callback()
            } else {
                return Promise.reject();
            }
        }

    }

    on(type, callback) {
        onWatch(this.__version, type, this.__proxy, callback);
        distributedObject.recordCallback(this.__version, type, this.__objectId, callback);
    }

    off(type, callback) {
        offWatch(this.__version, type, this.__proxy, callback);
        if (callback != undefined || callback != null) {
            distributedObject.deleteCallback(this.__version, type, this.__objectId, callback);
        } else {
            distributedObject.deleteCallback(this.__version, type, this.__objectId);
        }
    }

    save(deviceId, callback) {
        if (this.__proxy[SESSION_ID] == null || this.__proxy[SESSION_ID] == "") {
            console.info("not join a session, can not do save");
            return JS_ERROR;
        }
        return this.__proxy.save(deviceId, this[VERSION], callback);
    }

    revokeSave(callback) {
        if (this.__proxy[SESSION_ID] == null || this.__proxy[SESSION_ID] == "") {
            console.info("not join a session, can not do revoke save");
            return JS_ERROR;
        }
        return this.__proxy.revokeSave(this.__version, callback);
    }

    __context;
    __proxy;
    __objectId;
    __version;
}

export default {
    createDistributedObject: newDistributed,
    create: newDistributedV9,
    genSessionId: randomNum
}