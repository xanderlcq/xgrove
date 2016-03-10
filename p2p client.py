"""
P2P chat client
"""
import socket
from thread import *
def recieving_thread(conn):
    while True:
        data = conn.recv(1024)
        if not data:
            break
        print(data)
from constant import *
def Main():
    print("Send 'q' to exit\n")
    nick = raw_input("nick: ")
    assert nick != '', 'Nick name cannot be empty'
    rec = raw_input("Recipient: ")
    assert rec != '', 'Recipient cannot be empty'
    host = TCP_IP
    port = TCP_PORT

    # Connection and initial return value
    s = socket.socket()
    s.connect((host, port))
    s.send(nick)
    data = s.recv(1024)
    print data
    start_new_thread(recieving_thread,(s,))
    while True:
        message = raw_input()
        if message == 'q':
            s.close()
            break
        data = rec+':'+message+':'+nick
        try:
            s.sendall(data)
        except socket.error:
            print 'TCP closed'
            break

if __name__ == "__main__":
    Main()