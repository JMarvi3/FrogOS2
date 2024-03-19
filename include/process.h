#include <stdint.h>
#include <gdt.h>

#define MAX_PROCESSES 128

typedef struct {
    uint64_t ds, es, fs, gs; // Segment registers
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t unused1, unused2;
    uint64_t rip, cs, rflags, rsp, ss;
} process_context_t;

#define PROCESS_STATE_RUNNING 0
#define PROCESS_STATE_SLEEPING 1
#define PROCESS_STATE_WAITING 2
#define PROCESS_STATE_READY 3

typedef struct {
    uint32_t pid;
    uint32_t time_remaining;
    uint32_t state;
    uint32_t total_ticks;
    process_context_t *context;
} process_t;

extern process_t process_table[MAX_PROCESSES];
extern int32_t current_pid;

// Queue of empty process slots
extern uint16_t free_queue[MAX_PROCESSES];
extern uint16_t free_queue_head;
extern uint16_t free_queue_tail;

extern uint16_t run_queue[MAX_PROCESSES];
extern uint16_t run_queue_head;
extern uint16_t run_queue_tail;

extern uint16_t sleep_queue[MAX_PROCESSES];
extern uint16_t sleep_queue_head;
extern uint16_t sleep_queue_tail;

uint32_t process_create(void (*entry_point)(), int kernel_task);
void add_to_run_queue(int pid);
void sleep_process(uint32_t time);
void schedule_process(process_context_t *context);
