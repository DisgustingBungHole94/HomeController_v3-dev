interface OfflineDeviceProps {
    deviceName: string,
    deviceNote: string,
}

export default function OfflineDeviceComponent({ deviceName, deviceNote }: OfflineDeviceProps) {    
    return (
        <div className="max-w-xs bg-white border border-gray-200 rounded shadow">
            <div className="bg-gray-600 flex px-5 py-5 rounded items-center justify-center">
                <div className="w-1/4 text-center">
                    <p className="text-gray-200">OFFLINE</p>
                </div>
                <div className="w-3/4 text-center whitespace-nowrap">
                    <h1 className="text-2xl text-white text-bold">{deviceName}</h1>
                </div>
            </div>
            <div className="px-5 py-5 text-center">
                <p className="text-xs text-gray-500">NOT CONNECTED</p>
            </div>
        </div>
    );
}