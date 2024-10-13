; 1. Configurar o Modo Protegido (Protected Mode)
BITS 16
org 0x7c00                  ; Definindo o ponto de partida do código do boot

; Desabilitar interrupções
cli

; Configurar o A20 gate para acessar mais de 1MB de memória
in al, 0x64
test al, 2
jnz $-2
mov al, 0xD1
out 0x64, al
in al, 0x64
test al, 2
jnz $-2
mov al, 0xDF
out 0x60, al

; Definir o início da GDT
gdt_start:

; Descritor nulo
dq 0x0000000000000000
; Código segmento
dq 0x00cf9a000000ffff
; Dados segmento
dq 0x00cf92000000ffff

gdt_end:

; Carregar o endereço da tabela de descritores (GDT)
lgdt [gdt_descriptor]

; Ativar o modo protegido
mov eax, cr0
or eax, 1
mov cr0, eax

; Saltar para o código de 32 bits (modo protegido)
jmp $+2                   ; Pequeno salto para garantir que o código posterior seja em 32 bits

[BITS 32]
protected_mode:
    ; Configurar os segmentos
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9000

    ; 2. Carregar o Kernel
    mov bx, 0x1000          ; Endereço onde o kernel será carregado
    mov dh, 0x02            ; Número de setores a carregar (Deve ser ajustado conforme o tamanho do kernel)
    call load_kernel

    ; Saltar para o kernel em 0x1000
    jmp 0x1000              ; Salto para o kernel carregado

    ; Função para carregar o kernel da imagem do disco
load_kernel:
        mov ah, 0x02        ; Função 02h do INT 13h - Ler setores
        mov al, dh          ; Número de setores a carregar
        mov ch, 0x00        ; Cilindro 0
        mov cl, 0x02        ; Setor 2 ( o kernel começa após o setor de boot)
        mov dh, 0x00        ; Cabeça 0
        mov dl, 0x80        ; Disco 0 ( Disco Rígido)
        int 0x13            ; Chama a interrupção da BIOS para ler o disco
        jc disk_error       ; Se houver erro, pular para o tratamento de erro
        ret

disk_error:
        hlt                 ; Se houver erro, suspender o sistema

    ; Suspender a execução
    hlt

gdt_descriptor:
    dw gdt_end - gdt_start - 1 ; Tamanho da GDT
    dd gdt_start              ; Endereço da GDT

times 510 - ($ - $$) db 0      ; Preencher até 510 bytes
dw 0xAA55                      ; Assinatura do setor de boot
