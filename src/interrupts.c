#include <stdint.h>
#include "idt.h"

extern void idt_flush(uint32_t idt_ptr);

// Declare a estrutura para a IDT corretamente
extern idt_entry_t idt[IDT_ENTRIES];  // Usando o nome correto

void interrupts_init() {
    // Inicializa a IDT
    uintptr_t idt_ptr = (uintptr_t)&idt;  // Use uintptr_t para armazenar um ponteiro

    // Limpa a IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt[i].base_low = 0;
        idt[i].base_high = 0;
        idt[i].sel = 0;
        idt[i].always0 = 0;
        idt[i].flags = 0;
    }

    // Agora você pode definir as entradas da IDT usando set_idt_gate()

    // Exemplo de configuração para interrupção de hardware
    set_idt_gate(0x20, (uint32_t)isr0, 0x08, 0x8E);  // Exemplo para interrupção 32 (timer)

    // Finalmente, carregue a IDT
    idt_flush((uint32_t)idt_ptr);
}
