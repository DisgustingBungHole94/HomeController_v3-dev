import * as bits from '@/deps/hc/util/bits';

export enum Program {
    NONE =              0x00,
    RAINBOW_FADE =      0x01,
    PSYCHEDELIC_FADE =  0x02,
    GUITAR_SYNC =       0x03,
    MUSIC_SYNC =        0x04
};

const MIN_PROGRAM: number = 0x00;
const MAX_PROGRAM: number = 0x04;

const MIN_STATE_SIZE: number = 8;

export class RGBLightsState {
    private r: number;
    private g: number;
    private b: number;

    private speed: number;

    private program: Program;

    private data: Uint8Array;

    constructor() {
        this.r = 0x00;
        this.g = 0x00;
        this.b = 0x00;
        this.speed = 0.0;
        this.program = Program.NONE;
        this.data = new Uint8Array(0);
    }

    public setR(r: number) {
        this.r = r;
    }

    public getR(): number {
        return this.r;
    }

    public setG(g: number) {
        this.g = g;
    }

    public getG(): number {
        return this.g;
    }

    public setB(b: number) {
        this.b = b;
    }

    public getB(): number {
        return this.b;
    }

    public setSpeed(speed: number) {
        this.speed = speed;
    }

    public getSpeed(): number {
        return this.speed;
    }

    public setProgram(program: Program) {
        this.program = program;
    }

    public getProgram(): Program {
        return this.program;
    }

    public setDataFromStr(data: string) {
        let dataArray: Uint8Array = new TextEncoder().encode(data);
        this.setDataFromArray(dataArray);
    }

    public setDataFromArray(data: Uint8Array) {
        this.data = data;
    }

    public getData(): Uint8Array {
        return this.data;
    }

    public parse(data: Uint8Array): boolean {
        if (data.length < MIN_STATE_SIZE) {
            return false;
        }

        //console.log(data);

        let index: number = 0;

        this.r = data[index];
        index += 1;

        this.g = data[index];
        index += 1;

        this.b = data[index];
        index += 1;

        this.speed = bits.u8ArrayToFloat(data.slice(index, index + 4));
        index += 4;

        if (data[index] < MIN_PROGRAM || data[index] > MAX_PROGRAM) {
            return false;
        }

        this.program = data[index];
        index += 1;

        this.data = data.slice(index);

        return true;
    }

    public serialize(): Uint8Array {
        let array = new Uint8Array(MIN_STATE_SIZE + this.data.length);
        let index = 0;

        array[index] = this.r;
        index += 1;

        array[index] = this.g;
        index += 1;

        array[index] = this.b;
        index += 1;

        let speedArray: Uint8Array = bits.floatToU8Array(this.speed);
        array.set(speedArray, index);
        index += 4;

        array[index] = this.program;
        index += 1;

        array.set(this.data, index);
        index += this.data.length;

        return array;
    }
};