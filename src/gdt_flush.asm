section .text
global gdt_flush

gdt_flush:
    ; Pega o endereço do ponteiro da GDT passado na pilha
    mov eax, [esp + 4]
    lgdt [eax]              ; Carrega o GDTR com o ponteiro da GDT

    cli
    ; Configura todos os segmentos de dados para o seletor 0x10 (descritor 2)
    mov ax, 0x10            ; Selector do segmento de dados (offset 2 na GDT, index 2 * 8 = 0x10)
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Ajusta o ponteiro da pilha (ESP) para um local válido
    ; Você precisa garantir que _stack_end está definido e aponta para uma área de pilha válida
    extern _stack_end
    mov esp, _stack_end
    sti

    ; Far jump para carregar o segmento de código CS corretamente
    ; 0x08 é o seletor da entrada 1 (descritor de código) na GDT (1 * 8 = 0x08)
    jmp 0x08:.flush_end

.flush_end:
    ret
