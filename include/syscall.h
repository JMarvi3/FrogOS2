#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stdint.h>

uint64_t syscall0(uint64_t syscall);
uint64_t syscall1(uint64_t syscall, uint64_t arg1);
uint64_t syscall2(uint64_t syscall, uint64_t arg1, uint64_t arg2);
uint64_t syscall3(uint64_t syscall, uint64_t arg1, uint64_t arg2, uint64_t arg3);
uint64_t syscall4(uint64_t syscall, uint64_t arg1, uint64_t arg2, uint64_t arg3, uint64_t arg4);

#define SYSCALL_WRITE 0
#define SYSCALL_SLEEP 1
#define SYSCALL_GETPID 2
#define SYSCALL_HLT 3

#endif