"""
This is a general p2p TCP relay server.
"""
import socket
import sys
from thread import *


def client_thread(clients,conn,conn_id):
    while True:
        try:
            data = conn.recv(1024)
            if not data:
                break
            print(data)
            rec = data[0:data.index(':')]
            try:
                clients[rec].sendall(data[data.index(':')+1:])
            except KeyError:
                conn.sendall(rec+' is not connected')
        except socket.timeout:
            print conn_id + ' timed out!'
            break
    print 'closing connection'
    conn.close()
    del clients[conn_id]
    print 'connection closed, id:' + conn_id
    exit_thread()



def Main():
    # Initialization
    HOST = '' # all availabe interfaces
    PORT = 52551 # arbitrary non privileged port
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

    while True:
        conn, addr = s.accept()
        conn.settimeout(10)
        conn_id= conn.recv(1024)
        if not conn_id in clients:
            clients[conn_id] = conn
            conn.sendall(conn_id.upper()+" Welcome to the Server. Type messages and "
                                     "press enter to send.\nrecipient:msgg:sender")
            print("[-] Connected to " + addr[0] + ":" + str(addr[1])+" name:"+conn_id)
            start_new_thread(client_thread, (clients,conn,conn_id,))
        else:
            conn.sendall("Your user id is already in use")
            print("[-] Connection from " + addr[0] + ":" + str(addr[1])+
                  " name:"+conn_id +" is refused.")
            conn.close()

if __name__ == "__main__":
    Main()


