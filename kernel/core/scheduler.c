#include "scheduler.h"
#include "process.h"

extern void terminal_writestring(const char* str);

static process_t* ready_list = NULL;
static uint64_t tick_count = 0;

void scheduler_init(void) {
    terminal_writestring("[Scheduler] Initializing round-robin scheduler...\n");
    ready_list = NULL;
    tick_count = 0;
    terminal_writestring("[Scheduler] Ready\n");
}

void scheduler_add(process_t* proc) {
    if (!proc) return;
    
    proc->state = PROC_READY;  // Mudado de PROC_RUNNABLE para PROC_READY
    proc->next = ready_list;
    ready_list = proc;
    
    terminal_writestring("[Scheduler] Added process to ready queue\n");
}

void scheduler_remove(process_t* proc) {
    if (!proc) return;
    
    process_t** current = &ready_list;
    while (*current) {
        if (*current == proc) {
            *current = proc->next;
            proc->next = NULL;
            break;
        }
        current = &(*current)->next;
    }
}

void scheduler_tick(void) {
    tick_count++;
}

void scheduler_yield(void) {
    if (ready_list && ready_list->next) {
        process_t* first = ready_list;
        ready_list = ready_list->next;
        
        process_t* last = ready_list;
        while (last->next) last = last->next;
        last->next = first;
        first->next = NULL;
    }
}
