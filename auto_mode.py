"""
Automation Program
"""
import glob
import time
import serial
import json
import requests
import socket
from thread import *
from constant import *
import urllib2

arduino_connection = False
tcp_connection = False
mode = Mode_record
tcp_socket = None
arduino_serial = None


def secondary_thread():
    global tcp_connection
    if not tcp_connection:
        exit_thread()
    global mode
    global tcp_socket
    while True:
        try:
            time.sleep(5)
            pulse = 'state check'
            tcp_socket.sendall(pulse)
            pulse_back = tcp_socket.recv(1024)
            if not pulse_back:
                print 'TCP failed, attempting to reconnect'
                tcp_socket.close()
                tcp_socket = connect_tcp()
                if tcp_socket is None:
                    print 'TCP connection dropped'
                    tcp_connection = False
                    exit_thread()
            else:
                if pulse_back == str(Mode_control):
                    mode = Mode_control
                    exit_thread()
        except socket.error:
            print 'TCP failed, attempting to reconnect'
            tcp_socket = connect_tcp()
            if tcp_socket is None:
                print 'TCP connection dropped'
                tcp_connection = False
                exit_thread()


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
        s.connect((TCP_IP, TCP_PORT))
        s.send(device_id)
        print(s.recv(1024))
        return s
    except socket.timeout,socket.error:
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


def init_arduino():
    global arduino_connection
    global arduino_serial
    print 'Initializing Arduino connection'
    while not arduino_connection:
        arduino_serial = connect_arduino()
        if arduino_serial is not None:
            arduino_connection = True
            print 'Arduino connection initialized\n'
        else:
            print 'Can not connect to arduino, please check usb connection'
            time.sleep(1)


def init_tcp():
    global tcp_connection
    global tcp_socket
    # Initialize TCP socket
    print 'Initializing TCP connection'
    tcp_socket = connect_tcp()
    if tcp_socket is not None:
        tcp_connection = True
        print 'TCP initialized\n'
    else:
        print 'TCP failed to initialize\n'


def init_secondary_thread():
    # Initialize secondary thread
    if tcp_connection:
        print 'Starting secondary tcp listening thread'
        start_new_thread(secondary_thread, (tcp_socket,))
        print 'Secondary thread started\n'
    else:
        print 'TCP is not connected, can not initialize secondary thread. ' \
              'System will attempt to connect and start the secondary' \
              ' thread later.\n'


def api_upload(data_string):
    params = {'username':+API_USERNAME, 'api_key': API_KEY, 'format':'json'}
    headers = {'content-type': 'application/json'}
    requests.post(API_URL, params=params, data=data_string, headers=headers)


def check_tcp():
    pass


if is_internet_on():
    # All initialization
    print 'Initializing the system\n'
    # Initialize Arduino (Required)
    init_arduino()
    # Initialize TCP
    init_tcp()
    # Initialize Secondary Thread
    init_secondary_thread()
    # All initialization done
    print 'All initialization done\n'
    print 'Starting Loop\n'
    while True:
        if mode == Mode_record:
            try:
                arduino_serial.write("pull_all")
                data = arduino_serial.readline()
                api_upload(data)
                for i in range(60):
                    time.sleep(1)
                    if mode == Mode_control:
                        break
            except ValueError:
                print 'invalid data reading'
            except AssertionError:
                print 'Serial reading failed'
        if mode == Mode_control:
            # TO-DO
            pass
else:
    print 'No internet connection, ' \
          'please connect to internet and reboot the system.'
