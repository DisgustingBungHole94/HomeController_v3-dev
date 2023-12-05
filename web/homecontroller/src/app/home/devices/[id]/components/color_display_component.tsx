import { DeviceContext } from '@/app/home/contexts/device_context';
import { RGBLightsState } from '@/deps/hc/device_states/rgb_lights_state';

import { useContext, useEffect, useRef, useState } from 'react';

interface ColorDisplayComponentProps {
    deviceId: string
}

export default function ColorDisplayComponent({ deviceId }: ColorDisplayComponentProps) {
    const deviceContext = useContext(DeviceContext);
    
    const canvasRef = useRef<HTMLCanvasElement>(null);
    
    const [windowWidth, setWindowWidth] = useState(window.innerWidth);

    window.addEventListener('resize', () => {
        setWindowWidth(window.innerWidth);
    });

    const drawCanvas = (state: RGBLightsState) => {
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
            context.fillStyle = 'rgb(' + state.getR() + ' ' + state.getG() + ' ' + state.getB() + ')';
            context.fillRect(i * RECT_WIDTH, 0, RECT_WIDTH, context.canvas.height);

            context.fillStyle = 'rgb(255, 255, 255)';
            context.fillRect((i + 1) * RECT_WIDTH, 0, RECT_WIDTH, context.canvas.height);
        }
    };

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

        drawCanvas(state);
    }, [deviceContext, windowWidth]);

    return <canvas ref={canvasRef} />
}