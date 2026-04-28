#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>
#include <stddef.h>

// Process states
#define PROC_UNUSED     0
#define PROC_NEW        1
#define PROC_READY      2
#define PROC_RUNNING    3
#define PROC_WAITING    4
#define PROC_SLEEPING   5
#define PROC_STOPPED    6
#define PROC_ZOMBIE     7
#define PROC_DEAD       8

// Maximum processes
#define MAX_PROCS       64
#define PROC_STACK_SIZE 8192
#define PROC_NAME_MAX   32

// Process structure
typedef struct process {
    uint64_t pid;
    uint64_t ppid;
    int state;
    int flags;
    int priority;
    int nice;
    char name[PROC_NAME_MAX];
    
    // Context
    struct {
        uint64_t rax, rbx, rcx, rdx;
        uint64_t rsi, rdi, rbp, rsp;
        uint64_t r8, r9, r10, r11;
        uint64_t r12, r13, r14, r15;
        uint64_t rip, rflags;
        uint64_t cr3;
        uint64_t cs, ds, es, fs, gs, ss;
        uint8_t fpu_state[512];
        int fpu_used;
    } context;
    
    uint64_t* kernel_stack;
    uint64_t* user_stack;
    size_t kernel_stack_size;
    size_t user_stack_size;
    
    uint64_t remaining_ticks;
    uint64_t total_ticks;
    uint64_t sleep_until;
    
    struct process* next;
    struct process* prev;
    struct process* parent;
    struct process* children;
    
    uint64_t start_time;
    uint64_t exit_code;
    
    struct {
        uint64_t user_ticks;
        uint64_t kernel_ticks;
        uint64_t total_ticks;
        uint64_t voluntary_yields;
        uint64_t involuntary_yields;
        uint64_t page_faults;
        uint64_t syscalls;
        uint64_t ipc_sent;
        uint64_t ipc_received;
    } stats;
    
} process_t;

// Function declarations
void process_init(void);
process_t* process_create(const char* name, void* entry, uint64_t priority, int flags);
void process_destroy(process_t* proc);
void process_exit(int code);
void process_yield(void);
void process_sleep(uint64_t ticks);
void process_wakeup(process_t* proc);
void process_set_priority(process_t* proc, int priority);
process_t* process_get_current(void);
process_t* process_get_by_pid(uint64_t pid);
void process_list(void);
void process_info(process_t* proc);
void process_stats(process_t* proc);

#endif
