#include "screen.h"
#include "ports.h"  // Necessário para outb()

static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

// Função para limpar a tela
void screen_clear() {
    for (int i = 0; i < SCREEN_WIDTH * SCREEN_HEIGHT; i++) {
        VIDEO_MEMORY[i] = (uint16_t)(' ' | WHITE_ON_BLACK);  // Espaço + cor
    }
    cursor_x = 0;
    cursor_y = 0;
    screen_update_cursor();
}

void screen_put_char(char c);

void screen_write(const char* str) {
    unsigned int i = 0;
    while (str[i] != '\0') {
        screen_put_char(str[i]);
        i++;
    }
}

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

    for (int j = i - 1; j >= 0; j--) {
        screen_put_char(buf[j]);
    }
}

void screen_scroll() {
    // Move as linhas para cima
    for (int y = 1; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            VIDEO_MEMORY[(y - 1) * SCREEN_WIDTH + x] = VIDEO_MEMORY[y * SCREEN_WIDTH + x];
        }
    }
    // Limpa a última linha
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        VIDEO_MEMORY[(SCREEN_HEIGHT - 1) * SCREEN_WIDTH + x] = (uint16_t)(' ' | WHITE_ON_BLACK);
    }
}

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
        screen_scroll();
        cursor_y = SCREEN_HEIGHT - 1;
    }

    screen_update_cursor();
}

void screen_set_cursor(uint8_t x, uint8_t y) {
    cursor_x = x;
    cursor_y = y;
    screen_update_cursor();
}

void screen_update_cursor() {
    uint16_t position = cursor_y * SCREEN_WIDTH + cursor_x;

    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
}
