#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "shell.h"
#include "screen.h"
#include "keyboard.h"
#include "kernel.h"
#include <stddef.h>

void clear_screen(uint8_t bg_color, uint8_t fg_color);

__attribute__((section(".multiboot")))
const uint32_t multiboot_header[] = {
    0x1BADB002,
    0x00,
    -(0x1BADB002 + 0x00)
};

int string_compare(const char* str1, const char* str2) {
    int i = 0;
    while (str1[i] != '\0' && str2[i] != '\0') {
        if (str1[i] != str2[i]) return 0;
        i++;
    }
    return (str1[i] == '\0' && str2[i] == '\0');
}

void kernel_panic(const char *msg) {
    clear_screen(0x4, 0x0F);  // Fundo vermelho, texto branco
    screen_write("\nKERNEL PANIC - SISTEMA PARADO!\n\n");
    screen_write(msg);
    screen_write("\n\nSistema travado.\n");

    asm volatile("cli; hlt");
}

void clear_screen(uint8_t bg_color, uint8_t fg_color) {
    uintptr_t vidptr = 0xb8000;
    uint16_t color = (bg_color << 4) | fg_color;

    for (unsigned int i = 0; i < 80 * 25; ++i) {
        *((uint16_t*)vidptr + i) = (uint16_t)(color << 8 | ' ');
    }
}

void print_string(const char *str) {
    static uint16_t *vidmem = (uint16_t*)0xb8000;
    static uint8_t x = 0, y = 0;
    uint16_t attr = 0x0F;  // Texto branco sobre fundo preto

    for (size_t i = 0; str[i] != '\0'; i++) {
        if (str[i] == '\n') {
            x = 0;
            y++;
        } else {
            vidmem[y * 80 + x] = (attr << 8) | str[i];
            x++;
            if (x >= 80) {
                x = 0;
                y++;
            }
        }

        if (y >= 25) {
            x = 0;
            y = 0;
        }
    }
}

// Função dummy para forçar General Protection Fault (testar ISR)
void trigger_gpf() {
    asm volatile (
        "mov $0x23, %ax\n\t"
        "mov %ax, %ds\n\t"
    );
}

void shell_main() {
    print_string("\nWelcome to ShellZer0!\n> ");

    while (1) {
        // Aqui você precisa implementar a leitura do teclado para preencher input_buffer
        // Por hora só para debug, vamos simular que não entra nada e evitar travar.

        // DEBUG: só imprime prompt e espera (sem input real)
        // Comente o bloco abaixo se implementar teclado:
        asm volatile("hlt");

        if (input_index > 0 && input_buffer[input_index - 1] == '\n') {
            input_buffer[input_index - 1] = '\0';

            print_string("\nBuffer recebido: ");
            print_string(input_buffer);
            print_string("\n> ");

            if (string_compare(input_buffer, "clear")) {
                clear_screen(0x00, 0x0F);
                print_string("> ");
            }
            else if (string_compare(input_buffer, "panic")) {
                kernel_panic("Comando panic executado!");
            }
            else if (string_compare(input_buffer, "hello")) {
                print_string("Hello, Zer0!\n> ");
            }
            else {
                print_string("Comando nao reconhecido.\n> ");
            }

            input_index = 0;
        }
    }
}

void kernel_main(void) {
    clear_screen(0x01, 0x0F);
    print_string("Kernel iniciado...\n");

    print_string("Inicializando IDT...\n");
    idt_init();
    print_string("IDT inicializada.\n");

    print_string("Remapeando PIC...\n");
    pic_remap();
    print_string("PIC remapeado.\n");

    print_string("Instalando ISRs...\n");
    isr_install();
    print_string("ISRs instaladas.\n");

    print_string("Instalando IRQs...\n");
    irq_install();
    print_string("IRQs instaladas.\n");

    asm volatile("sti");
    print_string("Interrupcoes habilitadas.\n");

    print_string("Shell iniciando...\n");
    shell_main();

    // Nunca deve chegar aqui
    print_string("Fim do kernel_main\n");
    while (1) {
        asm volatile("hlt");
    }
}
