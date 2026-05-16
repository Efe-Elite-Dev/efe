ENTRY(_start)

SECTIONS
{
    /* Çekirdeğin RAM'e yükleneceği başlangıç adresi.
       2 GB üst sınırına kadar olan Protected Mode alanını pürüzsüz kullanabilmek için
       temel yükleme adresini yine güvenli olan 1M (ya da tercihine göre üst adresler) yapıyoruz.
    */
    . = 1M;

    /* Kod bölümü (Executable Code) - 4KB sayfa hizalamalı */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)   /* GRUB'ın tanıması için Multiboot Header en başta olmalı! */
        *(.text)
    }

    /* Sadece okunabilir veriler (Yazılar, sabit renk tanımları) */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata)
    }

    /* Düzenlenebilir veriler (Global değişkenler, fare konumları, pencereler) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data)
    }

    /* Başlatılmamış veriler ve Stack alanı (Hafızada yer kaplamayan statik bloklar) */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss)
    }

    /* Sistem bittiğinde linker'ın sınır kontrolü yapabilmesi için.
       Çekirdeğin toplam boyutunun senin belirttiğin 2 GB sınırını aşmadığından emin oluyoruz.
    */
    _kernel_end = .;
}
