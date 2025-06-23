#include <stdint.h>
#include "gdt.h"
#include "idt.h"
#include "isr.h"
#include "irq.h"
#include "shell.h"
#include "screen.h"
#include "keyboard.h"
#include "kernel.h"

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
        *((uint16_t*)vidptr + i) = (uint16_t)(color | ' ');
    }
}

void print_string(const char *str) {
    uintptr_t vidptr = 0xb8000;
    unsigned int i = 0;

    while (str[i] != '\0') {
        *((char*)vidptr + i * 2) = str[i];
        *((char*)vidptr + i * 2 + 1) = 0x07;
        ++i;
    }
}

void trigger_gpf() {
    asm volatile (
        "mov $0x23, %ax\n\t"
        "mov %ax, %ds\n\t"
    );
}

void shell_main() {
    print_string("\nWelcome to ShellZer0!\n> ");

    while (1) {
        if (input_index > 0 && input_buffer[input_index - 1] == '\n') {
            input_buffer[input_index - 1] = '\0';

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
    const char *str = "Hello, OS World!";

    gdt_init();
    idt_init();
    pic_remap();
    isr_install();
    irq_install();

    asm volatile("sti");

    clear_screen(0x00, 0x0F);
    print_string(str);

    shell_main();

    while (1) {}
}
