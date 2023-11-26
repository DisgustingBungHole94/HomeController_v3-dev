export default function errorResponse(message: string, code: number) {
    const json = {
        success: false,
        errorMsg: message,
        errorCode: code
    }

    return json;
}