#include "terminal.h"
#include <stdint.h>

// Variáveis globais de estado do terminal
static uint16_t* terminal_buffer;  // A variável não é mais inicializada aqui
static int terminal_row = 0;
static int terminal_column = 0;
static uint8_t terminal_color = 0x07;  // Cor padrão: branco sobre fundo preto

// Função para inicializar o terminal
void terminal_initialize() {
    terminal_buffer = (uint16_t*) 0xB8000;  // Agora inicializa em tempo de execução
    terminal_clear();
}

// Função para escrever um caractere na tela
void terminal_put_char_at(char c, int x, int y) {
    terminal_buffer[(y * 80) + x] = (uint16_t) c | ((uint16_t) terminal_color << 8);
}

// Função para mover o cursor
void terminal_move_cursor() {
    uint16_t pos = (uint16_t)(terminal_row * 80 + terminal_column);
    outb(0x0F, 0x3D4);  // Porta de controle do cursor
    outb((uint8_t)(pos & 0xFF), 0x3D5);
    outb(0x0E, 0x3D4);
    outb((uint8_t)((pos >> 8) & 0xFF), 0x3D5);
}

// Função para escrever um caractere no terminal
void terminal_write_char(char c) {
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row >= 25) {
            terminal_row = 0;
        }
    } else {
        terminal_put_char_at(c, terminal_column, terminal_row);
        terminal_column++;
        if (terminal_column >= 80) {
            terminal_column = 0;
            terminal_row++;
            if (terminal_row >= 25) {
                terminal_row = 0;
            }
        }
    }
    terminal_move_cursor();
}

// Função para escrever uma string no terminal
void terminal_write(const char* str) {
    for (int i = 0; str[i] != '\0'; i++) {
        terminal_write_char(str[i]);
    }
}

// Função para limpar o terminal
void terminal_clear() {
    for (int i = 0; i < 25; i++) {
        for (int j = 0; j < 80; j++) {
            terminal_put_char_at(' ', j, i);
        }
    }
    terminal_row = 0;
    terminal_column = 0;
    terminal_move_cursor();
}

// Função para imprimir uma string de boas-vindas
void terminal_welcome() {
    terminal_write("Bem-vindo ao Zer0S Shell\n");
    terminal_write("Digite 'help' para ver os comandos disponíveis.\n");
}

// Função para lidar com a entrada do usuário
void terminal_handle_input(char* input) {
    if (strcmp(input, "help") == 0) {
        terminal_write("Comandos disponíveis:\n");
        terminal_write("clear - Limpa o terminal\n");
        terminal_write("help  - Exibe essa ajuda\n");
        terminal_write("exit  - Sai do terminal\n");
    } else if (strcmp(input, "clear") == 0) {
        terminal_clear();
    } else if (strcmp(input, "exit") == 0) {
        terminal_write("Saindo...\n");
        // Função para desligar o sistema ou retornar ao bootloader
        // shutdown();
    } else {
        terminal_write("Comando não reconhecido.\n");
    }
}

// Função para a execução do terminal
void terminal_run() {
    terminal_initialize();  // Inicializa o terminal

    terminal_welcome();

    char input[256];
    int i = 0;

    while (1) {
        // Captura a entrada do usuário (pode ser implementado usando interrupções ou polled I/O)
        char ch = teclado_get_char();  // Função de leitura do teclado (precisa ser implementada)

        if (ch == '\n') {
            input[i] = '\0';  // Finaliza a string
            terminal_write("\n");
            terminal_handle_input(input);
            i = 0;  // Reseta o índice para a próxima entrada
        } else if (ch == '\b') {  // Backspace
            if (i > 0) {
                i--;
                terminal_write_char('\b');
            }
        } else if (ch >= 32 && ch <= 126) {  // Apenas caracteres imprimíveis
            input[i++] = ch;
            terminal_write_char(ch);
        }
    }
}
