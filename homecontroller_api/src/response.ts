import { User, Device, Node } from './user_manager';


export function errorResponse(message: string, code: number) {
    const json = {
        success: false,
        errorMsg: message,
        errorCode: code
    }

    return json;
}

export interface NodeTicket {
    node: Node,
    ticket: string
}

export interface LoginUserResponse {
    success: boolean,
    userId: string,
    username: string,
    nodes: Array<NodeTicket>,
    devices: Array<Device>,
    token: string
}

export interface LoginDeviceResponse {
    success: boolean,
    userId: string,
    deviceId: string,
    node: Node,
    ticket: string,
    token: string
}

export interface ValidateUserResponse {
    success: boolean,
    userId: string
}

export interface ValidateDeviceResponse {
    success: boolean,
    userId: string,
    deviceId: string
}