#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Estrutura para os registradores ao entrar em uma ISR
typedef struct {
    uint32_t gs, fs, es, ds;         // Segment registers
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;  // Os registros de propósito geral
    uint32_t interrupt_number;       // Número da interrupção
    uint32_t error_code;             // Código de erro (se houver)
} registers_t;

// Prototipos das funções ISR
void isr_install();
void isr_handler(registers_t regs);

// Definições dos números de interrupção
#define IRQ0_TIMER       32  // Timer Interrupt
#define IRQ1_KEYBOARD    33  // Keyboard Interrupt
#define IRQ14_PAGEFAULT  14  // Page Fault Interrupt
#define IRQ13_GENERAL    13  // General Protection Fault
#define IRQ0_DIVZERO     0   // Divisão por Zero

#endif // ISR_H
