import socket
import sys
from thread import start_new_thread
# Initialization
HOST = '' # all availabe interfaces
PORT = 9998 # arbitrary non privileged port
try:
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
except socket.error, msg:
    print("Could not create socket. Error Code: ", str(msg[0]), "Error: ", msg[1])
    sys.exit(0)

print("[-] Socket Created")

# bind socket
try:
    s.bind((HOST, PORT))
    print("[-] Socket Bound to port " + str(PORT))
except socket.error, msg:
    print("Bind Failed. Error Code: {} Error: {}".format(str(msg[0]), msg[1]))
    sys.exit()

s.listen(10)
print("Listening...")

clients = {}


def client_thread(clients,conn):
    while True:
        data = conn.recv(1024)
        if not data:
            break
        print(data)
        #conn.sendall(data)
        rec = data[0:data.index(':')]
        try:
            clients[rec].sendall(data[data.index(':')+1:])
        except KeyError:
            conn.sendall(rec+' is not connected')

    conn.close()

while True:
    conn, addr = s.accept()
    conn_id= conn.recv(1024)
    conn.sendall(conn_id.upper()+" Welcome to the Server. Type messages and "
                                 "press enter to send.\nrecipient:msgg:sender")
    clients[conn_id] = conn
    print("[-] Connected to " + addr[0] + ":" + str(addr[1])+" name:"+conn_id)
    start_new_thread(client_thread, (clients,conn,))



