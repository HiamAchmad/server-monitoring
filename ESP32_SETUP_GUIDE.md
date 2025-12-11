# 📡 Panduan Setup ESP32 Absensi System

## Tanggal: 6 November 2025

---

## 🛠️ Hardware Requirements

### 1. **ESP32 DevKit**
- ESP32-WROOM-32
- atau ESP32 DevKit V1
- Harga: ~Rp 50.000

### 2. **RFID RC522 Module**
- MFRC522 RFID Reader
- Frequency: 13.56 MHz
- Harga: ~Rp 15.000

### 3. **LED Indicators**
- LED Green (1 pcs)
- LED Red (1 pcs)
- Resistor 220Ω (2 pcs)

### 4. **Buzzer**
- Active Buzzer 5V (1 pcs)
- Harga: ~Rp 3.000

### 5. **RFID Cards/Tags**
- Kartu RFID 13.56MHz
- Minimal 3-5 kartu untuk karyawan
- Harga: ~Rp 2.000/kartu

### 6. **Lain-lain**
- Breadboard
- Kabel Jumper Male-Female
- Kabel USB Micro (untuk programming ESP32)

**Total Budget**: ~Rp 100.000 - 150.000

---

## 🔌 Wiring Diagram

### RFID RC522 → ESP32

| RFID Pin | ESP32 Pin | Keterangan |
|----------|-----------|------------|
| SDA      | GPIO 5    | Chip Select |
| SCK      | GPIO 18   | SPI Clock |
| MOSI     | GPIO 23   | Master Out Slave In |
| MISO     | GPIO 19   | Master In Slave Out |
| IRQ      | -         | Tidak digunakan |
| GND      | GND       | Ground |
| RST      | GPIO 22   | Reset |
| 3.3V     | 3.3V      | Power Supply |

### LED & Buzzer → ESP32

| Komponen | ESP32 Pin | Keterangan |
|----------|-----------|------------|
| LED Green (+) | GPIO 2 | Indikator sukses |
| LED Green (-) | GND via resistor 220Ω | |
| LED Red (+) | GPIO 4 | Indikator error |
| LED Red (-) | GND via resistor 220Ω | |
| Buzzer (+) | GPIO 15 | Buzzer aktif |
| Buzzer (-) | GND | |

### Diagram ASCII:
```
         ESP32
    ┌─────────────┐
    │             │
    │  GPIO 5  ───┼─── SDA  ┐
    │  GPIO 18 ───┼─── SCK  │
    │  GPIO 23 ───┼─── MOSI │  RFID
    │  GPIO 19 ───┼─── MISO │  RC522
    │  GPIO 22 ───┼─── RST  │
    │  3.3V    ───┼─── VCC  │
    │  GND     ───┼─── GND  ┘
    │             │
    │  GPIO 2  ───┼─── LED Green (+ 220Ω)
    │  GPIO 4  ───┼─── LED Red (+ 220Ω)
    │  GPIO 15 ───┼─── Buzzer (+)
    │  GND     ───┼─── GND (semua komponen)
    │             │
    └─────────────┘
```

---

## 📚 Library yang Dibutuhkan

### Install Library di Arduino IDE:

1. **WiFi** (Built-in ESP32)
   - Sudah include di ESP32 board

2. **PubSubClient** (MQTT)
   - Buka: Sketch → Include Library → Manage Libraries
   - Cari: "PubSubClient"
   - Install versi terbaru (by Nick O'Leary)

3. **MFRC522** (RFID)
   - Buka: Sketch → Include Library → Manage Libraries
   - Cari: "MFRC522"
   - Install versi terbaru (by GithubCommunity)

4. **SPI** (Built-in)
   - Sudah include di ESP32 board

### Install ESP32 Board di Arduino IDE:

1. Buka: File → Preferences
2. Di "Additional Board Manager URLs", tambahkan:
   ```
   https://dl.espressif.com/dl/package_esp32_index.json
   ```
3. Buka: Tools → Board → Boards Manager
4. Cari "ESP32"
5. Install "esp32 by Espressif Systems"
6. Pilih Board: Tools → Board → ESP32 Arduino → ESP32 Dev Module

---

## ⚙️ Konfigurasi Kode

### 1. **WiFi Settings**

Edit di file `ESP32_ABSENSI_CODE.ino`:

```cpp
const char* ssid = "NAMA_WIFI_ANDA";           // Nama WiFi Anda
const char* password = "PASSWORD_WIFI_ANDA";   // Password WiFi
```

### 2. **MQTT Server Settings**

Ganti dengan IP address server Anda:

```cpp
const char* mqtt_server = "192.168.1.100";  // IP Server (cek dengan 'hostname -I')
const int mqtt_port = 1883;
```

**Cara cek IP Server:**
```bash
hostname -I
# atau
ip addr show
```

### 3. **Database Kartu RFID**

Tambahkan kartu RFID karyawan Anda:

```cpp
RFIDCard registeredCards[] = {
    {"A1 B2 C3 D4", 1, "John Doe"},       // UID, ID Pegawai, Nama
    {"E5 F6 G7 H8", 2, "Jane Smith"},
    {"11 22 33 44", 3, "Ahmad Zainudin"},
    // Tambahkan kartu lainnya
};
```

**Cara mendapatkan UID kartu:**
1. Upload kode ke ESP32
2. Buka Serial Monitor (115200 baud)
3. Tempelkan kartu RFID
4. Lihat UID yang muncul di Serial Monitor
5. Copy UID dan masukkan ke array `registeredCards`

### 4. **Jam Masuk (Keterlambatan)**

Default jam masuk: **08:00**

Edit fungsi `checkAttendanceStatus()`:

```cpp
// Jam masuk: 08:00
if (hour > 8 || (hour == 8 && minute > 0)) {
    return "Terlambat";
}
```

Jika ingin jam masuk 07:30:
```cpp
if (hour > 7 || (hour == 7 && minute > 30)) {
    return "Terlambat";
}
```

---

## 🚀 Upload Kode ke ESP32

### Langkah-langkah:

1. **Hubungkan ESP32 ke Komputer**
   - Gunakan kabel USB Micro

2. **Buka Arduino IDE**
   - Buka file: `ESP32_ABSENSI_CODE.ino`

3. **Pilih Board dan Port**
   - Tools → Board → ESP32 Dev Module
   - Tools → Port → (pilih port COM yang muncul)

4. **Upload**
   - Klik tombol Upload (→)
   - Tunggu sampai selesai (Done uploading)

5. **Buka Serial Monitor**
   - Tools → Serial Monitor
   - Set baud rate: **115200**

### Troubleshooting Upload:

**Error: "Failed to connect to ESP32"**
- Tekan dan tahan tombol **BOOT** di ESP32
- Klik Upload
- Lepas tombol BOOT setelah "Connecting..."

**Port tidak muncul:**
- Install driver CP210x atau CH340
- Download di: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers

---

## 🧪 Testing

### 1. **Test WiFi Connection**

Serial Monitor akan menampilkan:
```
Menghubungkan ke WiFi: NamaWiFiAnda
...
WiFi terhubung!
IP Address: 192.168.1.105
```

### 2. **Test MQTT Connection**

Serial Monitor:
```
MQTT terhubung!
```

Buzzer akan beep 1x saat MQTT terhubung.

### 3. **Test RFID Scan**

Tempelkan kartu RFID, Serial Monitor:
```
--- KARTU TERDETEKSI ---
UID: A1 B2 C3 D4
Status: TERDAFTAR
Nama: John Doe
ID: 1

=== MENGIRIM DATA ABSENSI ===
Nama: John Doe
ID Pegawai: 1
Waktu: 08:15:30
Status: Terlambat
Payload: {"pegawai_id":1,"waktu_absen":"08:15:30","keterangan":"Terlambat"}
✓ Data berhasil dikirim!
=============================
```

### 4. **Indikator Visual**

| Kondisi | LED Green | LED Red | Buzzer |
|---------|-----------|---------|--------|
| WiFi Connected | ON | OFF | 2x beep |
| MQTT Connected | - | - | 1x beep |
| Scan Hadir | Blink | OFF | 2x beep pendek |
| Scan Terlambat | Blink | OFF | 3x beep |
| Kartu Tidak Terdaftar | OFF | ON | 4x beep cepat |
| Error Kirim Data | OFF | ON | 1x beep panjang |

---

## 🔧 Troubleshooting

### Problem 1: WiFi Tidak Terhubung

**Gejala**: LED merah berkedip terus, Serial Monitor: "Gagal terhubung ke WiFi!"

**Solusi**:
- Pastikan SSID dan password benar (case-sensitive)
- Pastikan WiFi 2.4GHz (ESP32 tidak support 5GHz)
- Jarak ESP32 tidak terlalu jauh dari router
- Restart ESP32 dengan tombol EN/RST

### Problem 2: MQTT Terputus

**Gejala**: Serial Monitor: "MQTT terputus! Reconnecting..."

**Solusi**:
- Pastikan server Node.js berjalan
- Cek IP server benar
- Pastikan port 1883 tidak diblok firewall
- Test MQTT dengan mosquitto client

### Problem 3: RFID Tidak Terdeteksi

**Gejala**: Kartu ditempelkan tapi tidak ada respon

**Solusi**:
- Cek wiring RFID (terutama SDA, SCK, MOSI, MISO)
- Pastikan RFID dapat power 3.3V (bukan 5V!)
- Restart ESP32
- Coba kartu RFID lain

### Problem 4: Waktu Salah

**Gejala**: Waktu yang dikirim tidak sesuai

**Solusi**:
- Pastikan ESP32 terhubung internet
- Cek timezone: `gmtOffset_sec = 25200` (WIB GMT+7)
- Tunggu 5-10 detik untuk NTP sync
- Restart ESP32

### Problem 5: Data Tidak Masuk ke Database

**Gejala**: ESP32 kirim data tapi tidak masuk ke website

**Solusi**:
- Cek server log di terminal
- Pastikan pegawai_id ada di database
- Cek format JSON payload
- Test dengan mosquitto_pub manual

---

## 📊 Monitoring ESP32

### Via Serial Monitor:

```
ESP32 SISTEM ABSENSI RFID
=================================

RFID RC522 Ready!
WiFi terhubung!
IP Address: 192.168.1.105
NTP Time configured
Waktu sekarang: 14:23:45

Sistem siap!
Tempelkan kartu RFID...
```

### Via Server Log:

```bash
# Di terminal server, akan muncul:
📡 MQTT Client terhubung: ESP32_Absensi_001
📥 MQTT Message diterima dari ESP32_Absensi_001
   Topic: absensi/data
   Message: {"pegawai_id":1,"waktu_absen":"14:23:45","keterangan":"Hadir"}
✅ Data berhasil disimpan ke database
```

---

## 🎯 Flow Sistem Lengkap

```
Karyawan tap kartu RFID
    ↓
ESP32 baca UID kartu
    ↓
Cari di database lokal (array registeredCards)
    ↓
┌─────────────────────────┐
│ Kartu Terdaftar?        │
└─────────────────────────┘
    │             │
   YES           NO
    │             │
    ↓             ↓
Cek waktu    LED merah + Beep 4x
    │        (Kartu tidak terdaftar)
    ↓
Cek jam masuk (08:00)
    │
    ├─ Sebelum 08:00 → "Hadir"
    └─ Setelah 08:00 → "Terlambat"
    │
    ↓
Buat JSON payload
{"pegawai_id":1, "waktu_absen":"08:15", "keterangan":"Terlambat"}
    ↓
Publish ke MQTT → Topic: "absensi/data"
    ↓
Server Node.js terima via MQTT
    ↓
Simpan ke PostgreSQL
    ↓
Broadcast ke semua client via Socket.IO
    ↓
Website real-time update (dengan notifikasi)
```

---

## 📝 Kustomisasi

### 1. Tambah Status Absensi Baru

Edit fungsi `checkAttendanceStatus()`:

```cpp
String checkAttendanceStatus() {
    struct tm timeinfo;
    getLocalTime(&timeinfo);

    int hour = timeinfo.tm_hour;

    // Jam pulang (contoh: 17:00)
    if (hour >= 17) {
        return "Pulang";
    }

    // Jam istirahat (contoh: 12:00-13:00)
    if (hour >= 12 && hour < 13) {
        return "Istirahat";
    }

    // Jam masuk (08:00)
    if (hour > 8) {
        return "Terlambat";
    }

    return "Hadir";
}
```

### 2. Tambah Display LCD

Tambahkan library:
```cpp
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
```

Di fungsi setup():
```cpp
lcd.init();
lcd.backlight();
lcd.print("Sistem Absensi");
```

Setelah scan berhasil:
```cpp
lcd.clear();
lcd.setCursor(0, 0);
lcd.print(card.nama);
lcd.setCursor(0, 1);
lcd.print(status);
```

### 3. Simpan Log ke SD Card

Tambahkan library:
```cpp
#include <SD.h>
#define SD_CS 5
```

Simpan log:
```cpp
File logFile = SD.open("/absensi_log.txt", FILE_APPEND);
logFile.println(String(pegawai_id) + "," + waktu + "," + keterangan);
logFile.close();
```

---

## ✅ Checklist Setup

- [ ] Hardware terpasang dengan benar
- [ ] Library terinstall di Arduino IDE
- [ ] WiFi SSID & password dikonfigurasi
- [ ] MQTT server IP dikonfigurasi
- [ ] Kartu RFID terdaftar di database
- [ ] Kode ter-upload ke ESP32
- [ ] Serial Monitor menampilkan "Sistem siap!"
- [ ] LED hijau menyala (WiFi connected)
- [ ] Test scan kartu berhasil
- [ ] Data masuk ke database server
- [ ] Website menampilkan data real-time

---

## 📌 Catatan Penting

1. **Power Supply**: ESP32 bisa powered via USB atau 5V external. RFID RC522 HARUS 3.3V!

2. **WiFi Range**: Pastikan ESP32 dalam jangkauan WiFi yang baik untuk stabilitas.

3. **MQTT Broker**: Server harus running sebelum ESP32 connect.

4. **Cooldown**: Ada delay 3 detik untuk prevent double scan.

5. **Security**: Username/password WiFi ada di kode. Jangan share kode ke publik!

6. **Database Sinkron**: Pastikan pegawai_id di ESP32 sama dengan di database server.

---

## 🆘 Support

Jika menemui masalah:

1. **Serial Monitor**: Selalu cek log di Serial Monitor (115200 baud)
2. **Server Log**: Cek terminal server untuk error MQTT
3. **Wiring**: Double-check semua koneksi hardware
4. **Restart**: Restart ESP32 dan server jika perlu

---

**Last Updated**: 6 November 2025
**Status**: Ready for Production
**Tested**: ESP32 DevKit V1 + MFRC522

Happy Coding! 🚀
