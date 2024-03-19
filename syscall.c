#include <stdint.h>
#include <conio.h>
#include <stdio.h>
#include <process.h>
#include <syscall.h>

extern unsigned long long pit_counter;
struct syscall_state {
    uint64_t rax;
    uint64_t rbp;
    uint64_t rbx;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rsi;
    uint64_t rdi;
} __attribute__((packed));

extern unsigned long long pit_counter;
void ksyscall(struct syscall_state *state)
{
    switch (state->rax) {
        case SYSCALL_WRITE:
            // write
            uint64_t len = state->rsi;
            char *buffer = (char *)state->rdi;
            for (uint64_t i = 0; i < len; i++) {
                putch(buffer[i]);
            }
            break;
        case SYSCALL_SLEEP:
            unsigned long long start = pit_counter;
            state->rdi = (state->rdi + 9) / 10;
            
            while (pit_counter - start < state->rdi) {
                disable();
                add_to_run_queue(current_pid);
                process_table[current_pid].state = PROCESS_STATE_READY;
                enable();
                asm("hlt");
            }
            state->rax = 10 *(pit_counter - start);
            // sleep ms
            // sleep_process(state->rdi);
            break;
        case SYSCALL_GETPID:
            // getpid
            state->rax = current_pid;
            break;
        case SYSCALL_HLT:
            // hlt
            asm("hlt");
            break;
        case 4:
            // yield
            process_table[current_pid].state = PROCESS_STATE_READY;
            add_to_run_queue(current_pid);
            break;
        case 5:
            puts("syscall 5\n");
            break;
        case 6:
            puts("syscall 6\n");
            break;
        case 7:
            puts("syscall 7\n");
            break;
        case 8:
            puts("syscall 8\n");
            break;
        case 9:
            puts("syscall 9\n");
            break;
        case 10:
            puts("syscall 10\n");
            break;
        case 11:
            puts("syscall 11\n");
            break;
        case 12:
            puts("syscall 12\n");
            break;
        case 13:
            puts("syscall 13\n");
            break;
        case 14:
            puts("syscall 14\n");
            break;
        case 15:
            puts("syscall 15\n");
            break;
        case 16:
            puts("syscall 16\n");
            break;
        case 17:
            puts("syscall 17\n");
            break;
        case 18:
            puts("syscall 18\n");
            break;
        case 19:
            puts("syscall 19\n");
            break;
        case 20:
            puts("syscall 20\n");
            break;
        case 21:
            puts("syscall 21\n");
            break;
        case 22:
            puts("syscall 22\n");
            break;
        case 23:
            puts("syscall 23\n");
            break;
        case 24:
            puts("syscall 24\n");
            break;
        case 25:
            puts("syscall 25\n");
            break;
        case 26:
            puts("syscall 26\n");
            break;
        case 27:
            puts("syscall 27\n");
            break;
        case 28:
            puts("syscall 28\n");
            break;
        case 29:
            puts("syscall 29\n");
            break;
    }
}