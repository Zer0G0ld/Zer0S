#include "terminal.h"
#include <stdint.h>

/* Protótipo da função kernel_main */
void kernel_main(void);

// Função principal do kernel
void kernel_main() {
    terminal_run();  // Inicia o terminal
}

/* Cabeçalho Multiboot */
#define MULTIBOOT_HEADER_MAGIC   0x1BADB002
#define MULTIBOOT_HEADER_FLAGS   0x0
#define MULTIBOOT_HEADER_CHECKSUM (-(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS))

typedef struct multiboot_header {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
    uint32_t header_addr;
    uint32_t load_addr;
    uint32_t load_end_addr;
    uint32_t bss_end_addr;
    uint32_t entry_addr;
} multiboot_header_t;

/* Definir o cabeçalho multiboot, sem a inicialização do entry_addr */
multiboot_header_t __attribute__((section(".multiboot"))) multiboot_header = {
    .magic = MULTIBOOT_HEADER_MAGIC,
    .flags = MULTIBOOT_HEADER_FLAGS,
    .checksum = MULTIBOOT_HEADER_CHECKSUM,
    .header_addr = 0,
    .load_addr = 0,
    .load_end_addr = 0,
    .bss_end_addr = 0,
    .entry_addr = 0 // Inicialize com 0 e depois defina em tempo de execução
};

/* Função para configurar o cabeçalho multiboot */
void setup_multiboot_header(void) {
    extern void kernel_main(void);
    multiboot_header.entry_addr = (uintptr_t)&kernel_main; // Definir o endereço da função kernel_main
}

/* Função para imprimir uma string na tela */
void print_string(const char *str) {
    uintptr_t vidptr = 0xb8000; // Endereço de vídeo da tela
    unsigned int i = 0;

    // Percorre a string e imprime cada caractere
    while (str[i] != '\0') {
        *((char*)vidptr + i * 2) = str[i];       // Coloca o caractere
        *((char*)vidptr + i * 2 + 1) = 0x07;    // Cor (branco sobre fundo preto)
        ++i;
    }
}

/* Função principal do kernel */
void kernel_main(void) {
    const char *str = "Hello, OS World!";
    
    // Limpa a tela (80 colunas por 25 linhas, cada célula tem 2 bytes: caractere e atributo de cor)
    uintptr_t vidptr = 0xb8000; // Endereço de vídeo da tela
    for (unsigned int j = 0; j < 80 * 25; ++j) {
        *((char*)vidptr + j * 2) = ' ';       // Caractere vazio
        *((char*)vidptr + j * 2 + 1) = 0x07;  // Atributo de cor (branco sobre fundo preto)
    }

    // Exibe a string "Hello, OS World!" na tela
    print_string(str);

    // Fica preso no laço, para que o kernel não termine
    while (1) { }
}
