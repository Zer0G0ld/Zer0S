ENTRY(kernel_main)

SECTIONS {
    . = 1M; /* A partir de 1 MB, onde o kernel será carregado */

    /* O código do kernel */
    .text : {
        *(.text)       /* Inclui todas as seções .text dos objetos */
    }

    /* Dados não inicializados do kernel (bss) */
    .bss : {
        *(.bss)        /* Inclui todas as seções .bss dos objetos */
    }

    /* Dados inicializados do kernel */
    .data : {
        *(.data)       /* Inclui todas as seções .data dos objetos */
    }

    /* O final do kernel */
    . = 4M;          /* O kernel acaba em 4MB */

    /* Stack */
    .stack : {
        *(.stack)      /* Aloca a pilha */
    }
}
