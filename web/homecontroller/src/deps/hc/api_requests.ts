import { Exception } from "./util/exception";

const apiHost: string = 'localhost';
const apiPort: string = '4435';

interface RequestData {
    url: string,
    method: string,
    token?: string,
    data?: Object
};

export async function makeRequest(host: string, port: string, requestData: RequestData): Promise<any> {
    return new Promise((resolve, reject) => {
        let headers = new Headers();
        let body = null;

        if (requestData.token) {
            headers.set('Authorization', 'HCAuth Type=Client Token=' + requestData.token);
        }

        if (requestData.data) {
            headers.set('Content-Type', 'application/json');
            body = JSON.stringify(requestData.data);
        }

        let fetchData: RequestInit = {
            method: requestData.method,
            headers: headers,
            body: body
        };

        fetch('https://' + host + ':' + port + requestData.url, fetchData)
        .then((res) => {
            resolve(res.json());
        })
        .catch(() => {
            reject(new Exception('failed to connect to api', 'makeRequest'));
        });
    });
}

export interface Node {
    id: string,
    host: string,
    port: string
};

export interface NodeTicket {
    node: Node,
    ticket: string
}

export interface Device {
    id: string,
    nodeId: string,
    type: string,
    name: string,
    note: string
}

export interface LoginUserResponse {
    success: boolean,
    userId: string,
    username: string,
    nodes: Array<NodeTicket>,
    devices: Array<Device>,
    token: string
};

function isLoginUserResponse(obj: any): boolean {
    if (typeof obj.success !== 'boolean') return false;
    if (typeof obj.userId !== 'string') return false;
    if (typeof obj.username !== 'string') return false;

    if (!Array.isArray(obj.nodes)) return false;
    obj.nodes.forEach((elem: any) => {
        if (typeof elem.node !== 'object') return false;
        if (typeof elem.node.id !== 'string') return false;
        if (typeof elem.node.host !== 'string') return false;
        if (typeof elem.node.port !== 'string') return false;
        if (typeof elem.ticket !== 'string') return false;
    });

    if (!Array.isArray(obj.devices)) return false;
    obj.devices.forEach((elem: any) => {
        if (typeof elem.id !== 'string') return false;
        if (typeof elem.nodeId !== 'string') return false;
        if (typeof elem.type !== 'string') return false;
        if (typeof elem.name !== 'string') return false;
        if (typeof elem.note !== 'string') return false;
    });

    if (typeof obj.token !== 'string') return false;

    return true;
}

export async function loginUser(username: string, password: string): Promise<LoginUserResponse> {
    return new Promise<LoginUserResponse>(async (resolve, reject) => {
        const requestData: RequestData = {
            url: '/login/user',
            method: 'post',
            data: {
                username: username,
                password: password
            }
        };
        
        makeRequest(apiHost, apiPort, requestData)
            .then((response) => {
                if (!isLoginUserResponse(response)) {
                    reject(new Exception('bad server response', 'loginUser'));
                }

                let responseData: LoginUserResponse = response as LoginUserResponse;
                if (!responseData.success) {
                    reject(new Exception('request failed', 'loginUser'));
                }

                resolve(responseData);
            })
            .catch(() => {
                reject(new Exception('login request failed', 'loginUser'));
            })
    });
}

export interface ReconnectUserResponse {
    success: boolean,
    nodes: Array<NodeTicket>,
    devices: Array<Device>
};

function isReconnectUserResponse(obj: any): boolean {
    if (typeof obj.success !== 'boolean') return false;

    if (!Array.isArray(obj.nodes)) return false;
    obj.nodes.forEach((elem: any) => {
        if (typeof elem.node !== 'object') return false;
        if (typeof elem.node.id !== 'string') return false;
        if (typeof elem.node.host !== 'string') return false;
        if (typeof elem.node.port !== 'string') return false;
        if (typeof elem.ticket !== 'string') return false;
    });

    if (!Array.isArray(obj.devices)) return false;
    obj.devices.forEach((elem: any) => {
        if (typeof elem.id !== 'string') return false;
        if (typeof elem.nodeId !== 'string') return false;
        if (typeof elem.type !== 'string') return false;
        if (typeof elem.name !== 'string') return false;
        if (typeof elem.note !== 'string') return false;
    });

    return true;
}

export async function reconnectUser(token: string): Promise<ReconnectUserResponse> {
    return new Promise<ReconnectUserResponse>(async (resolve, reject) => {
        const requestData: RequestData = {
            url: '/reconnect/user',
            method: 'post',
            data: {
                token: token
            }
        };
        
        makeRequest(apiHost, apiPort, requestData)
            .then((response) => {
                if (!isReconnectUserResponse(response)) {
                    reject(new Exception('bad server response', 'reconnectUser'));
                }

                let responseData: ReconnectUserResponse = response as ReconnectUserResponse;
                if (!responseData.success) {
                    reject(new Exception('request failed', 'reconnectUser'));
                }

                resolve(responseData);
            })
            .catch(() => {
                reject(new Exception('reconnect request failed', 'reconnectUser'));
            })
    });
}