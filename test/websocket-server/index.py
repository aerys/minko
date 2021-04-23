#!/usr/bin/env python

import asyncio
import websockets

num_messages_before_stop = 2

async def echo(websocket, path):
    async for message in websocket:
        # Stop the server once all tests have been performed.
        if num_messages_before_stop == 0:
            break
        else
            num_messages_before_stop = num_messages_before_stop - 1

        await websocket.send(message)

start_server = websockets.serve(echo, "localhost", 8765)

asyncio.get_event_loop().run_until_complete(start_server)
asyncio.get_event_loop().run_forever()
