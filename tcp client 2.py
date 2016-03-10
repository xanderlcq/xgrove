"""
Direct TCP socket test
"""
import socket

def Main():
    print("Send 'q' to exit\n")
    nick = input("nick: ")
    host = '127.0.0.1'
    port = 9998

    # Connection and initial return value
    s = socket.socket()
    s.connect((host, port))
    s.send(nick)
    data = s.recv(1024)
    print(data)
    message = input("-> ")
    # Chat going on
    while message != "q":
        s.send(message + ":" + nick)
        data = s.recv(1024)
        print(data)
        if not data:
            break

        message = input("-> ")
    s.close()

if __name__ == "__main__":
    Main()