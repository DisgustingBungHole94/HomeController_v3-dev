import { createContext } from 'react';
import { useState, Dispatch, SetStateAction } from 'react';

import { Device } from '@/deps/hc/api_requests';
import { State } from '@/deps/hc/state';

export interface DeviceState {
    device: Device,
    state: State
};

export type DeviceListState = [Map<string, DeviceState>, Dispatch<SetStateAction<Map<string, DeviceState>>>];

export const DeviceContext = createContext<DeviceListState>(useState(new Map<string, DeviceState>()));