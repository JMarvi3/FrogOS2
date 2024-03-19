#include <syscall.h>
#include <conio.h>
#include <process.h>
#include <stdio.h>

void init(int pid) {
    for(;;) {
        asm("hlt");
    }
}

// glibc (used by GCC)[17] 	231 	1103515245 	12345 	bits 30..0 
#define A 1103515245L
#define C 12345L
#define MODULUS (1L << 30)

void user_task(int pid) {
    int64_t seed = (pid + 42) * 1000;
    for(;;) {
        seed = (A * seed + C) % MODULUS;
        int r = seed % 10000;
        printf("Process %4d sleeping for %5d ms\n", pid, r);
        uint64_t time_slept = syscall1(SYSCALL_SLEEP, r);
        // printf("%lu: ", time_slept);
    }
}