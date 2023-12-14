import { ClientPacket, Opcode } from '@/deps/hc/client_packet';
import { State } from '@/deps/hc/state';
import { Device, NodeTicket } from '@/deps/hc/api_requests'

import { Dispatch, SetStateAction } from 'react';

type DeviceChangedCallback = (deviceInfo: DeviceInfo) => void;
type DeviceStateUpdateCallback = (device: Device, state: State) => void;

export interface DeviceList {
    onlineDevices: Map<string, DeviceInfo>,
    offlineDevices: Map<string, Device>,
    loading: boolean
}

export function emptyDeviceList(): DeviceList {
    let deviceList: DeviceList = {
        onlineDevices: new Map<string, DeviceInfo>(),
        offlineDevices: new Map<string, Device>(),
        loading: true
    };
    
    return deviceList;
}

export interface DeviceInfo {
    device: Device,
    lastState: State,
    onStateUpdateCallbacks: Map<string, DeviceStateUpdateCallback>
};

class NodeConnection {
    public onClose: () => void;
    public onDeviceConnect: DeviceChangedCallback;
    public onDeviceDisconnect: DeviceChangedCallback;
    public onDeviceStateUpdate: DeviceChangedCallback;

    private deviceList: Map<string, DeviceInfo>;

    private host: string;
    private port: string;

    private socket?: WebSocket;

    private messageQueue: Array<(packet: ClientPacket) => void>;

    private connected: boolean;
    private authenticated: boolean;

    constructor(host: string, port: string, deviceList: Map<string, DeviceInfo>) {
        this.onClose = () => void {};
        this.onDeviceConnect = (deviceInfo: DeviceInfo) => void {};
        this.onDeviceDisconnect = (deviceInfo: DeviceInfo) => void {};
        this.onDeviceStateUpdate = (deviceInfo: DeviceInfo) => void {};

        this.deviceList = deviceList;
        
        this.host = host;
        this.port = port;

        this.messageQueue = new Array<(packet: ClientPacket) => void>;

        this.connected = false;
        this.authenticated = false;
    }

    public async connect(ticket: string): Promise<void> {
        return new Promise((resolve, reject) => {
            this.socket = new WebSocket('wss://' + this.host + ':' + this.port);
            this.socket.binaryType = 'arraybuffer';
    
            this.connected = true;

            this.socket.onopen = () => {
                const authPacket = new ClientPacket();
                authPacket.setMessageId(0x00000000);
                authPacket.setOpcode(Opcode.AUTHENTICATE);
                authPacket.blankDeviceId();
                authPacket.setDataFromStr(ticket);

                this.socket?.send(authPacket.serialize());
            };
    
            this.socket.onmessage = (e) => {
                if (!(e.data instanceof ArrayBuffer)) {
                    return;
                }

                const data: Uint8Array = new Uint8Array(e.data);

                const packet = new ClientPacket();
                if (!packet.parse(data)) {
                    return;
                }

                switch(packet.getOpcode()) {
                    case Opcode.AUTHENTICATE:
                        if (!this.authenticated) {
                            this.authenticated = true;
                            resolve();
                        } else {
                            return;
                        }
                        break;
                    case Opcode.NOTIFICATION:
                        this.handleNotification(packet);
                        break;
                    case Opcode.CONNECT:
                        this.handleConnect(packet);
                        break;
                    case Opcode.DISCONNECT:
                        this.handleDisconnect(packet);
                        break;
                    default:
                        if (!this.authenticated) {
                            reject();
                        }
                        this.handleRequest(packet);
                        break;
                }
            }
    
            this.socket.onerror = () => {
                if (this.socket?.readyState === WebSocket.CLOSED) {
                    this.connected = false;
                }

                if (!this.authenticated) {
                    reject();
                } else {
                    this.onClose();
                }
            }
    
            this.socket.onclose = () => {
                if (!this.authenticated) {
                    reject();
                } else {
                    if (this.connected) {
                        this.onClose();
                        this.connected = false;
                    }
                }
            }

            setTimeout(() => {
                if (!this.authenticated) {
                    if (this.socket?.readyState === WebSocket.OPEN) {
                        this.socket.close();
                    }

                    reject();
                }
            }, 5000);
        });
    }

    private handleNotification(packet: ClientPacket): boolean {    
        const state = new State();
        if (!state.parse(packet.getData())) {
            return false;
        }

        // do stuff with state
        let deviceInfo: DeviceInfo | undefined = this.deviceList.get(packet.getDeviceIdAsStr());
        if (!deviceInfo) {
            return false;
        }

        deviceInfo.lastState = state;
        this.onDeviceStateUpdate(deviceInfo);
        //deviceInfo.onStateUpdate(deviceInfo.device, state);

        return true;
    }

    private handleConnect(packet: ClientPacket): boolean {
        const state = new State();
        if (!state.parse(packet.getData())) {
            return false;
        }

        // do stuff with state
        let deviceInfo: DeviceInfo | undefined = this.deviceList.get(packet.getDeviceIdAsStr());
        if (!deviceInfo) {
            return false;
        }

        deviceInfo.lastState = state;
        this.onDeviceConnect(deviceInfo);
        //deviceInfo.onConnect(deviceInfo.device, state);

        return true;
    }

    private handleDisconnect(packet: ClientPacket): boolean {
        let deviceInfo: DeviceInfo | undefined = this.deviceList.get(packet.getDeviceIdAsStr());
        if (!deviceInfo) {
            return false;
        }

        this.onDeviceDisconnect(deviceInfo);
        //deviceInfo.onDisconnect(deviceInfo.device);
        
        return true;
    }

    private handleRequest(packet: ClientPacket): boolean {
        if (this.messageQueue.length > 0) {
            let nextCallback: (packet: ClientPacket) => void = this.messageQueue[0];
            this.messageQueue.shift();

            nextCallback(packet);
        }

        return true;
    }

    public async send(packet: ClientPacket): Promise<ClientPacket> {
        return new Promise((resolve, reject) => {
            if (!this.connected) {
                reject();
            }

            this.messageQueue.push((packet) => {
                resolve(packet);
            });

            try {
                this.socket?.send(packet.serialize());
            } catch(err) {
                reject();
            }
        })
    }

    public close(): boolean {
        if (!this.connected) {
            return false;
            //throw new Exception('not connected', 'NodeConnection::close');
        }

        this.connected = false;
        this.socket?.close();
        return true;
    }
}
class NodeConnectionManager {
    public onDisconnect: () => void;

    public devicesState: [DeviceList, Dispatch<SetStateAction<DeviceList>>] | null;

    private deviceList: Map<string, DeviceInfo>;

    private connections: Map<string, NodeConnection>;
    private connected: boolean;

    constructor() {
        this.onDisconnect = () => {};

        this.devicesState = null;

        this.deviceList = new Map<string, DeviceInfo>();

        this.connections = new Map<string, NodeConnection>();
        this.connected = false;
    }

    public isConnected(): boolean {
        return this.connected;
    }

    public setDeviceList(devices: Array<Device>) {
        if (!this.devicesState) {
            return;
        }

        devices.forEach((device) => {
            let deviceState = {
                device: device,
                lastState: new State(),
                onStateUpdateCallbacks: new Map<string, DeviceStateUpdateCallback>()
            };

            this.deviceList.set(device.id, deviceState);
            this.devicesState![0].offlineDevices.set(device.id, device);
        });

        this.updateState();
    }

    public getDeviceList(): Map<string, DeviceInfo> {
        return this.deviceList;
    }

    public async connect(nodes: Array<NodeTicket>) { 
        if (!this.devicesState) {
            return;
        }

        const onClose = () => {
            this.connected = false;
            this.onDisconnect();
        };

        const onDeviceConnect = (deviceInfo: DeviceInfo) => {                
            this.devicesState![0].offlineDevices.delete(deviceInfo.device.id);
            this.devicesState![0].onlineDevices.set(deviceInfo.device.id, deviceInfo);
            this.updateState();
        };

        const onDeviceDisconnect = (deviceInfo: DeviceInfo) => {                
            this.devicesState![0].onlineDevices.delete(deviceInfo.device.id);
            this.devicesState![0].offlineDevices.set(deviceInfo.device.id, deviceInfo.device);
            this.updateState();
        };

        const onDeviceStateUpdate = (deviceInfo: DeviceInfo) => {
            deviceInfo.onStateUpdateCallbacks.forEach((callback) => {
                callback(deviceInfo.device, deviceInfo.lastState);
            });
        };
        
        for(let i = 0; i < nodes.length; i++) {
            if (!this.connections.has(nodes[i].node.id)) {
                const connection = new NodeConnection(nodes[i].node.host, nodes[i].node.port, this.deviceList);
                connection.onClose = onClose;
                connection.onDeviceConnect = onDeviceConnect;
                connection.onDeviceDisconnect = onDeviceDisconnect;
                connection.onDeviceStateUpdate = onDeviceStateUpdate;

                await connection.connect(nodes[i].ticket);
                this.connections.set(nodes[i].node.id, connection);
            }
        }

        this.devicesState[0].loading = false;
        this.updateState();

        this.connected = true;
    }

    public async send(nodeId: string, packet: ClientPacket): Promise<ClientPacket> {
        return new Promise((resolve, reject) => {
            const connection = this.connections.get(nodeId);
            if (!connection) {
                reject();
            }

            connection!.send(packet)
                .then((packet) => resolve(packet))
                .catch(() => reject());
        });
    }

    public clear() {
        this.connections.forEach((connection) => {
            connection.close();
        });
        this.connections.clear();

        this.connected = false;
    }

    public addCallback(deviceId: string, callbackId: string, callback: DeviceStateUpdateCallback, initialCall: boolean = true): State | null {
        const device = this.deviceList.get(deviceId);
        if (!device) {
            return null;
        }

        device.onStateUpdateCallbacks.set(callbackId, callback);

        if (initialCall) {
            callback(device.device, device.lastState);
        }

        return device.lastState;
    }

    public removeCallback(deviceId: string, callbackId: string) {
        const device = this.deviceList.get(deviceId);
        if (!device) {
            return;
        }

        device.onStateUpdateCallbacks.delete(callbackId);
    }

    public useState(devicesState: [DeviceList, Dispatch<SetStateAction<DeviceList>>]) {
        this.devicesState = devicesState;
    }

    private updateState() {
        if (!this.devicesState) {
            return;
        }

        let newList: DeviceList = {
            onlineDevices: this.devicesState[0].onlineDevices,
            offlineDevices: this.devicesState[0].offlineDevices,
            loading: this.devicesState[0].loading
        }

        this.devicesState[1](newList);
    }
}

export const myConnManager = new NodeConnectionManager();