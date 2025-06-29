#ifndef SCREEN_H
#define SCREEN_H

#include <stdint.h>

// Endereço de memória do buffer de vídeo (modo texto VGA)
#define VIDEO_MEMORY ((volatile uint16_t*)0xB8000)

// Tamanho da tela (80 colunas x 25 linhas)
#define SCREEN_WIDTH 80
#define SCREEN_HEIGHT 25

// Definições de cores para texto e fundo
#define WHITE_ON_BLACK 0x0F00 // Fundo preto, texto branco
#define RED_ON_BLACK   0x04F0 // Fundo preto, texto vermelho
#define BLUE_ON_BLACK  0x01F0 // Fundo preto, texto azul

// Protótipos das funções
void screen_clear();
void screen_write(const char* str);
void screen_write_int(uint32_t num);
void screen_put_char(char c);
void screen_set_cursor(uint8_t x, uint8_t y);
void screen_update_cursor();

#endif // SCREEN_H
