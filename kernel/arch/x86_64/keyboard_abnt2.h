#ifndef _KEYBOARD_ABNT2_H
#define _KEYBOARD_ABNT2_H

// Scancode to ABNT2 character mapping (Set 1)
// Base table (normal press)
static const char scancode_abnt2_normal[128] = {
    0,    0,    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', // 0x0E
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,    // 0x1C
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,   '\\', 'z',  // 0x2C
    'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,   0,   0,   ' ', 0,   0,      // 0x3C
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,     // 0x4C
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,     // 0x5C
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,     // 0x6C
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0      // 0x7C
};

// Shift table (uppercase and symbols)
static const char scancode_abnt2_shift[128] = {
    0,    0,    '!', '@', '#', '$', '%', '¨', '&', '*', '(', ')', '_', '+', '\b', // 0x0E
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,    // 0x1C
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,   '|', 'Z',    // 0x2C
    'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,   0,   0,   ' ', 0,   0,      // 0x3C
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,     // 0x4C
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,     // 0x5C
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,     // 0x6C
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0      // 0x7C
};

// Special ABNT2 keys
#define SCANCODE_CEDILA     0x27  // 'ç' key (usually next to L)
#define SCANCODE_TILDE      0x29  // '~' key (next to backspace)
#define SCANCODE_ACUTE      0x2B  // '´' key (next to ç)
#define SCANCODE_CIRCUMFLEX 0x28  // '^' key (with tilde)

// ABNT2 specific mappings
static const char abnt2_special_normal[128] = {
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 0x0E
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 0x1C
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    'ç',  0,    0,    0,    0, // 0x2C (ç at 0x27)
    0,    0,    0,    0,    0,    0,    0,    0,    0,    '~',  '^',  '´',  0,    0,    0, // 0x3C
};

static const char abnt2_special_shift[128] = {
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 0x0E
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    0, // 0x1C
    0,    0,    0,    0,    0,    0,    0,    0,    0,    0,    'Ç',  0,    0,    0,    0, // 0x2C (Ç at 0x27)
    0,    0,    0,    0,    0,    0,    0,    0,    0,    '^',  '~',  '`',  0,    0,    0, // 0x3C
};

#endif
