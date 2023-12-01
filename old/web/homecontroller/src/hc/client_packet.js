import * as bits from './util/bits.js';

export const opcode = {
    AUTHENTICATE    : 0x00,
    ON              : 0x01,
    OFF             : 0x02,
    DATA            : 0x03,
    GET_STATE       : 0x04,
    RESPONSE        : 0x05,
    ERROR           : 0x06,
    NOTIFICATION    : 0x07
};

const MIN_OPCODE = 0x00;
const MAX_OPCODE = 0x07;
const DEVICE_ID_LENGTH = 32;
const PACKET_HEADER_SIZE = 11 + DEVICE_ID_LENGTH;
const MAX_DATA_LENGTH = 4096;

export default class ClientPacket {
    constructor() {
        this.magic = 0xBEEF;
        this.messageId = 0;
        this.opcode = 0;
        this.deviceId = 0;
        this.data = new Uint8Array();
    }

    setMagic(magic) {
        this.magic = magic;
    }

    getMagic() {
        return this.magic;
    }

    setMessageId(messageId) {
        this.messageId = messageId;
    }

    getMessageId() {
        return this.messageId;
    }

    setOpcode(opcode) {
        this.opcode = opcode;
    }

    getOpcode() {
        return this.opcode;
    }

    blankDeviceId() {
        this.deviceId = new Uint8Array(DEVICE_ID_LENGTH);
    }

    setDeviceIdFromStr(deviceId) {
        const enc = new TextEncoder();
        this.deviceId = enc.encode(deviceId);

        if (this.deviceId.length != DEVICE_ID_LENGTH) {
            throw 'invalid device id';
        }
    }

    setDeviceIdFromArray(deviceId) {
        if (deviceId.length != DEVICE_ID_LENGTH) {
            throw 'invalid device id';
        }

        this.deviceId = deviceId;
    }

    getDeviceId() {
        let dec = new TextDecoder('utf-8');

        return dec.decode(this.deviceId);
    }

    setData(data) {
        const enc = new TextEncoder();
        this.data = enc.encode(data);
    }

    setDataRaw(data) {
        this.data = data;
    }

    getData() {
        return this.data;
    }

    getDataLength() {
        return this.data.length;
    }

    parse(data) {
        if (data.length < PACKET_HEADER_SIZE) {
            throw 'packet too small';
        }

        let index = 0;

        this.magic = bits.u8ArrayToU16(data.slice(index, index + 2));
        index += 2;

        this.messageId = bits.u8ArrayToU32(data.slice(index, index + 4));
        index += 4;

        this.opcode = data[index];
        index += 1;

        if (opcode < MIN_OPCODE || opcode > MAX_OPCODE) {
            throw 'invalid opcode';
        }

        this.setDeviceIdFromArray(data.slice(index, index + DEVICE_ID_LENGTH));
        index += DEVICE_ID_LENGTH;

        let packetDataLength = bits.u8ArrayToU32(data.slice(index, index + 4));
        index += 4;

        if (data.length - PACKET_HEADER_SIZE != packetDataLength) {
            throw 'packet size mismatch';
        }

        if (packetDataLength > MAX_DATA_LENGTH) {
            throw 'packet is too big';
        }

        this.data = data.slice(index);
    }

    serialize() {
        let array = new Uint8Array(PACKET_HEADER_SIZE + this.data.length);
        let offset = 0;

        let magicArray = bits.u16ToU8Array(this.magic);
        array.set(magicArray, offset);
        offset += 2;

        let messageIdArray = bits.u32ToU8Array(this.messageId);
        array.set(messageIdArray, offset);
        offset += 4;

        array[offset] = this.opcode;
        offset += 1;

        array.set(this.deviceId, offset);
        offset += DEVICE_ID_LENGTH;

        let dataLengthArray = bits.u32ToU8Array(this.data.length);
        array.set(dataLengthArray, offset);
        offset += 4;
        
        array.set(this.data, offset);
        offset += this.data.length;

        return array;
    }


};

//export default ClientPacket;