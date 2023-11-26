"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.errorResponse = void 0;
function errorResponse(message, code) {
    const json = {
        success: false,
        errorMsg: message,
        errorCode: code
    };
    return json;
}
exports.errorResponse = errorResponse;
