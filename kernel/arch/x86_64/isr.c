#include "idt.h"
#include "keyboard.h"
#include "io.h"
#include <stdint.h>

// Register structure for interrupt context
typedef struct {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
} registers_t;

// Array of exception messages
static const char* exception_messages[] = {
    "Division By Zero",
    "Debug",
    "Non Maskable Interrupt",
    "Breakpoint",
    "Into Detected Overflow",
    "Out of Bounds",
    "Invalid Opcode",
    "No Coprocessor",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Bad TSS",
    "Segment Not Present",
    "Stack Fault",
    "General Protection Fault",
    "Page Fault",
    "Unknown Interrupt",
    "Coprocessor Fault",
    "Alignment Check",
    "Machine Check",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved"
};

// External VGA functions
extern void terminal_writestring(const char* str);
extern void terminal_writechar(char c);
extern void print_hex(unsigned long long value);

// ISR handler
void isr_handler(registers_t* regs) {
    if (regs->int_no < 32) {
        terminal_writestring("\n[EXCEPTION] ");
        terminal_writestring(exception_messages[regs->int_no]);
        terminal_writestring(" (");
        print_hex(regs->int_no);
        terminal_writestring(")\n");
        
        terminal_writestring("Error code: ");
        print_hex(regs->err_code);
        terminal_writestring("\n");
        
        terminal_writestring("RIP: ");
        print_hex(regs->rip);
        terminal_writestring("\n");
        
        terminal_writestring("CS: ");
        print_hex(regs->cs);
        terminal_writestring("\n");
        
        terminal_writestring("RFLAGS: ");
        print_hex(regs->rflags);
        terminal_writestring("\n");
        
        terminal_writestring("RSP: ");
        print_hex(regs->rsp);
        terminal_writestring("\n");
        
        terminal_writestring("SS: ");
        print_hex(regs->ss);
        terminal_writestring("\n");
        
        // Halt on exception
        while (1) {
            __asm__ volatile ("hlt");
        }
    }
}

// IRQ handler
void irq_handler(registers_t* regs) {
    // Handle keyboard interrupt (IRQ1 = 33)
    if (regs->int_no == 33) {
        keyboard_handler();
    }
    
    // Send EOI to PIC
    if (regs->int_no >= 40) {
        // Send to slave
        outb(0xA0, 0x20);
    }
    // Send to master
    outb(0x20, 0x20);
}
