ENTRY(_start)

SECTIONS
{
    /* Çekirdek x86 standartlarında 1MB adresine yüklenir */
    . = 0x100000;

    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)   /* boot.asm içindeki başlık buraya kilitlenir */
        *(.text)
    }

    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
