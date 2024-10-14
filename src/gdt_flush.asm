section .text
    global gdt_flush

gdt_flush:
    ; Carrega o ponteiro da GDT que está no argumento passado via registrador
    ; esp+4 é onde o primeiro argumento está no stack (gdt_ptr)
    lgdt [esp+4]

    ; Atualiza os registradores de segmento
    mov ax, 0x10        ; Seleciona o segmento de dados (0x10 é o offset da segunda entrada da GDT)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Pule para o novo segmento de código
    jmp 0x08:.flush_end ; 0x08 é o offset da primeira entrada da GDT (segmento de código)

.flush_end:
    ret
