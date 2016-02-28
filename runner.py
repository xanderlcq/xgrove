import glob
import time
import serial
import json
import requests
import socket
import thread
from constant import *
Arduino_connection = False
time.sleep(2)
mode = Mode_record
s = None
def mode_switching_thread():
    pass
try:
    s = socket.socket()
    s.settimeout(5)
    s.connect((TCP_IP, TCP_PORT))
    s.send(device_id)
    print(s.recv(1024))
except socket.timeout:
    print 'TCP connection failed'

while True:
    time.sleep(1)
    device_list = []
    ser = None
    try:
        device_list = glob.glob('/dev/tty.usb*')
        print device_list[0]
        ser = serial.Serial(device_list[0],9600)
        Arduino_connection = True
    except IndexError:
        print 'Failed to connect to Arduino, Check USB connection'
    # Main loop
    while True:
        if Arduino_connection:
            if mode == Mode_record:
                try:
                    data = ser.readline()
                    j_data = json.loads(data)
                    params = {'username':+API_USERNAME, 'api_key': API_KEY, 'format':'json'}
                    headers = {'content-type': 'application/json'}
                    requests.post(API_URL, params=params, data=data, headers=headers)
                except ValueError:
                    print 'invalid data reading'
                except AssertionError:
                    print 'Serial reading failed'
            if mode == Mode_control:
                assert s is not None, 'Please check you tcp connection first'
                while True:
                    data = s.recv(1024)
                    if not data:
                        break
                    ser.write(data)
                    return_data = ser.readline()
                    s.sendall(return_data)
        else:  # If Arduino is not connected
            break



