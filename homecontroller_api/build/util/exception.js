"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
class Exception {
    constructor(errorMessage, errorCode) {
        this.errorMessage = errorMessage;
        this.errorCode = errorCode;
    }
    what() {
        return this.what;
    }
    getErrorCode() {
        return this.errorCode;
    }
}
exports.default = Exception;
