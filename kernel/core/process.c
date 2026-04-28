#include "process.h"
#include <stdint.h>
#include <stddef.h>

extern void terminal_writestring(const char* str);
extern void print_hex(uint64_t value);
extern void* kmalloc(size_t size);
extern void kfree(void* ptr);

// Função simples de memset (já que não temos string.h funcionando)
static void* my_memset(void* s, int c, size_t n) {
    unsigned char* p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

// Process table
static process_t processes[MAX_PROCS];
static process_t* current_process = NULL;
static process_t* ready_queue = NULL;
static uint64_t next_pid = 1;

// Find free process slot
static process_t* find_free_process(void) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (processes[i].state == PROC_UNUSED || processes[i].state == PROC_DEAD) {
            return &processes[i];
        }
    }
    return NULL;
}

// Initialize process manager
void process_init(void) {
    terminal_writestring("\n[Process] Initializing process manager...\n");
    for (int i = 0; i < MAX_PROCS; i++) {
        my_memset(&processes[i], 0, sizeof(process_t));
        processes[i].state = PROC_UNUSED;
        processes[i].pid = 0;
    }
    terminal_writestring("[Process] Process manager ready\n");
}

// Create a new process
process_t* process_create(const char* name, void* entry, uint64_t priority, int flags) {
    process_t* proc = find_free_process();
    if (!proc) return NULL;
    
    my_memset(proc, 0, sizeof(process_t));
    
    proc->pid = next_pid++;
    proc->state = PROC_READY;
    proc->priority = priority;
    proc->flags = flags;
    
    // Copy name
    int i = 0;
    while (name[i] && i < PROC_NAME_MAX - 1) {
        proc->name[i] = name[i];
        i++;
    }
    proc->name[i] = '\0';
    
    // Allocate kernel stack
    proc->kernel_stack_size = PROC_STACK_SIZE;
    proc->kernel_stack = (uint64_t*)kmalloc(proc->kernel_stack_size);
    if (!proc->kernel_stack) return NULL;
    
    // Setup context
    proc->context.rsp = (uint64_t)proc->kernel_stack + proc->kernel_stack_size;
    proc->context.rip = (uint64_t)entry;
    proc->context.rflags = 0x202;
    
    // Add to ready queue
    proc->next = ready_queue;
    ready_queue = proc;
    
    terminal_writestring("[Process] Created: ");
    terminal_writestring(name);
    terminal_writestring(" (PID: ");
    print_hex(proc->pid);
    terminal_writestring(")\n");
    
    return proc;
}

// Destroy a process
void process_destroy(process_t* proc) {
    if (!proc) return;
    
    terminal_writestring("[Process] Destroying: ");
    terminal_writestring(proc->name);
    terminal_writestring(" (PID: ");
    print_hex(proc->pid);
    terminal_writestring(")\n");
    
    if (proc->kernel_stack) kfree(proc->kernel_stack);
    proc->state = PROC_DEAD;
}

// Exit current process
void process_exit(int code) {
    if (!current_process) return;
    terminal_writestring("[Process] Exit: ");
    terminal_writestring(current_process->name);
    terminal_writestring(" with code ");
    print_hex(code);
    terminal_writestring("\n");
    current_process->state = PROC_ZOMBIE;
    process_yield();
}

// Yield CPU - simple round robin
void process_yield(void) {
    if (!ready_queue) return;
    
    if (current_process && current_process->state == PROC_RUNNING) {
        current_process->state = PROC_READY;
        // Add to end of queue
        process_t* last = ready_queue;
        while (last->next) last = last->next;
        last->next = current_process;
        current_process->next = NULL;
    }
    
    current_process = ready_queue;
    ready_queue = ready_queue->next;
    current_process->next = NULL;
    current_process->state = PROC_RUNNING;
    current_process->stats.total_ticks++;
}

// Sleep for ticks
void process_sleep(uint64_t ticks) {
    if (!current_process) return;
    current_process->sleep_until = ticks;
    current_process->state = PROC_SLEEPING;
    process_yield();
}

// Wake up a sleeping process
void process_wakeup(process_t* proc) {
    if (!proc || proc->state != PROC_SLEEPING) return;
    proc->state = PROC_READY;
    proc->next = ready_queue;
    ready_queue = proc;
}

// Get current process
process_t* process_get_current(void) {
    return current_process;
}

// Get process by PID
process_t* process_get_by_pid(uint64_t pid) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (processes[i].pid == pid && processes[i].state != PROC_UNUSED) {
            return &processes[i];
        }
    }
    return NULL;
}

// List all processes
void process_list(void) {
    terminal_writestring("\n=== Process List ===\n");
    for (int i = 0; i < MAX_PROCS; i++) {
        if (processes[i].state != PROC_UNUSED && processes[i].state != PROC_DEAD) {
            print_hex(processes[i].pid);
            terminal_writestring(" │ ");
            terminal_writestring(processes[i].name);
            terminal_writestring(" │ ");
            switch (processes[i].state) {
                case PROC_READY:   terminal_writestring("READY"); break;
                case PROC_RUNNING: terminal_writestring("RUNNING"); break;
                case PROC_SLEEPING: terminal_writestring("SLEEPING"); break;
                case PROC_ZOMBIE:  terminal_writestring("ZOMBIE"); break;
                default:           terminal_writestring("UNKNOWN"); break;
            }
            terminal_writestring("\n");
        }
    }
    terminal_writestring("===================\n");
}

// Show process info
void process_info(process_t* proc) {
    if (!proc) return;
    terminal_writestring("\nPID: ");
    print_hex(proc->pid);
    terminal_writestring("\nName: ");
    terminal_writestring(proc->name);
    terminal_writestring("\nState: ");
    print_hex(proc->state);
    terminal_writestring("\nPriority: ");
    print_hex(proc->priority);
    terminal_writestring("\n");
}

// Show process stats
void process_stats(process_t* proc) {
    if (!proc) return;
    terminal_writestring("\n=== Process Stats ===\n");
    terminal_writestring("Total ticks: ");
    print_hex(proc->stats.total_ticks);
    terminal_writestring("\n");
}

// Initialize process manager
void process_init(void) {
    terminal_writestring("\n[Process] Initializing process manager...\n");
    for (int i = 0; i < MAX_PROCS; i++) {
        my_memset(&processes[i], 0, sizeof(process_t));
        processes[i].state = PROC_UNUSED;
    }
    terminal_writestring("[Process] Process manager ready\n");
}

// Create a new process
process_t* process_create(const char* name, void* entry, uint64_t priority, int flags) {
    process_t* proc = find_free_process();
    if (!proc) return NULL;
    
    my_memset(proc, 0, sizeof(process_t));
    proc->pid = next_pid++;
    proc->state = PROC_READY;
    proc->priority = priority;
    proc->flags = flags;
    
    int i = 0;
    while (name[i] && i < PROC_NAME_MAX - 1) {
        proc->name[i] = name[i];
        i++;
    }
    proc->name[i] = '\0';
    
    proc->kernel_stack_size = PROC_STACK_SIZE;
    proc->kernel_stack = (uint64_t*)kmalloc(proc->kernel_stack_size);
    if (!proc->kernel_stack) return NULL;
    
    proc->context.rsp = (uint64_t)proc->kernel_stack + proc->kernel_stack_size;
    proc->context.rip = (uint64_t)entry;
    proc->context.rflags = 0x202;
    
    proc->next = ready_queue;
    ready_queue = proc;
    
    terminal_writestring("[Process] Created: ");
    terminal_writestring(name);
    terminal_writestring(" (PID: ");
    print_hex(proc->pid);
    terminal_writestring(")\n");
    
    return proc;
}

// Destroy a process
void process_destroy(process_t* proc) {
    if (!proc) return;
    if (proc->kernel_stack) kfree(proc->kernel_stack);
    proc->state = PROC_DEAD;
}

// Exit current process
void process_exit(int code) {
    if (!current_process) return;
    current_process->state = PROC_ZOMBIE;
    process_yield();
}

// Yield CPU
void process_yield(void) {
    if (!ready_queue) return;
    
    if (current_process && current_process->state == PROC_RUNNING) {
        current_process->state = PROC_READY;
        process_t* last = ready_queue;
        while (last->next) last = last->next;
        last->next = current_process;
        current_process->next = NULL;
    }
    
    current_process = ready_queue;
    ready_queue = ready_queue->next;
    current_process->next = NULL;
    current_process->state = PROC_RUNNING;
}

// Sleep
void process_sleep(uint64_t ticks) {
    if (!current_process) return;
    current_process->sleep_until = ticks;
    current_process->state = PROC_SLEEPING;
    process_yield();
}

// Wake up
void process_wakeup(process_t* proc) {
    if (!proc || proc->state != PROC_SLEEPING) return;
    proc->state = PROC_READY;
    proc->next = ready_queue;
    ready_queue = proc;
}

// Get current process
process_t* process_get_current(void) {
    return current_process;
}

// Get process by PID
process_t* process_get_by_pid(uint64_t pid) {
    for (int i = 0; i < MAX_PROCS; i++) {
        if (processes[i].pid == pid && processes[i].state != PROC_UNUSED) {
            return &processes[i];
        }
    }
    return NULL;
}

// List processes
void process_list(void) {
    terminal_writestring("\n=== Process List ===\n");
    for (int i = 0; i < MAX_PROCS; i++) {
        if (processes[i].state != PROC_UNUSED && processes[i].state != PROC_DEAD) {
            print_hex(processes[i].pid);
            terminal_writestring(" │ ");
            terminal_writestring(processes[i].name);
            terminal_writestring(" │ ");
            switch (processes[i].state) {
                case PROC_READY:   terminal_writestring("READY"); break;
                case PROC_RUNNING: terminal_writestring("RUNNING"); break;
                case PROC_SLEEPING: terminal_writestring("SLEEPING"); break;
                case PROC_ZOMBIE:  terminal_writestring("ZOMBIE"); break;
                default:           terminal_writestring("UNKNOWN"); break;
            }
            terminal_writestring("\n");
        }
    }
    terminal_writestring("===================\n");
}

// Process info
void process_info(process_t* proc) {
    if (!proc) return;
    terminal_writestring("\nPID: ");
    print_hex(proc->pid);
    terminal_writestring("\nName: ");
    terminal_writestring(proc->name);
    terminal_writestring("\nState: ");
    print_hex(proc->state);
    terminal_writestring("\nPriority: ");
    print_hex(proc->priority);
    terminal_writestring("\n");
}

// Process stats
void process_stats(process_t* proc) {
    if (!proc) return;
    terminal_writestring("\n=== Process Stats ===\n");
    terminal_writestring("Total ticks: ");
    print_hex(proc->stats.total_ticks);
    terminal_writestring("\n");
}
