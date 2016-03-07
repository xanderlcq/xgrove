
import socket
nick = 'app123'
rec = '0004'
host = '52.6.89.200'
port = 52550
s = socket.socket()
s.connect((host, port))
s.send(nick)
data = s.recv(1024)
print data
import Tkinter
# Connection and initial return value

def print_value(val):
    print val
    s.sendall(rec+':'+val+':'+nick)

root = Tkinter.Tk()

scale = Tkinter.Scale(orient='horizontal', from_=0, to=9, command=print_value)
scale.pack()

root.mainloop()