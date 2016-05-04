import serial
from threading import Thread
from tcp import Client
import glob
import urllib2
import time
import requests
from constant import *
def main():
    r = Runner(ip='52.201.81.226')
    r.run()

class Runner:

    arduino_is_connected = False
    server_is_connected = False
    arduinno_serial = None
    client = None

    STATUS_CREATED = 201
    SERIAL_BAUD_RATE = 250000 # bits per second (must match arduino)
    DEVICE_ID = '3'
    LOGGING_DELAY_MINUTES = 60
    LOGGING_DELAY_SECONDS = LOGGING_DELAY_MINUTES*60 # minutes * 60seconds/minutes

    def __init__(self, ip='127.0.0.1', port=52551, username='root', api_key='123'):
        self.ip = ip
        self.port = port
        self.username = username
        self.api_key = api_key
        self.my_name = 'my_name'
        self.server_name = 'server'

    def run(self):
        print '1'
        self.connect_server()
        print '2'
        self.connect_arduino()

        auto_record = Thread(target=self.auto_record).start()

        #auto_control = Thread(target=self.auto_control).start()
        #manual_control = Thread(target=self.manual_control).start()
        # Add safety control thread

    def auto_record(self):
        print '3'
        while True:
            try:
                time.sleep(5)
                self.arduino_serial.write('get_all\n')
                print '4'
                data = self.arduino_serial.readline()
                data = '{"device":{"id":"'+self.DEVICE_ID+'"},' + data[1:]
                print data.__str__()
                status_code = self.api_upload(data)
                if not status_code == self.STATUS_CREATED:
                    print 'API error: ' + status_code.__str__()
            except Exception as e:
                print e
                break

    def auto_control(self):
        Thread(target=self.cycle_output, args=('water', 10, 60, True)).start()

    def cycle_output(self, transducer_name, minutes_on, minutes_off, recursive=False):
        seconds_on = minutes_on*60
        seconds_off = minutes_off*60
        period = seconds_on + seconds_off
        seconds_passed = (time.time() // 1) % period

        if (seconds_on - seconds_passed) >= 0: 
            seconds_remaining = seconds_on - seconds_passed 
            # MAKE SURE IT IS ON
        else: # (seconds_on - seconds_passed) < 0
            seconds_remaining = period - seconds_passed
            # MAKE SURE IT IS OFF

        time.sleep(seconds_remaining)

        if recursive: 
            self.cycle_output(self, transducer_name, minutes_on, minutes_off, recursive)
        # else: allows the current state to be restored from manual_control

    def manual_control(self):
        while True:
            if len(self.client.received) > 0:
                recvd = self.client.received.pop(0)
            # perform instructions

    def connect_arduino(self):
        while not self.arduino_is_connected:
            try:
                usb_device_list = glob.glob('/dev/tty.usb*')
                self.arduino_serial = serial.Serial(usb_device_list[0], self.SERIAL_BAUD_RATE)
            except IndexError:
                self.arduino_serial = None
            self.arduino_is_connected = (self.arduino_serial is not None)
        print '[+ Connected to arduino.]'

    def connect_server(self):
        while not self.internet_is_connected():
            pass
        print '[+ Reached server via http.]'

        #while not self.server_is_connected:
            #self.client = Client(self.my_name, self.server_name, ip=self.ip, port=self.port)
            #self.client.run()
            #self.server_is_connected = self.client.is_connected
        print '[+ Connected to server.]'

    def internet_is_connected(self):
        try:
            response=urllib2.urlopen('http://greenwall.elasticbeanstalk.com',timeout=1)
            return True
        except urllib2.URLError: pass
        return False

    def api_upload(self, data_string):
        params = {'username':self.username, 'api_key':self.api_key, 'format':'json'}
        headers = {'content-type': 'application/json'}
        response = requests.post(API_URL, params=params, data=data_string, headers=headers)
        return response.status_code

if __name__ == "__main__":
    main()
