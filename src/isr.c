#include "isr.h"
#include "idt.h"
#include "screen.h"       // Biblioteca para exibição de mensagens na tela
#include "interrupts.h"   // Arquivo que registra a IDT

// Função que será chamada quando uma interrupção ocorrer
// Trocar isr_handler para um único manipulador centralizado, sem duplicação
void isr_handler(registers_t regs) {
    screen_clear();
    screen_write("Interrupção: ");
    screen_write_int(regs.interrupt_number);
    screen_write("\n");

    switch (regs.interrupt_number) {
        case IRQ0_DIVZERO: screen_write("Erro de Divisão por Zero!\n"); break;
        case IRQ13_GENERAL: screen_write("Erro de Proteção Geral!\n"); break;
        case IRQ14_PAGEFAULT: screen_write("Erro de Falha de Página!\n"); break;
        case IRQ6_INVALID_OPCODE: screen_write("Erro de Opcode Inválido!\n"); break;
        case IRQ3_BREAKPOINT: screen_write("Ponto de Interrupção!\n"); break;
        default: screen_write("Interrupção Desconhecida!\n"); break;
    }

    while (1); // Loop infinito
}


// Função que instala as ISRs na IDT
void isr_install() {
    // Instalar as ISRs para as interrupções
    set_idt_gate(0, (uint32_t)isr0, 0x08, 0x8E);  // Exemplo para ISR0, ajustar para cada caso
    // set_idt_gate(1, (uint32_t)isr1, 0x08, 0x8E);
    // set_idt_gate(2, (uint32_t)isr2, 0x08, 0x8E);
    // Adicionar mais interrupções conforme necessário
}

// Função que será chamada quando a interrupção de divisão por zero (ISR0) ocorrer
void isr0(void) {
    registers_t regs = {0};  // Inicializa a estrutura de registros
    regs.interrupt_number = IRQ0_DIVZERO;
    isr_handler(regs);  // Chama o manipulador de interrupção
}