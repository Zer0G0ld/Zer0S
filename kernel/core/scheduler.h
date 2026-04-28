#ifndef _SCHEDULER_H
#define _SCHEDULER_H

#include "process.h"

void scheduler_init(void);
void scheduler_add(process_t* proc);
void scheduler_remove(process_t* proc);
void scheduler_yield(void);
void scheduler_tick(void);  // Called by timer interrupt

#endif
