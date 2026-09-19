#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <fftw3.h>

#define SAMPLE_RATE 44100
#define BIT_DURATION 0.01
#define FREQ_0 8000  //düşürüldü şuanlık
#define FREQ_1 10000
#define N 882
#define SYNC_BYTE 0xD5
#define MAG_THRESHOLD 3.0

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Kullanım: %s <kayit.raw> <cikti.txt>\n", argv[0]);
        return 1;
    }

    // 1. Kaydedilen ham ses dosyasını aç
    FILE *file = fopen(argv[1], "rb");
    if (file == NULL) {
        perror("Kayıt dosyası açılamadı");
        return 1;
    }

    // 2. Çıktı dosyasını oluştur
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

    // Frekans bin indeksleri
    int bin_18k = (int)(FREQ_0 * N / SAMPLE_RATE);
    int bin_19k = (int)(FREQ_1 * N / SAMPLE_RATE);

    // Alıcı durum değişkenleri
    int synced = 0;
    int silent_count = 0;
    int bit_count = 0;
    unsigned char current_byte = 0;
    int total_bits = 0;
    int sync_buffer[8] = {0};

    printf("[ThruSonic Recv] Kayıt analiz ediliyor, preamble aranıyor...\n");

    // 4. Dosyayı 441'eerli örnekler halinde oku
    while (fread(buffer, sizeof(short), N, file) == N) {
        // Normalize et
        for (int i = 0; i < N; i++) {
            in[i] = (double)buffer[i] / 32768.0;
        }

        // FFT'yi çalıştır
        fftw_execute(plan);

        // 18kHz ve 19kHz genlikleri
        double mag_18k = sqrt(out[bin_18k][0]*out[bin_18k][0] + out[bin_18k][1]*out[bin_18k][1]);
        double mag_19k = sqrt(out[bin_19k][0]*out[bin_19k][0] + out[bin_19k][1]*out[bin_19k][1]);

        // Sessizlik kontrolü (sync öncesi)
        if (!synced && mag_18k < MAG_THRESHOLD && mag_19k < MAG_THRESHOLD) {
            continue;
        }

        // Veri bitti mi? Uzun sessizlik varsa dur
        if (mag_18k < MAG_THRESHOLD && mag_19k < MAG_THRESHOLD) {
            silent_count++;
            if (silent_count > 25) break;
        } else {
            silent_count = 0;
        }

	// Baskın frekansa göre bit belirle
        int bit = (mag_19k > mag_18k) ? 1 : 0;

        // Preamble arama
        if (!synced) {
            for (int k = 0; k < 7; k++) sync_buffer[k] = sync_buffer[k+1];
            sync_buffer[7] = bit;

            int match = 1;
            for (int k = 0; k < 8; k++) {
                int expected = (SYNC_BYTE >> (7 - k)) & 1;
                if (sync_buffer[k] != expected) { match = 0; break; }
            }

            if (match) {
                printf("[SYNC] Preamble bulundu! Veri okunuyor...\n");
                synced = 1;
            }
            continue;
        }

        // Veri okuma (8 data + 1 paritty = 9 bit)
        current_byte = (current_byte << 1) | bit;
        bit_count++;
        total_bits++;

        if (bit_count == 9) {
            // Son bit parity, kalan 8 bit data
            int data_byte = (current_byte >> 1) & 0xFF;
            int received_parity = current_byte & 1;

            // Parity kontrolü
            int calc_parity = 0;
            for (int i = 0; i < 8; i++) {
                if ((data_byte >> i) & 1) calc_parity ^= 1;
            }

            if (calc_parity == received_parity) {
                fwrite(&data_byte, 1, 1, out_file);
            } else {
                fprintf(stderr, "[HATA] Parity hatası! Byte atlandı.\n");
            }

            current_byte = 0;
            bit_count = 0;
        }
    }

    if (!synced) {
        fprintf(stderr, "[UYARI] Preamble bulunamadı!\n");
    }

    printf("[Tamamlandı] Toplam %d bit işlendi, dosya oluşturuldu: %s\n", total_bits, argv[2]);

    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    free(buffer);
    fclose(file);
    fclose(out_file);

    return 0;
}
