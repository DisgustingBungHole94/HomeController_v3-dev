'use client';

import { ErrorContext } from '@/app/home/contexts/error_context'
import { DeviceContext } from '@/app/home/contexts/device_context'

import { Device, reconnectUser, ReconnectUserResponse } from '@/deps/hc/api_requests'
import { myConnManager } from '@/deps/hc/node';

import { useEffect, useState } from 'react';
import { useRouter } from 'next/navigation';

import Cookies from 'js-cookie';

export default function HomeLayout({ children }: { children: React.ReactNode }) {
    const [error, setError] = useState<string>('');
    const [devices, setDevices] = useState<Map<string, Device>>(new Map<string, Device>);
    
    const router = useRouter();

    const loadDevices = async () => {
        const token = Cookies.get('token');

        if (!token) {
            router.replace('/login');
        }

        if (!myConnManager.isConnected()) {
            try {
                const reconnectUserResponse: ReconnectUserResponse = await reconnectUser(token!);
                await myConnManager.connect(reconnectUserResponse.nodes)
            } catch(e) {
                setError('Unable to connect to server!');
            }

            console.log(devices);
        }
    };

    useEffect(() => {
        loadDevices();
    }, []);

    return (
        <div>
            <ErrorContext.Provider value={ [error, setError] }>
            <DeviceContext.Provider value={ [devices, setDevices ]}>
                {children}
            </DeviceContext.Provider>
            </ErrorContext.Provider>
        </div>
    )
}