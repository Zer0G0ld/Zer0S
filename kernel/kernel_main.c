// kernel_main.c - Zer0S 64-bit Kernel

#include "idt.h"
#include "keyboard.h"
#include "pic.h"
#include "shell.h"
#include "memory.h"

// VGA functions
#define VGA_MEMORY ((unsigned short*)0xB8000)
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

static int row = 0;
static int col = 0;

void terminal_writechar(char c) {
    if (c == '\n') {
        col = 0;
        row++;
        return;
    }
    
    if (c == '\b') {
        if (col > 0) {
            col--;
            VGA_MEMORY[row * VGA_WIDTH + col] = (unsigned short)' ' | (0x0F << 8);
        }
        return;
    }
    
    VGA_MEMORY[row * VGA_WIDTH + col] = (unsigned short)c | (0x0F << 8);
    col++;
    
    if (col >= VGA_WIDTH) {
        col = 0;
        row++;
    }
    
    if (row >= VGA_HEIGHT) {
        // Scroll
        for (int i = 0; i < (VGA_HEIGHT - 1) * VGA_WIDTH; i++)
            VGA_MEMORY[i] = VGA_MEMORY[i + VGA_WIDTH];
        for (int i = (VGA_HEIGHT - 1) * VGA_WIDTH; i < VGA_HEIGHT * VGA_WIDTH; i++)
            VGA_MEMORY[i] = (unsigned short)' ' | (0x0F << 8);
        row = VGA_HEIGHT - 1;
    }
}

void terminal_writestring(const char* str) {
    while (*str) terminal_writechar(*str++);
}

void terminal_clear(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = (unsigned short)' ' | (0x0F << 8);
    row = col = 0;
}

void print_hex(unsigned long long value) {
    const char hex[] = "0123456789ABCDEF";
    
    terminal_writechar('0');
    terminal_writechar('x');
    
    int started = 0;
    for (int j = 60; j >= 0; j -= 4) {
        char digit = hex[(value >> j) & 0xF];
        if (digit != '0' || started || j == 0) {
            terminal_writechar(digit);
            started = 1;
        }
    }
}

void kernel_main(unsigned long magic, unsigned long addr) {
    terminal_clear();
    
    terminal_writestring("========================================\n");
    terminal_writestring("  Zer0S v2.0 - 64-bit Kernel!\n");
    terminal_writestring("========================================\n\n");
    
    terminal_writestring("[OK] Running in 64-bit Long Mode\n");
    terminal_writestring("[OK] VGA Text Mode: 80x25\n\n");
    
    // Initialize IDT
    terminal_writestring("[INFO] Initializing IDT...\n");
    idt_init();
    terminal_writestring("[OK] IDT initialized\n");
    
    // Remap PIC
    terminal_writestring("[INFO] Remapping PIC...\n");
    pic_remap();
    terminal_writestring("[OK] PIC remapped (IRQs 32-47)\n");
    
    // Initialize keyboard
    terminal_writestring("[INFO] Initializing keyboard...\n");
    keyboard_init();
    
    // Initialize memory manager
    terminal_writestring("[INFO] Initializing memory manager...\n");
    memory_init(addr, 0);  // Use 'addr' from multiboot
    terminal_writestring("[OK] Memory manager initialized\n");
    
    // Test kmalloc
    char* test = kmalloc(64);
    if (test) {
        terminal_writestring("[TEST] kmalloc(64) successful at address: ");
        print_hex((uint64_t)test);
        terminal_writestring("\n");
        kfree(test);
        terminal_writestring("[TEST] kfree() successful\n");
    }
    
    // Enable interrupts
    terminal_writestring("[INFO] Enabling interrupts...\n");
    __asm__ volatile ("sti");
    terminal_writestring("[OK] Interrupts enabled\n\n");
    
    // Start shell
    shell_init();
    
    while (1) {
        __asm__ volatile ("hlt");
    }
}
