# ThruSonic 🦈🔊

İnternet, Wi-Fi veya Bluetooth bağlantısı olmadan, iki Linux bilgisayar arasında yüksek frekanslı (ultrasonik) ses dalgaları kullanarak gizli ve güvenli dosya transferi sağlayan C tabanlı bir "Data-over-Sound" aracı.

## 🚀 Nasıl Çalışır?
- **Verici (`tsonic-send`):** Gönderilmek istenen dosyayı binary (0-1) formatına çevirir. Frekans Kaydırmalı Anahtarlama (FSK) yöntemiyle 0'ları 18,000 Hz, 1'leri 19,000 Hz sinüs dalgalarına dönüştürür ve Linux ALSA altyapısı ile hoparlörden yayınlar.
- **Alıcı (`tsonic-recv`):** Mikrofondan ortam sesini dinler, Hızlı Fourier Dönüşümü (FFT) algoritmalarını kullanarak anlık frekans analizi yapar. Gelen frekansları tekrar binary koda ve ardından orijinal dosyaya dönüştürür.

## 🗺️ Geliştirme Yol Haritası (Roadmap)
- [x] Adım 1: Proje Temeli, Git ve GitHub Kurulumu
- [x] Adım 2: Dosyayı İkili Koda (Binary) Çeviren Modül (`tsonic-send`)
- [x] Adım 3: ALSA ile Frekans Modülasyonlu Ses Üretimi
- [x] Adım 4: Mikrofondan Ham Ses Yakalama Modülü (`tsonic-recv`)
- [ ] Adım 5: FFTW3 ile Anlık Frekans Analizi ve Bit Çözümleme
- [ ] Adım 6: Dosyayı Yeniden Oluşturma ve Testler



> ⚠️ **NOT (Geçici Test Aşaması):**  
> Senkronizasyon sorunu devam ediyor.
