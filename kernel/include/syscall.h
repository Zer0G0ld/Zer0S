#ifndef _SYSCALL_H
#define _SYSCALL_H

#include <stdint.h>

// Syscall numbers
#define SYS_WRITE 1
#define SYS_READ 2
#define SYS_PRINT_STRING 3
#define SYS_PRINT_HEX 4
#define SYS_GETCHAR 5

// Syscall functions for userspace
static inline uint64_t syscall(uint64_t num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    uint64_t ret;
    __asm__ volatile (
        "int $0x80"
        : "=a"(ret)
        : "a"(num), "b"(arg1), "c"(arg2), "d"(arg3)
        : "memory"
    );
    return ret;
}

static inline void sys_print_string(const char* str) {
    syscall(SYS_PRINT_STRING, (uint64_t)str, 0, 0);
}

static inline void sys_print_hex(uint64_t value) {
    syscall(SYS_PRINT_HEX, value, 0, 0);
}

#endif