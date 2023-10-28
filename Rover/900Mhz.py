import time
from pySerialTransfer import pySerialTransfer as txfer

port = 'COM7'

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

                print('drive: {}  steer: {}'.format(dataRX.drive, dataRX.steer))

            elif link.status < 0:
                if link.status == txfer.CRC_ERROR:
                    print('ERROR: CRC_ERROR')
                elif link.status == txfer.PAYLOAD_ERROR:
                    print('ERROR: PAYLOAD_ERROR')
                elif link.status == txfer.STOP_BYTE_ERROR:
                    print('ERROR: STOP_BYTE_ERROR')
                else:
                    print('ERROR: {}'.format(link.status))
    except KeyboardInterrupt:
        link.close()