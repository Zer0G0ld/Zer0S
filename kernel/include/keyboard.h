#ifndef _KEYBOARD_H
#define _KEYBOARD_H

#include <stdint.h>

void keyboard_init(void);
void keyboard_handler(void);
uint8_t keyboard_read_scancode(void);

#endif
