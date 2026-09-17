#include <stdio.h>
#include <stdlib.h>

// Her bir byte'ı bitlerine (0 ve 1) ayıran fonksiyon
void byte_to_binary(unsigned char byte) {
    // 8 bit üzerinde soldan sağa doğru kaydırma (shift) işlemi yapıyoruz
    for (int i = 7; i >= 0; i--) {
        int bit = (byte >> i) & 1;
        printf("%d", bit);
    }
    printf(" "); // Okunabilirlik için her byte arasına boşluk bırakalım
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Kullanım: %s <dosya_yolu>\n", argv[0]);
        return 1;
    }

    // Dosyayı binary read (rb) modunda açıyoruz
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Dosya açılamadı");
        return 1;
    }

    int ch;
    printf("[ThruSonic Encoder] Dosya ikili koda dönüştürülüyor:\n");
    
    // Dosyanın sonuna (EOF) gelene kadar her bir byte'ı tek tek okuyoruz
    while ((ch = fgetc(file)) != EOF) {
        byte_to_binary((unsigned char)ch);
    }
    
    printf("\n[Dönüşüm Tamamlandı]\n");

    fclose(file);
    return 0;
}
