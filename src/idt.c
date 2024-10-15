#include "idt.h"

// Definindo a IDT e o ponteiro da IDT
idt_entry_t idt[IDT_ENTRIES];
idt_ptr_t idt_ptr;

// Função que inicializa a IDT
void idt_init() {
    // Configurar o ponteiro para a IDT
    idt_ptr.limit = (sizeof(idt_entry_t) * IDT_ENTRIES) - 1;
    idt_ptr.base = (uint32_t)&idt;

    // Limpar a IDT
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt[i].base_low = 0;
        idt[i].base_high = 0;
        idt[i].sel = 0;
        idt[i].always0 = 0;
        idt[i].flags = 0;
    }

    // Agora, as ISRs precisam ser configuradas. Exemplo:
    // set_idt_gate(0, (uint32_t)isr_handler);  // exemplo de uso para registrar uma ISR

    // Carregar a IDT
    idt_flush((uint32_t)&idt_ptr);
}

// Função para definir um "gate" da IDT
void set_idt_gate(int num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}
