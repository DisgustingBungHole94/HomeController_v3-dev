import ClientPacket from './client_packet';
import { opcode } from './client_packet';
import { nodeConnect } from './api_requests.js'

import Cookies from 'js-cookie';

class NodeConnection {
    constructor(host, port, setStateCallback, disconnectCallback) {
        this.host = host;
        this.port = port;
        this.socket = null;

        this.messageQueue = [];

        this.connected = false;
        this.authenticated = false;

        this.setStateCallback = setStateCallback;
        this.disconnectCallback = disconnectCallback;
    }

    async connect() {
        return new Promise(async (resolve, reject) => {
            if (this.connected) {
                reject();
                return;
            }

            const token = Cookies.get('token');
            if (!token) {
                reject();
                return;
            }

            let ticket = '';

            try {
                const response = await nodeConnect(this.host, this.port, token);
                ticket = response.ticket;
            } catch(err) {
                reject();
                return;
            }

            setTimeout(() => {
                if (!this.authenticated) {
                    if (this.socket.readyState === WebSocket.OPEN) {
                        this.socket.close();
                    }

                    reject();
                }
            }, 5000);

            this.socket = new WebSocket('wss://' + this.host + ':' + this.port);
            this.socket.binaryType = 'arraybuffer';

            this.messageQueue = [];

            this.connected = true;
            this.authenticated = false;
            
            this.socket.onopen = () => {
                const packet = new ClientPacket();
                packet.setOpcode(opcode.AUTHENTICATE);
                packet.setMessageId(0x00000000);
                packet.blankDeviceId();
                packet.setData(ticket);

                this.socket.send(packet.serialize());
            };

            this.socket.onmessage = (e) => {
                if (!(e.data instanceof ArrayBuffer)) {
                    //console.log('received invalid message from device');
                    return;
                }

                const packet = new ClientPacket();
                try {
                    packet.parse(new Uint8Array(e.data));
                } catch(err) {
                    //console.log('failed to parse message: ' + err);
                    return;
                }

                if (!this.authenticated && packet.getOpcode() !== opcode.NOTIFICATION) {
                    if (packet.getOpcode() === opcode.AUTHENTICATE && !this.authenticated) {
                        this.authenticated = true;
                        resolve();
                    } else {
                        this.socket.close();
                        reject();
                    }
                } 
                
                else if (packet.getOpcode() === opcode.NOTIFICATION) {
                    if (packet.getData().length < 1) {
                        return;
                    }

                    this.setStateCallback((devices) => {
                        let newDevice = devices.list.get(packet.getDeviceId());

                        switch(packet.getData()[0]) {
                            case 0x00: // disconnected
                                newDevice['online'] = false;
                                newDevice['power'] = false;
                                break;
                            case 0x01: // connected, on
                                newDevice['online'] = true;
                                newDevice['power'] = true;
                                break;
                            case 0x02: // connected, off
                                newDevice['online'] = true;
                                newDevice['power'] = false;
                                break;
                        }
                        
                        return { list: devices.list.set(packet.getDeviceId(), newDevice) };
                    });
                } 
                
                else {
                    let nextCallback = this.messageQueue.shift();
                    nextCallback(packet);
                }
            };

            this.socket.onerror = () => {
                if (this.socket.readyState === WebSocket.CLOSED) {
                    this.connected = false;
                }

                if (!this.authenticated) {
                    reject();
                }
            }

            this.socket.onclose = () => {
                this.connected = false;

                if (!this.authenticated) {
                    reject();
                } else {
                    this.disconnectCallback();
                }
            };

        });
    }

    async send(packet, responseCallback, errorCallback) {
        try {
            if (!this.connected) {
                await this.connect();
            }

            this.messageQueue.push(responseCallback);
            this.socket.send(packet.serialize());
        } catch(err) {
            errorCallback();
        }
    }

    close() {
        if (!connected) {
            throw 'not connected';
        }

        this.socket.close();
        this.connected = false;
    }
};

class ConnectionManager {
    constructor() {
        this.nodes = null;
        this.connections = new Map();

        this.devices = [];
        this.setStateCallback = null;
        this.disconnectCallback = null;
    }

    enableDeviceList(devices, setStateCallback, disconnectCallback) {
        this.setStateCallback = setStateCallback;
        this.disconnectCallback = disconnectCallback;

        let deviceMap = new Map();

        for (let i = 0; i < devices.length; i++) {
            devices[i]['online'] = false;
            devices[i]['power'] = false;
            deviceMap.set(devices[i].id, devices[i]);
        }

        this.setStateCallback({ list: deviceMap });
    }

    async connect(nodes) {
        for (let i = 0; i < nodes.length; i++) {
            if (!this.connections.has(nodes[i].id)) {
                const connection = new NodeConnection(nodes[i].host, nodes[i].port, this.setStateCallback, this.disconnectCallback);
                await connection.connect();    
                this.connections[nodes[i].id] = connection;            
            }
        }

        this.nodes = nodes;
    }

    send(nodeId, packet, responseCallback, errorCallback) {
        const nodeConn = this.connections[nodeId];
        if (!nodeConn) {
            throw 'bad node id';
        }

        nodeConn.send(packet, responseCallback, errorCallback);
    }

    getNodes() {
        return this.nodes;
    }
};

export const myConnManager = new ConnectionManager();