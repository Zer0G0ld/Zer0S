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

    /* Pilha (opcional - só pra marcar espaço, já que a pilha é criada manualmente no código C) */
    .stack ALIGN(4K) : {
        . = . + 16K;
    }

    /* Símbolos úteis */
    _stack_end = .;
    _stack_start = _stack_end - 16K;

    _end = .;
}
