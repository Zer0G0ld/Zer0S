; switch.asm - Context switching for process manager
[BITS 64]

section .text
global switch_context
global switch_to_user

; void switch_context(proc_context_t* old, proc_context_t* new)
switch_context:
    ; Save current context
    mov [rdi + 0x00], rax
    mov [rdi + 0x08], rbx
    mov [rdi + 0x10], rcx
    mov [rdi + 0x18], rdx
    mov [rdi + 0x20], rsi
    mov [rdi + 0x28], rdi
    mov [rdi + 0x30], rbp
    mov [rdi + 0x38], rsp
    mov [rdi + 0x40], r8
    mov [rdi + 0x48], r9
    mov [rdi + 0x50], r10
    mov [rdi + 0x58], r11
    mov [rdi + 0x60], r12
    mov [rdi + 0x68], r13
    mov [rdi + 0x70], r14
    mov [rdi + 0x78], r15
    
    ; Save RIP and RFLAGS
    mov rax, [rsp]      ; RIP is at top of stack
    mov [rdi + 0x80], rax
    mov rax, [rsp + 8]  ; RFLAGS is next
    mov [rdi + 0x88], rax
    
    ; Load new context
    mov rax, [rsi + 0x00]
    mov rbx, [rsi + 0x08]
    mov rcx, [rsi + 0x10]
    mov rdx, [rsi + 0x18]
    mov rbp, [rsi + 0x30]
    mov r8,  [rsi + 0x40]
    mov r9,  [rsi + 0x48]
    mov r10, [rsi + 0x50]
    mov r11, [rsi + 0x58]
    mov r12, [rsi + 0x60]
    mov r13, [rsi + 0x68]
    mov r14, [rsi + 0x70]
    mov r15, [rsi + 0x78]
    
    ; Load stack and return address
    mov rsp, [rsi + 0x38]
    push qword [rsi + 0x88]  ; RFLAGS
    push qword [rsi + 0x80]  ; RIP
    
    ; Load remaining registers
    mov rsi, [rsi + 0x20]
    mov rdi, [rsi + 0x28]    ; RDI is last
    
    ; Return to new context
    iretq

; void switch_to_user(uint64_t rip, uint64_t rsp)
switch_to_user:
    cli
    mov rsp, rsi
    push 0x23        ; User SS
    push rsi         ; User RSP
    pushfq
    push 0x1B        ; User CS
    push rdi         ; User RIP
    iretq
