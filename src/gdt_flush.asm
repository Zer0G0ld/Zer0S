section .text
global gdt_flush

gdt_flush:
    mov eax, [esp + 4]  ; pega ponteiro para gdt_ptr
    lgdt [eax]          ; carrega o GDTR

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    jmp 0x08:.flush_end

.flush_end:
    ret
