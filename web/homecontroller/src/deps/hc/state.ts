import { Exception } from '@/deps/hc/util/exception';
import * as bits from '@/deps/hc/util/bits';

export enum StateType {
    DATA        = 0x00, 
    DISCONNECT  = 0x01
}

export enum StatePower {
    ON          = 0x00,
    OFF         = 0x01
}

const MIN_STATE_SIZE: number = 2;

export class State {
    private type: StateType;
    private power: StatePower;
    private data: Uint8Array;

    constructor() {
        this.type = StateType.DISCONNECT;
        this.power = StatePower.OFF;
        this.data = new Uint8Array(0);
    }

    public setType(type: StateType) {
        this.type = type;
    }

    public getType(): StateType {
        return this.type;
    }

    public setPower(power: StatePower) {
        this.power = power;
    }

    public getPower(): StatePower {
        return this.power;
    }

    public setData(data: Uint8Array) {
        this.data = data;
    }

    public getData(): Uint8Array {
        return this.data;
    }

    public parse(data: Uint8Array): boolean {
        if (data.length < MIN_STATE_SIZE) {
            return false;
            //throw new Exception('state too small', 'State::parse');
        }

        if (data[0] < 0 || data[0] > 1) {
            return false;
            //throw new Exception('invalid state type', 'State::parse');
        }

        this.type = data[0];

        if (data[1] < 0 || data[1] > 1) {
            return false;
            //throw new Exception('invalid power code', 'State::parse');
        }

        this.power = data[1];

        this.data = data.slice(2);

        return true;
    }

    public serialize(): Uint8Array {
        let array = new Uint8Array(MIN_STATE_SIZE + this.data.length);

        array[0] = this.type;
        array[1] = this.power;

        array.set(this.data, 2);

        return array;
    }
};