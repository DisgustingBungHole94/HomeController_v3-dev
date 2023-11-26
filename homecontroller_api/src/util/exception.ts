export default class Exception {
    private errorMessage: string;
    private errorCode: number;

    constructor(errorMessage: string, errorCode: number) {
        this.errorMessage = errorMessage;
        this.errorCode = errorCode;
    }

    public what() {
        return this.what;
    }

    public getErrorCode() {
        return this.errorCode;
    }
}