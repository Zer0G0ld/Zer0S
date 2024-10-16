ENTRY(kernel_main)

SECTIONS {
    /* Alocação de memória do kernel e outros segmentos */
    . = 1M; /* O kernel será carregado a partir de 1 MB */

    /* Seção de código (somente leitura e execução) */
    .text : ALIGN(4K) {
        *(.text)       /* Inclui todas as seções .text dos objetos */
    } :text = 0x100000 /* Endereço de início do código */

    /* Seção de dados inicializados (leitura e escrita) */
    .data : ALIGN(4K) {
        *(.data)       /* Inclui todas as seções .data dos objetos */
    } :data = . + 4K

    /* Seção de dados não inicializados (BSS) (leitura e escrita) */
    .bss : ALIGN(4K) {
        *(.bss)        /* Inclui todas as seções .bss dos objetos */
        *(COMMON)
    } :bss = . + 8K

    /* Seção da pilha */
    .stack ALIGN(4K) : {
        *(.stack)      /* Aloca a pilha */
    }

    /* Definir o tamanho da pilha (exemplo: 16KB) */
    _stack_size = 16K;
    _stack_start = .;
    _stack_end = _stack_start + _stack_size;

    /* Definir o símbolo de término */
    _end = .;
}
