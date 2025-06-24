section .text
global gdt_flush

gdt_flush:
    mov eax, [esp + 4]
    lgdt [eax]

    ; Carrega os segmentos de dados
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Importante! Corrigir o ESP após trocar o SS
    ; Define uma pilha temporária segura (por exemplo, no topo da BSS)
    extern _stack_end
    mov esp, _stack_end

    ; Faz o far jump para carregar o novo CS
    jmp 0x08:.flush_end

.flush_end:
    ret
