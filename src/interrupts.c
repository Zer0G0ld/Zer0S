#include <stdint.h>
#include "idt.h"
#include "isr.h"

extern void idt_flush(uint32_t idt_ptr);

// Estrutura para a IDT
extern idt_entry_t idt[IDT_ENTRIES];  // Usando a estrutura definida no arquivo idt.h

// Função para inicializar as interrupções
void interrupts_init() {
    // Ponteiro para a IDT
    uintptr_t idt_ptr = (uintptr_t)&idt;

    // Limpa todas as entradas da IDT, garantindo que estejam configuradas para valores padrão
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt[i].base_low = 0;
        idt[i].base_high = 0;
        idt[i].sel = 0;      // Seleciona o código de segmento (geralmente 0x08 para o kernel)
        idt[i].always0 = 0;  // Sempre 0
        idt[i].flags = 0;    // Flags padrão
    }

    // Configura a entrada de interrupção 32 para o temporizador
    // Esta é a interrupção de timer, que geralmente é configurada para
    // gerar o "tick" do sistema operacional
    set_idt_gate(0x20, (uint32_t)isr0, 0x08, 0x8E);  // Parâmetros:
                                                    // 0x20: IDT para interrupção 32 (temporizador)
                                                    // (uint32_t)isr0: Ponteiro para o manipulador
                                                    // 0x08: Selecionando o código de segmento do kernel
                                                    // 0x8E: Flags (interrupção de privilégio 0, tipo 14, etc.)

    // Configura outras interrupções aqui, se necessário, por exemplo:
    // set_idt_gate(0x21, (uint32_t)isr1, 0x08, 0x8E); // Exemplo para a interrupção do teclado

    // Finalmente, carrega a IDT para que o processador comece a usá-la
    idt_flush((uint32_t)idt_ptr);
}
