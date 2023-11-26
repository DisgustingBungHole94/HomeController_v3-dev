class LoggerImpl {
    private masterPrefix: string;
    private logPrefix: string;
    private errPrefix: string;
    private cshPrefix: string;
    private dbgPrefix: string;

    private debugEnabled: boolean;

    constructor() {
        this.masterPrefix = "[hc]";
        this.logPrefix = "[LOG]   ";
        this.errPrefix = "[ERR]   ";
        this.cshPrefix = "[FATAL] ";
        this.dbgPrefix = "[DEBUG] ";

        this.debugEnabled = false;
    }

    public enableDebug(): void {
        this.debugEnabled = true;
    }

    public disableDebug(): void {
        this.debugEnabled = false;
    }

    public log(msg: string, nl: boolean = false): void {
        this.print(this.logPrefix, msg, nl, '');
    }

    public err(msg: string, nl: boolean = false): void {
        this.print(this.errPrefix, msg, nl, '\x1b[0;93m');
    }

    public csh(msg: string, nl: boolean = false): void {
        this.print(this.cshPrefix, msg, nl, '\x1b[0;91m');
    }

    public dbg(msg: string, nl: boolean = false): void {
        if (this.debugEnabled) {
            this.print(this.dbgPrefix, msg, nl, '\x1b[3;90m');
        }
    }

    private timestamp(): string {
        const currDate = new Date();

        const dateStr: string = (currDate.getMonth() + 1) + '-' + currDate.getDate().toString().padStart(2, '0') + '-' + currDate.getFullYear().toString().substring(2);
        const timeStr: string = currDate.getHours().toString().padStart(2, '0') + ':' + currDate.getMinutes() + ':' + currDate.getSeconds().toString().padStart(2, '0');

        return dateStr + ' ' + timeStr;
    }

    private print(prefix: string, msg: string, nl: boolean, color: string): void {
        let outMsg: string = color + this.masterPrefix + ' ' + this.timestamp() + ' ' + prefix + '\t' + msg;
        
        if (nl) {
            outMsg += '\n';
        }

        outMsg += '\x1b[0m';

        console.log(outMsg);
    }
}

export const Logger = new LoggerImpl();