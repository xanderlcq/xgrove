import socket
from thread import *
def recieving_thread(conn):
    while True:
        data = conn.recv(1024)
        if not data:
            break
        print(data)

def Main():
    print("Send 'q' to exit\n")
    nick = raw_input("nick: ")
    assert nick != '', 'Nick name cannot be empty'
    rec = raw_input("Recipient: ")
    assert rec != '', 'Recipient cannot be empty'
    host = '52.6.89.200'
    port = 52550

    # Connection and initial return value
    s = socket.socket()
    s.connect((host, port))
    s.send(nick)
    data = s.recv(1024)
    print data
    start_new_thread(recieving_thread,(s,))
    while True:
        message = raw_input()
        data = rec+':'+message+':'+nick
        s.sendall(data)

if __name__ == "__main__":
    Main()