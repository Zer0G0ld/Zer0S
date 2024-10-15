#include "keyboard.h"
#include "ports.h"

// Definindo a porta de entrada do teclado (0x60)
#define KEYBOARD_PORT 0x60

// Expandir mapeamento de teclas
char teclado_get_char() {
    char scancode = inb(KEYBOARD_PORT);
    
    // Mapeamento básico expandido (adicionar mais teclas)
    if (scancode == 0x1E) return 'a';  // 'a'
    if (scancode == 0x30) return 'b';  // 'b'
    // Adicionar mais teclas
    if (scancode == 0x39) return ' ';  // Espaço
    return 0;
}


// Função que será chamada para processar interrupções de teclado (exemplo básico)
void keyboard_handler() {
    // Lógica de manuseio do teclado
    // Aqui, você pode usar a função 'teclado_get_char' para pegar o caractere
    char key = teclado_get_char();
    
    if (key != 0) {
        // Faça algo com a tecla, como passar para o shell ou algo mais
    }
}
