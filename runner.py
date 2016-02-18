import glob
import time
import serial
Arduino_connection = False
time.sleep(2)


while True:
    time.sleep(1)
    device_list=[]
    try:
        device_list = glob.glob('/dev/tty.usb*')
        print device_list[0]
        ser = serial.Serial(device_list[0],9600)
        Arduino_connection = True
    except IndexError:
        print 'Failed to connect to Arduino, Check USB connection'


    while True:
        if Arduino_connection:
            try:
                data = ser.readline()
            except AssertionError:
                print 'Serial reading failed'
        else:
            break

