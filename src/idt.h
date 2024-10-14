#ifndef IDT_H
#define IDT_H

#include <stdint.h>

// Definição da estrutura do descritor IDT
typedef struct {
    uint16_t base_low;
    uint16_t sel;
    uint8_t always0;
    uint8_t flags;
    uint16_t base_high;
} __attribute__((packed)) idt_entry_t;

typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

// IDT com 256 entradas
#define IDT_ENTRIES 256
extern idt_entry_t idt[IDT_ENTRIES];
extern idt_ptr_t idt_ptr;

// Funções para configurar a IDT
void idt_init(void);
void set_idt_gate(int num, uint32_t base, uint16_t sel, uint8_t flags);

#endif
