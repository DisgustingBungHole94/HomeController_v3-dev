'use client';

import { myConnManager } from '@/hc/node.js';

import { ErrorMessageContext } from '../../error_message_component.js';
import { MyDevicesContext } from '../../my_devices_context.js';

import { useEffect } from 'react';
import { useContext } from 'react';

export default function Page({ params }) {
    return <div>ID: {params.id}</div>
}

