#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "idt.h"

process_t process_table[MAX_PROCESSES] = {0};
int32_t current_pid = -1;

// Queue of empty process slots
uint16_t free_queue[MAX_PROCESSES];
uint16_t free_queue_head = 0;
uint16_t free_queue_tail = 0;

uint16_t run_queue[MAX_PROCESSES];
uint16_t run_queue_head = 0;
uint16_t run_queue_tail = 0;

uint16_t sleep_queue[MAX_PROCESSES];
uint16_t sleep_queue_head = 0;
uint16_t sleep_queue_tail = 0;


void add_to_run_queue(int pid) {
    run_queue[run_queue_tail] = pid;
    run_queue_tail = (run_queue_tail + 1) % MAX_PROCESSES;
}

// 1000 ticks = 1 second.

uint32_t process_create(void (*entry_point)(), int kernel_task) {
    uint16_t pid = free_queue[free_queue_head];
    free_queue_head = (free_queue_head + 1) % MAX_PROCESSES;
    printf("Creating process %d\n", pid);
    process_t *process = &process_table[pid];
    process->pid = pid;
    process->state = PROCESS_STATE_READY;
    process->context = (process_context_t *)malloc(sizeof(process_context_t));
    memset(process->context, 0, sizeof(process_context_t));
    process->context->rip = (uint64_t)entry_point;
    if (kernel_task) {
        process->context->cs = KERNEL_CODE_SEGMENT;
        process->context->ss = KERNEL_DATA_SEGMENT;
    } else {
        process->context->cs = USER_CODE_SEGMENT | 3;
        process->context->ss = USER_DATA_SEGMENT | 3;
    }
    process->context->rflags = 0x202; // IF = 1, IOPL = 0
    process->context->rsp = (uint64_t)malloc(4096) + 4096;
    process->context->rdi = pid;  // Pass the pid as an argument

    add_to_run_queue(pid);
    return pid;
}

int32_t get_next_pid() {
    if (run_queue_head == run_queue_tail) {
        return 0;
    }
    int32_t result = run_queue[run_queue_head];
    run_queue_head = (run_queue_head + 1) % MAX_PROCESSES;
    return result;
}

void start_process(uint32_t pid, process_context_t *context) {
    if (current_pid != -1) {
        memcpy(process_table[current_pid].context, context, sizeof(process_context_t));
    }
    memcpy(context, process_table[pid].context, sizeof(process_context_t));
    process_table[pid].state = PROCESS_STATE_RUNNING;
    process_table[pid].time_remaining = 100;
    current_pid = pid;
}

void schedule_process(process_context_t *context) {
    if (current_pid != -1) {
        process_table[current_pid].total_ticks++;
    }
    if (current_pid == -1) {
        start_process(0, context);
    } else if (current_pid == 0) {
        if (run_queue_head != run_queue_tail) {
            start_process(get_next_pid(), context);
        }
    } else if (process_table[current_pid].state == PROCESS_STATE_RUNNING) {
        process_table[current_pid].time_remaining--;
        if (process_table[current_pid].time_remaining == 0) {
            process_table[current_pid].state = PROCESS_STATE_READY;
            add_to_run_queue(current_pid);
            start_process(get_next_pid(), context);
        }
    } else {
        start_process(get_next_pid(), context);
    }
}

void sleep_process(uint32_t time) {
    int pid = current_pid;
    process_table[pid].time_remaining = time;
    process_table[pid].state = PROCESS_STATE_SLEEPING;

    sleep_queue[sleep_queue_tail] = pid;
    sleep_queue_tail = (sleep_queue_tail + 1) % MAX_PROCESSES;
    int sorted = 1;

    for (int i = sleep_queue_head; i != sleep_queue_tail; i = (i + 1) % MAX_PROCESSES) {
        if (process_table[sleep_queue[i]].time_remaining < process_table[pid].time_remaining) {
            sorted = 0;
            break;
        }
    }
    if (sorted) return;
    for (int i = sleep_queue_head; i != sleep_queue_tail; i = (i + 1) % MAX_PROCESSES) {
        for (int j = i; j != sleep_queue_tail; j = (j + 1) % MAX_PROCESSES) {
            if (process_table[sleep_queue[i]].time_remaining > process_table[sleep_queue[j]].time_remaining) {
                int temp = sleep_queue[i];
                sleep_queue[i] = sleep_queue[j];
                sleep_queue[j] = temp;
            }
        }
    }
}

void init(int);
void user_task(int);

void setup_process() {
    for (int i = 0; i < MAX_PROCESSES; i++) {
        free_queue[i] = i;
    }
    disable();
    process_create(init, 1);
    // remove init from run_queue
    run_queue_head = (run_queue_head + 1) % MAX_PROCESSES;
    for(int i=0; i<4; ++i)
        process_create(user_task, 0);
    enable();
}

void print_run_queue() {
    printf("Run queue: ");
    for (int i = run_queue_head; i != run_queue_tail; i = (i + 1) % MAX_PROCESSES) {
        printf("%d ", run_queue[i]);
    }
    printf("\n");
}
