#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <alsa/asoundlib.h>

#define SAMPLE_RATE 44100 // CD Kalitesinde örnekleme hızı
#define DURATION 2        // Sesin kaç saniye çalacağı (2 saniye)

int main() {
    long loops;
    int rc;
    int size;
    snd_pcm_t *handle;
    snd_pcm_hw_params_t *params;
    unsigned int val;
    int dir;
    snd_pcm_uframes_t frames;
    
    // Test için duyabileceğimiz bir frekans seçelim (440 Hz = La Notası)
    double frequency = 440.0; 

    // 1. ALSA ses aygıtını (varsayılan hoparlörü) çalma modunda açıyoruz
    rc = snd_pcm_open(&handle, "default", SND_PCM_STREAM_PLAYBACK, 0);
    if (rc < 0) {
        fprintf(stderr, "Ses aygıtı açılamadı.\n");
        return 1;
    }

    // Donanım parametrelerı için yer ayırıyoruzz
    snd_pcm_hw_params_alloca(&params);
    snd_pcm_hw_params_any(handle, params);

    // Kesintisiz (Interleaved) okuma/yazma modunu seçiyoruz
    snd_pcm_hw_params_set_access(handle, params, SND_PCM_ACCESS_RW_INTERLEAVED);

    // Ses formatını 16-bıt Küçük Uçlu (Little Endian) yapıyoruz
    snd_pcm_hw_params_set_format(handle, params, SND_PCM_FORMAT_S16_LE);

    // Tek kanal (Mono) ses üreteceğiz
    snd_pcm_hw_params_set_channels(handle, params, 1);

    // Örnekleme hızını (44100 Hz) bağlıyoruz
    val = SAMPLE_RATE;
    snd_pcm_hw_params_set_rate_near(handle, params, &val, &dir);

    // Parametreleri ses kartına gönderiyoruz
    rc = snd_pcm_hw_params(handle, params);
    if (rc < 0) {
        fprintf(stderr, "Parametreler ayarlanamadı.\n");
        snd_pcm_close(handle);
        return 1;
    }

    // Buffer boyutunu alıyoruz
    snd_pcm_hw_params_get_period_size(params, &frames, &dir);
    size = frames * 2; // 16-bit (2 byte) her örnek için
    short *buffer = (short *)malloc(size * sizeof(short));

    // Kaç döngü boyunca ses çalınacağını hesaplıyoruz
    snd_pcm_hw_params_get_period_time(params, &val, &dir);
    loops = (DURATION * 1000000) / val;

    printf("[ThruSonic Generator] %d saniye boyunca %.1f Hz ses üretiliyor...\n", DURATION, frequency);

    for (int i = 0; i < loops; i++) {
        // Matematiksel olarak sinüs dalgası (ses) üretiyoruz
        for (int j = 0; j < frames; j++) {
            double t = (double)(i * frames + j) / SAMPLE_RATE;
            buffer[j] = (short)(32767.0 * sin(2.0 * M_PI * frequency * t));
        }

        // Üretilen bu sinüs dalgası verisini hoparlöre basıyoruz!
        rc = snd_pcm_writei(handle, buffer, frames);
        if (rc == -EPIPE) {
            snd_pcm_prepare(handle);
        } else if (rc < 0) {
            fprintf(stderr, "Hoparlöre yazma hatası.\n");
        }
    }

    // Temizlik
    snd_pcm_drain(handle);
    snd_pcm_close(handle);
    free(buffer);
    printf("[Tamamlandı]\n");
    return 0;
}
