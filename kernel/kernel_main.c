// kernel_main.c - Zer0S 64-bit Kernel

#define VGA_MEMORY ((unsigned short*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static int row = 0;
static int col = 0;

static void putchar(char c) {
    if (c == '\n') {
        col = 0;
        row++;
        return;
    }
    
    VGA_MEMORY[row * VGA_WIDTH + col] = (unsigned short)c | (0x0F << 8);
    col++;
    
    if (col >= VGA_WIDTH) {
        col = 0;
        row++;
    }
}

static void write(const char* str) {
    while (*str) putchar(*str++);
}

static void clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = (unsigned short)' ' | (0x0F << 8);
    row = col = 0;
}

void kernel_main(unsigned long magic, unsigned long addr) {
    clear();
    write("========================================\n");
    write("  Zer0S v2.0 - 64-bit Kernel!\n");
    write("========================================\n\n");
    write("[OK] Running in 64-bit Long Mode\n");
    write("[OK] VGA Text Mode: 80x25\n\n");
    write("Zer0S> ");
    
    while (1) {
        __asm__ volatile ("hlt");
    }
}
