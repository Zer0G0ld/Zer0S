#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "shell.h"
#include "screen.h"  // Para a exibição de texto no terminal

/* Multiboot Header - Para o GRUB carregar */
__attribute__((section(".multiboot")))
const uint32_t multiboot_header[] = {
    0x1BADB002,                // magic
    0x00,                      // flags
    -(0x1BADB002 + 0x00)       // checksum
};

/* Função para limpar a tela com cor de fundo e texto configuráveis */
void clear_screen(uint8_t bg_color, uint8_t fg_color) {
    uintptr_t vidptr = 0xb8000; // Endereço de vídeo da tela (modo texto)
    uint16_t color = (bg_color << 4) | fg_color;

    for (unsigned int i = 0; i < 80 * 25; ++i) {
        *((uint16_t*)vidptr + i) = (uint16_t)(color | ' ');
    }
}

/* Função para imprimir uma string na tela */
void print_string(const char *str) {
    uintptr_t vidptr = 0xb8000;
    unsigned int i = 0;

    while (str[i] != '\0') {
        *((char*)vidptr + i * 2) = str[i];
        *((char*)vidptr + i * 2 + 1) = 0x07; // Cor: branco no fundo preto
        ++i;
    }
}

void trigger_gpf() {
    asm volatile (
        "mov $0x23, %ax\n\t"  // Seletor de segmento inválido
        "mov %ax, %ds\n\t"
    );
}


/* Shell básico */
void shell_main() {
    print_string("\nWelcome to ShellZer0!\n");
    while (1) {
        // Loop infinito de shell (por enquanto só exibe a mensagem)
    }
}

/* Função principal do kernel */
void kernel_main(void) {
    const char *str = "Hello, OS World!";

    gdt_init();         // Inicializar a GDT
    idt_init();         // Inicializar a estrutura da IDT
    pic_remap();        // Remapear as IRQs (PIC)
    isr_install();      // Instalar ISRs (0-31)
    irq_install();      // Instalar IRQs (32-47)

    asm volatile("sti");  // Habilitar interrupções

    clear_screen(0x00, 0x0F);  // Limpar a tela (preto fundo, branco texto)
    print_string(str);         // Exibir texto inicial

    shell_main();              // Entrar no shell

    while (1) {}               // Loop eterno
}
