/* Linker script for Zer0S 64-bit - Higher Half Kernel */
ENTRY(start)

SECTIONS
{
    /* Kernel physical load address */
    . = 1M;

    /* Multiboot header (must be in first 8KB) */
    .multiboot : ALIGN(4K)
    {
        *(.multiboot)
    }

    /* Higher half - kernel will be mapped here */
    . += 0xFFFFFFFF80000000;

    /* Code */
    .text : AT(ADDR(.text) - 0xFFFFFFFF80000000)
    {
        code_start = .;
        *(.text*)
        . = ALIGN(4K);
        code_end = .;
    }

    /* Read-only data */
    .rodata : AT(ADDR(.rodata) - 0xFFFFFFFF80000000)
    {
        rodata_start = .;
        *(.rodata*)
        . = ALIGN(4K);
        rodata_end = .;
    }

    /* Data */
    .data : AT(ADDR(.data) - 0xFFFFFFFF80000000)
    {
        data_start = .;
        *(.data*)
        . = ALIGN(4K);
        data_end = .;
    }

    /* BSS */
    .bss : AT(ADDR(.bss) - 0xFFFFFFFF80000000)
    {
        bss_start = .;
        *(COMMON)
        *(.bss*)
        . = ALIGN(4K);
        bss_end = .;
    }

    /* Stack */
    stack_bottom = .;
    . = . + 16K;
    stack_top = .;

    kernel_end = .;
}
