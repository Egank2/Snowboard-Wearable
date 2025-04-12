import socket

computerMACAddress = "5C:01:3B:61:FD:A2"

port = 1

s = socket.socket(socket.AF_BLUETOOTH, socket.SOCK_STREAM, socket.BTPROTO_RFCOMM)
s.connect((computerMACAddress,port))


data = 'Hello world!'
s.send(bytearray([0x03]))

while 1:
   resp = s.recv(100)
   # print(data)
   print(resp)