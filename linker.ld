ENTRY(_start)

SECTIONS
{
    /* Çekirdek x86 standartlarında 1MB (0x100000) fiziksel adresine yüklenir */
    . = 0x100000;

    /* CRITICAL ZIRH: Multiboot başlığını dosyanın EN BAŞINA çiviliyoruz */
    .text : AT(ADDR(.text))
    {
        *(.multiboot)    /* boot.asm içindeki Multiboot header kesinlikle İLK sırada olmak zorunda! */
        *(.text)         /* Ardından çekirdeğin makine kodları gelir */
    }

    /* Salt okunur veriler, string sabitleri ve font matrisleri */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Başlangıç değeri atanmış global ve statik veriler */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Başlangıç değeri atanmamış değişkenler ve yığın (Stack) alanı bittiği yer */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }
}
