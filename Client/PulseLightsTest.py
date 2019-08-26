#Pulses LED's to test connectivity


import asyncio
import websockets
import time

#Defines function which creates the websocket, at the given URI
async def hello(uri):
    print("Attempting Connection with " + uri)
    async with websockets.connect(uri,ping_interval=None) as websocket:
        i = 0
        j = 0
        up = True
        while(True):
            if(j>=65534):
                up = False
            if(j<=0):
                j = 0
                up = True
                i = i + 1
            if(i==4):
                i = 0
            if(i==0):
                Cmd = str(j) + " 0 0"
            if(i==1):
                Cmd ="0 " + str(j) +" 0"
            if(i==2):
                Cmd = "0 0 " + str(j)
            if(i == 3):
                Cmd = str(j) + " " + str(j) + " " + str(j)
            print(j)
            await websocket.send(Cmd)
            time.sleep(0.005)
            if(up==True):
                j = j + 500
            if(up==False):
                j = j - 500


#Actually attempts the above function
asyncio.get_event_loop().run_until_complete(
    hello('ws://192.168.1.110:81'))
