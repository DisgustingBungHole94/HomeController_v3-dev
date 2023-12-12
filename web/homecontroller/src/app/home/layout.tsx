'use client';

import { ErrorContext } from '@/app/home/contexts/error_context'
import { DeviceContext } from '@/app/home/contexts/device_context'

import { connectUser, ConnectUserResponse, Device } from '@/deps/hc/api_requests'
import { myConnManager, DeviceList } from '@/deps/hc/node';
import { State } from '@/deps/hc/state';

import { useEffect, useState } from 'react';
import { useRouter } from 'next/navigation';

import Cookies from 'js-cookie';

export default function HomeLayout({ children }: { children: React.ReactNode }) {
    const [error, setError] = useState('');
    const devices: DeviceList = myConnManager.useState();

    const router = useRouter();

    myConnManager.onDisconnect = () => {
        setError('Lost connection with server! Please refresh!');
    };

    const loadDevices = async () => {
        const token = Cookies.get('token');

        if (!token) {
            router.replace('/login');
        }

        if (!myConnManager.isConnected()) {
            try {
                const connectUserResponse: ConnectUserResponse = await connectUser(token!);
                myConnManager.setDeviceList(connectUserResponse.devices);                
                
                await myConnManager.connect(connectUserResponse.nodes);
            } catch(e) {
                setError('Unable to connect to server!');
            }
        }
    };

    useEffect(() => {
        loadDevices();
    }, []);

    return (
        <div>
            <div>
                {error && (
                    <div role="alert">
                        <div className="bg-red-500 text-white font-bold rounded-t px-4 py-2">
                            Warning
                            <button
                                onClick={ () => setError('') }
                                className="bg-red-800 text-white rounded px-1 px-1 float-right"
                            >
                                X
                            </button>
                        </div>
                        <div className="border border-t-0 border-red-400 rounded-b bg-red-100 px-4 py-3 text-red-500">
                            {error}
                        </div>
                    </div>
                )}
            </div>
            <ErrorContext.Provider value={{ error: error, setError: setError }}>
            <DeviceContext.Provider value={ devices }>
                {children}
            </DeviceContext.Provider>
            </ErrorContext.Provider>
        </div>
    )
}