'use client';

import { fetchDevices } from '@/hc/api_requests.js';
import { myConnManager } from '@/hc/node.js';
import ClientPacket from '@/hc/client_packet.js'
import { ErrorMessageContext } from './error_message_component.js';
import { MyDevicesContext } from './my_devices_context.js';
import { opcode } from '@/hc/client_packet.js';


import { useEffect } from 'react';
import { useState } from 'react';
import { useRouter } from 'next/navigation';

import Cookies from 'js-cookie';
import { Map } from 'immutable';

export default function HomeLayout({ children }) {
    const [errorMessage, setErrorMessage] = useState('');
    const [devices, setDevices] = useState({ list: new Map() });

    const router = useRouter();

    useEffect(() => {
        async function getDevices() {
            const token = Cookies.get('token');
            if (!token) {
                router.replace('/login');
                return;
            }

            try {
                const response = await fetchDevices(token);

                myConnManager.enableDeviceList(response.devices, setDevices, () => {
                    setDevices({ list: new Map() });
                    setErrorMessage('Connection lost! Please refresh the page.');
                });
                await myConnManager.connect(response.nodes);
            } catch(err) {
                //console.log(err);
                setErrorMessage('Unable to connect! Please try refreshing the page.');
            }
        }

        getDevices();
    }, []);


    return (
        <div>
            <div>
                {errorMessage && (
                    <div role="alert">
                        <div className="bg-red-500 text-white font-bold rounded-t px-4 py-2">
                            Warning
                            <button
                                onClick={ () => setErrorMessage('') }
                                className="bg-red-800 text-white rounded px-1 px-1 float-right"
                            >
                                X
                            </button>
                        </div>
                        <div className="border border-t-0 border-red-400 rounded-b bg-red-100 px-4 py-3 text-red-500">
                            {errorMessage}
                        </div>
                    </div>
                )}
            </div>
            <ErrorMessageContext.Provider value={{ setErrorMessage }}>
                <MyDevicesContext.Provider value={{ devices }}>
		            {children}
                </MyDevicesContext.Provider>
            </ErrorMessageContext.Provider>
        </div>
	);
}