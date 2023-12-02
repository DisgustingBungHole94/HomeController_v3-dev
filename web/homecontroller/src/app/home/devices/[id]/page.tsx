'use client';

import { myConnManager } from '@/deps/hc/node';
import { useEffect, useContext, useState } from 'react';

import { DeviceContext } from '@/app/home/contexts/device_context';
import { ErrorContext } from '@/app/home/contexts/error_context';

import { Device } from '@/deps/hc/api_requests';
import { State } from '@/deps/hc/state';


export default function DevicePage({ params }: { params: { id: string } }) {
    let deviceContext = useContext(DeviceContext);
    let errorContext = useContext(ErrorContext);

    const [testText, setTestText] = useState<string>('Loading...');

    useEffect(() => {
        if (deviceContext.loading) {
            return;
        }

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

        setTestText('Device Note: ' + deviceState.device.note);
    }, [deviceContext]);

    return (
        <div>
            {testText && (
                <p>{testText}</p>
            )}
        </div>
    )
}