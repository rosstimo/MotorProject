# This code uses the asyncio library to achieve asyncronous task processing.
# v0: testing the concept updating only direction and communicating it using lights


import asyncio
import board
import digitalio
from time import monotonic as mt

class encoderStatus:
    """Simple class to hold values used between functions."""

    def __init__(self):
        self.counts = 0
        self.sigState = False
        self.Forward = True
        
        
async def main():
    # Pin definitions
    pin1 = digitalio.DigitalInOut(board.GP16)
    pin1.switch_to_input(pull=digitalio.Pull.DOWN)

    pin2 = digitalio.DigitalInOut(board.GP17)
    pin2.switch_to_input(pull=digitalio.Pull.DOWN)
    
    fLed = digitalio.DigitalInOut(board.GP18)
    fLed.switch_to_output(value=False)

    rLed = digitalio.DigitalInOut(board.GP19)
    rLed.switch_to_output(value=False)
    
    # creat encoderStatus object
    eStatus = encoderStatus()
    
    # Define asyncio tasks
    encoder_task = asyncio.create_task(encoderRead(pin1, pin2, eStatus))
    comms_task = asyncio.create_task(comms(eStatus, fLed, rLed))
    
    await asyncio.gather(encoder_task, comms_task)


async def encoderRead(pin1, pin2, eStatus):
    old = (int(pin1.value) << 1) + int(pin2.value)
    while True:
        current = (int(pin1.value) << 1) + int(pin2.value)
        
        # If a change of state occurs, evaluate direction and increment counts
        if current != old:
            if (old == 0 and current == 1 or 
                    old == 1 and current == 3 or
                    old == 3 and current == 2 or
                    old == 2 and current == 0):
                eStatus.Forward = True
            else:
                eStatus.Forward = False
            
            old = current
        
        await asyncio.sleep(0)
        

async def comms(eStatus, FwdLed, RevLed):
    start = mt() * 1000
    
    while True:
        
        if eStatus.Forward:
            FwdLed.value = True
            RevLed.value = False
        else:
            FwdLed.value = False
            RevLed.value = True
            
        now = mt() * 1000
        if now - start > 500:
            start = now
            # print(f'direction is forward? {eStatus.Forward}')
        await asyncio.sleep(0)


asyncio.run(main())
