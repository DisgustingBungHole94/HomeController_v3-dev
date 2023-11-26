'use client';

import { ErrorMessageContext } from '../error_message_component.js';
import { MyDevicesContext } from '../my_devices_context.js';

import OnlineDeviceComponent from './online_device_component.js';
import OfflineDeviceComponent from './offline_device_component.js';

import { useState } from 'react';
import { useEffect } from 'react';
import { useContext } from 'react';


export default function DevicesPage() {
	const errorContext = useContext(ErrorMessageContext);
	const myDevicesContext = useContext(MyDevicesContext);

	const [onlineDevices, setOnlineDevices] = useState([]);
	const [offlineDevices, setOfflineDevices] = useState([]);

	useEffect(() => {
		async function getDevices() {
			const onlineDevicesArray = [];
			const offlineDevicesArray = [];

			for (const [id, device] of myDevicesContext.devices.list.entries()) {
				if (device.online) {
					onlineDevicesArray.push(<OnlineDeviceComponent key={id} id={id} nodeId={device.node_id} type={device.type} name={device.name} note={device.note} power={device.power} />);
				} else {
					offlineDevicesArray.push(<OfflineDeviceComponent key={id} type={device.type} name={device.name} note={device.note} />);
				}
			}

			setOnlineDevices(onlineDevicesArray);
			setOfflineDevices(offlineDevicesArray);
		}

		getDevices();
	}, [myDevicesContext]);

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
	);
}
