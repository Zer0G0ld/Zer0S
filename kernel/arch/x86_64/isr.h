#ifndef _ISR_H
#define _ISR_H

#include <stdint.h>

void isr_handler(void* regs);
void irq_handler(void* regs);

#endif
