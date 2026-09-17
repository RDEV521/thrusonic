#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

#define SAMPLE_RATE 44100
#define BIT_DURATION 0.01 // Verici ile aynı: 10ms
#define SAMPLES_PER_BIT 441 // 44100 * 0.01
#define FREQ_0 18000
#define FREQ_1 19000
#define N 441 // Her seferinde 10ms'lik pencere analiz edeceğiz

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Kullanım: %s <kayit_dosyasi.raw> <cikti_dosyasi>\n", argv[0]);
        return 1;
    }

    // 1. Kaydedilen ham ses dosyasını aç
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Kayıt dosyası açılamadı");
        return 1;
    }

    // 2. Çıktıi dosyasını (orijinal dosya) oluştur
    FILE *out_file = fopen(argv[2], "wb");
    if (out_file == NULL) {
        perror("Çıktı dosyası oluşturulamadı");
        fclose(file);
        return 1;
    }

    // 3. FFTW için bellek ayır
    double *in = (double*) fftw_malloc(sizeof(double) * N);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (N/2 + 1));
    fftw_plan plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);

    short *buffer = (short*) malloc(sizeof(short) * N);
    unsigned char current_byte = 0;
    int bit_count = 0;
    int total_bits = 0;

    printf("[ThruSonic Recv] Kayıt analiz ediliyor ve dosya yeniden oluşturuluyor...\n");

    // 4. Dosyayı 441'erli örnekler halinde oku
    while (fread(buffer, sizeof(short), N, file) == N) {
        // Normalize et
        for (int i = 0; i < N; i++) {
            in[i] = (double)buffer[i] / 32768.0;
        }

        // FFT'yi çalıştır
        fftw_execute(plan);

        // 18kHz ve 19kHz genliklerini bul
        double mag_18k = 0.0, mag_19k = 0.0;
        int bin_18k = (int)(FREQ_0 * N / SAMPLE_RATE);
        int bin_19k = (int)(FREQ_1 * N / SAMPLE_RATE);

        // Genlikleri hesapla (sqrt(re^2 + im^2))
        mag_18k = sqrt(out[bin_18k][0]*out[bin_18k][0] + out[bin_18k][1]*out[bin_18k][1]);
        mag_19k = sqrt(out[bin_19k][0]*out[bin_19k][0] + out[bin_19k][1]*out[bin_19k][1]);

        // Karar ver: Hangisi daha güçlü?
        int bit = (mag_19k > mag_18k) ? 1 : 0;

        // Bit'i byte'a ekle
        current_byte = (current_byte << 1) | bit;
        bit_count++;
        total_bits++;

        // 8 bit olduysa byte'ı dosyaya yaz
        if (bit_count == 8) {
            fwrite(&current_byte, 1, 1, out_file);
            current_byte = 0;
            bit_count = 0;
        }
    }

    printf("[Tamamlandı] Toplam %d bit işlendi, dosya oluşturuldu: %s\n", total_bits, argv[2]);

    // Temizlik
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    free(buffer);
    fclose(file);
    fclose(out_file);

    return 0;
}
