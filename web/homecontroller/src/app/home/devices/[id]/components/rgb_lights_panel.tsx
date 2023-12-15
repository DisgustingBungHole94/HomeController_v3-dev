import { DeviceContext } from '@/app/home/contexts/device_context';
import { ErrorContext } from '@/app/home/contexts/error_context';
import { StatePower } from '@/deps/hc/state';
import { ClientPacket, Opcode } from '@/deps/hc/client_packet';
import { myConnManager } from '@/deps/hc/node';
import { State } from '@/deps/hc/state';
import { Device } from '@/deps/hc/api_requests';
import { RGBLightsState, Program } from '@/deps/hc/device_states/rgb_lights_state';

import { SketchPicker, RGBColor } from 'react-color';

import React, { useContext, useEffect, useState, useRef } from 'react';

interface RGBLightsPanelProps {
    deviceId: string,
    nodeId: string
};

export default function RGBLightsPanel({ deviceId, nodeId }: RGBLightsPanelProps) {
    const deviceContext = useContext(DeviceContext);
    const errorContext = useContext(ErrorContext);

    // for color preview
    const canvasRef = useRef<HTMLCanvasElement>(null);
    const [windowWidth, setWindowWidth] = useState(window.innerWidth);

    window.addEventListener('resize', () => {
        setWindowWidth(window.innerWidth);
    })

    // device info
    const [deviceName, setDeviceName] = useState('');
    const [deviceNote, setDeviceNote] = useState('');

    // selected values for UI
    const [selectedColor, setSelectedColor] = useState<RGBColor>({r: 0, g: 0, b: 0});
    const [selectedProgram, setSelectedProgram] = useState('none');
 
    // RGBLightsState values
    const [power, setPower] = useState<boolean>(false);
    const [speed, setSpeed] = useState<number>(0.0);
    const [program, setProgram] = useState<Program>(Program.NONE);

    const rebuildPage = (device: Device, state: State) => {
        setPower(state.getPower() === StatePower.ON);
        
        const rgbLightsState = new RGBLightsState();
        if (!rgbLightsState.parse(state.getData())) {
            return;
        }

        let color: RGBColor = {
            r: rgbLightsState.getR(),
            g: rgbLightsState.getG(),
            b: rgbLightsState.getB()
        }

        setSelectedColor(color);
        
        setSpeed(rgbLightsState.getSpeed());
        setProgram(rgbLightsState.getProgram());

        switch(rgbLightsState.getProgram()) {
            case Program.NONE:
                setSelectedProgram('none');
                break;
            case Program.RAINBOW_FADE:
                setSelectedProgram('rainbow_fade');
                break;
            case Program.GUITAR_SYNC:
                setSelectedProgram('guitar_sync');
                break;
        }

        updateColorPreview(color);
    };

    const updateColorPreview = (color: RGBColor) => {
        const canvas: HTMLCanvasElement | null = canvasRef.current;
        if (!canvas) {
            return;
        }

        const context = canvas.getContext('2d');
        if (!context) {
            return;
        }

        canvas.style.width = '100%';
        canvas.style.height = '0.5rem';

        context.canvas.width = canvas.getBoundingClientRect().width;
        context.canvas.height = canvas.getBoundingClientRect().height;

        const RECT_WIDTH = 10;
        const CANVAS_WIDTH = canvas.width;

        const NUM_RECTS = CANVAS_WIDTH / RECT_WIDTH;

        for(let i = 0; i < NUM_RECTS; i += 2) {
            context.fillStyle = 'rgb(' + color.r + ' ' + color.g + ' ' + color.b + ')';
            context.fillRect(i * RECT_WIDTH, 0, RECT_WIDTH, context.canvas.height);

            context.fillStyle = 'rgb(255, 255, 255)';
            context.fillRect((i + 1) * RECT_WIDTH, 0, RECT_WIDTH, context.canvas.height);
        }
    };

    useEffect(() => {
        updateColorPreview(selectedColor);
    }, [windowWidth]);

    useEffect(() => {
        if (deviceContext.loading) {
            return;
        }

        let deviceInfo = deviceContext.onlineDevices.get(deviceId);
        if (!deviceInfo) {
            return;
        }

        myConnManager.addCallback(deviceInfo.device.id, deviceInfo.device.id + '_RGBLightsPanel', rebuildPage);

        setDeviceName(deviceInfo.device.name);
        setDeviceNote(deviceInfo.device.note);
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
        setSelectedColor(color);
        
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
            errorContext?.setError('Failed to set color!');
        })
    };

    const startProgram = (program: Program) => {
        const packet = new ClientPacket();
        packet.setMessageId(0x00000000);
        packet.setOpcode(Opcode.DATA);
        packet.setDeviceIdFromStr(deviceId);

        const state = new RGBLightsState();
        state.setR(0x00);
        state.setG(0x00);
        state.setB(0x00);
        state.setSpeed(speed);
        state.setProgram(program);

        packet.setDataFromArray(state.serialize());

        myConnManager.send(nodeId, packet)
        .catch(() => {
            errorContext?.setError('Failed to start program!');
        })
    };

    const updateProgram = (e: any) => {
        switch(e.target.value) {
            case 'none':
                startProgram(Program.NONE);
                break;
            case 'rainbow_fade':
                startProgram(Program.RAINBOW_FADE);
                break;
            case 'guitar_sync':
                startProgram(Program.GUITAR_SYNC);
                break;
            default:
                break;
        };
    };

    return (
        <div className="p-6" style={{
            backgroundImage: 'linear-gradient(transparent, rgba(' + selectedColor.r + ', ' + selectedColor.g + ', ' + selectedColor.b + ', 0.2) 15%, transparent)'
        }}>
            <div>
                <h1 className="text-5xl">{deviceName}</h1>
                <hr />
                <h2 className="text-lg text-gray-500 mx-2 my-4">{deviceNote}</h2>
            </div>
            <div className="my-5">
                <canvas ref={canvasRef} />
            </div>
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
                            color={selectedColor}
                            onChange={updateColor}
                        />
                        <br />
                    </div>
                </div>
                <div className="md:w-1/3 bg-white text-center m-2 p-5 rounded shadow">
                    <h1 className="text-2xl text-gray-600 pb-5">Program</h1>
                    <select
                        onChange={updateProgram}
                        value={selectedProgram}
                    >
                        <option value="none">None</option>
                        <option value="rainbow_fade">Rainbow Fade</option>
                        <option value="guitar_sync">Guitar Sync</option>
                    </select>
                </div>
            </div>
        </div>
    );
}