#include <stdint.h>

/* Protótipo da função kernel_main */
void kernel_main(void);

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

/* Definir o cabeçalho multiboot, sem inicialização de entry_addr */
multiboot_header_t __attribute__((section(".multiboot"))) multiboot_header = {
    .magic = MULTIBOOT_HEADER_MAGIC,
    .flags = MULTIBOOT_HEADER_FLAGS,
    .checksum = MULTIBOOT_HEADER_CHECKSUM,
    .header_addr = 0,
    .load_addr = 0,
    .load_end_addr = 0,
    .bss_end_addr = 0,
    .entry_addr = 0 // Inicialize como 0
};

/* Função para limpar a tela */
void clear_screen() {
    uintptr_t vidptr = 0xb8000; // Endereço de vídeo da tela
    for (unsigned int j = 0; j < 80 * 25; ++j) {
        *((char*)vidptr + j * 2) = ' ';       // Caractere vazio
        *((char*)vidptr + j * 2 + 1) = 0x07;  // Atributo de cor (branco sobre fundo preto)
    }
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

    // Inicializa o entry_addr após o kernel ser carregado
    multiboot_header.entry_addr = (uintptr_t)&kernel_main;

    clear_screen();       // Limpa a tela
    print_string(str);    // Exibe a string

    // Fica preso no laço, para que o kernel não termine
    while (1) { }
}
