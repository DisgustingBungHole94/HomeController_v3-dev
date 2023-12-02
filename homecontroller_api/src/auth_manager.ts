import Exception from './util/exception';
import generateToken from './util/token';

export enum AuthType {
    CLIENT, NODE
};

export default class AuthManager {
    constructor() {

    }

    public parseAuthHeader(request: any, expectedType: AuthType): string | undefined {
        if (!request.headers.authorization) {
            return null;
            //throw new Exception('no auth header', 700);
        }

        return 'abc123';
    }

    public createSession(userId: string): string {
        let token: string = generateToken(32);

        return token;
    }

    public validateSession(token: string): string | undefined {
        return 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA';
    }

    public validateNode(secret: string): string | undefined {
        return 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC';
    }
}