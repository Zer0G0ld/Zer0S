#ifndef _KERNEL_H
#define _KERNEL_H

// Core kernel headers
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Architecture specific
#include <idt.h>
#include <isr.h>
#include <pic.h>
#include <memory.h>

// Drivers
#include <keyboard.h>

// Shell
#include <shell.h>

// Kernel entry point
void kernel_main(unsigned long magic, unsigned long addr);

// VGA console functions
void terminal_clear(void);
void terminal_writechar(char c);
void terminal_writestring(const char* str);
void print_hex(unsigned long long value);

#endif