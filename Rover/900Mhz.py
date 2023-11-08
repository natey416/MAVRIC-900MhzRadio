'''
THIS CODE IS NOT ACTIVELY UPDATED
THIS CODE IS FOR TESTING ONLY
IT MAY NOT WORK WITH CURRENT RECIEVER CODE
'''

import time
from pySerialTransfer import pySerialTransfer as txfer

port = 'COM7'

drive = 0.0
steer = 0.0

deadzone = 0.1

class struct(object):
    drive = float
    steer = float
    mode = float

if __name__=='__main__':
    try:
        dataRX = struct
        print("Attempting to Connect to {}".format(port))
        link = txfer.SerialTransfer(port)
        link.open()
        print("Connected to {}".format(port))

        dataRX.drive = 0
        dataRX.steer = 0
        dataRX.mode = 0
        time.sleep(1)
        Loop = True

        while Loop:
            if link.available():    # read all values from serial
                # This works by reading from the beginning of serial data line,
                # then advancing the "cursor" to after the read data and starting
                # the next reading data at the current cursor position
                recSize = 0

                dataRX.drive = link.rx_obj(obj_type='f', start_pos=recSize)
                recSize += txfer.STRUCT_FORMAT_LENGTHS['f']

                dataRX.steer = link.rx_obj(obj_type='f', start_pos=recSize)
                recSize += txfer.STRUCT_FORMAT_LENGTHS['f']

                dataRX.mode = link.rx_obj(obj_type='f', start_pos=recSize)
                recSize += txfer.STRUCT_FORMAT_LENGTHS['f']
            
            elif link.status < 0:   # check link errors
                if link.status == txfer.CRC_ERROR:
                    print('ERROR: CRC_ERROR')
                elif link.status == txfer.PAYLOAD_ERROR:
                    print('ERROR: PAYLOAD_ERROR')
                elif link.status == txfer.STOP_BYTE_ERROR:
                    print('ERROR: STOP_BYTE_ERROR')
                else:
                    print('ERROR: {}'.format(link.status))

            # clean up data and make sure it's within deadzone and rounded to 2 decimal places
            dataRX.drive = round(dataRX.drive, 2)
            dataRX.steer = round(dataRX.steer, 2)
            if abs(dataRX.drive) < deadzone:
                dataRX.drive = 0
            if abs(dataRX.steer) < deadzone:
                dataRX.steer = 0
            print("Drive: {}    Steer: {}    Mode: {}".format(dataRX.drive,dataRX.steer,dataRX.mode))
    except KeyboardInterrupt:
        link.close()
        Loop = False    # stop loop