from threading import Thread
import socket
import struct
import time
from thread import *

def command_line_client():
    nick = raw_input("Nickname: ")
    rec = raw_input("Recipient: ")

    c = Client(nick, rec, ip='52.201.81.226', port=52551)
    c.run()

    Thread(target=read_in, args=(c,)).start()

    while 1:
        if len(c.received) > 0:
            recvd = c.received.pop(0)
            if recvd[1] == Client.server_name and recvd[0] == '0':
                print rec + ' is not connected.'
                continue
            print '[' + recvd[1] + '] ' + recvd[0]
        if not c.is_connected:
            print 'TCP disconnected. Trying to reconnect.'
            while not c.is_connected:
                time.sleep(Client.conn_keep_alive)
                c.run()

def read_in(c):
    while True:
        ri = raw_input()
        c.sending.append(ri)

class Client:
    """
    Client
    """

    # conn settings
    conn_timeout = 10
    conn_keep_alive = conn_timeout / 2

    # reserved names
    server_name = 'server'
    anonymous_name = '*'
    no_name = ''
    reserved_names = [server_name, anonymous_name, no_name, ]

    # input/output buffers
    received = []
    sending = []

    def __init__(self, name, recipient, ip='127.0.0.1', port=52551):
        self.ip = ip
        self.port = port
        self.conn = None
        self.name = name
        self.recipient = recipient
        self.is_connected = False

        self.server_notifier_thread = None

    def setup_socket(self):
        if self.conn and type(self.conn) is type(socket.socket()):
            self.conn.close()
        try:
            self.conn = socket.socket()
            self.conn.connect((self.ip, self.port))
            self.is_connected = self.handshake()
            return self.is_connected
        except socket.error:
            return False

    def run(self):
        while not self.setup_socket():
            time.sleep(self.conn_keep_alive)
        print '[+ Connected to server.]'
        Thread(target=self.server_listener).start()
        self.server_notifier_thread = Thread(target=self.server_notifier)
        self.server_notifier_thread.start()
        Thread(target=self.server_keep_alive).start()

    def server_listener(self):
        while True:
            try:
                data = Data.string_to_data(self.receive())
                if data:
                    self.received.append( (data.message, data.sender) )
            except socket.error:
                self.is_connected = False
                self.server_notifier_thread.join()
                break

    def server_notifier(self):
        while True:
            try:
                if len(self.sending) > 0:
                    self.send(Data(self.recipient, self.sending.pop(0), self.name))
            except socket.error:
                break

    def server_keep_alive(self):
        while True:
            try:
                time.sleep(self.conn_keep_alive)
                self.send(Data(self.server_name, 'keep alive', self.name))
            except socket.error:
                self.is_connected = False
                self.server_notifier_thread.join()
                break

    def handshake(self):
        """
        Sends client name. Receives response.
        """
        self.send(Data(self.server_name, '', self.name))
        response = Data.string_to_data(self.receive())
        return (response and response.message is '1')

    def send(self, data):
        msg = data.__str__()
        # `>I` is big-endian unsigned int (4 bytes)
        size = struct.pack('>I', len(msg))
        self.conn.sendall(size + msg)

    def receive(self):
        # '>I' is big-endian unsigned int (4 bytes)
        raw_size = self.conn.recv(4)
        if raw_size:
            size = struct.unpack('>I', raw_size)[0] # only packed 1 elem in tuple
            msg = self.conn.recv(size)
            return msg

class Data:
    """
    Encapsulates input/output information
    """

    receiver = ''
    message = ''
    sender = ''

    _fields = (receiver, message, sender)
    delimeter = ':'


    def __init__(self, receiver=Client.anonymous_name, message='', sender=Client.server_name):
        # validates data by removing the delimeter
        self.receiver = receiver.__str__().replace(self.delimeter, '')
        self.message = message.__str__().replace(self.delimeter, '')
        self.sender = sender.__str__().replace(self.delimeter, '')

    @staticmethod
    def string_to_data(string):
        if not string or not isinstance(string, str):
            return Data()

        args = string.split(Data.delimeter)
        if len(args) is len(Data._fields):
            return Data(*args)
        return Data()

    def __str__(self):
        return self.delimeter.join([self.receiver, self.message, self.sender])

if __name__ == "__main__":
    command_line_client()
