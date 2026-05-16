ENTRY(_start)

SECTIONS
{
    /* Çekirdek 1MB adresinden başlar */
    . = 0x100000;

    /* En kritik alan: Multiboot başlığını dosyanın EN BAŞINA çiviliyoruz */
    .text : AT(ADDR(.text))
    {
        *(.multiboot)    /* boot.asm içindeki Multiboot header kesinlikle İLK sırada */
        *(.text)         /* Ardından normal kodlar */
    }

    .rodata : { *(.rodata) }
    .data   : { *(.data)   }
    .bss    : { *(.bss)    }
}
