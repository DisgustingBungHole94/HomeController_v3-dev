import { Exception } from '@/deps/hc/util/exception';
import * as bits from '@/deps/hc/util/bits';

export enum Opcode {
    AUTHENTICATE =  0x00,
    ON =            0x01,
    OFF =           0x02,
    DATA =          0x03,
    GET_STATE =     0x04,
    RESPONSE =      0x05,
    ERROR =         0x06,
    NOTIFICATION =  0x07,
    CONNECT =       0x08,
    DISCONNECT =    0x09
};

export const MAGIC: number = 0xBEEF;

const MIN_OPCODE: number = 0x00;
const MAX_OPCODE: number = 0x09;
const DEVICE_ID_LENGTH: number = 32;
const PACKET_HEADER_SIZE: number = 11 + DEVICE_ID_LENGTH;
const MAX_DATA_LENGTH: number = 4096;

export class ClientPacket {
    private magic: number;
    private messageId: number;
    private opcode: Opcode;
    private deviceId: Uint8Array;
    private data: Uint8Array;

    constructor() {
        this.magic = MAGIC;
        this.messageId = 0x00000000;
        this.opcode = Opcode.ERROR;
        this.deviceId = new Uint8Array(DEVICE_ID_LENGTH);
        this.data = new Uint8Array(0);
    }

    public setMagic(magic: number) {
        this.magic = magic;
    }

    public getMagic(): number {
        return this.magic;
    }

    public setMessageId(messageId: number) {
        this.messageId = messageId;
    }

    public getMessageId(): number {
        return this.messageId;
    }

    public setOpcode(opcode: Opcode) {
        this.opcode = opcode;
    }

    public getOpcode(): Opcode {
        return this.opcode;
    }

    public setDeviceIdFromStr(deviceId: string): boolean {
        let deviceIdArray = new TextEncoder().encode(deviceId);
        return this.setDeviceIdFromArray(deviceIdArray);
    }

    public setDeviceIdFromArray(deviceId: Uint8Array): boolean {
        if (deviceId.length !== DEVICE_ID_LENGTH) {
            //throw new Exception('invalid device id', 'ClientPacket::setDeviceId');
            return false;
        }

        this.deviceId = deviceId;
        return true;
    }

    public blankDeviceId() {
        this.deviceId = new Uint8Array(DEVICE_ID_LENGTH);
    }

    public getDeviceIdAsStr(): string {
        let deviceIdStr: string = new TextDecoder().decode(this.deviceId);
        return deviceIdStr;
    }

    public getDeviceIdAsArray(): Uint8Array {
        return this.deviceId;
    }

    public setDataFromStr(data: string) {
        let dataArray: Uint8Array = new TextEncoder().encode(data);
        this.setDataFromArray(dataArray);
    }

    public setDataFromArray(data: Uint8Array): boolean {
        if (data.length > MAX_DATA_LENGTH) {
            //throw new Exception('data exceeds max allowed length', 'ClientPacket::setData');
            return false;
        }

        this.data = data;
        return true;
    }

    public getData(): Uint8Array {
        return this.data;
    }

    public parse(data: Uint8Array): boolean {
        if (data.length < PACKET_HEADER_SIZE) {
            return false;
            //throw new Exception('packet too small', 'ClientPacket::parse');
        }

        let index: number = 0;

        this.magic = bits.u8ArrayToU16(data.slice(index, index + 2));
        index += 2;

        this.messageId = bits.u8ArrayToU32(data.slice(index, index + 4));
        index += 4;

        if (data[index] < MIN_OPCODE || data[index] > MAX_OPCODE) {
            return false;
            //throw new Exception('invalid opcode', 'ClientPacket::parse');
        }

        this.opcode = data[index];
        index += 1;

        this.deviceId = data.slice(index, index + DEVICE_ID_LENGTH);
        index += DEVICE_ID_LENGTH;

        let packetDataLength: number = bits.u8ArrayToU32(data.slice(index, index + 4));
        index += 4;

        if (data.length - PACKET_HEADER_SIZE != packetDataLength) {
            return false;
            //throw new Exception('packet size mismatch', 'ClientPacket::parse');
        }

        if (packetDataLength > MAX_DATA_LENGTH) {
            return false;
            //throw new Exception('packet too big', 'ClientPacket::parse');
        }

        this.data = data.slice(index);
        return true;
    }

    public serialize(): Uint8Array {
        let array = new Uint8Array(PACKET_HEADER_SIZE + this.data.length);
        let index = 0;

        let magicArray: Uint8Array = bits.u16ToU8Array(this.magic);
        array.set(magicArray, index);
        index += 2;

        let messageIdArray: Uint8Array = bits.u32ToU8Array(this.messageId);
        array.set(messageIdArray, index);
        index += 4;

        array[index] = this.opcode;
        index += 1;

        array.set(this.deviceId, index);
        index += this.deviceId.length;

        let dataLengthArray = bits.u32ToU8Array(this.data.length);
        array.set(dataLengthArray, index);
        index += 4;

        array.set(this.data, index);
        index += this.data.length;

        return array;
    }
}
