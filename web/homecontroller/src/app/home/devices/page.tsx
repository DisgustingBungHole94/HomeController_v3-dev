'use client';

import { DeviceContext } from '@/app/home/contexts/device_context';
import { ErrorContext } from '@/app/home/contexts/error_context';

import OnlineDeviceComponent from '@/app/home/devices/components/online_device_component';
import OfflineDeviceComponent from '@/app/home/devices/components/offline_device_component';

import React, { useContext, useEffect, useState } from 'react';
import { Device } from '@/deps/hc/api_requests';
import { State } from '@/deps/hc/state';
import { myConnManager } from '@/deps/hc/node';

export default function DevicesPage() {
    let deviceContext = useContext(DeviceContext);
    let errorContext = useContext(ErrorContext);

    const [onlineDevices, setOnlineDevices] = useState<React.ReactElement[]>([]);
    const [offlineDevices, setOfflineDevices] = useState<React.ReactElement[]>([]);

    const rebuildPage = () => {
        const onlineDevicesArr: React.ReactElement[] = [];
        const offlineDevicesArr: React.ReactElement[] = [];

        deviceContext.onlineDevices.forEach((deviceInfo) => {
            let device = deviceInfo.device;
            onlineDevicesArr.push(<OnlineDeviceComponent key={device.id} deviceId={device.id} nodeId={device.nodeId} state={deviceInfo.lastState} deviceName={device.name} deviceNote={device.note} />)
        });

        deviceContext.offlineDevices.forEach((device) => {
            offlineDevicesArr.push(<OfflineDeviceComponent key={device.id} deviceName={device.name} deviceNote={device.note} />)
        });

        setOnlineDevices(onlineDevicesArr);
        setOfflineDevices(offlineDevicesArr);
    };

    useEffect(() => {
        if (deviceContext.loading) {
            return;
        }

        deviceContext.onlineDevices.forEach((deviceInfo) => {
            myConnManager.addCallback(deviceInfo.device.id, deviceInfo.device.id + '_DevicesPage', rebuildPage, false);
        });

        rebuildPage();
    }, [deviceContext]);

    return (
        <div className="px-5 py-5">
        <div>
            <p className="text-m text-gray-500">ONLINE DEVICES</p>
            <hr />
            <br />
            {onlineDevices}
        </div>
        <br />
        {offlineDevices && (
            <div>
                <p className="text-m text-gray-500">OFFLINE DEVICES</p>
                <hr />
                <br />
                {offlineDevices}
            </div>
        )}
    </div>
    )
}