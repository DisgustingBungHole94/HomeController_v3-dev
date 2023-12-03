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

        canvas.style.width = '100%';
        canvas.style.height = '0.5rem';

        context.fillStyle = 'rgb(' + state.getR() + ' ' + state.getG() + ' ' + state.getB() + ')'

        const RECT_WIDTH = 5;
        for(let i = 0; i < context.canvas.width; i += RECT_WIDTH * 2) {
            let x = i + 0.5;
            context.fillRect(x, 0, RECT_WIDTH, context.canvas.height);
        }
    }, [deviceContext]);

    return <canvas ref={canvasRef} />
}