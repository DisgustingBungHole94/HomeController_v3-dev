import { ClientPacket, Opcode } from '@/deps/hc/client_packet';
import { State, StateType } from '@/deps/hc/state';
import { Exception } from '@/deps/hc/util/exception';
import { Device, NodeTicket } from '@/deps/hc/api_requests'

import { DeviceListState, DeviceState } from '@/app/home/contexts/device_context'

import { useState, Dispatch, SetStateAction } from 'react';


class NodeConnection {
    public state: DeviceListState;
    public onClose: () => void;

    private host: string;
    private port: string;

    private socket?: WebSocket;

    private messageQueue: Array<(packet: ClientPacket) => void>;

    private connected: boolean;
    private authenticated: boolean;

    constructor(host: string, port: string, state: DeviceListState) {
        this.state = state;
        this.onClose = () => {};
        
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
                this.connected = false;

                if (!this.authenticated) {
                    reject();
                } else {
                    this.onClose();
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
        let deviceList: Map<string, DeviceState> = this.state[0];

        switch(state.getType()) {
            case StateType.DATA:
                let updateList = new Map(deviceList);
                let updateState = deviceList.get(packet.getDeviceIdAsStr());
                if (updateState) {
                    updateState.state = state;
                    updateList.set(updateState.device.id, updateState);
                    this.state[1](updateList);
                }
                break;
            case StateType.DISCONNECT:
                break;
        }

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

    private connections: Map<string, NodeConnection>;
    private connected: boolean;

    constructor() {
        this.onDisconnect = () => {};

        this.connections = new Map<string, NodeConnection>();
        this.connected = false;
    }

    public isConnected(): boolean {
        return this.connected;
;    }

    public async connect(nodes: Array<NodeTicket>, devices: Array<Device>, state: DeviceListState) {
        let deviceMap = new Map<string, DeviceState>();
        devices.forEach((device) => {
            let deviceState: DeviceState = {
                device: device,
                state: new State()
            };
            deviceState.state.setType(StateType.DISCONNECT);

            deviceMap.set(device.id, deviceState);
        });
        
        for(let i = 0; i < nodes.length; i++) {
            if (!this.connections.has(nodes[i].node.id)) {
                const connection = new NodeConnection(nodes[i].node.host, nodes[i].node.port, state);

                await connection.connect(nodes[i].ticket, );
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
}

export const myConnManager = new NodeConnectionManager();