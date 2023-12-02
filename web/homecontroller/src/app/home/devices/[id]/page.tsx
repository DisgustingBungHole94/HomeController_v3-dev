'use client';

import { myConnManager } from '@/deps/hc/node';
import { useEffect, useContext, useState } from 'react';

import { DeviceContext } from '@/app/home/contexts/device_context';
import { Device } from '@/deps/hc/api_requests';
import { State } from '@/deps/hc/state';


export default function DevicePage({ params }: { params: { id: string } }) {
    let deviceContext = useContext(DeviceContext);

    const [testText, setTestText] = useState<string>('Loading...');

    useEffect(() => {
        if (!params.id) {
            setTestText('No device specified!');
            return;
        }

        let deviceState = deviceContext.onlineDevices.get(params.id);
        if (!deviceState) {
            setTestText('Device not found!');
            return;
        }

        console.log(deviceState.state);

        setTestText('Device Name: ' + deviceState.device.name);
    }, [deviceContext]);

    return (
        <div>
            {testText && (
                <p>{testText}</p>
            )}
        </div>
    )
}