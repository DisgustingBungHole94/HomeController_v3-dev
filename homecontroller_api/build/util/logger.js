"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
exports.Logger = void 0;
class LoggerImpl {
    constructor() {
        this.masterPrefix = "[hc]";
        this.logPrefix = "[LOG]   ";
        this.errPrefix = "[ERR]   ";
        this.cshPrefix = "[FATAL] ";
        this.dbgPrefix = "[DEBUG] ";
        this.debugEnabled = false;
    }
    enableDebug() {
        this.debugEnabled = true;
    }
    disableDebug() {
        this.debugEnabled = false;
    }
    log(msg, nl = false) {
        this.print(this.logPrefix, msg, nl, '');
    }
    err(msg, nl = false) {
        this.print(this.errPrefix, msg, nl, '\x1b[0;93m');
    }
    csh(msg, nl = false) {
        this.print(this.cshPrefix, msg, nl, '\x1b[0;91m');
    }
    dbg(msg, nl = false) {
        if (this.debugEnabled) {
            this.print(this.dbgPrefix, msg, nl, '\x1b[3;90m');
        }
    }
    timestamp() {
        const currDate = new Date();
        const dateStr = (currDate.getMonth() + 1) + '-' + currDate.getDate().toString().padStart(2, '0') + '-' + currDate.getFullYear().toString().substring(2);
        const timeStr = currDate.getHours().toString().padStart(2, '0') + ':' + currDate.getMinutes() + ':' + currDate.getSeconds().toString().padStart(2, '0');
        return dateStr + ' ' + timeStr;
    }
    print(prefix, msg, nl, color) {
        let outMsg = color + this.masterPrefix + ' ' + this.timestamp() + ' ' + prefix + '\t' + msg;
        if (nl) {
            outMsg += '\n';
        }
        outMsg += '\x1b[0m';
        console.log(outMsg);
    }
}
exports.Logger = new LoggerImpl();
