import socket
from thread import start_new_thread
def recieving_thread(conn):
    while True:
        data = conn.recv(1024)
        if not data:
            break
        print(data)

def Main():
    print("Send 'q' to exit\n")
    nick = input("nick: ")
    rec = input("Recipient: ")
    host = '127.0.0.1'
    port = 9998

    # Connection and initial return value
    s = socket.socket()
    s.connect((host, port))
    s.send(nick)
    data = s.recv(1024)
    print data
    start_new_thread(recieving_thread,(s,))
    while True:
        message = input()
        data = rec+':'+message+':'+nick
        s.sendall(data)

if __name__ == "__main__":
    Main()