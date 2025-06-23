#include "keyboard.h"
#include "ports.h"
#include "screen.h"

#define KEYBOARD_PORT 0x60

char input_buffer[INPUT_BUFFER_SIZE];
int input_index = 0;

char teclado_get_char() {
    uint8_t scancode = inb(KEYBOARD_PORT);

    // Ignorar key releases (teclas soltas)
    if (scancode & 0x80) {
        return 0;
    }

    switch (scancode) {
        case 0x1E: return 'a';
        case 0x30: return 'b';
        case 0x2E: return 'c';
        case 0x20: return 'd';
        case 0x12: return 'e';
        case 0x21: return 'f';
        case 0x22: return 'g';
        case 0x23: return 'h';
        case 0x17: return 'i';
        case 0x24: return 'j';
        case 0x25: return 'k';
        case 0x26: return 'l';
        case 0x32: return 'm';
        case 0x39: return ' ';  // Espaço
        default: return 0;
    }
}

void keyboard_handler() {
    char key = teclado_get_char();
    if (key != 0 && input_index < INPUT_BUFFER_SIZE - 1) {
        input_buffer[input_index++] = key;
        input_buffer[input_index] = '\0';

        // Imprimir na tela ao digitar (exceto para '\n' pra evitar lixo visual)
        if (key == '\n') {
            screen_write("\n");
        } else {
            char str[2] = { key, '\0' };
            screen_write(str);
        }
    }
}
