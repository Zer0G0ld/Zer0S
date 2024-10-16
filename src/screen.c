#include "screen.h"

static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

// Função para limpar a tela
void screen_clear() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        VIDEO_MEMORY[i] = WHITE_ON_BLACK;  // Limpar para fundo preto e texto branco
    }
    cursor_x = 0;
    cursor_y = 0;
    screen_update_cursor();  // Atualizar a posição do cursor após limpar
}

// Função para escrever uma string na tela
void screen_write(const char* str) {
    unsigned int i = 0;
    while (str[i] != '\0') {
        screen_put_char(str[i]);
        i++;
    }
}

// Função para escrever um número inteiro na tela
void screen_write_int(uint32_t num) {
    char buf[32];
    int i = 0;

    if (num == 0) {
        screen_put_char('0');
        return;
    }

    while (num > 0) {
        buf[i++] = '0' + (num % 10);
        num /= 10;
    }

    // Imprimir o número na ordem correta
    for (int j = i - 1; j >= 0; j--) {
        screen_put_char(buf[j]);
    }
}

// Função para escrever um caractere na tela
void screen_put_char(char c) {
    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        VIDEO_MEMORY[cursor_y * SCREEN_WIDTH + cursor_x] = (uint16_t)(c | WHITE_ON_BLACK);
        cursor_x++;
    }

    if (cursor_x >= SCREEN_WIDTH) {
        cursor_x = 0;
        cursor_y++;
    }

    if (cursor_y >= SCREEN_HEIGHT) {
        cursor_y = 0;  // Simples rolagem de tela para cima
    }

    screen_update_cursor();  // Atualiza a posição do cursor
}

// Função para atualizar a posição do cursor
void screen_set_cursor(uint8_t x, uint8_t y) {
    cursor_x = x;
    cursor_y = y;
    screen_update_cursor();
}

// Função para atualizar o cursor no hardware
void screen_update_cursor() {
    uint16_t position = cursor_y * SCREEN_WIDTH + cursor_x;

    // Enviar a posição do cursor para as portas de controle
    outb(0x3D4, 0x0F);  // Registrar de cursor
    outb(0x3D5, (uint8_t)(position & 0xFF));

    outb(0x3D4, 0x0E);  // Registrar de cursor
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}
