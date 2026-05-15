/* Sky-OS Saf Çekirdek Ana Motoru */

// Bilgisayarın ekran kartı hafızasının tam merkez adresi (VGA Metin Modu)
#define VGA_ADDRESS 0xB8000

void kernel_main(void) {
    // Ekrana basacağımız siber-punk mesaj
    const char *str = "WELCOME TO SKY-OS: THE AI CORE IS ALIVE!";
    
    // Ekran hafızasına doğrudan erişim sağlayan siber pointer
    char *video_memory = (char *)VGA_ADDRESS;
    
    // Önce tüm ekranı temizleyelim (Siyah arka plan)
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i+1] = 0x07; // Beyaz yazı, siyah arka plan
    }
    
    // Mesajımızı doğrudan RAM'deki ekran bloklarına enjekte ediyoruz
    int i = 0;
    while (str[i] != '\0') {
        video_memory[i * 2] = str[i];     // Karakterin kendisi
        video_memory[i * 2 + 1] = 0x0F; // Parlak beyaz renk kodu!
        i++;
    }
}
