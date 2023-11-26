import Exception from './util/exception';

export interface Device {
    id: string,
    nodeId: string,
    type: string,
    name: string,
    note: string
};

export interface Node {
    id: string,
    host: string,
    port: string
}

export interface User {
    id: string,
    username: string,
    nodes: Map<string, Node>
    devices: Map<string, Device>
}

export default class UserManager {
    constructor() {

    }

    public createUser(username: string, password: string): void {

    }

    public getUser(username: string, password: string): User {
        if (username !== 'test' && password !== '1234') {
            throw new Exception('bad login credentials', 100);
        }

        return this.getUserInfo('AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA');
    }

    public addDevice(userId: string, type: string, name: string, note: string): void {

    }

    public getUserInfo(userId: string): User {
        if (userId !== 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA') {
            throw new Exception('bad user id', 300);
        }

        let node: Node = {
            id: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC',
            host: 'localhost',
            port: '4434'
        }

        let device: Device = {
            id: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB',
            nodeId: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC',
            type: 'test_device',
            name: 'Test Device',
            note: 'A nice test device'
        }

        let user: User = {
            id: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA',
            username: 'TestUser',
            nodes: new Map<string, Node>(),
            devices: new Map<string, Device>()
        }

        user.nodes.set(node.id, node);
        user.devices.set(device.id, device);

        return user;
    }
}