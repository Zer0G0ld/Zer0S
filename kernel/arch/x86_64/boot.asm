; boot.asm - Zer0S 64-bit Bootloader
; Transição de 32-bit para 64-bit Long Mode

[BITS 32]

section .multiboot
align 4
    dd 0x1BADB002
    dd 0x03
    dd -(0x1BADB002 + 0x03)

section .text
global start
extern kernel_main

start:
    ; Save multiboot info
    mov [magic], eax
    mov [mbd], ebx
    
    ; Setup stack
    mov esp, stack_low_end
    
    ; Check long mode
    call check_long_mode
    test eax, eax
    jnz setup_long_mode
    
    ; Error: no long mode
    mov esi, error_msg
    call print_string
    cli
    hlt

check_long_mode:
    ; Check CPUID
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
    
    ; Check extended functions
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

setup_long_mode:
    ; Clear page tables
    mov edi, 0x1000
    mov cr3, edi
    xor eax, eax
    mov ecx, 0x3000 / 4
    rep stosd
    mov edi, cr3
    
    ; PML4
    mov dword [edi], 0x2003
    add edi, 0x1000
    
    ; PDPT
    mov dword [edi], 0x3003
    add edi, 0x1000
    
    ; PD (2MB pages for first 16MB)
    mov dword [edi], 0x83
    mov dword [edi+8], 0x200083
    mov dword [edi+16], 0x400083
    mov dword [edi+24], 0x600083
    mov dword [edi+32], 0x800083
    mov dword [edi+40], 0xA00083
    mov dword [edi+48], 0xC00083
    mov dword [edi+56], 0xE00083
    
    ; Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    
    ; Set long mode in EFER
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr
    
    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax
    
    ; Load GDT
    lgdt [gdt_desc]
    
    ; Jump to 64-bit
    jmp 0x08:long_mode_entry

print_string:
    pusha
    mov edi, 0xB8000
.loop:
    lodsb
    test al, al
    jz .done
    mov [edi], al
    inc edi
    mov byte [edi], 0x0C
    inc edi
    jmp .loop
.done:
    popa
    ret

[BITS 64]
long_mode_entry:
    ; Update segments
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Setup stack
    mov rsp, stack_high_end
    
    ; Call kernel
    mov rdi, [magic]
    mov rsi, [mbd]
    call kernel_main
    
    cli
    hlt

section .data
error_msg db "Error: Long mode not supported", 0
magic dd 0
mbd dd 0

; GDT
align 8
gdt:
    dq 0x0000000000000000
    dq 0x00209A0000000000  ; Code segment
    dq 0x0000920000000000  ; Data segment
gdt_end:

gdt_desc:
    dw gdt_end - gdt - 1
    dq gdt

section .bss
align 16
stack_low:
    resb 16384
stack_low_end:

stack_high:
    resb 16384
stack_high_end:
