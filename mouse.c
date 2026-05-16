/* Sky-OS Bağımsız Fare Sürücüsü Modülü */

#define MOUSE_DATA_PORT 0x60
#define MOUSE_STATUS_PORT 0x64

// kernel.c içindeki port okuma ve ekran fonksiyonlarını buraya köprü yapıyoruz
extern unsigned char inb(unsigned short port);
extern void print_string(const char *str, char color);

void check_mouse_pure(void) {
    static int cycle = 0;
    static unsigned char mouse_packet[3];

    // Portta okunacak veri var mı?
    if (inb(MOUSE_STATUS_PORT) & 1) {
        // Gelen veri fareden mi geliyor kontrolü (Bit 5)
        if (inb(MOUSE_STATUS_PORT) & 0x20) {
            unsigned char current_byte = inb(MOUSE_DATA_PORT);
            mouse_packet[cycle] = current_byte;
            cycle++;

            if (cycle == 3) {
                cycle = 0;
                int left_click = mouse_packet[0] & 1;

                if (left_click) {
                    print_string("[TIK!] ", 0x0C); // Tıklanınca Kırmızı [TIK!]
                }
            }
        }
    }
}
