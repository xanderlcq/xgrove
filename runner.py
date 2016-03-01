import glob
import time
import serial
import json
import requests
import socket
from thread import *
from constant import *
import urllib2


def secondary_thread(s):
    global mode
    while True:
        try:
            time.sleep(5)
            pulse = 'state check'
            s.sendall(pulse)
            data = s.recv(1024)
            if not data:
                print 'TCP failed, attempting to reconnect'
                s = connect_tcp()
            else:
                if data == Mode_control:
                    mode = Mode_control
                    exit_thread()
        except socket.error:
            print 'TCP failed, attempting to reconnect'
            s = connect_tcp()


def is_internet_on():
    try:
        response=urllib2.urlopen('http://www.google.com',timeout=1)
        print '\nInternet is connected.\n'
        return True
    except urllib2.URLError as err: pass
    return False

def connect_tcp():
    try:
        s = socket.socket()
        s.settimeout(2)
        s.connect((TCP_IP, TCP_PORT))
        s.send(device_id)
        print(s.recv(1024))
        return s
    except socket.timeout:
        print 'TCP connection failed, timeout'
        return None


def connect_arduino():
    try:
        device_list = glob.glob('/dev/tty.usb*')
        print device_list[0]
        ser = serial.Serial(device_list[0],9600)
        return ser
    except IndexError:
        return None



arduino_connection = False
tcp_connection = False
mode = Mode_record
tcp_socket = None
arduino_serial = None
internet_connection = is_internet_on()

if internet_connection:
    # All initialization
    print 'Initializing the system\n'
    # Initialize TCP socket
    print 'Initializing TCP connection'
    tcp_socket = connect_tcp()
    if tcp_socket is not None:
        tcp_connection = True
        print 'TCP initialized\n'
    else:
        print 'TCP failed to initialize\n'

    # Initialize Arduino (Required)
    print 'Initializing Arduino connection'
    while not arduino_connection:
        arduino_serial = connect_arduino()
        if arduino_serial is not None:
            arduino_connection = True
            print 'Arduino connection initialized\n'
        else:
            print 'Can not connect to arduino, please check usb connection'
            time.sleep(1)

    # Initialize secondary thread
    if tcp_connection:
        print 'Starting secondary tcp listening thread'
        start_new_thread(secondary_thread, (tcp_socket,))
        print 'Seconday thread started\n'
    else:
        print 'TCP is not connected, can not initialize secondary thread. ' \
              'System will attempt to connect and start the secondary' \
              ' thread later.\n'

    # All initialization done
    print 'All initialization done\n'





'''
def listening_thread(s):
    assert s is not None
    while True:
        data = s.recv(1024)
        if not data:
            break
        if data == Mode_control:
            mode = Mode_control
            exit_thread()

try:
    s = socket.socket()
    s.settimeout(5)
    s.connect((TCP_IP, TCP_PORT))
    s.send(device_id)
    print(s.recv(1024))
    TCP_connection = True
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
                    if data == Mode_record:
                        mode = Mode_record
                        #start a new thread
                    ser.write(data)
                    return_data = ser.readline()
                    s.sendall(return_data)
        else:  # If Arduino is not connected
            break


'''
