import Exception from './util/exception';
import generateToken from './util/token';

export default class AuthManager {
    constructor() {

    }

    public parseAuthHeader(req: any): string {
        if (!req.headers.authorization) {
            throw new Exception('no auth header', 700);
        }

        return '';
    }

    public createSession(userId: string): string {
        let token: string = generateToken(32);

        return token;
    }

    public validateSession(token: string): string {
        return '';
    }

    public validateNode(secret: string): string {
        return 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC';
    }
}