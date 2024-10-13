#include "gdt.h"

gdt_entry_t gdt_entries[5];  // 5 entradas, uma para cada segmento (segundo o padrão x86)
gdt_ptr_t gdt_ptr;           // Ponteiro para a GDT

// Função que inicializa a GDT
void gdt_init(void) {
    // Configura a GDT com descritores válidos

    // Inicializa a GDT com entradas "vazias"
    gdt_ptr.limit = (sizeof(gdt_entry_t) * 5) - 1;  // 5 entradas de 8 bytes cada
    gdt_ptr.base = (uint32_t)&gdt_entries;            // Endereço base da GDT

    // Definir os descritores válidos aqui (entry 1 a entry 4, por exemplo)
    gdt_set_gate(0, 0, 0, 0, 0);  // Entrada de nulidade
    gdt_set_gate(1, 0, 0xFFFFFFFF, 0x9A, 0xCF);  // Código de segmento (exemplo)
    gdt_set_gate(2, 0, 0xFFFFFFFF, 0x92, 0xCF);  // Dados de segmento (exemplo)
    gdt_set_gate(3, 0x0, 0xFFFFFFFF, 0xFA, 0xCF);  // Tarefa (Task)
    gdt_set_gate(4, 0, 0xFFFFFFFF, 0xF2, 0xCF);  // Tarefa (Task)

    // Carregar a GDT
    gdt_flush((uint32_t)&gdt_ptr);
}

// Função que configura uma entrada da GDT
void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t granularity) {
    gdt_entries[num].base_low = base & 0xFFFF;
    gdt_entries[num].base_middle = (base >> 16) & 0xFF;
    gdt_entries[num].base_high = (base >> 24) & 0xFF;

    gdt_entries[num].limit_low = limit & 0xFFFF;
    gdt_entries[num].granularity = (limit >> 16) & 0x0F;
    gdt_entries[num].granularity |= granularity & 0xF0;

    gdt_entries[num].access = access;
}
