; boot.asm - Bootloader para Zer0S 64-bit
; Transição de 32-bit protected mode para 64-bit long mode

[BITS 32]

section .multiboot
align 4
    dd 0x1BADB002              ; Magic number
    dd 0x03                    ; Flags (align modules)
    dd -(0x1BADB002 + 0x03)    ; Checksum

section .text
global start
extern kernel_main

start:
    ; Save multiboot info
    mov [multiboot_magic], eax
    mov [multiboot_addr], ebx
    
    ; Setup temporary stack (lower half)
    mov esp, stack_bottom_low
    
    ; Check for long mode support
    call check_long_mode
    test eax, eax
    jnz .long_mode_ok
    
    ; No long mode - print error and halt
    mov esi, error_no_long_mode
    call print_string_32
    cli
    hlt

.long_mode_ok:
    ; Setup page tables and enter long mode
    call setup_long_mode
    
    ; We should not return here
    cli
    hlt

; Check if CPU supports long mode
check_long_mode:
    ; Check CPUID support
    pushfd
    pop eax
    mov ecx, eax
    xor eax, 1 << 21
    push eax
    popfd
    pushfd
    pop eax
    push ecx
    popfd
    cmp eax, ecx
    je .no_long_mode
    
    ; Check extended function support
    mov eax, 0x80000000
    cpuid
    cmp eax, 0x80000001
    jb .no_long_mode
    
    ; Check long mode bit
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    
    mov eax, 1
    ret
    
.no_long_mode:
    xor eax, eax
    ret

; Setup paging and enter long mode
setup_long_mode:
    ; Clear page tables (using physical address 0x1000-0x3FFF)
    mov edi, 0x1000
    mov cr3, edi
    xor eax, eax
    mov ecx, 0x3000 / 4
    rep stosd
    mov edi, cr3
    
    ; Setup PML4 (level 4)
    mov dword [edi], 0x2003      ; Point to PDPT (present, r/w, user)
    add edi, 0x1000
    
    ; Setup PDPT (level 3)
    mov dword [edi], 0x3003      ; Point to PD (present, r/w, user)
    add edi, 0x1000
    
    ; Setup PD (level 2) - Identity map first 16MB with 2MB pages
    mov dword [edi], 0x83        ; 0-2MB
    mov dword [edi+8], 0x200083  ; 2-4MB
    mov dword [edi+16], 0x400083 ; 4-6MB
    mov dword [edi+24], 0x600083 ; 6-8MB
    mov dword [edi+32], 0x800083 ; 8-10MB
    mov dword [edi+40], 0xA00083 ; 10-12MB
    mov dword [edi+48], 0xC00083 ; 12-14MB
    mov dword [edi+56], 0xE00083 ; 14-16MB
    
    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    
    ; Set long mode bit in EFER
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    
    ; Load GDT
    lgdt [gdt_pointer]
    
    ; Far jump to 64-bit code
    jmp 0x08:.long_mode_entry
    
[BITS 64]
.long_mode_entry:
    ; Update segment registers
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Setup stack (higher half)
    mov rsp, stack_top_high
    
    ; Call kernel main with multiboot info
    mov rdi, [multiboot_magic]
    mov rsi, [multiboot_addr]
    call kernel_main
    
    ; Halt if returns
    cli
    hlt

; 32-bit print function (for error messages)
[BITS 32]
print_string_32:
    pusha
    mov edi, 0xB8000
.loop:
    lodsb
    test al, al
    jz .done
    mov [edi], al
    inc edi
    mov byte [edi], 0x0C  ; Red color
    inc edi
    jmp .loop
.done:
    popa
    ret

; Data section (lower half)
section .data
error_no_long_mode db "Error: CPU does not support 64-bit long mode", 0
multiboot_magic dd 0
multiboot_addr dd 0

; Temporary GDT for long mode transition
align 8
gdt:
    dq 0x0000000000000000  ; Null descriptor
    dq 0x00209A0000000000  ; 64-bit code segment (present, exec, read)
    dq 0x0000920000000000  ; 64-bit data segment (present, write)
gdt_end:

gdt_pointer:
    dw gdt_end - gdt - 1
    dq gdt

; Stack (lower half - temporary)
section .bss
align 16
stack_bottom_low:
    resb 16384
stack_top_low:

; Stack (higher half - permanent) - Moved to BSS to avoid warning
section .bss
align 16
stack_bottom_high:
    resb 16384
stack_top_high:
