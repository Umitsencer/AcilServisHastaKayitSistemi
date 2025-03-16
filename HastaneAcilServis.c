#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAKS_HASTA_SAYISI 100
#define DOSYA_ADI "patients.hl7"
#define MAKS_KUYRUK_BOYUTU 100

// HL7 Formatında hasta bilgisi için yapı (struct)
typedef struct {
    char tcKimlik[12];            // Hasta TC Kimlik No
    char ad[50];                  // Hasta adı
    char SoyAd[50];             // Hasta Soyadı
    int yas;                       // Hasta yaşı
    char cinsiyet;                // Hasta cinsiyeti
    char durum[100];              // Hasta durumu (örneğin: Kırmızı, Sarı, Yeşil)
    int oncelik;                  // Hasta önceliği (1: Yüksek, 2: Orta, 3: Düşük)
    char receteNo[15];            // Reçete numarası
    int taburcuDurumu;            // Taburcu edilmiş mi? (0: Hayır, 1: Evet)
    char gelisSaati[20];          // Hastanın acil servise geliş saati
    char triageKategori[20];      // Hastanın triage kategorisi (Kırmızı, Sarı, Yeşil)
} Hasta;

// Kuyruk yapısı
typedef struct {
    Hasta hastalar[MAKS_KUYRUK_BOYUTU];
    int on;   // Kuyruğun başı
    int arka; // Kuyruğun sonu
} Kuyruk;

// Kuyruk fonksiyonları

// Kuyruğu başlatmak için fonksiyon
void kuyruğuBaslat(Kuyruk *kuyruk) {
    kuyruk->on = -1;
    kuyruk->arka = -1;
}

// Kuyruğun boş olup olmadığını kontrol eden fonksiyon
int kuyrukBosmu(Kuyruk *kuyruk) {
    return (kuyruk->on == -1);
}

// Kuyruğun dolu olup olmadığını kontrol eden fonksiyon
int kuyrukDoluMu(Kuyruk *kuyruk) {
    return (kuyruk->arka == MAKS_KUYRUK_BOYUTU - 1);
}

// Kuyruğa hasta eklemek için fonksiyon
void kuyrugaEkle(Kuyruk *kuyruk, Hasta h) {
    if (kuyrukDoluMu(kuyruk)) {
        printf("Kuyruk dolu! Yeni hasta eklenemiyor.\n");
        return;
    }
    if (kuyrukBosmu(kuyruk)) {
        kuyruk->on = 0;
    }
    kuyruk->hastalar[++kuyruk->arka] = h;
}

// Kuyruğdan hasta çıkarmak için fonksiyon
Hasta kuyruktanCikar(Kuyruk *kuyruk) {
    if (kuyrukBosmu(kuyruk)) {
        printf("Kuyruk boş! Çıkartılacak hasta yok.\n");
        exit(1);
    }
    return kuyruk->hastalar[kuyruk->on++];
}

// Kuyruğu yazdırmak için fonksiyon
void kuyruguYazdir(Kuyruk *kuyruk) {
    if (kuyrukBosmu(kuyruk)) {
        printf("Kuyruk boş.\n");
        return;
    }
    for (int i = kuyruk->on; i <= kuyruk->arka; i++) {
        printf("TC: %s, Ad: %s, Yaş: %d, Cinsiyet: %c, Durum: %s, Öncelik: %d, Reçete No: %s, Triage: %s, Gelis Saati: %s\n",
               kuyruk->hastalar[i].tcKimlik, kuyruk->hastalar[i].ad,kuyruk->hastalar[i].SoyAd, kuyruk->hastalar[i].yas, kuyruk->hastalar[i].cinsiyet,
               kuyruk->hastalar[i].durum, kuyruk->hastalar[i].oncelik, kuyruk->hastalar[i].receteNo,
               kuyruk->hastalar[i].triageKategori, kuyruk->hastalar[i].gelisSaati);
    }
}

// HL7 formatında hasta kaydının yazılması
void hl7KaydiniYaz(FILE *dosya, Hasta *hasta) {
    fprintf(dosya, "MSH|^~\\&|HastaneAcil|1|Eczane|1|%s|%s|ORM^O01|%s|P|2.5|||%s\n", 
            hasta->gelisSaati, "Kabul Edildi", hasta->receteNo, hasta->tcKimlik);
    fprintf(dosya, "PID|1||%s||%s^%s||%d|%c|%s|||||||||%s\n", 
            hasta->tcKimlik, hasta->ad, hasta->SoyAd, hasta->yas, hasta->cinsiyet, hasta->durum, hasta->triageKategori);
    fprintf(dosya, "ORC|RE|%s|||\n", hasta->receteNo);
    fprintf(dosya, "RXO|%s|%s|%s\n", hasta->receteNo, hasta->durum, hasta->triageKategori);
}

// Reçete numarasını üretme (HL7 formatına uygun)
void receteNoUret(char *receteNo) {
    time_t suan = time(NULL);
    struct tm *yerel = localtime(&suan);
    sprintf(receteNo, "RX%02d%02d%04d%02d%02d", yerel->tm_mday, yerel->tm_mon + 1, yerel->tm_year + 1900, yerel->tm_hour, yerel->tm_min);
}

// Geçerli saati almak
void gecerliSaatiAl(char *buffer) {
    time_t suan = time(NULL);
    struct tm *yerel = localtime(&suan);
    sprintf(buffer, "%02d:%02d:%02d %02d/%02d/%04d", yerel->tm_hour, yerel->tm_min, yerel->tm_sec, yerel->tm_mday, yerel->tm_mon + 1, yerel->tm_year + 1900);
}

// Hasta kaydı almak
void hastaKaydiAl(Kuyruk *kuyruklar[3]) {
    Hasta yeniHasta;
    printf("Hasta TC Kimlik No: ");
    scanf("%11s", yeniHasta.tcKimlik);
    printf("Hasta Adı: ");
    scanf(" %49[^\n]", yeniHasta.ad);
    printf("Hasta Soyadı: ");
    scanf(" %49[^\n]", yeniHasta.SoyAd);
    printf("Hasta Yaşı: ");
    scanf("%d", &yeniHasta.yas);
    printf("Hasta Cinsiyeti (E/K): ");
    scanf(" %c", &yeniHasta.cinsiyet);
    printf("Hasta Durumu (Kırmızı/Sarı/Yeşil): ");
    scanf(" %99[^\n]", yeniHasta.durum);

    // Yaş kontrolü, 65 yaş üstü hastalar öncelikli olarak kaydedilir
    if (yeniHasta.yas >= 65) {
        yeniHasta.oncelik = 1; // 65 yaş üstü hastalar yüksek öncelikli
    } else {
        printf("Öncelik Durumu (1: Yüksek, 2: Orta, 3: Düşük): ");
        scanf("%d", &yeniHasta.oncelik);
    }

    printf("Triage Kategorisi (Kırmızı/Sarı/Yeşil): ");
    scanf("%19s", yeniHasta.triageKategori);

    receteNoUret(yeniHasta.receteNo);
    gecerliSaatiAl(yeniHasta.gelisSaati);
    yeniHasta.taburcuDurumu = 0;

    // Öncelik sırasına göre kuyruk ekle
    kuyrugaEkle(kuyruklar[yeniHasta.oncelik - 1], yeniHasta);
    printf("Hasta başarıyla kaydedildi. Reçete No: %s\n", yeniHasta.receteNo);
}

// Hastaları listelemek
void hastalariListele(Kuyruk *kuyruklar[3]) {
    printf("\nKayıtlı Hastalar (Öncelik sırasına göre):\n");

    // Yüksek öncelikli hastalar
    printf("\nÖncelik 1 (Yüksek):\n");
    kuyruguYazdir(kuyruklar[0]);

    // Orta öncelikli hastalar
    printf("\nÖncelik 2 (Orta):\n");
    kuyruguYazdir(kuyruklar[1]);

    // Düşük öncelikli hastalar
    printf("\nÖncelik 3 (Düşük):\n");
    kuyruguYazdir(kuyruklar[2]);
}

// Hastaları HL7 formatında dosyaya kaydetme
void hastalariDosyayaKaydet(Kuyruk *kuyruklar[3]) {
    FILE *dosya = fopen(DOSYA_ADI, "w");
    if (dosya == NULL) {
        printf("Dosya açılamadı!\n");
        return;
    }

    // Yüksek öncelikli hastalar
    for (int i = 0; i <= kuyruklar[0]->arka; i++) {
        hl7KaydiniYaz(dosya, &kuyruklar[0]->hastalar[i]);
    }
    // Orta öncelikli hastalar
    for (int i = 0; i <= kuyruklar[1]->arka; i++) {
        hl7KaydiniYaz(dosya, &kuyruklar[1]->hastalar[i]);
    }
    // Düşük öncelikli hastalar
    for (int i = 0; i <= kuyruklar[2]->arka; i++) {
        hl7KaydiniYaz(dosya, &kuyruklar[2]->hastalar[i]);
    }

    fclose(dosya);
    printf("Hasta bilgileri HL7 formatında dosyaya kaydedildi.\n");
}

int main() {
    int secim;

    // Her öncelik seviyesi için kuyrukları başlat
    Kuyruk *kuyruklar[3];
    for (int i = 0; i < 3; i++) {
        kuyruklar[i] = (Kuyruk *)malloc(sizeof(Kuyruk));
        kuyruğuBaslat(kuyruklar[i]);
    }

    do {
        printf("\n1. Hasta Kaydı Al\n2. Hastaları Listele\n3. Hasta Verilerini HL7 Formatında Dosyaya Kaydet\n0. Çıkış\nSeçiminiz: ");
        scanf("%d", &secim);

        switch (secim) {
            case 1:
                hastaKaydiAl(kuyruklar);
                break;
            case 2:
                hastalariListele(kuyruklar);
                break;
            case 3:
                hastalariDosyayaKaydet(kuyruklar);
                break;
            case 0:
                printf("Çıkılıyor...\n");
                break;
            default:
                printf("Geçersiz seçim. Lütfen tekrar deneyin.\n");
        }
    } while (secim != 0);

    // Belleği serbest bırak
    for (int i = 0; i < 3; i++) {
        free(kuyruklar[i]);
    }

    return 0;
}