OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(kernel_main)

SECTIONS {
    /* O GRUB vai carregar o kernel a partir de 1 MiB (0x00100000) */
    . = 1M;

    /* Seção Multiboot Header (precisa vir logo no início) */
    .multiboot :
    {
        KEEP(*(.multiboot))
    }

    /* Código */
    .text ALIGN(4K) : {
        *(.text)
    }

    /* Dados inicializados */
    .data ALIGN(4K) : {
        *(.data)
    }

    /* Dados não inicializados (bss) */
    .bss ALIGN(4K) : {
        *(.bss)
        *(COMMON)
    }

    /* Espaço reservado para a pilha (16 KB), alinhado em 4 KB */
    .stack ALIGN(4K) : {
        . = . + 16K;        /* reserva 16KB para pilha */
    }

    /* Símbolos para marcar os limites da pilha */
    _stack_end = .;          /* topo da pilha (endereço mais alto) */
    _stack_start = _stack_end - 16K; /* início da pilha */

    /* Símbolo para o fim do kernel */
    _end = .;
}
