#ifndef _PIC_H
#define _PIC_H

#include <stdint.h>

void pic_remap(void);
void pic_enable_irq(uint8_t irq);
void pic_disable_irq(uint8_t irq);

#endif
