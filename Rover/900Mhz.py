import time
from pySerialTransfer import pySerialTransfer as txfer

port = 'COM7'

drive = 0.0
steer = 0.0

deadzone = 0.1

class struct(object):
    drive = float
    steer = float

if __name__=='__main__':
    try:
        dataRX = struct
        print("Attempting to Connect to {}".format(port))
        link = txfer.SerialTransfer(port)
        link.open()
        print("Connected to {}".format(port))
        time.sleep(1)

        while True:
            if link.available():
                recSize = 0

                dataRX.drive = link.rx_obj(obj_type='f', start_pos=recSize)
                recSize += txfer.STRUCT_FORMAT_LENGTHS['f']

                dataRX.steer = link.rx_obj(obj_type='f', start_pos=recSize)
                recSize += txfer.STRUCT_FORMAT_LENGTHS['f']

            elif link.status < 0:
                if link.status == txfer.CRC_ERROR:
                    print('ERROR: CRC_ERROR')
                elif link.status == txfer.PAYLOAD_ERROR:
                    print('ERROR: PAYLOAD_ERROR')
                elif link.status == txfer.STOP_BYTE_ERROR:
                    print('ERROR: STOP_BYTE_ERROR')
                else:
                    print('ERROR: {}'.format(link.status))
            dataRX.drive = round(dataRX.drive, 2)
            dataRX.steer = round(dataRX.steer, 2)
            if abs(dataRX.drive) < deadzone:
                dataRX.drive = 0
            if abs(dataRX.steer) < deadzone:
                dataRX.steer = 0
            print("Drive: {}    Steer: {}".format(dataRX.drive,dataRX.steer))
    except KeyboardInterrupt:
        link.close()