OUTPUT_FORMAT("elf32-i386")
OUTPUT_ARCH(i386)
ENTRY(kernel_main)

MEMORY {
    ram (rw) : ORIGIN = 0x00000000, LENGTH = 4M /* Ajuste o comprimento conforme necessário */
}

SECTIONS {
    . = 1M; /* O kernel será carregado a partir de 1 MB */

    /* Seção de código (somente leitura e execução) */
    .text ALIGN(4K) : {
        *(.text)       /* Inclui todas as seções .text dos objetos */
    }

    /* Seção de dados inicializados (leitura e escrita) */
    .data ALIGN(4K) : {
        *(.data)       /* Inclui todas as seções .data dos objetos */
    }

    /* Seção de dados não inicializados (BSS) (leitura e escrita) */
    .bss ALIGN(4K) : {
        *(.bss)        /* Inclui todas as seções .bss dos objetos */
        *(COMMON)
    }

    /* Seção da pilha */
    .stack ALIGN(4K) : {
        . = . + 16K;    /* Define o tamanho da pilha (16KB) */
    } >ram   /* Coloca a pilha na RAM */

    /* Símbolos de fim e início da pilha */
    _stack_end = .;       /* O topo da pilha */
    _stack_start = _stack_end - 16K; /* O início da pilha */

    /* Definir o símbolo de término */
    _end = .;
}
