#Test Program to see if there is a viable connection to a websocket available. It echos hello world to an awaiting server-side python script


import asyncio
import websockets

#Defines function which creates the websocket, at the given URI
async def hello(uri):
    async with websockets.connect(uri) as websocket:

#This line just takes in a string to be echoed to the websocket listener on the other end
        Typing = input("Put in a string to echo: ")

#Echos the string sent. The user on the host side would then get the string and print it to the screen.
        await websocket.send(Typing)


#Actually attempts the above function, at ws://192.168.0.101:8765
asyncio.get_event_loop().run_until_complete(
    hello('ws://192.168.1.110:81'))
