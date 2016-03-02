import glob
import time
import serial
import json
import requests
import socket
from thread import *
from constant import *
import urllib2


def secondary_thread():
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
                tcp_socket = connect_tcp()
                if tcp_socket is None:
                    print 'TCP connection dropped'
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
        print 'Secondary thread started\n'
    else:
        print 'TCP is not connected, can not initialize secondary thread. ' \
              'System will attempt to connect and start the secondary' \
              ' thread later.\n'

    # All initialization done
    print 'All initialization done\n'

    print 'Starting Loop\n'
    while True:
        # Controlling Mode
        if mode == Mode_control:
            data = arduino_serial.readline()
            tcp_socket.sendall(data)
            command = tcp_socket.recv(1024)
            if not command:
                print 'TCP disconnected, attempting to reconnect'
                tcp_socket = connect_tcp()
                if tcp_socket is None:
                    'Failed to reconnect to tcp\n'
                    tcp_connection = False
                else:
                    'Reconnected to tcp\n'
                    tcp_connection = True
            elif command == str(Mode_record):
                mode = Mode_record
                start_new_thread(secondary_thread,(tcp_socket,))
            else:
                arduino_serial.write(command)
        # Recording Mode
        elif mode == Mode_record:
            try:
                data = arduino_serial.readline()
                j_data = json.loads(data)
                params = {'username':+API_USERNAME, 'api_key': API_KEY, 'format':'json'}
                headers = {'content-type': 'application/json'}
                requests.post(API_URL, params=params, data=data, headers=headers)
                if not tcp_connection:
                    print 'Attempting to reconnect to TCP'
                    tcp_socket = connect_tcp()
                    if tcp_socket is None:
                        'Failed to reconnect to tcp\n'
                        tcp_connection = False
                    else:
                        'Reconnected to tcp\n'
                        tcp_connection = True
                        start_new_thread(secondary_thread, ())

            except ValueError:
                print 'invalid data reading'
            except AssertionError:
                print 'Serial reading failed'
else:
    print 'No internet connection, ' \
          'please connect to internet and reboot the system.'
