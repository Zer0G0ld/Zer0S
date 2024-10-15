#ifndef TERMINAL_H
#define TERMINAL_H

// Funções do terminal
void terminal_clear();
void terminal_write(const char* str);
void terminal_write_char(char c);
void terminal_welcome();
void terminal_run();
void terminal_handle_input(char* input);

// Função placeholder para captura de entrada de teclado
char teclado_get_char();

void shell_main(void);

#endif /* TERMINAL_H */
