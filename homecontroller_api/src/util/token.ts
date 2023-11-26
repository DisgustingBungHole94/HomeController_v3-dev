export default function generateToken(length: number): string {
    const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789';

    let token: string = '';

    for (let i = 0; i < length; i++) {
        token += chars.charAt(Math.floor(Math.random() * length));
    }

    return token;
}