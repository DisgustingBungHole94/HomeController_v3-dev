import { DeviceContext } from '@/app/home/contexts/device_context';
import { RGBLightsState } from '@/deps/hc/device_states/rgb_lights_state';

import { useContext, useEffect, useRef } from 'react';

interface ColorDisplayComponentProps {
    deviceId: string
}

export default function ColorDisplayComponent({ deviceId }: ColorDisplayComponentProps) {
    const deviceContext = useContext(DeviceContext);
    
    const canvasRef = useRef<HTMLCanvasElement>(null);
    
    useEffect(() => {
        if (deviceContext.loading) {
            return;
        }

        let deviceState = deviceContext.onlineDevices.get(deviceId);
        if (!deviceState) {
            return;
        }

        const state = new RGBLightsState();
        if (!state.parse(deviceState.state.getData())) {
            return;
        }

        const canvas: HTMLCanvasElement | null = canvasRef.current;
        if (!canvas) {
            return;
        }

        const context = canvas.getContext('2d');
        if (!context) {
            return;
        }

        context.fillStyle = 'rgb(' + state.getR() + ' ' + state.getG() + ' ' + state.getB() + ')'
        context.fillRect(0, 0, context.canvas.width, context.canvas.height);
    }, [deviceContext]);

    return <canvas ref={canvasRef} />
}