import { createContext } from 'react';
import { DeviceList, emptyDeviceList } from '@/deps/hc/node';

/*export interface DeviceState {
    device: Device,
    online: boolean,
    onStateUpdate: (state: State) => void
};

export interface DeviceList {
    devices: Map<string, DeviceState>
}*/

/*export interface DeviceState {
    device: Device,
    state: State
}

export interface DeviceList {
    onlineDevices: Map<string, DeviceState>,
    offlineDevices: Map<string, Device>,
    loading: boolean
}

export function emptyDeviceList(): DeviceList {
    let list: DeviceList = {
        onlineDevices: new Map<string, DeviceState>(),
        offlineDevices: new Map<string, Device>(),
        loading: true
    };
    
    return list;
}

export type DeviceListState = [DeviceList, Dispatch<SetStateAction<DeviceList>>] | null;*/

export const DeviceContext = createContext<DeviceList>(emptyDeviceList());

/*export function updateDeviceList(state: DeviceListState, device: Device, online: boolean) {    
    if (!state) {
        return;
    }

    let newList: DeviceList = { 
        onlineDevices: new Map<string, Device>(state[0].onlineDevices),
        offlineDevices: new Map<string, Device>(state[0].offlineDevices)
    };

    if (online) {
        newList.offlineDevices.delete(device.id);
        newList.onlineDevices.set(device.id, device);
    } else {
        newList.onlineDevices.delete(device.id);
        newList.offlineDevices.set(device.id, device);
    }

    return true;
}*/