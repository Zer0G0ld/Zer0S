extern void idt_flush(uint32_t);

void set_idt_gate(int num, uint32_t base) {
    // Definir o endereço de base e os detalhes da entrada da IDT para a interrupção
    idt[num].base_low = base & 0xFFFF;
    idt[num].base_high = (base >> 16) & 0xFFFF;
    idt[num].selector = 0x08;  // O segmento de código, geralmente o código do kernel
    idt[num].always0 = 0;
    idt[num].flags = 0x8E;  // Flags: presente (bit 7), tipo de interrupção (0xE = interrupção)
}

void idt_flush(uint32_t idt_ptr) {
    // Função para carregar a IDT
    __asm__ __volatile__ ("lidt (%0)" : : "r"(idt_ptr));
}
