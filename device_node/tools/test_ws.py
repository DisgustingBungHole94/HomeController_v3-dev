import asyncio
import pathlib
import ssl
import websockets

ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_CLIENT)
ssl_context.load_verify_locations('../cert/fullchain.pem')
ssl_context.check_hostname = False
ssl_context.verify_mode = ssl.CERT_NONE

async def hello():
    uri = "wss://localhost:4434"
    async with websockets.connect(
        uri, ssl=ssl_context
    ) as websocket:
        while(True):
            await websocket.send('Hello, world')
            print('Sent')
            print(await websocket.recv())
            print(await websocket.recv())
            print(await websocket.recv())
            #print(await websocket.recv())
            #await websocket.close()

asyncio.get_event_loop().run_until_complete(hello())