#include <stdint.h>
#include "idt.h"

extern void idt_flush(uint32_t idt_ptr);

void set_idt_gate(int num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

void idt_flush(uint32_t idt_ptr) {
    // Função para carregar a IDT
    __asm__ __volatile__ ("lidt (%0)" : : "r"(idt_ptr));
}
