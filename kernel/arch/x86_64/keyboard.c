#include "keyboard.h"
#include "shell.h"
#include "io.h"
#include <stdint.h>

// PS/2 Keyboard ports
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// Keyboard scancode to ASCII mapping (US layout)
static const char scancode_to_ascii[] = {
    0,   27,  '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,   'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',
    0,   '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,
    '*', 0,   ' ', 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
};

// Keyboard state
static int shift_pressed = 0;
static int caps_lock = 0;

// Command buffer for shell
static char cmd_buffer[256];
static int cmd_pos = 0;

// External VGA functions
extern void terminal_writechar(char c);
extern void terminal_writestring(const char* str);
extern void shell_handle_command(char* input);

uint8_t keyboard_read_scancode(void) {
    while (!(inb(KEYBOARD_STATUS_PORT) & 1));
    return inb(KEYBOARD_DATA_PORT);
}

void keyboard_handler(void) {
    uint8_t scancode = keyboard_read_scancode();
    uint8_t key = scancode & 0x7F;
    int pressed = !(scancode & 0x80);
    
    // Handle special keys
    if (key == 0x2A || key == 0x36) {  // Left/Right Shift
        shift_pressed = pressed;
        return;
    }
    
    if (key == 0x3A && pressed) {  // Caps Lock
        caps_lock = !caps_lock;
        return;
    }
    
    // Handle Enter key
    if (key == 0x1C && pressed) {
        terminal_writechar('\n');
        cmd_buffer[cmd_pos] = '\0';
        shell_handle_command(cmd_buffer);
        cmd_pos = 0;
        cmd_buffer[0] = '\0';
        return;
    }
    
    // Handle Backspace
    if (key == 0x0E && pressed) {
        if (cmd_pos > 0) {
            cmd_pos--;
            terminal_writechar('\b');
            terminal_writechar(' ');
            terminal_writechar('\b');
        }
        return;
    }
    
    // Only handle key press events for regular keys
    if (pressed) {
        char ascii = scancode_to_ascii[key];
        
        if (ascii && ascii >= ' ') {  // Printable characters
            // Apply shift/caps modifications
            if (shift_pressed || caps_lock) {
                if (ascii >= 'a' && ascii <= 'z') {
                    ascii = ascii - 'a' + 'A';
                }
            }
            
            // Add to command buffer
            if (cmd_pos < 255) {
                cmd_buffer[cmd_pos++] = ascii;
                terminal_writechar(ascii);
            }
        }
    }
}

void keyboard_init(void) {
    terminal_writestring("[INFO] Keyboard driver initialized\n");
}
