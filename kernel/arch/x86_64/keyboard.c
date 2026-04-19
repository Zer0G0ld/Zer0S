#include "keyboard.h"
#include "shell.h"
#include "io.h"
#include <stdint.h>

// PS/2 Keyboard ports
#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

// ABNT2 Scancode mappings (Set 1)
static const char scancode_normal[128] = {
    0,    0,    '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b', // 0x0E
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n', 0,    // 0x1C
    'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0,   '\\', 'z',  // 0x2C
    'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0,   0,   0,   ' ', 0,   0,      // 0x3C
};

static const char scancode_shift[128] = {
    0,    0,    '!', '@', '#', '$', '%', '¨', '&', '*', '(', ')', '_', '+', '\b', // 0x0E
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n', 0,    // 0x1C
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0,   '|', 'Z',    // 0x2C
    'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0,   0,   0,   ' ', 0,   0,      // 0x3C
};

// ABNT2 specific keys (scancodes beyond standard US)
// ç (cedilla) and accent keys
static const char scancode_abnt2_normal[128] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x00-0x26
    'ç',   // 0x27 - ç
    0,     // 0x28
    '~',   // 0x29 - ~
    0,0,   // 0x2A-0x2B
    '^',   // 0x2C - ^
    '´',   // 0x2D - ´
};

static const char scancode_abnt2_shift[128] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x00-0x26
    'Ç',   // 0x27 - Ç
    0,     // 0x28
    '^',   // 0x29 - ^ (shift + ~ becomes ^)
    0,0,   // 0x2A-0x2B
    '~',   // 0x2C - ~ (shift + ^ becomes ~)
    '`',   // 0x2D - ` (shift + ´ becomes `)
};

// Keyboard state
static int shift_pressed = 0;
static int ctrl_pressed = 0;
static int alt_pressed = 0;
static int caps_lock = 0;
static int num_lock = 0;
static int scroll_lock = 0;

// Dead key state for accents
static int dead_key = 0;
static char dead_char = 0;

// Command buffer for shell
static char cmd_buffer[256];
static int cmd_pos = 0;

// External VGA functions
extern void terminal_writechar(char c);
extern void terminal_writestring(const char* str);
extern void shell_handle_command(char* input);

// Function to handle dead keys (accents)
static char handle_dead_key(char base_char, char accent) {
    // ABNT2 accent combinations
    if (accent == '´') {  // Acute accent
        if (base_char == 'a') return 'á';
        if (base_char == 'e') return 'é';
        if (base_char == 'i') return 'í';
        if (base_char == 'o') return 'ó';
        if (base_char == 'u') return 'ú';
        if (base_char == 'A') return 'Á';
        if (base_char == 'E') return 'É';
        if (base_char == 'I') return 'Í';
        if (base_char == 'O') return 'Ó';
        if (base_char == 'U') return 'Ú';
    }
    else if (accent == '`') {  // Grave accent
        if (base_char == 'a') return 'à';
        if (base_char == 'e') return 'è';
        if (base_char == 'i') return 'ì';
        if (base_char == 'o') return 'ò';
        if (base_char == 'u') return 'ù';
        if (base_char == 'A') return 'À';
        if (base_char == 'E') return 'È';
        if (base_char == 'I') return 'Ì';
        if (base_char == 'O') return 'Ò';
        if (base_char == 'U') return 'Ù';
    }
    else if (accent == '^') {  // Circumflex
        if (base_char == 'a') return 'â';
        if (base_char == 'e') return 'ê';
        if (base_char == 'i') return 'î';
        if (base_char == 'o') return 'ô';
        if (base_char == 'u') return 'û';
        if (base_char == 'A') return 'Â';
        if (base_char == 'E') return 'Ê';
        if (base_char == 'I') return 'Î';
        if (base_char == 'O') return 'Ô';
        if (base_char == 'U') return 'Û';
    }
    else if (accent == '~') {  // Tilde
        if (base_char == 'a') return 'ã';
        if (base_char == 'n') return 'ñ';
        if (base_char == 'o') return 'õ';
        if (base_char == 'A') return 'Ã';
        if (base_char == 'N') return 'Ñ';
        if (base_char == 'O') return 'Õ';
    }
    return base_char;
}

uint8_t keyboard_read_scancode(void) {
    while (!(inb(KEYBOARD_STATUS_PORT) & 1));
    return inb(KEYBOARD_DATA_PORT);
}

void keyboard_handler(void) {
    uint8_t scancode = keyboard_read_scancode();
    uint8_t key = scancode & 0x7F;
    int pressed = !(scancode & 0x80);
    
    // Handle modifier keys
    if (key == 0x2A || key == 0x36) {  // Left/Right Shift
        shift_pressed = pressed;
        return;
    }
    if (key == 0x1D) {  // Left Ctrl
        ctrl_pressed = pressed;
        return;
    }
    if (key == 0x38) {  // Left Alt
        alt_pressed = pressed;
        return;
    }
    
    // Handle lock keys
    if (key == 0x3A && pressed) {  // Caps Lock
        caps_lock = !caps_lock;
        return;
    }
    if (key == 0x45 && pressed) {  // Num Lock
        num_lock = !num_lock;
        return;
    }
    if (key == 0x46 && pressed) {  // Scroll Lock
        scroll_lock = !scroll_lock;
        return;
    }
    
    // Handle Enter key
    if (key == 0x1C && pressed) {
        if (dead_key) dead_key = 0;
        terminal_writechar('\n');
        cmd_buffer[cmd_pos] = '\0';
        shell_handle_command(cmd_buffer);
        cmd_pos = 0;
        cmd_buffer[0] = '\0';
        return;
    }
    
    // Handle Backspace
    if (key == 0x0E && pressed) {
        if (dead_key) dead_key = 0;
        if (cmd_pos > 0) {
            cmd_pos--;
            terminal_writechar('\b');
            terminal_writechar(' ');
            terminal_writechar('\b');
        }
        return;
    }
    
    // Handle Tab key
    if (key == 0x0F && pressed) {
        if (dead_key) dead_key = 0;
        terminal_writechar('\t');
        if (cmd_pos < 255) {
            cmd_buffer[cmd_pos++] = '\t';
        }
        return;
    }
    
    // Handle Escape key
    if (key == 0x01 && pressed) {
        if (dead_key) dead_key = 0;
        terminal_writechar('\n');
        terminal_writestring("^C\nZer0S> ");
        cmd_pos = 0;
        cmd_buffer[0] = '\0';
        return;
    }
    
    // Only handle key press events
    if (pressed) {
        char ascii = 0;
        int is_abnt2 = 0;
        
        // Check for ABNT2 special keys
        if (key == 0x27) { // ç key
            ascii = shift_pressed ? scancode_abnt2_shift[key] : scancode_abnt2_normal[key];
            is_abnt2 = 1;
        }
        else if (key == 0x29) { // ~ key
            ascii = shift_pressed ? scancode_abnt2_shift[key] : scancode_abnt2_normal[key];
            is_abnt2 = 1;
        }
        else if (key == 0x2C) { // ^ key
            ascii = shift_pressed ? scancode_abnt2_shift[key] : scancode_abnt2_normal[key];
            is_abnt2 = 1;
        }
        else if (key == 0x2D) { // ´ key
            ascii = shift_pressed ? scancode_abnt2_shift[key] : scancode_abnt2_normal[key];
            is_abnt2 = 1;
        }
        else {
            // Standard keys
            ascii = shift_pressed ? scancode_shift[key] : scancode_normal[key];
        }
        
        // Handle dead keys (accents)
        if (dead_key && ascii && ascii >= 'a' && ascii <= 'z') {
            char result = handle_dead_key(ascii, dead_char);
            if (result != ascii) {
                ascii = result;
                dead_key = 0;
            } else {
                // Output dead char + current char
                terminal_writechar(dead_char);
                if (cmd_pos < 255) cmd_buffer[cmd_pos++] = dead_char;
                dead_key = 0;
            }
        }
        
        // Check if this is a dead key
        if (!dead_key && (ascii == '´' || ascii == '`' || ascii == '^' || ascii == '~')) {
            dead_key = 1;
            dead_char = ascii;
            return;
        }
        
        if (ascii && ascii >= ' ' && ascii <= '~') {
            // Apply caps lock for letters
            if (caps_lock && !shift_pressed && ascii >= 'a' && ascii <= 'z') {
                ascii = ascii - 'a' + 'A';
            }
            
            // Add to command buffer
            if (cmd_pos < 255) {
                cmd_buffer[cmd_pos++] = ascii;
                terminal_writechar(ascii);
            }
        }
        else if (ascii == '\t') {
            if (cmd_pos < 255) {
                cmd_buffer[cmd_pos++] = ascii;
                terminal_writechar(ascii);
            }
        }
    }
}

void keyboard_init(void) {
    terminal_writestring("[INFO] Keyboard driver initialized (ABNT2 support)\n");
    terminal_writestring("[INFO] Supported: ç, á, é, í, ó, ú, â, ê, ô, ã, õ, ñ\n");
}
