"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Logger = void 0;
var LoggerImpl = /** @class */ (function () {
    function LoggerImpl() {
        this.masterPrefix = "[hc]";
        this.logPrefix = "[LOG]   ";
        this.errPrefix = "[ERR]   ";
        this.cshPrefix = "[FATAL] ";
        this.dbgPrefix = "[DEBUG] ";
        this.debugEnabled = false;
    }
    LoggerImpl.prototype.enableDebug = function () {
        this.debugEnabled = true;
    };
    LoggerImpl.prototype.disableDebug = function () {
        this.debugEnabled = false;
    };
    LoggerImpl.prototype.log = function (msg, nl) {
        if (nl === void 0) { nl = false; }
        this.print(this.logPrefix, msg, nl, '');
    };
    LoggerImpl.prototype.err = function (msg, nl) {
        if (nl === void 0) { nl = false; }
        this.print(this.errPrefix, msg, nl, '\x1b[0;93m');
    };
    LoggerImpl.prototype.csh = function (msg, nl) {
        if (nl === void 0) { nl = false; }
        this.print(this.cshPrefix, msg, nl, '\x1b[0;91m');
    };
    LoggerImpl.prototype.dbg = function (msg, nl) {
        if (nl === void 0) { nl = false; }
        if (this.debugEnabled) {
            this.print(this.dbgPrefix, msg, nl, '\x1b[3;90m');
        }
    };
    LoggerImpl.prototype.timestamp = function () {
        var currDate = new Date();
        var dateStr = (currDate.getMonth() + 1) + '-' + currDate.getDate().toString().padStart(2, '0') + '-' + currDate.getFullYear().toString().substring(2);
        var timeStr = (currDate.getHours() % 12).toString().padStart(2, '0') + ':' + currDate.getMinutes() + ':' + currDate.getSeconds().toString().padStart(2, '0');
        return dateStr + ' ' + timeStr;
    };
    LoggerImpl.prototype.print = function (prefix, msg, nl, color) {
        var outMsg = color + this.masterPrefix + ' ' + this.timestamp() + ' ' + prefix + '\t' + msg;
        if (nl) {
            outMsg += '\n';
        }
        outMsg += '\x1b[0m';
        console.log(outMsg);
    };
    return LoggerImpl;
}());
exports.Logger = new LoggerImpl();
