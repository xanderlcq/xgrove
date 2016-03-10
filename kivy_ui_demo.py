"""
LED demo, based on KIVY UI. Can run a many platforms
"""

import socket
from constant import *
nick = 'app123123345671238'
rec = device_id
host = '52.6.89.200'
port = 52550
s = socket.socket()
s.connect((host, port))
s.send(nick)
data = s.recv(1024)
print data
from kivy.uix.slider import Slider

prev_value = -1


def OnSliderValueChange(self, value):
    global prev_value
    value = int(value)
    if value != prev_value:
        print value
        s.sendall(rec+':'+str(value)+':'+nick)
        prev_value = value
    else:
        pass


if __name__ == '__main__':
    from kivy.app import App

    class SliderApp(App):
        def build(self):
            s = Slider(min = 0,max = 9,value = 0)
            s.bind(value=OnSliderValueChange)
            return s

    SliderApp().run()