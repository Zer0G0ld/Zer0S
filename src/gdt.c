#include <stdint.h>
#include "gdt.h"
#include <stddef.h>  // Para uintptr_t

// Define 5 entradas para a GDT (zero, código, dados, etc)
gdt_entry_t gdt_entries[5];
gdt_ptr_t gdt_ptr;  // Ponteiro usado pelo lgdt

// Protótipo da função que configura uma entrada da GDT
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity);

// Inicializa a GDT
void gdt_init(void) {
    // Define o tamanho e o endereço base da GDT
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;
    gdt_ptr.base = (uint32_t)&gdt_entries;

    // Configura as entradas da GDT
    gdt_set_gate(0, 0, 0, 0, 0);              // Null segment (obrigatório)
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);  // Código (executable, readable, accessed)
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);  // Dados (read/write, accessed)

    // Removidas as tarefas (opcional)
    // gdt_set_gate(3, 0, 0xFFFFFFFF, 0xFA, 0xCF);  // Task segment (exemplo)
    // gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);  // Task segment (exemplo)

    // Carrega a GDT com a instrução LGDT
    gdt_flush((uint32_t)&gdt_ptr);
}

// Configura uma entrada específica na GDT
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt_entries[num].base_low = base & 0xFFFF;
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low = limit & 0xFFFF;
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    gdt_entries[num].granularity |= granularity & 0xF0;

    gdt_entries[num].access = access;
}
