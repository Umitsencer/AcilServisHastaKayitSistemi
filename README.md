# Acil Servis Hasta Kayıt Sistemi

Bu proje, acil servis hastalarını öncelik sırasına göre yönetmek için bir hasta kayıt ve kuyruk sistemidir. Hastalar HL7 formatına uygun olarak kaydedilir ve kuyruk yapısı kullanılarak işlenir.

## 📚 Proje İçeriği
- **HL7 formatında hasta kayıtları**: Hasta bilgileri HL7 formatında saklanır.
- **Kuyruk Yönetimi**: Hastalar öncelik seviyelerine göre kuyruğa eklenir.
- **Hasta Kayıt Sistemi**: Kullanıcıdan hasta bilgilerini alır ve saklar.
- **Dosya İşlemleri**: Hasta bilgileri `patients.hl7` dosyasına kaydedilir.

## 🛠 Derleme ve Çalıştırma
Projeyi derlemek ve çalıştırmak için aşağıdaki adımları takip edebilirsiniz:

### 1️⃣ Derleme
```bash
gcc hasta_kayit.c -o hasta_kayit
```

### 2️⃣ Çalıştırma
```bash
./hasta_kayit
```

## 📌 Kullanım
Program çalıştırıldığında aşağıdaki seçenekler sunulur:
1. **Hasta Kaydı Al**: Kullanıcıdan hasta bilgilerini alarak uygun kuyruğa ekler.
2. **Hastaları Listele**: Kayıtlı hastaları öncelik seviyelerine göre listeler.
3. **Hasta Verilerini HL7 Formatında Dosyaya Kaydet**: Tüm hasta bilgilerini `patients.hl7` dosyasına HL7 formatında kaydeder.
4. **Çıkış**: Programdan çıkış yapar.

## 📊 HL7 Formatı
Hasta bilgileri aşağıdaki HL7 formatında kaydedilir:
```
MSH|^~\&|HastaneAcil|1|Eczane|1|GelisSaati|Kabul Edildi|ORM^O01|ReceteNo|P|2.5|||TCKimlik
PID|1||TCKimlik||Ad^Soyad||Yas|Cinsiyet|Durum|||||||||TriageKategori
ORC|RE|ReceteNo|||
RXO|ReceteNo|Durum|TriageKategori
```

## ✅ Lisans
Bu proje eğitim amaçlı geliştirilmiştir.

🚀 İyi çalışmalar! 😊

