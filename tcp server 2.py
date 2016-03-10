"""
TCP socket sample test
"""
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

list = [[],[],[]]


# for each thread/connection
def client_thread(list1,name):
    conn.send("Welcome to the Server. Type messages and press enter to send.\n")
    while True:
        index = list1[2].index(name)
        data = list1[index][index].recv(1024)
        if not data:
            break
        print(data)
        reply = "OK . . I got it! " + data
        conn.sendall(reply)
    conn.close()
# pipe
# main loop
while True:
    # blocking call, waits to accept a connection
    conn, addr = s.accept()
    conn_name = conn.recv(1024)
    list[0].append(conn)
    list[1].append(addr)
    list[2].append(conn_name)
    print("[-] Connected to " + addr[0] + ":" + str(addr[1])+" name:"+conn_name)

    start_new_thread(client_thread, (list,conn_name,))

s.close()