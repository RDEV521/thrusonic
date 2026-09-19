# ThruSonic 🦈🔊

İnternet, Wi-Fi veya Bluetooth bağlantısı olmadan, iki Linux bilgisayar arasında yüksek frekanslı (ultrasonik) ses dalgaları kullanarak gizli ve güvenli dosya transferi sağlayan C tabanlı bir "Data-over-Sound" aracı.


## 🚀 Nasıl Çalışır?
- **Verici (`tsonic-send`):** Gönderilmek istenen dosyayı binary (0-1) formatına çevirir. Frekans Kaydırmalı Anahtarlama (FSK) yöntemiyle 0'ları 18,000 Hz, 1'leri 19,000 Hz sinüs dalgalarına dönüştürür ve Linux ALSA altyapısı ile hoparlörden yayınlar.
- **Alıcı (`tsonic-recv`):** Mikrofondan ortam sesini dinler, Hızlı Fourier Dönüşümü (FFT) algoritmalarını kullanarak anlık frekans analizi yapar. Gelen frekansları tekrar binary koda ve ardından orijinal dosyaya dönüştürür.



> 🔬 **NOT (Ultrasonik Mod Hakkında):**  
> Projenin hedefi **18.000 Hz (0 biti)** ve **19.000 Hz (1 biti)** ultrasonik frekanslardır.  
> Ancak testlerde kullanılan laptop mikrofonu bu frekansları algılayamadığı için  
> geçici olarak **8.000 Hz (0)** ve **10.000 Hz (1)** frekansları kullanılmaktadır.  
> Harici bir USB mikrofon ile ultrasonik moda geçmek için `tsonic-send.c` ve `tsonic-recv.c`  
> dosyalarındaki `FREQ_0` ve `FREQ_1` değerlerini değiştirmeniz yeterlidir.
>
> 📌 **Yapılacaklar (TODO):**  
> - [ ] Harici mikrofon ile ultrasonik mod testi  
> - [ ] Hamming Kodu ile ileri seviye hata düzeltme  
> - [ ] Text dışında binary dosya transferi (resim, PDF, vs.)  
> - [ ] Gerçek zamanlı (real-time) transfer modu  
> - [ ] GUI (Grafik Arayüz) geliştirme
