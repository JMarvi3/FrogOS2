#include <syscall.h>


uint64_t syscall0(uint64_t rax) {
    uint64_t ret;
    __asm__("syscall" : "=a"(ret) : "a"(rax));
    return ret;
}

uint64_t syscall1(uint64_t rax, uint64_t rdi) {
    uint64_t ret;
    __asm__("syscall" : "=a"(ret) : "a"(rax), "D"(rdi));
    return ret;
}

uint64_t syscall2(uint64_t rax, uint64_t rdi, uint64_t rsi) {
    uint64_t ret;
    __asm__("syscall" : "=a"(ret) : "a"(rax), "D"(rdi), "S"(rsi));
    return ret;
}

uint64_t syscall3(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx) {
    uint64_t ret;
    __asm__("syscall" : "=a"(ret) : "a"(rax), "D"(rdi), "S"(rsi), "d"(rdx));
    return ret;
}

uint64_t syscall4(uint64_t rax, uint64_t rdi, uint64_t rsi, uint64_t rdx, uint64_t rcx) {
    uint64_t ret;
    __asm__("syscall" : "=a"(ret) : "a"(rax), "D"(rdi), "S"(rsi), "d"(rdx), "c"(rcx));
    return ret;
}
