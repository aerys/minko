#!/usr/bin/env python

# WSS (WS over TLS) server example, with a self-signed certificate

import asyncio
import pathlib
import ssl
import websockets
import signal

def quit_gracefully(*args):
    print("exiting secured websocket server")
    exit(0)

async def echo(websocket, path):
    async for message in websocket:
        await websocket.send(message)

ssl_context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
localhost_pem = pathlib.Path(__file__).with_name("ca.pem")
ssl_context.load_cert_chain(localhost_pem)

start_server = websockets.serve(
    echo, "127.0.0.1", 8766, ssl=ssl_context
)

signal.signal(signal.SIGINT, quit_gracefully)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()

