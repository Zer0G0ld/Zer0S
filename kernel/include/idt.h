#ifndef _IDT_H
#define _IDT_H

#include <stdint.h>

// IDT entry for 64-bit
struct idt_entry {
    uint16_t base_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t flags;
    uint16_t base_mid;
    uint32_t base_high;
    uint32_t reserved;
} __attribute__((packed));

// IDT pointer
struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

// Initialize IDT
void idt_init(void);

// Set an interrupt gate
void idt_set_gate(uint8_t num, uint64_t base, uint16_t selector, uint8_t flags);

// Default interrupt handler
void isr_default(void);

#endif
