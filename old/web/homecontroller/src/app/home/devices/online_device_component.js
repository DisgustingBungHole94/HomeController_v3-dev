import ClientPacket, { opcode } from '@/hc/client_packet.js';
import { myConnManager } from '@/hc/node.js';
import { ErrorMessageContext } from '../error_message_component.js';

import Link from 'next/link';
import { useContext } from 'react'; 
import { useState } from 'react'; 


export default function OnlineDeviceComponent({ id, nodeId, type, name, note, power }) {    
    const errorContext = useContext(ErrorMessageContext);
    
    const togglePower = async () => {
        const packet = new ClientPacket();
        packet.setMessageId(0x00000000);
        packet.setOpcode((power) ? opcode.OFF : opcode.ON);
        packet.setDeviceIdFromStr(id);
        packet.setData('');

        await myConnManager.send(nodeId, packet, (res) => {
            if (res.getOpcode() !== opcode.RESPONSE) {
                errorContext.setErrorMessage('Failed to toggle power!');
            }
        }, () => {
            errorContext.setErrorMessage('Failed to send message!');
        });
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
                    <Link href={'/home/devices/' + id}>
                        <h1 className="text-2xl text-white text-bold">{name}</h1>
                    </Link>
                </div>
            </div>
            <div className="px-5 py-5 text-center">
                <p className="text-xs text-gray-500">CONNECTED, POWERED {(power) ? 'ON' : 'OFF'}</p>
            </div>
        </div>
    );
}