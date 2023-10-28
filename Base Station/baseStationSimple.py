import time
from pySerialTransfer import pySerialTransfer as txfer

# first attempt
# https://projecthub.arduino.cc/ansh2919/serial-communication-between-python-and-arduino-663756

port = 'COM3'

class struct(object):
    drive = float
    steer = float
    
if __name__ == '__main__':
    try:
        testStruct = struct
        link = txfer.SerialTransfer(port)
        link.open()
        time.sleep(1)
        '''
        drive = input("Enter first number: ") # Taking input from user 
        steer = input("Enter second number: ")
        '''
        while True:
            sendSize = 0
            testStruct.drive = 0.1258588
            testStruct.steer = -0.125656

            sendSize = link.tx_obj(testStruct.drive, start_pos=sendSize)
            sendSize = link.tx_obj(testStruct.steer, start_pos=sendSize)
            link.send(sendSize)
            time.sleep(1)
    except KeyboardInterrupt:
        link.close()

