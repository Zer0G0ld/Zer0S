#ifndef ISR_H
#define ISR_H

#include <stdint.h>

// Estrutura para os registradores ao entrar em uma ISR
// A ordem dos campos segue exatamente o empilhamento feito no assembly,
// começando com interrupt_number e error_code (ou zero), depois os registradores gerais,
// e por último os registradores de segmento.
typedef struct {
    uint32_t interrupt_number;  // pushado antes do pusha
    uint32_t error_code;        // pushado antes do pusha (ou zero)
    uint32_t edi;
    uint32_t esi;
    uint32_t ebp;
    uint32_t esp;   // valor salvo pelo pusha, atenção: esp real pode ser diferente
    uint32_t ebx;
    uint32_t edx;
    uint32_t ecx;
    uint32_t eax;
    uint32_t ds;    // pushado no assembly antes do pusha (segment registers)
    uint32_t es;
    uint32_t fs;
    uint32_t gs;
} registers_t;

// Protótipos das funções ISR
void isr_install();
void isr_handler(registers_t regs);

// Declaração das funções ISR (implementadas em assembly)
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr9(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr15(void);
extern void isr16(void);
extern void isr17(void);
extern void isr18(void);
extern void isr19(void);
extern void isr20(void);
extern void isr21(void);
extern void isr22(void);
extern void isr23(void);
extern void isr24(void);
extern void isr25(void);
extern void isr26(void);
extern void isr27(void);
extern void isr28(void);
extern void isr29(void);
extern void isr30(void);
extern void isr31(void);

// Definições dos números de interrupção (IRQ - Interrupt Request)
#define IRQ0_TIMER       32  // Timer Interrupt
#define IRQ1_KEYBOARD    33  // Keyboard Interrupt
#define IRQ14_PAGEFAULT  14  // Page Fault Interrupt
#define IRQ13_GENERAL    13  // General Protection Fault
#define IRQ0_DIVZERO     0   // Divisão por Zero Interrupt (Exceção)

// Definições adicionais de interrupções e exceções
#define IRQ6_INVALID_OPCODE  6  // Exceção: Invalid Opcode
#define IRQ3_BREAKPOINT      3  // Exceção: Breakpoint

#endif // ISR_H
