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
    // Exemplo de como instalar as ISRs
    set_idt_gate(IRQ0_DIVZERO, (uint32_t)isr0, 0x08, 0x8E);  // Registrar a ISR para a interrupção de divisão por zero
    set_idt_gate(IRQ13_GENERAL, (uint32_t)isr_handler, 0x08, 0x8E); // Registrar a ISR para a exceção de proteção geral
    set_idt_gate(IRQ14_PAGEFAULT, (uint32_t)isr_handler, 0x08, 0x8E); // Registrar a ISR para a falha de página
    set_idt_gate(IRQ6_INVALID_OPCODE, (uint32_t)isr_handler, 0x08, 0x8E);  // Registrar a ISR para o erro de opcode inválido
    set_idt_gate(IRQ3_BREAKPOINT, (uint32_t)isr_handler, 0x08, 0x8E);  // Registrar a ISR para o ponto de interrupção

    // Adicione mais interrupções conforme necessário
}

// Função que será chamada quando a interrupção de divisão por zero (ISR0) ocorrer
void isr0(void) {
    // No caso de divisão por zero, chamamos o manipulador para exibir a mensagem e parar o sistema
    registers_t regs = {0};  // Inicializa a estrutura de registros com valores padrão
    regs.interrupt_number = IRQ0_DIVZERO;
    isr_handler(regs);  // Chama o manipulador de interrupção
}
