#!/usr/bin/env python

# WSS (WS over TLS) server example, with a self-signed certificate generated with:
# openssl req -newkey rsa:2048 -new -nodes -x509 -days 3650 -keyout key.pem -out cert.pem
# And by hitting the Enter key at each question to use the default values.

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
cert_pem = pathlib.Path(__file__).with_name("cert.pem")
key_pem = pathlib.Path(__file__).with_name("key.pem")
ssl_context.load_cert_chain(cert_pem, key_pem)

start_server = websockets.serve(
    echo, "127.0.0.1", 8766, ssl=ssl_context
)

signal.signal(signal.SIGINT, quit_gracefully)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()

