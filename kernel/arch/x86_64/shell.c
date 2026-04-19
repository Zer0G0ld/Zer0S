#include "shell.h"

// External VGA functions
extern void terminal_writestring(const char* str);
extern void terminal_writechar(char c);
extern void terminal_clear(void);
extern void print_hex(unsigned long long value);

// Simple string compare
static int string_compare(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(const unsigned char*)s1 - *(const unsigned char*)s2;
}

// Command handlers
static void cmd_help(void) {
    terminal_writestring("\nAvailable commands:\n");
    terminal_writestring("  help      - Show this help\n");
    terminal_writestring("  clear     - Clear screen\n");
    terminal_writestring("  info      - Show system info\n");
    terminal_writestring("  echo      - Print text\n");
    terminal_writestring("  reboot    - Reboot system\n");
    terminal_writestring("  status    - Show system status\n");
    terminal_writestring("  hex       - Show number in hex\n");
    terminal_writestring("  about     - About Zer0S\n");
}

static void cmd_info(void) {
    terminal_writestring("\n=== Zer0S System Info ===\n");
    terminal_writestring("Version: 2.0 (64-bit)\n");
    terminal_writestring("Architecture: x86_64\n");
    terminal_writestring("Mode: Long Mode\n");
    terminal_writestring("Interrupts: Enabled\n");
    terminal_writestring("Keyboard: PS/2\n");
    terminal_writestring("Video: VGA Text 80x25\n");
    terminal_writestring("========================\n");
}

static void cmd_echo(char* args) {
    terminal_writestring("\n");
    terminal_writestring(args);
    terminal_writestring("\n");
}

static void cmd_status(void) {
    terminal_writestring("\n=== System Status ===\n");
    terminal_writestring("Status: RUNNING\n");
    terminal_writestring("Kernel: Stable\n");
    terminal_writestring("Interrupts: Active\n");
    terminal_writestring("===================\n");
}

static void cmd_hex(char* args) {
    unsigned long long num = 0;
    for (int i = 0; args[i] >= '0' && args[i] <= '9'; i++) {
        num = num * 10 + (args[i] - '0');
    }
    terminal_writestring("\nHex: ");
    print_hex(num);
    terminal_writestring("\n");
}

static void cmd_about(void) {
    terminal_writestring("\n=== Zer0S ===\n");
    terminal_writestring("A 64-bit hobby OS built from scratch\n");
    terminal_writestring("Created by Zer0G0ld\n");
    terminal_writestring("License: GPL-3.0\n");
    terminal_writestring("Features:\n");
    terminal_writestring("  - 64-bit Long Mode\n");
    terminal_writestring("  - IDT with interrupt handling\n");
    terminal_writestring("  - PS/2 Keyboard driver\n");
    terminal_writestring("  - VGA Text Mode\n");
    terminal_writestring("  - Interactive Shell\n");
    terminal_writestring("========================\n");
}

static void cmd_reboot(void) {
    terminal_writestring("\nRebooting system...\n");
    __asm__ volatile (
        "cli\n"
        "mov $0xFE, %al\n"
        "out %al, $0x64\n"
    );
    while (1) {
        __asm__ volatile ("hlt");
    }
}

// Parse and execute command
void shell_handle_command(char* input) {
    if (input[0] == '\0') return;
    
    // Parse command
    char cmd[32];
    char args[224];
    int i = 0;
    
    while (input[i] && input[i] != ' ' && i < 31) {
        cmd[i] = input[i];
        i++;
    }
    cmd[i] = '\0';
    
    while (input[i] == ' ') i++;
    int j = 0;
    while (input[i] && j < 223) {
        args[j++] = input[i++];
    }
    args[j] = '\0';
    
    // Execute command
    if (string_compare(cmd, "help") == 0) {
        cmd_help();
    }
    else if (string_compare(cmd, "clear") == 0) {
        terminal_clear();
    }
    else if (string_compare(cmd, "info") == 0) {
        cmd_info();
    }
    else if (string_compare(cmd, "echo") == 0) {
        cmd_echo(args);
    }
    else if (string_compare(cmd, "status") == 0) {
        cmd_status();
    }
    else if (string_compare(cmd, "hex") == 0) {
        cmd_hex(args);
    }
    else if (string_compare(cmd, "about") == 0) {
        cmd_about();
    }
    else if (string_compare(cmd, "reboot") == 0) {
        cmd_reboot();
    }
    else if (string_compare(cmd, "") != 0) {
        terminal_writestring("\nUnknown command: ");
        terminal_writestring(cmd);
        terminal_writestring("\nType 'help' for commands\n");
    }
    
    terminal_writestring("\nZer0S> ");
}

// Initialize shell
void shell_init(void) {
    terminal_writestring("\n========================================\n");
    terminal_writestring("  Zer0S Shell v1.0\n");
    terminal_writestring("========================================\n");
    terminal_writestring("Type 'help' for available commands\n\n");
    terminal_writestring("Zer0S> ");
}
