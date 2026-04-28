; boot.asm - Zer0S 64-bit Bootloader
; Transição de 32-bit para 64-bit Long Mode
; Melhorado: Suporte a mais memória (64MB), melhor organização

[BITS 32]

;==============================================================================
; MULTIBOOT HEADER
;==============================================================================
section .multiboot
align 4
    dd 0x1BADB002          ; Magic number
    dd 0x03                ; Flags (align modules)
    dd -(0x1BADB002 + 0x03) ; Checksum

;==============================================================================
; CODE SECTION - 32-bit Protected Mode
;==============================================================================
section .text
global start
extern kernel_main

start:
    ; Save multiboot info (GRUB passes: EAX=magic, EBX=addr)
    mov [magic], eax
    mov [mbd], ebx
    
    ; Setup temporary stack (lower half)
    mov esp, stack_low_end
    
    ; Check if CPU supports long mode
    call check_long_mode
    test eax, eax
    jnz setup_long_mode
    
    ; Error: long mode not supported
    mov esi, error_msg
    call print_string
    cli
    hlt

;------------------------------------------------------------------------------
; Check if CPU supports Long Mode (64-bit)
; Returns: EAX = 1 if supported, 0 if not
;------------------------------------------------------------------------------
check_long_mode:
    ; Check CPUID support (flip ID bit in EFLAGS)
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
    
    ; Check long mode bit (bit 29 in EDX)
    mov eax, 0x80000001
    cpuid
    test edx, 1 << 29
    jz .no_long_mode
    
    mov eax, 1
    ret
    
.no_long_mode:
    xor eax, eax
    ret

;------------------------------------------------------------------------------
; Setup Page Tables and Enter Long Mode
;------------------------------------------------------------------------------
setup_long_mode:
    ; Clear page tables (using physical address 0x1000-0x3FFF)
    mov edi, 0x1000
    mov cr3, edi
    xor eax, eax
    mov ecx, 0x3000 / 4
    rep stosd
    mov edi, cr3
    
    ; Level 4: PML4 (Page Map Level 4)
    mov dword [edi], 0x2003      ; Point to PDPT (present, r/w, user)
    add edi, 0x1000
    
    ; Level 3: PDPT (Page Directory Pointer Table)
    mov dword [edi], 0x3003      ; Point to PD (present, r/w, user)
    add edi, 0x1000
    
    ; Level 2: PD (Page Directory) - 2MB pages for first 64MB
    ; Each entry maps 2MB, 32 entries = 64MB
    mov ebx, 0x83                ; Base entry (present, r/w, user, PS=1)
    mov ecx, 32                  ; 32 entries for 64MB
    xor edx, edx
.setup_pd:
    mov dword [edi + edx*8], ebx
    add ebx, 0x200000            ; Next 2MB
    inc edx
    loop .setup_pd
    
    ; Enable PAE (Physical Address Extension)
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax
    
    ; Set Long Mode Enable (LME) bit in EFER
    mov ecx, 0xC0000080          ; EFER MSR
    rdmsr
    or eax, 1 << 8               ; LME bit
    wrmsr
    
    ; Enable paging
    mov eax, cr0
    or eax, 1 << 31              ; PG bit
    mov cr0, eax
    
    ; Load temporary GDT
    lgdt [gdt_desc]
    
    ; Far jump to 64-bit code segment
    jmp 0x08:long_mode_entry

;------------------------------------------------------------------------------
; Print string in 32-bit mode (for error messages)
; Input: ESI = pointer to string
;------------------------------------------------------------------------------
print_string:
    pusha
    mov edi, 0xB8000             ; VGA text memory
.loop:
    lodsb
    test al, al
    jz .done
    mov [edi], al
    inc edi
    mov byte [edi], 0x0C         ; Red color
    inc edi
    jmp .loop
.done:
    popa
    ret

;==============================================================================
; 64-BIT LONG MODE ENTRY
;==============================================================================
[BITS 64]

long_mode_entry:
    ; Update segment registers
    mov ax, 0x10                 ; Data segment selector
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    
    ; Setup permanent stack (higher half)
    mov rsp, stack_high_end
    
    ; Clear direction flag
    cld
    
    ; Call kernel main with multiboot info
    mov rdi, [magic]             ; First parameter (RDI) = magic
    mov rsi, [mbd]               ; Second parameter (RSI) = multiboot addr
    call kernel_main
    
    ; Halt if kernel returns
    cli
    hlt

;==============================================================================
; DATA SECTION
;==============================================================================
section .data
error_msg db "Error: Long mode not supported by this CPU", 0
magic dd 0
mbd dd 0

; Global Descriptor Table for 64-bit mode
align 8
gdt:
    dq 0x0000000000000000        ; Null descriptor
    dq 0x00209A0000000000        ; 64-bit Code segment (present, exec, read)
    dq 0x0000920000000000        ; 64-bit Data segment (present, write)
gdt_end:

gdt_desc:
    dw gdt_end - gdt - 1         ; GDT limit
    dq gdt                       ; GDT base address

;==============================================================================
; BSS SECTION - Stacks
;==============================================================================
section .bss
align 16

; Temporary stack (lower half - used during boot)
stack_low:
    resb 16384                   ; 16KB stack
stack_low_end:

; Permanent stack (higher half - used after entering long mode)
stack_high:
    resb 32768                   ; 32KB stack (doubled for safety)
stack_high_end: