export function u8ArrayToU16(array) {
    return u8ArrayToUInt(array, 16);
}

export function u8ArrayToU32(array) {
    return u8ArrayToUInt(array, 32);
}

export function u8ArrayToU64(array) {
    return u8ArrayToUInt(array, 64);
}

export function u16ToU8Array(num) {
    return uIntToU8Array(num, 16);
}

export function u32ToU8Array(num) {
    return uIntToU8Array(num, 32);
}

export function u64ToU8Array(num) {
    return uIntToU8Array(num, 64);
}

const NUM_BITS_IN_BYTE = 8;

function u8ArrayToUInt(array, width) {
    let num = 0x00;

    for (let i = 0; i < width / NUM_BITS_IN_BYTE; i++) {
        num |= (array[i] << NUM_BITS_IN_BYTE * i);
    }

    return num;
}

function uIntToU8Array(num, width) {
    let array = new Uint8Array(width / NUM_BITS_IN_BYTE);

    for (let i = 0; i < width / NUM_BITS_IN_BYTE; i++) {
        array[i] = (num >> i * NUM_BITS_IN_BYTE) & 0xFF;
    }

    return array;
}