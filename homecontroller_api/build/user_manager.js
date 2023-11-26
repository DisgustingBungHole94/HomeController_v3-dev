"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const exception_1 = require("./util/exception");
;
class UserManager {
    constructor() {
    }
    createUser(username, password) {
    }
    getUser(username, password) {
        if (username !== 'test' && password !== '1234') {
            throw new exception_1.default('bad login credentials', 100);
        }
        return this.getUserInfo('AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA');
    }
    addDevice(userId, type, name, note) {
    }
    getUserInfo(userId) {
        if (userId !== 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA') {
            throw new exception_1.default('bad user id', 300);
        }
        let node = {
            id: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC',
            host: 'localhost',
            port: '4434'
        };
        let device = {
            id: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAB',
            nodeId: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAC',
            type: 'test_device',
            name: 'Test Device',
            note: 'A nice test device'
        };
        let user = {
            id: 'AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA',
            username: 'TestUser',
            nodes: new Map(),
            devices: new Map()
        };
        user.nodes.set(node.id, node);
        user.devices.set(device.id, device);
        return user;
    }
}
exports.default = UserManager;
