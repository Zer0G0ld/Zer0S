#include "isr.h"
#include "interrupts.h"  // Arquivo que registra a IDT

// Função que será chamada quando uma interrupção ocorrer
void isr_handler(registers_t regs) {
    // Exibir o número da interrupção para depuração
    // Aqui você pode adicionar código para lidar com as exceções ou interrupções
    switch (regs.interrupt_number) {
        case IRQ0_DIVZERO:
            // Lidar com erro de divisão por zero
            break;
        case IRQ13_GENERAL:
            // Lidar com erro de proteção geral
            break;
        case IRQ14_PAGEFAULT:
            // Lidar com falha de página
            break;
        default:
            // Outras interrupções
            break;
    }
}

// Função que instala as ISRs na IDT
void isr_install() {
    // Para cada ISR, registramos a função apropriada na IDT
    // Usaremos uma função que você já deve ter em `interrupts.c`, como `set_idt_gate`
    
    // Exemplo:
    set_idt_gate(0, (uint32_t)isr_handler); // Registrar uma ISR para o número 0 (divisão por zero)
    set_idt_gate(13, (uint32_t)isr_handler); // Registrar a ISR para a exceção de proteção geral
    set_idt_gate(14, (uint32_t)isr_handler); // Registrar a ISR para a falha de página
    // Você pode adicionar mais interrupções conforme necessário
}
