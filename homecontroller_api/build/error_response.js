"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
function errorResponse(message, code) {
    const json = {
        success: false,
        errorMsg: message,
        errorCode: code
    };
    return json;
}
exports.default = errorResponse;
