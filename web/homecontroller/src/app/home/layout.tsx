'use client';

import { ErrorContext } from '@/app/home/contexts/error_context'
import { DeviceContext, DeviceList, DeviceState, emptyDeviceList } from '@/app/home/contexts/device_context'

import { connectUser, ConnectUserResponse, Device } from '@/deps/hc/api_requests'
import { myConnManager } from '@/deps/hc/node';
import { State } from '@/deps/hc/state';

import { useEffect, useState } from 'react';
import { useRouter } from 'next/navigation';

import Cookies from 'js-cookie';

export default function HomeLayout({ children }: { children: React.ReactNode }) {
    const [error, setError] = useState('');
    const [devices, setDevices] = useState<DeviceList>(emptyDeviceList());

    const router = useRouter();

    myConnManager.onDisconnect = () => {
        setError('Unable to connect to server!');
    };

    const onDeviceStateUpdate = (device: Device, state: State) => {
        updateDevicesState(device, state, true);
    };

    const onDeviceConnect = (device: Device, state: State) => {
        updateDevicesState(device, state, true);
    };

    const onDeviceDisconnect = (device: Device) => {
        updateDevicesState(device, null, false);
    };

    const updateDevicesState = (device: Device, state: State | null, online: boolean) => {
        let newList: DeviceList = { 
            onlineDevices: new Map<string, DeviceState>(devices.onlineDevices),
            offlineDevices: new Map<string, Device>(devices.offlineDevices)
        };
    
        if (online) {
            if (!state) {
                return;
            }

            let newState: DeviceState = {
                device: device,
                state: state
            };

            newList.offlineDevices.delete(device.id);
            newList.onlineDevices.set(device.id, newState);
        } else {
            newList.onlineDevices.delete(device.id);
            newList.offlineDevices.set(device.id, device);
        }

        setDevices(newList);
    };

    const loadDevices = async () => {
        const token = Cookies.get('token');

        if (!token) {
            router.replace('/login');
        }

        if (!myConnManager.isConnected()) {
            try {
                const connectUserResponse: ConnectUserResponse = await connectUser(token!);

                let deviceList: DeviceList = emptyDeviceList();
                connectUserResponse.devices.forEach((device) => {
                    deviceList.offlineDevices.set(device.id, device);
                });
                setDevices(deviceList);

                myConnManager.setDeviceList(connectUserResponse.devices);                
                myConnManager.setAllCallbacks(onDeviceStateUpdate, onDeviceConnect, onDeviceDisconnect);
                
                await myConnManager.connect(connectUserResponse.nodes);
            } catch(e) {
                console.log(e);
                setError('Unable to connect to server!');
            }
        } else {
            myConnManager.setAllCallbacks(onDeviceStateUpdate, onDeviceConnect, onDeviceDisconnect);
        }
    };

    useEffect(() => {
        loadDevices();
    }, []);

    return (
        <div>
            <ErrorContext.Provider value={{ error: error, setError: setError }}>
            <DeviceContext.Provider value={ devices }>
                {children}
            </DeviceContext.Provider>
            </ErrorContext.Provider>
        </div>
    )
}