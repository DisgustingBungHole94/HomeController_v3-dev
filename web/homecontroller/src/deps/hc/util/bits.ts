export function u8ArrayToFloat(array: Uint8Array): number {
    let buf: ArrayBuffer = new ArrayBuffer(4);
    let view: DataView = new DataView(buf);

    array.forEach((b, i) => {
        view.setUint8(i, b);
    });

    return view.getFloat32(0);
}

export function floatToU8Array(num: number): Uint8Array {
    let floatArr = new Float32Array(1);
    floatArr[0] = num;

    return new Uint8Array(floatArr.buffer);
}

export function u8ArrayToU16(array: Uint8Array): number {
    return u8ArrayToUInt(array, 16);
}

export function u8ArrayToU32(array: Uint8Array): number {
    return u8ArrayToUInt(array, 32);
}

export function u8ArrayToU64(array: Uint8Array): number {
    return u8ArrayToUInt(array, 64);
}

export function u16ToU8Array(num: number): Uint8Array {
    return uIntToU8Array(num, 16);
}

export function u32ToU8Array(num: number): Uint8Array {
    return uIntToU8Array(num, 32);
}


export function u64ToU8Array(num: number): Uint8Array {
    return uIntToU8Array(num, 64);
}

const NUM_BITS_IN_BYTE: number = 8;

function u8ArrayToUInt(array: Uint8Array, width: number): number {
    let num: number = 0x00;

    for(let i = 0; i < width / NUM_BITS_IN_BYTE; i++) {
        num |= (array[i] << NUM_BITS_IN_BYTE * i);
    }

    return num;
}

function uIntToU8Array(num: number, width: number): Uint8Array {
    let array: Uint8Array = new Uint8Array(width / NUM_BITS_IN_BYTE);

    for (let i = 0; i < width / NUM_BITS_IN_BYTE; i++) {
        array[i] = (num >> i * NUM_BITS_IN_BYTE) & 0xFF;
    }

    return array;
}