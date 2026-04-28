#ifndef _IPC_H
#define _IPC_H

#include <stdint.h>
#include <stddef.h>

#define IPC_MAX_MSG_SIZE 256
#define IPC_MAX_QUEUE 16

typedef struct ipc_message {
    uint64_t from;
    uint64_t to;
    uint32_t type;
    uint32_t size;
    char data[IPC_MAX_MSG_SIZE];
} ipc_message_t;

void ipc_init(void);
int ipc_send(uint64_t to, ipc_message_t* msg);
int ipc_receive(uint64_t from, ipc_message_t* msg);

#endif
