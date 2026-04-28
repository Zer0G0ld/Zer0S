#include <stdint.h>
#include <stddef.h>

// External VGA functions
extern void terminal_writestring(const char* str);
extern void terminal_writechar(char c);
extern void print_hex(uint64_t value);

// Syscall numbers
#define SYS_WRITE 1
#define SYS_READ 2
#define SYS_PRINT_STRING 3
#define SYS_PRINT_HEX 4
#define SYS_GETCHAR 5

// Syscall dispatcher
uint64_t syscall_dispatcher(uint64_t syscall_num, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    switch (syscall_num) {
        case SYS_WRITE:
            for (size_t i = 0; i < arg2; i++) {
                terminal_writechar(((char*)arg1)[i]);
            }
            return arg2;
            
        case SYS_PRINT_STRING:
            terminal_writestring((const char*)arg1);
            return 0;
            
        case SYS_PRINT_HEX:
            print_hex(arg1);
            return 0;
            
        case SYS_GETCHAR:
            return 0;
            
        default:
            terminal_writestring("\n[ERROR] Unknown syscall: ");
            print_hex(syscall_num);
            terminal_writestring("\n");
            return -1;
    }
}
