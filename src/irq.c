#include "idt.h"
#include "irq.h"
#include "isr.h"
#include "keyboard.h"

#define PIC1 0x20
#define PIC2 0xA0
#define PIC1_COMMAND PIC1
#define PIC1_DATA (PIC1+1)
#define PIC2_COMMAND PIC2
#define PIC2_DATA (PIC2+1)

void outb(uint16_t port, uint8_t val) {
    __asm__ volatile("outb %0, %1" : : "a"(val), "Nd"(port));
}

void pic_remap() {
    uint8_t a1, a2;

    __asm__ volatile ("inb %1, %0" : "=a"(a1) : "Nd"(PIC1_DATA));
    __asm__ volatile ("inb %1, %0" : "=a"(a2) : "Nd"(PIC2_DATA));

    outb(PIC1_COMMAND, 0x11);
    outb(PIC2_COMMAND, 0x11);
    outb(PIC1_DATA, 0x20);  // offset master PIC
    outb(PIC2_DATA, 0x28);  // offset slave PIC
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, 0x01);
    outb(PIC2_DATA, 0x01);

    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);
}

void irq_install() {
    set_idt_gate(32, (uint32_t)irq0, 0x08, 0x8E);
    set_idt_gate(33, (uint32_t)irq1, 0x08, 0x8E);
    set_idt_gate(34, (uint32_t)irq2, 0x08, 0x8E);
    set_idt_gate(35, (uint32_t)irq3, 0x08, 0x8E);
    set_idt_gate(36, (uint32_t)irq4, 0x08, 0x8E);
    set_idt_gate(37, (uint32_t)irq5, 0x08, 0x8E);
    set_idt_gate(38, (uint32_t)irq6, 0x08, 0x8E);
    set_idt_gate(39, (uint32_t)irq7, 0x08, 0x8E);
    set_idt_gate(40, (uint32_t)irq8, 0x08, 0x8E);
    set_idt_gate(41, (uint32_t)irq9, 0x08, 0x8E);
    set_idt_gate(42, (uint32_t)irq10, 0x08, 0x8E);
    set_idt_gate(43, (uint32_t)irq11, 0x08, 0x8E);
    set_idt_gate(44, (uint32_t)irq12, 0x08, 0x8E);
    set_idt_gate(45, (uint32_t)irq13, 0x08, 0x8E);
    set_idt_gate(46, (uint32_t)irq14, 0x08, 0x8E);
    set_idt_gate(47, (uint32_t)irq15, 0x08, 0x8E);
}

void irq_handler(registers_t* regs) {
    if (regs->interrupt_number == 33) { // IRQ1 (teclado)
        keyboard_handler();
    }

    // Enviar EOI (End of Interrupt) pro PIC
    if (regs->interrupt_number >= 40) {
        outb(0xA0, 0x20);   // EOI para o slave PIC
    }
    outb(0x20, 0x20);       // EOI para o master PIC
}