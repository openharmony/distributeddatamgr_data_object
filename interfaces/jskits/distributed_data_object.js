const distributedObject = requireInternal("data.distributedDataObject");

function joinSession(obj, sessionId) {
    console.info("start joinSession " + sessionId);
    if (obj == null) {
        console.error("object is null");
        return obj;
    }

    let object = distributedObject.createObjectSync(sessionId);
    if (object == null || object == undefined) {
        console.error("create fail");
        return obj;
    }
    Object.keys(obj).forEach(key => {
        console.info("start define " + key);
        Object.defineProperty(object, key, {
            enumerable : true,
            configurable : true,
            get: function () {
                return object.get(key);
            },
            set: function (newValue) {
                console.info("hanlu start set " + key + " " + newValue );
                object.put(key, newValue);
                console.info("hanlu end set " + key + " " + newValue );
            }
        });
        object[key] = obj[key];
    });

    Object.defineProperty(object, "sessionId", {
        value:sessionId,
        configurable:true,
    });
    return object;
}

function leaveSession(obj) {
    console.info("start leaveSession");
    if (obj == null) {
        console.error("object is null");
        return obj;
    }
    // disconnect,delete object
    distributedObject.destroyObjectSync(obj.sessionId);
    Object.keys(obj).forEach(key => {
        Object.defineProperty(obj, key, {
            value:obj[key],
            configurable:true,
            writable:true,
            enumerable:true,
        });
    });
    delete obj.sessionId;
    return obj;
}

function on(type, obj, callback) {
    console.info("start on " + obj.sessionId);
    if (obj.sessionId != null && obj.sessionId != undefined && obj.sessionId.length > 0) {
        distributedObject.on(type, obj, callback);
    }
}

function off(type, obj, callback = undefined) {
    console.info("start off " + obj.sessionId);
    if (obj.sessionId != null && obj.sessionId != undefined && obj.sessionId.length > 0) {
        distributedObject.off(type, obj, callback);
    }
}
export default {
    joinSession: joinSession,
    leaveSession: leaveSession,
    on: on,
    off: off,
}