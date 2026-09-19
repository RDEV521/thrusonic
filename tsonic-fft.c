#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

#define N 220500
#define SAMPLE_RATE 44100

int main() {
    // 1. Kaydedilen ham ses dosyasını aç
    FILE *file = fopen("recorded.raw", "rb");
    if (file == NULL) {
        perror("recorded.raw dosyası bulunamadı. Önce tsonic-rec çalıştırın.");
        return 1;
    }

    // 2. FFTW için bellek ayır (double array)
    double *in = (double*) fftw_malloc(sizeof(double) * N);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * (N/2 + 1));

    // 3. Dosyadan N kadar 16-bit (short) oku ve double'a çevir
    short *temp_buffer = (short*) malloc(sizeof(short) * N);
    size_t read_count = fread(temp_buffer, sizeof(short), N, file);
    fclose(file);

    if (read_count < N) {
        printf("Uyarı: Dosyada yeterli veri yok, okunan: %zu\n", read_count);
    }

    for (int i = 0; i < N; i++) {
        // Ses verisini -1.0 ile 1.0 arasına normalize et
        in[i] = (double)temp_buffer[i] / 32768.0; 
    }

    // 4. FFT Planını oluştur (Gerçek veriden karmaşık veriye)
    fftw_plan plan = fftw_plan_dft_r2c_1d(N, in, out, FFTW_ESTIMATE);
    
    // 5. FFT'yi Çalıştır!
    fftw_execute(plan);

    // 6. Frekansları analiz et (En yüksek genliği bul)
    double max_magnitude = 0.0;
    int max_index = 0;

    // DC bileşenini (0 Hz) atla, sadece 1. indeksten başla
    for (int i = 1; i < (N/2 + 1); i++) {
        double magnitude = sqrt(out[i][0]*out[i][0] + out[i][1]*out[i][1]);
        if (magnitude > max_magnitude) {
            max_magnitude = magnitude;
            max_index = i;
        }
    }

    // 7. İndeksi frekansa çevir (Frekans = İndeks * Örnekleme Hızı / N)
    double peak_frequency = (double)max_index * SAMPLE_RATE / N;

    printf("--- ThruSonic FFT Analizi ---\n");
    printf("Analiz Edilen Örnek Sayısı: %d\n", N);
    printf("Baskın Frekans: %.2f Hz\n", peak_frequency);
    printf("Genlik (Şiddet): %.2f\n", max_magnitude);

    // Temizlik
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    free(temp_buffer);

    return 0;
}
