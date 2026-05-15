void kernel_main(void) {
    volatile char* video_memory = (volatile char*)0xB8000;
    const char* message = "Sky-OS Saf AI Kernel Basariyla Baslatildi!";
    char color = 0x0F; 

    // Ekranı temizle
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = color;
    }

    // Mesajı yazdır
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
