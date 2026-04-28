#ifndef _PROCESS_H
#define _PROCESS_H

#include <stdint.h>
#include <stddef.h>

//==============================================================================
// Process States
//==============================================================================
#define PROC_UNUSED     0   // Slot livre
#define PROC_NEW        1   // Recém criado, não iniciado
#define PROC_READY      2   // Pronto para executar
#define PROC_RUNNING    3   // Executando atualmente
#define PROC_WAITING    4   // Aguardando evento (I/O, IPC)
#define PROC_SLEEPING   5   // Dormindo (timer)
#define PROC_STOPPED    6   // Parado (sinal)
#define PROC_ZOMBIE     7   // Terminado, aguardando wait
#define PROC_DEAD       8   // Morto, slot pode ser reutilizado

//==============================================================================
// Process Flags
//==============================================================================
#define PROC_FLAG_KERNEL    0x0001  // Processo do kernel
#define PROC_FLAG_USER      0x0002  // Processo de usuário
#define PROC_FLAG_SYSTEM    0x0004  // Processo crítico do sistema
#define PROC_FLAG_IDLE      0x0008  // Processo idle
#define PROC_FLAG_DEBUG     0x0010  // Modo debug
#define PROC_FLAG_TRACE     0x0020  // Trace syscalls

//==============================================================================
// Process Priorities (0 = mais alta, 255 = mais baixa)
//==============================================================================
#define PRIORITY_REALTIME   0
#define PRIORITY_HIGH       32
#define PRIORITY_NORMAL     128
#define PRIORITY_LOW        192
#define PRIORITY_IDLE       255

//==============================================================================
// Constants
//==============================================================================
#define MAX_PROCS           256     // Máximo de processos
#define PROC_STACK_SIZE     8192    // 8KB stack
#define PROC_NAME_MAX       64      // Tamanho máximo do nome
#define PROC_MAX_ARGS       32      // Máximo de argumentos
#define PROC_MAX_ENV        32      // Máximo de variáveis de ambiente

//==============================================================================
// Process Statistics
//==============================================================================
typedef struct proc_stats {
    uint64_t user_ticks;        // Tempo em modo usuário
    uint64_t kernel_ticks;      // Tempo em modo kernel
    uint64_t total_ticks;       // Tempo total
    uint64_t voluntary_yields;  // Yields voluntários
    uint64_t involuntary_yields; // Yields forçados
    uint64_t page_faults;       // Page faults
    uint64_t syscalls;          // Syscalls chamadas
    uint64_t ipc_sent;          // Mensagens IPC enviadas
    uint64_t ipc_received;      // Mensagens IPC recebidas
} proc_stats_t;

//==============================================================================
// Process Context (para salvar/restaurar)
//==============================================================================
typedef struct proc_context {
    // Registradores gerais
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp, rsp;
    uint64_t r8, r9, r10, r11;
    uint64_t r12, r13, r14, r15;
    
    // Registradores de controle
    uint64_t rip;           // Instruction pointer
    uint64_t rflags;        // Flags
    uint64_t cr3;           // Page table (espaço de endereço)
    
    // Registradores de segmento
    uint64_t cs, ds, es, fs, gs, ss;
    
    // Registradores FPU/SSE (para contexto completo)
    uint8_t fpu_state[512];
    int fpu_used;
    
} proc_context_t;

//==============================================================================
// Process Structure (COMPLETO)
//==============================================================================
typedef struct process {
    // Identificação
    uint64_t pid;                   // Process ID
    uint64_t ppid;                  // Parent process ID
    char name[PROC_NAME_MAX];       // Nome do processo
    char* args[PROC_MAX_ARGS];      // Argumentos
    char* env[PROC_MAX_ENV];        // Variáveis de ambiente
    
    // Estado
    int state;                      // Estado atual
    int flags;                      // Flags do processo
    int priority;                   // Prioridade (0-255)
    int nice;                       // Nice value (-20 a 19)
    
    // Contexto de execução
    proc_context_t context;         // Contexto salvo
    uint64_t* kernel_stack;         // Stack do kernel
    uint64_t* user_stack;           // Stack do usuário
    size_t kernel_stack_size;
    size_t user_stack_size;
    
    // Memória
    uint64_t heap_start;            // Início do heap
    uint64_t heap_end;              // Fim do heap
    uint64_t brk;                   // Break point
    uint64_t code_start;            // Início do código
    uint64_t code_end;              // Fim do código
    uint64_t data_start;            // Início dos dados
    uint64_t data_end;              // Fim dos dados
    
    // Scheduling
    uint64_t priority_boost;        // Boost temporário
    uint64_t remaining_ticks;       // Ticks restantes (preempção)
    uint64_t total_ticks;           // Ticks totais recebidos
    uint64_t sleep_until;           // Acordar em X ticks
    
    // Comunicação
    void* ipc_queue;                // Fila de mensagens
    uint64_t ipc_waiting;           // Aguardando IPC
    
    // Recursos
    void* file_table;               // Tabela de arquivos abertos
    uint64_t cwd;                   // Current working directory
    uint64_t root;                  // Root directory
    
    // Sinais
    uint64_t signal_mask;           // Máscara de sinais
    uint64_t signal_pending;        // Sinais pendentes
    void* signal_handlers[32];      // Handlers de sinais
    
    // Controle
    struct process* parent;         // Processo pai
    struct process* children;       // Lista de filhos
    struct process* next;           // Próximo na lista (scheduler)
    struct process* prev;           // Anterior na lista
    
    // Estatísticas
    proc_stats_t stats;
    uint64_t start_time;            // Tempo de criação
    uint64_t exit_code;             // Código de saída
    
    // Debug
    int debug_level;                // Nível de debug
    uint64_t breakpoints[4];        // Breakpoints
    
} process_t;

//==============================================================================
// Process Functions - Gerenciamento Básico
//==============================================================================
void process_init(void);
process_t* process_create(const char* name, void* entry, uint64_t priority, int flags);
void process_destroy(process_t* proc);
void process_exit(int code);
int process_wait(process_t* proc, int* code);

//==============================================================================
// Process Functions - Scheduling
//==============================================================================
void process_yield(void);
void process_sleep(uint64_t ticks);
void process_wakeup(process_t* proc);
void process_set_priority(process_t* proc, int priority);
void process_set_nice(process_t* proc, int nice);
process_t* process_get_current(void);
process_t* process_get_by_pid(uint64_t pid);

//==============================================================================
// Process Functions - Informação
//==============================================================================
void process_list(void);
void process_info(process_t* proc);
void process_stats(process_t* proc);
void process_print_context(process_t* proc);

//==============================================================================
// Process Functions - IPC
//==============================================================================
int process_send_message(uint64_t to, const void* data, size_t size);
int process_receive_message(void* buffer, size_t size, uint64_t* from);

//==============================================================================
// Process Functions - Debug
//==============================================================================
void process_set_debug(process_t* proc, int level);
void process_trace(process_t* proc, int enable);
void process_dump(process_t* proc);

#endif
