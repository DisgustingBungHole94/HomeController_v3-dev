'use client';

import { myConnManager } from '@/deps/hc/node';
import React, { useEffect, useContext, useState } from 'react';

import { DeviceContext } from '@/app/home/contexts/device_context';
import { ErrorContext } from '@/app/home/contexts/error_context';

import { Device } from '@/deps/hc/api_requests';
import { State } from '@/deps/hc/state';

import RGBLightsPanel from '@/app/home/devices/[id]/components/rgb_lights_panel';


export default function DevicePage({ params }: { params: { id: string } }) {
    let deviceContext = useContext(DeviceContext);
    let errorContext = useContext(ErrorContext);

    const [infoText, setInfoText] = useState<string | null>('Loading...');
    const [component, setComponent] = useState<React.ReactElement | null>(null);

    let initialized: boolean = false;

    useEffect(() => {
        if (deviceContext.loading) {
            return;
        }

        if (!params.id) {
            setInfoText('No device specified!');
            return;
        }

        let deviceState = deviceContext.onlineDevices.get(params.id);
        if (!deviceState) {
            if(deviceContext.offlineDevices.get(params.id)) {
                setComponent(null);
                setInfoText('Device is offline!')
            } else {
                setComponent(null);
                setInfoText('Device not found!');
            }
        } else {
            if (initialized) {
                return;
            }

            switch (deviceState.device.type) {
                case 'test_device':
                    setComponent(<RGBLightsPanel key={deviceState.device.id} deviceId={deviceState.device.id} nodeId={deviceState.device.nodeId} />);
                    break;
                default:
                    setInfoText('Unsupported device type!');
            }
            initialized = true;
        }
    }, [deviceContext]);

    return (
        <div>
            {infoText && !component && (
                <p>{infoText}</p>
            )}
            {component && (
                <div>
                    {component}
                </div>
            )}
        </div>
    )
}