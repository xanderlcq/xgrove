import glob
import time
import serial
import json
import requests
from constant import *
Arduino_connection = False
time.sleep(2)
mode = Mode_record

while True:
    time.sleep(1)
    device_list=[]
    ser = None
    try:
        device_list = glob.glob('/dev/tty.usb*')
        print device_list[0]
        ser = serial.Serial(device_list[0],9600)
        Arduino_connection = True
    except IndexError:
        print 'Failed to connect to Arduino, Check USB connection'

    while True:
        if Arduino_connection:
            if mode == Mode_record:
                try:
                    data = ser.readline()
                    j_data = json.loads(data)
                    params = {'username':+API_USERNAME, 'api_key':API_KEY, 'format':'json'}
                    headers = {'content-type': 'application/json'}
                    requests.post(API_URL, params=params, data=data, headers=headers)
                except ValueError:
                    print 'invalid data reading'
                except AssertionError:
                    print 'Serial reading failed'
            if mode == Mode_control:
                pass
        else:
            break



