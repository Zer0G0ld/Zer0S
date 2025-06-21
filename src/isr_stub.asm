[BITS 32]

%macro ISR_NOERRCODE 1
global isr%1
extern isr_handler
isr%1:
    cli
    push dword 0        ; Código de erro (0 para ISRs sem error code)
    push dword %1       ; Número da interrupção

    push ds
    push es
    push fs
    push gs

    pusha

    mov eax, esp
    push eax
    call isr_handler
    add esp, 4

    popa

    pop gs
    pop fs
    pop es
    pop ds

    add esp, 8          ; Remove int_no e error_code

    sti
    iret
%endmacro

%macro ISR_ERRCODE 1
global isr%1
extern isr_handler
isr%1:
    cli
    push dword %1       ; Número da interrupção

    push ds
    push es
    push fs
    push gs

    pusha

    mov eax, esp
    push eax
    call isr_handler
    add esp, 4

    popa

    pop gs
    pop fs
    pop es
    pop ds

    add esp, 4          ; Remove apenas int_no, pois error_code já veio da CPU

    sti
    iret
%endmacro

; ==========================
; ISRs sem código de erro:
; ==========================
ISR_NOERRCODE 0
ISR_NOERRCODE 1
ISR_NOERRCODE 2
ISR_NOERRCODE 3
ISR_NOERRCODE 4
ISR_NOERRCODE 5
ISR_NOERRCODE 6
ISR_NOERRCODE 7
; ISR 8 TEM error code (Double Fault)

ISR_NOERRCODE 9
; ISR 10–14 têm error code:
; 10: Invalid TSS
; 11: Segment Not Present
; 12: Stack-Segment Fault
; 13: General Protection Fault
; 14: Page Fault

ISR_NOERRCODE 15
ISR_NOERRCODE 16
ISR_NOERRCODE 17
; ISR 18: Machine Check (não tem err code em x86)
ISR_NOERRCODE 18
ISR_NOERRCODE 19
ISR_NOERRCODE 20
ISR_NOERRCODE 21
ISR_NOERRCODE 22
ISR_NOERRCODE 23
ISR_NOERRCODE 24
ISR_NOERRCODE 25
ISR_NOERRCODE 26
ISR_NOERRCODE 27
ISR_NOERRCODE 28
ISR_NOERRCODE 29
ISR_NOERRCODE 30
ISR_NOERRCODE 31

; ==========================
; ISRs que têm código de erro da CPU:
; ==========================
ISR_ERRCODE 8
ISR_ERRCODE 10
ISR_ERRCODE 11
ISR_ERRCODE 12
ISR_ERRCODE 13
ISR_ERRCODE 14

; ===============================
; IRQs 0 a 15 → Vetores 32 a 47
; ===============================
%macro IRQ 1
global irq%1
extern isr_handler
irq%1:
    cli
    push dword 0            ; Código de erro fictício
    push dword (32 + %1)    ; Número da interrupção (ex: IRQ0 → 32)

    push ds
    push es
    push fs
    push gs

    pusha

    mov eax, esp
    push eax
    call isr_handler
    add esp, 4

    popa
    pop gs
    pop fs
    pop es
    pop ds

    add esp, 8
    sti
    iret
%endmacro

IRQ 0
IRQ 1
IRQ 2
IRQ 3
IRQ 4
IRQ 5
IRQ 6
IRQ 7
IRQ 8
IRQ 9
IRQ 10
IRQ 11
IRQ 12
IRQ 13
IRQ 14
IRQ 15
