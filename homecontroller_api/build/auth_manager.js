"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.AuthType = void 0;
const token_1 = require("./util/token");
var AuthType;
(function (AuthType) {
    AuthType[AuthType["CLIENT"] = 0] = "CLIENT";
    AuthType[AuthType["NODE"] = 1] = "NODE";
})(AuthType || (exports.AuthType = AuthType = {}));
;
class AuthManager {
    constructor() {
    }
    parseAuthHeader(request) {
        if (!request.headers.authorization) {
            return null;
            //throw new Exception('no auth header', 700);
        }
        return 'abc123';
    }
    createSession(userId) {
        let token = (0, token_1.default)(32);
        return token;
    }
    validateSession(token) {
        return 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA';
    }
    validateNode(secret) {
        return 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC';
    }
}
exports.default = AuthManager;
