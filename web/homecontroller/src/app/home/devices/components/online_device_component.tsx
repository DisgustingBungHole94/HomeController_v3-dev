import { ClientPacket, Opcode} from '@/deps/hc/client_packet';
import { myConnManager } from '@/deps/hc/node';
import { ErrorContext } from '@/app/home/contexts/error_context';
import { State, StatePower } from '@/deps/hc/state';

import { useContext } from 'react';
import Link from 'next/link'

interface OnlineDeviceProps {
    deviceId: string,
    nodeId: string,
    state: State,
    deviceName: string,
    deviceNote: string,
}

export default function OnlineDeviceComponent({ deviceId, nodeId, state, deviceName, deviceNote}: OnlineDeviceProps) {    
    const errorContext = useContext(ErrorContext);
    
    let power: boolean = (state.getPower() === StatePower.ON);

    const togglePower = async () => {
        const packet = new ClientPacket();
        packet.setMessageId(0x00000000);
        packet.setOpcode(power ? Opcode.OFF : Opcode.ON);
        packet.setDeviceIdFromStr(deviceId);

        myConnManager.send(nodeId, packet)
        .catch(() => {
            errorContext?.setError('Failed to toggle power!');
        })
    };

    return (
        <div className="max-w-xs bg-white border border-gray-200 rounded shadow">
            <div className="bg-gray-600 flex px-5 py-5 rounded-t items-center justify-center">
                <div className="w-1/4 text-center">
                    <button 
                        className={((power) ? 'bg-green-500 hover:bg-green-600' : 'bg-gray-400 hover:bg-gray-500') + ' px-3 py-3 rounded-full shadow-lg'} 
                        onClick={() => togglePower()}
                    >
                        <img src="/img/power-icon.png" className="w-7 h-7" />
                    </button>
                </div>
                <div className="w-3/4 text-center whitespace-nowrap">
                    <Link href={'/home/devices/' + deviceId}>
                        <h1 className="text-2xl text-white text-bold">{deviceName}</h1>
                    </Link>
                </div>
            </div>
            <div className="px-5 py-5 text-center">
                <p className="text-xs text-gray-500">CONNECTED, POWERED {(power) ? 'ON' : 'OFF'}</p>
            </div>
        </div>
    );
}