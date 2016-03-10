"""
LED controller demo, relay information between the TCP server and the arduino
"""
import socket
import serial
from constant import *
import glob
import time
import urllib2
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

def is_internet_on():
    try:
        response=urllib2.urlopen('http://www.google.com',timeout=1)
        print '\nInternet is connected.\n'
        return True
    except urllib2.URLError as err: pass
    return False

def connect_arduino():
    try:
        device_list = glob.glob('/dev/tty.usb*')
        print device_list[0]
        ser = serial.Serial(device_list[0],9600)
        return ser
    except IndexError:
        return None

if is_internet_on():
    tcp_socket = connect_tcp()
    arduino = connect_arduino()
    while True:
        try:
            brightness = tcp_socket.recv(1024)
            if not brightness:
                break
            pulseback = brightness[brightness.index(':')+1:]+':recieved:'+device_id
            tcp_socket.sendall(pulseback)
            print brightness[0:brightness.index(':')]
            #tcp_socket.sendall(brightness[brightness.index(':'):]+':recieved:'+device_id)
            #arduino.write(brightness[0:brightness.index(':')])
            #arduino.flush()
        except:
            pass
