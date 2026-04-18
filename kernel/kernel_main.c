// kernel_main.c - Zer0S 64-bit Kernel Entry Point

// Basic type definitions
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

// VGA text mode (higher half address)
#define VGA_MEMORY ((uint16_t*)0xFFFFFFFF800B8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static uint16_t* video_memory = VGA_MEMORY;
static int row = 0;
static int col = 0;
static uint8_t color = 0x0F;  // White on black

// Clear screen
static void terminal_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++) {
        video_memory[i] = (uint16_t)' ' | ((uint16_t)0x0F << 8);
    }
    row = 0;
    col = 0;
}

// Write a single character
static void terminal_putchar(char c) {
    if (c == '\n') {
        col = 0;
        row++;
        return;
    }
    
    if (c == '\r') {
        col = 0;
        return;
    }
    
    if (c >= ' ') {
        video_memory[row * VGA_WIDTH + col] = (uint16_t)c | ((uint16_t)color << 8);
        col++;
        
        if (col >= VGA_WIDTH) {
            col = 0;
            row++;
        }
        
        if (row >= VGA_HEIGHT) {
            // Scroll screen
            for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++) {
                video_memory[i] = video_memory[i + VGA_WIDTH];
            }
            for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++) {
                video_memory[i] = (uint16_t)' ' | ((uint16_t)color << 8);
            }
            row = VGA_HEIGHT - 1;
        }
    }
}

// Write a string
static void terminal_write(const char* str) {
    while (*str) {
        terminal_putchar(*str++);
    }
}

// Print hexadecimal number
static void print_hex(uint64_t value) {
    const char hex[] = "0123456789ABCDEF";
    terminal_write("0x");
    
    // Skip leading zeros
    int started = 0;
    for (int i = 60; i >= 0; i -= 4) {
        uint8_t digit = (value >> i) & 0xF;
        if (digit != 0 || started || i == 0) {
            terminal_putchar(hex[digit]);
            started = 1;
        }
    }
}

// Kernel entry point
void kernel_main(uint64_t magic, uint64_t mboot_addr) {
    terminal_clear();
    
    terminal_write("========================================\n");
    terminal_write("  Zer0S v2.0 - 64-bit Kernel Loaded!\n");
    terminal_write("========================================\n\n");
    
    terminal_write("[INFO] CPU Mode: 64-bit Long Mode\n");
    terminal_write("[INFO] VGA Mode: Text 80x25\n");
    terminal_write("[INFO] Kernel Base: 0xFFFFFFFF80000000\n\n");
    
    terminal_write("[INFO] Multiboot Magic: ");
    print_hex(magic);
    terminal_write("\n");
    
    terminal_write("[INFO] Multiboot Info: ");
    print_hex(mboot_addr);
    terminal_write("\n\n");
    
    terminal_write("[STATUS] System initialized successfully!\n");
    terminal_write("[STATUS] Entering main loop...\n\n");
    
    terminal_write("Zer0S> ");
    
    // Main loop
    while (1) {
        // Halt CPU until next interrupt
        __asm__ volatile ("hlt");
    }
}
