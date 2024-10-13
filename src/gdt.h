#ifndef GDT_H
#define GDT_H

#include <stdint.h>

// Estrutura de um descritor de segmento da GDT
typedef struct gdt_entry {
    uint16_t limit_low;      // Parte baixa do limite
    uint16_t base_low;       // Parte baixa da base
    uint8_t base_middle;     // Parte média da base
    uint8_t access;          // Atributos de acesso (como presente, nível de privilégio)
    uint8_t granularity;     // Atributos de granularidade (como tamanho do segmento)
    uint8_t base_high;       // Parte alta da base
} __attribute__((packed)) gdt_entry_t;

// Estrutura do ponteiro para a GDT
typedef struct gdt_ptr {
    uint16_t limit;          // Tamanho da GDT
    uint32_t base;           // Endereço base da GDT
} __attribute__((packed)) gdt_ptr_t;

// Protótipos das funções
void gdt_init(void);        // Função para inicializar a GDT
extern void gdt_flush(uint32_t); // Função para carregar a GDT no registrador GDTR

#endif
