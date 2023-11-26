"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const exception_1 = require("./util/exception");
const token_1 = require("./util/token");
class AuthManager {
    constructor() {
    }
    parseAuthHeader(req) {
        if (!req.headers.authorization) {
            throw new exception_1.default('no auth header', 700);
        }
        return '';
    }
    createSession(userId) {
        let token = (0, token_1.default)(32);
        return token;
    }
    validateSession(token) {
        return '';
    }
    validateNode(secret) {
        return 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC';
    }
}
exports.default = AuthManager;
