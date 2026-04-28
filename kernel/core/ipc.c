#include "ipc.h"

extern void terminal_writestring(const char* str);

void ipc_init(void) {
    terminal_writestring("[IPC] Initializing inter-process communication...\n");
    terminal_writestring("[IPC] Ready\n");
}

int ipc_send(uint64_t to, ipc_message_t* msg) {
    // TODO: Implement message passing
    return 0;
}

int ipc_receive(uint64_t from, ipc_message_t* msg) {
    // TODO: Implement message receiving
    return 0;
}
