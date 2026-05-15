// Sky-OS Kernel Main (Workflow ilk satiri silecek, bu yazi gidecek)
void kernel_main(unsigned long magic, unsigned long addr) {
    volatile char* video_memory = (volatile char*)0xB8000;
    const char* message = "Sky-OS Saf AI Kernel Basariyla Baslatildi!";
    
    char color = 0x0F; // Beyaz yazi, siyah arka plan

    // Ekranı temizle
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = color;
    }

    // Mesajı ekrana yazdır
    int i = 0;
    while (message[i] != '\0') {
        video_memory[i * 2] = message[i];
        video_memory[i * 2 + 1] = color;
        i++;
    }

    while (1) {
        __asm__ __volatile__("hlt");
    }
}
// Workflow en son satiri silecek, bu yazi gidecek ve kodun parantezi korunacak!
