# ThruSonic 🦈🔊

İnternet, Wi-Fi veya Bluetooth bağlantısı olmadan, iki Linux bilgisayar arasında yüksek frekanslı (ultrasonik) ses dalgaları kullanarak gizli ve güvenli dosya transferi sağlayan C tabanlı bir "Data-over-Sound" aracı.

## 🚀 Nasıl Çalışır?
- **Verici (`tsonic-send`):** Gönderilmek istenen dosyayı binary (0-1) formatına çevirir. Frekans Kaydırmalı Anahtarlama (FSK) yöntemiyle 0'ları 18,000 Hz, 1'leri 19,000 Hz sinüs dalgalarına dönüştürür ve Linux ALSA altyapısı ile hoparlörden yayınlar.
- **Alıcı (`tsonic-recv`):** Mikrofondan ortam sesini dinler, Hızlı Fourier Dönüşümü (FFT) algoritmalarını kullanarak anlık frekans analizi yapar. Gelen frekansları tekrar binary koda ve ardından orijinal dosyaya dönüştürür.

## 🗺️ Geliştirme Yol Haritası (Roadmap)
- [x] Adım 1: Proje Temeli, Git ve GitHub Kurulumu
- [ ] Adım 2: Dosyayı İkili Koda (Binary) Çeviren Modül (`tsonic-send`)
- [ ] Adım 3: ALSA ile Frekans Modülasyonlu Ses Üretimi
- [ ] Adım 4: Mikrofondan Ham Ses Yakalama Modülü (`tsonic-recv`)
- [ ] Adım 5: FFTW3 ile Anlık Frekans Analizi ve Bit Çözümleme
- [ ] Adım 6: Dosyayı Yeniden Oluşturma ve Testler



> ⚠️ **NOT (Geçici Test Aşaması):**  
> Şu an `generator.c` dosyası, test amaçlı olarak **440 Hz (La notası)** sesi üretmektedir.  
> Bu ses sadece mikrofondan kayıt alıp FFT analizini test edebilmek için geçicidir.  
> Nihai sürümde bu ses, verici tarafta **0 biti = 18.000 Hz**, **1 biti = 19.000 Hz** olacak şekilde modüle edilecektir.
