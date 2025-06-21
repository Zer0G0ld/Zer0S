#include "idt.h"
#include "isr.h"

idt_entry_t idt[IDT_ENTRIES];
idt_ptr_t idt_ptr;

void idt_flush(idt_ptr_t* idt_ptr) {
    __asm__ volatile("lidt (%0)" : : "r"(idt_ptr) : "memory");
}

void idt_init() {
    idt_ptr.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
    idt_ptr.base = (uint32_t)&idt;

    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt[i].base_low = 0;
        idt[i].base_high = 0;
        idt[i].sel = 0;
        idt[i].always0 = 0;
        idt[i].flags = 0;
    }

    idt_flush(&idt_ptr);
}

void set_idt_gate(int num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}
