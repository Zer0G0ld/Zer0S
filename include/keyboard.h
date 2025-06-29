#ifndef KEYBOARD_H
#define KEYBOARD_H

#define INPUT_BUFFER_SIZE 256

extern char input_buffer[INPUT_BUFFER_SIZE];
extern int input_index;

// Declarações das funções relacionadas ao teclado
char teclado_get_char();      // Função que lê um caractere do teclado
void keyboard_handler();      // Função para lidar com interrupções do teclado

#endif // KEYBOARD_H
