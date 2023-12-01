export class Exception {
    private errorMessage: string;
    private funcName: string;

    constructor(errorMessage: string, funcName: string) {
        this.errorMessage = errorMessage;
        this.funcName = funcName;
    }

    public what() {
        return this.what;
    }

    public func() {
        return this.funcName;
    }
}

