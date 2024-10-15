#include <stdint.h>
#include "gdt.h"
#include "interrupts.h"
#include "shell.h"
#include "screen.h"  // Para a exibição de texto no terminal

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

typedef struct {
    uint32_t esp;
    uint32_t ebp;
    // Outros registros e informações do processo
} process_t;

/* Definir o cabeçalho multiboot */
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

void shell_main() {
    print_string("Welcome to ShellZer0!\n");
    // Aqui você pode implementar o loop principal do shell
    while (1) {
        // Aguardar comandos e executá-los
    }
}

/* Função para limpar a tela com cor de fundo e texto configuráveis */
void clear_screen(uint8_t bg_color, uint8_t fg_color) {
    uintptr_t vidptr = 0xb8000; // Endereço de vídeo da tela
    uint16_t color = (bg_color << 4) | fg_color; // Combina cores de fundo e texto

    // Limpa a tela
    for (unsigned int i = 0; i < 80 * 25; ++i) {
        *((uint16_t*)vidptr + i) = (uint16_t)(color | ' '); // Caractere ' ' com a cor definida
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

    // Inicializa a GDT (agora corretamente chamada de gdt_init de gdt.c)
    gdt_init();  // Chama a função de inicialização da GDT de gdt.c

    // Inicializa as interrupções
    interrupts_init();
    
    clear_screen(0x00, 0x0F);       // Limpa a tela com fundo preto e texto branco
    print_string(str);    // Exibe a string

    // Inicia o terminal ShellZer0
    shell_main(); // Chama a função do terminal para interação com o usuário

    // Fica preso no laço, para que o kernel não termine
    while (1) { }
}
