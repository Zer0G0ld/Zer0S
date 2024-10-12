BITS 16
org 0x7c00               ; Definindo o ponto de partida do código do boot

; Exibe uma mensagem simples na tela
mov si, message           ; Carrega o endereço da mensagem no SI (source index)

print_char:
    mov al, [si]          ; Carrega o próximo caractere da string
    cmp al, 0             ; Verifica se é o final da string (null terminator)
    je  done              ; Se for zero, fim da string
    mov ah, 0x0E          ; Função para exibir um caractere
    int 0x10              ; Chama a interrupção de vídeo (BIOS)
    inc si                ; Avança para o próximo caractere
    jmp print_char        ; Repete o processo

done:
    ; Suspende a execução com a instrução hlt
    cli                    ; Desabilita interrupções
    hlt                    ; Finaliza o código (interrompe o processador)

; Define a mensagem a ser exibida
message db 'Hello, Boot!', 0

; Preenche o resto do setor de boot com 0s até o byte 510
times 510 - ($ - $$) db 0

; Assinatura do setor de boot (0xAA55)
dw 0xAA55
