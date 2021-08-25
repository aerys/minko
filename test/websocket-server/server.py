#!/usr/bin/env python

import asyncio
import websockets
import signal

def quit_gracefully(*args):
    print("exiting websocket server")
    exit(0)

async def echo(websocket, path):
    async for message in websocket:
        await websocket.send(message)

start_server = websockets.serve(echo, "localhost", 8765)

signal.signal(signal.SIGINT, quit_gracefully)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
