import { DeviceContext } from '@/app/home/contexts/device_context';
import { ErrorContext } from '@/app/home/contexts/error_context';
import { StatePower } from '@/deps/hc/state';
import { ClientPacket, Opcode } from '@/deps/hc/client_packet';
import { myConnManager } from '@/deps/hc/node';
import { RGBLightsState, Program } from '@/deps/hc/device_states/rgb_lights_state';
import ColorDisplayComponent from '@/app/home/devices/[id]/components/color_display_component';

import { SketchPicker } from 'react-color';

import React, { useContext, useEffect, useState } from 'react';

interface RGBLightsPanelProps {
    deviceId: string,
    nodeId: string
};

interface Color {
    r: number,
    g: number,
    b: number
}

export default function RGBLightsPanel({ deviceId, nodeId }: RGBLightsPanelProps) {
    const deviceContext = useContext(DeviceContext);
    const errorContext = useContext(ErrorContext);

    const [deviceName, setDeviceName] = useState('Loading...');
    const [deviceNote, setDeviceNote] = useState('');

    const [pickerColor, setPickerColor] = useState('#000');

    const [colorDisplayComponent, setColorDisplayComponent] = useState<React.ReactElement | null>(null);

    const [power, setPower] = useState<boolean>(false);
    const [color, setColor] = useState<Color>({ r: 0, g: 0, b: 0});
    const [speed, setSpeed] = useState<number>(0.0);
    const [program, setProgram] = useState<Program>(Program.NONE);

    useEffect(() => {
        if (deviceContext.loading) {
            return;
        }

        let deviceState = deviceContext.onlineDevices.get(deviceId);
        if (!deviceState) {
            return;
        }

        setDeviceName(deviceState.device.name);
        setDeviceNote(deviceState.device.note);

        setPower(deviceState.state.getPower() === StatePower.ON);
        
        const state = new RGBLightsState();
        if (!state.parse(deviceState.state.getData())) {
            return;
        }

        setColor({r: state.getR(), g: state.getG(), b: state.getB() });
        setPickerColor('rgb(' + state.getR() + ' ' + state.getG() + ' ' + state.getB() + ')');

        setColorDisplayComponent(<ColorDisplayComponent deviceId={deviceState.device.id} />)

        setSpeed(state.getSpeed());
        setProgram(state.getProgram());
    }, [deviceContext]);

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

    const updateColor = (color: any, e: any) => {
        setPickerColor(color);
        
        const packet = new ClientPacket();
        packet.setMessageId(0x00000000);
        packet.setOpcode(Opcode.DATA);
        packet.setDeviceIdFromStr(deviceId);

        const state = new RGBLightsState();
        state.setR(color.rgb.r);
        state.setG(color.rgb.g);
        state.setB(color.rgb.b);
        state.setSpeed(speed);
        state.setProgram(Program.NONE);

        packet.setDataFromArray(state.serialize());

        myConnManager.send(nodeId, packet)
        .catch(() => {
            errorContext?.setError('Failed to toggle power!');
        })
    };

    return (
        <div className="p-6" style={{
            backgroundImage: 'linear-gradient(transparent, rgba(' + color.r + ', ' + color.g + ', ' + color.b + ', 0.2) 15%, transparent)'
        }}>
            <div>
                <h1 className="text-5xl">{deviceName}</h1>
                <hr />
                <h2 className="text-lg text-gray-500 mx-2 my-4">{deviceNote}</h2>
            </div>
            {colorDisplayComponent && (
                <div className="my-5">
                    {colorDisplayComponent}
                </div>
            )}
            <div className="md:flex bg-gray-100 p-6 rounded shadow">
                <div className="md:w-1/3 bg-white text-center m-2 p-5 rounded shadow">
                    <h1 className="text-2xl text-gray-600 pb-5">Power</h1>
                    <button
                        className={((power) ? 'bg-green-500 hover:bg-green-600' : 'bg-gray-400 hover:bg-gray-500') + ' px-3 py-3 rounded-full shadow-lg'}
                        onClick={() => togglePower()}
                    >
                        <img src="/img/power-icon.png" className="w-16 h-16" />
                    </button>
                </div>
                <div className="md:w-1/3 grid bg-white text-center m-2 p-5 rounded shadow">
                    <h1 className="text-2xl text-gray-600 pb-5">Color</h1>
                    <div className="place-self-center">
                        <SketchPicker 
                            color={pickerColor}
                            onChange={updateColor}
                        />
                        <br />
                    </div>
                </div>
                <div className="md:w-1/3 bg-white text-center m-2 p-5 rounded shadow">
                    <h1 className="text-2xl text-gray-600">Program</h1>
                </div>
            </div>
        </div>
    );
}