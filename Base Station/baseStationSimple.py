import time
from pySerialTransfer import pySerialTransfer as txfer

# THIS CODE ISN'T USED IN FINAL VERSION
# This is only for future reference in case MAVRIC
# decides to go from standalone 900 mhz to pc controlled 900 mhz

# first attempt
# https://projecthub.arduino.cc/ansh2919/serial-communication-between-python-and-arduino-663756

port = 'COM3'

class struct(object):
    drive = float
    steer = float
    
if __name__ == '__main__':
    try:
        # open serial transfer link
        testStruct = struct
        link = txfer.SerialTransfer(port)
        link.open()
        time.sleep(1)


        while True:
            # take input from user
            drive = input("Enter drive value from -1 to 1: ")
            steer = input("Enter steer value from -1 to 1: ")
            
            # send value to transmitter
            sendSize = 0
            sendSize = link.tx_obj(testStruct.drive, start_pos=sendSize)
            sendSize = link.tx_obj(testStruct.steer, start_pos=sendSize)
            link.send(sendSize)
            time.sleep(1)
    except KeyboardInterrupt:
        link.close()

