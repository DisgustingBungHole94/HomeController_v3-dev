import { ClientPacket, Opcode } from '@/deps/hc/client_packet';
import { State } from '@/deps/hc/state';
import { Exception } from '@/deps/hc/util/exception';
import { Device, NodeTicket } from '@/deps/hc/api_requests'

//import { DeviceListState, DeviceList, DeviceState, emptyDeviceList, updateDeviceList } from '@/app/home/contexts/device_context'

type ConnectCallback = (device: Device, state: State) => void;
type DisconnectCallback = (device: Device) => void;
type StateUpdateCallback = (device: Device, state: State) => void;

export interface DeviceInfo {
    device: Device,

    onConnect: ConnectCallback,
    onDisconnect: DisconnectCallback,
    onStateUpdate: StateUpdateCallback,
};

class NodeConnection {
    public onClose: () => void;

    private deviceList: Map<string, DeviceInfo>;

    private host: string;
    private port: string;

    private socket?: WebSocket;

    private messageQueue: Array<(packet: ClientPacket) => void>;

    private connected: boolean;
    private authenticated: boolean;

    constructor(host: string, port: string, deviceList: Map<string, DeviceInfo>) {
        this.onClose = () => {};

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

                const packet = new ClientPacket();
                if (!packet.parse(new Uint8Array(e.data))) {
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

        deviceInfo.onStateUpdate(deviceInfo.device, state);

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

        deviceInfo.onConnect(deviceInfo.device, state);

        return true;
    }

    private handleDisconnect(packet: ClientPacket): boolean {
        let deviceInfo: DeviceInfo | undefined = this.deviceList.get(packet.getDeviceIdAsStr());
        if (!deviceInfo) {
            return false;
        }

        deviceInfo.onDisconnect(deviceInfo.device);
        
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

    private deviceList: Map<string, DeviceInfo>;

    private connections: Map<string, NodeConnection>;
    private connected: boolean;

    constructor() {
        this.onDisconnect = () => {};

        this.deviceList = new Map<string, DeviceInfo>();

        this.connections = new Map<string, NodeConnection>();
        this.connected = false;
    }

    public isConnected(): boolean {
        return this.connected;
    }

    public setDeviceList(devices: Array<Device>) {
        devices.forEach((device) => {
            let deviceState = {
                device: device,
                onConnect: () => void {},
                onDisconnect: () => void {},
                onStateUpdate: () => void {},
            };

            this.deviceList.set(device.id, deviceState);
        });
    }

    public getDeviceList(): Map<string, DeviceInfo> {
        return this.deviceList;
    }

    public async connect(nodes: Array<NodeTicket>) { 
        const onCloseCallback = () => {
            this.connected = false;
            this.onDisconnect();
        };
        
        for(let i = 0; i < nodes.length; i++) {
            if (!this.connections.has(nodes[i].node.id)) {
                const connection = new NodeConnection(nodes[i].node.host, nodes[i].node.port, this.deviceList);
                connection.onClose = onCloseCallback;

                await connection.connect(nodes[i].ticket);
                this.connections.set(nodes[i].node.id, connection);
            }
        }

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

    public setCallbacks(stateUpdateCallback: StateUpdateCallback, connectCallback: ConnectCallback, disconnectCallback: DisconnectCallback) {
        this.deviceList.forEach((deviceState: DeviceInfo) => {
            deviceState.onStateUpdate = stateUpdateCallback;
            deviceState.onConnect = connectCallback;
            deviceState.onDisconnect = disconnectCallback;
        });
    }
}

export const myConnManager = new NodeConnectionManager();