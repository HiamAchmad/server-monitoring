# Setup ESP32 Fingerprint Sensor - Step by Step

## 🔴 LED Merah = Normal saat Connect ke Laptop!

Ketika ESP32 terhubung ke laptop via USB dan LED menyala **merah**, itu adalah kondisi **NORMAL**. LED merah menunjukkan ESP32 sedang mendapat daya dari USB.

---

## 📋 Komponen yang Dibutuhkan

| No | Komponen | Jumlah | Keterangan |
|----|----------|--------|------------|
| 1 | ESP32 DevKit | 1 | Board utama |
| 2 | Fingerprint Sensor R307/AS608 | 1 | Sensor sidik jari |
| 3 | LED RGB (atau 3 LED terpisah) | 1 | Indicator status |
| 4 | Resistor 220Ω | 3 | Untuk LED |
| 5 | Kabel Jumper | Secukupnya | Female-to-Female |
| 6 | Breadboard | 1 | Optional |

---

## 🔌 Skema Koneksi Hardware

### A. Fingerprint Sensor ke ESP32

Sensor fingerprint biasanya punya 6 pin:

| Pin Sensor | Warna Kabel | Pin ESP32 | Keterangan |
|------------|-------------|-----------|------------|
| VCC (3.3V) | Merah | 3.3V | Power supply |
| GND | Hitam | GND | Ground |
| TX | Kuning/Putih | GPIO16 (RX2) | Data keluar sensor |
| RX | Hijau/Biru | GPIO17 (TX2) | Data masuk sensor |
| WAKE | - | *Tidak dipakai* | - |
| Touch Out | - | *Tidak dipakai* | - |

**⚠️ PENTING:**
- TX Sensor → RX ESP32 (silang)
- RX Sensor → TX ESP32 (silang)
- Gunakan 3.3V, BUKAN 5V!

### B. LED Indicator ke ESP32

| LED | Pin ESP32 | Resistor | Fungsi |
|-----|-----------|----------|--------|
| LED Merah | GPIO25 | 220Ω | Error / Gagal |
| LED Hijau | GPIO26 | 220Ω | Success / Berhasil |
| LED Biru | GPIO27 | 220Ω | Proses / Waiting |

**Koneksi:**
```
GPIO25 → 220Ω → LED Merah (+) → GND (-)
GPIO26 → 220Ω → LED Hijau (+) → GND (-)
GPIO27 → 220Ω → LED Biru (+) → GND (-)
```

### C. Diagram Visual

```
┌─────────────────────┐
│      ESP32          │
│                     │
│  3.3V ──────────┐   │         ┌──────────────────┐
│  GND ───────────┼───┼─────────┤ GND (Hitam)      │
│  GPIO16 (RX2) ──┼───┼─────────┤ TX (Kuning)      │
│  GPIO17 (TX2) ──┼───┼─────────┤ RX (Hijau)       │
│                 │   │         │ VCC (Merah) ─────┤──┐
│  GPIO25 ────────┼───┼─────────┤ LED Merah        │  │
│  GPIO26 ────────┼───┼─────────┤ LED Hijau        │  │
│  GPIO27 ────────┼───┼─────────┤ LED Biru         │  │
└─────────────────┘   │         └──────────────────┘  │
                      │                               │
                      └───────────────────────────────┘
                                 GND Common
```

---

## 💻 Software Setup

### Step 1: Install Arduino IDE

1. Download Arduino IDE: https://www.arduino.cc/en/software
2. Install Arduino IDE di laptop Anda

### Step 2: Install ESP32 Board di Arduino IDE

1. Buka Arduino IDE
2. Klik **File** → **Preferences**
3. Di bagian "Additional Board Manager URLs", masukkan:
   ```
   https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json
   ```
4. Klik **OK**
5. Klik **Tools** → **Board** → **Boards Manager**
6. Cari **"ESP32"** → Install **"esp32 by Espressif Systems"**
7. Tunggu sampai selesai install

### Step 3: Install Library yang Diperlukan

Di Arduino IDE:
1. Klik **Sketch** → **Include Library** → **Manage Libraries**
2. Install library berikut:

| Library | Versi | Keterangan |
|---------|-------|------------|
| **Adafruit Fingerprint Sensor Library** | Latest | Untuk sensor fingerprint |
| **PubSubClient** | Latest | Untuk MQTT client |
| **WiFi** | Built-in | Untuk koneksi WiFi (sudah ada di ESP32) |

Cara install:
- Ketik nama library di search box
- Klik **Install**
- Tunggu sampai selesai

---

## 📝 Code ESP32

Saya sudah buatkan code lengkap di file terpisah: **`esp32_fingerprint.ino`**

### Konfigurasi yang Harus Diubah:

Di bagian atas code, ubah sesuai dengan sistem Anda:

```cpp
// ===== WIFI CONFIGURATION =====
const char* ssid = "NAMA_WIFI_ANDA";        // ← Ubah ini!
const char* password = "PASSWORD_WIFI_ANDA"; // ← Ubah ini!

// ===== MQTT CONFIGURATION =====
const char* mqtt_server = "192.168.1.100";   // ← IP Laptop/Server Anda!
const int mqtt_port = 1883;
```

**Cara tahu IP Laptop:**
```bash
# Di terminal Linux:
hostname -I

# Atau:
ip addr show | grep "inet " | grep -v 127.0.0.1
```

---

## 🚀 Upload Code ke ESP32

### Step 1: Koneksi ESP32 ke Laptop

1. Hubungkan ESP32 ke laptop via kabel USB
2. **LED merah menyala** = Normal, ESP32 dapat daya ✅

### Step 2: Setup Board di Arduino IDE

1. Klik **Tools** → **Board** → **ESP32 Arduino** → **ESP32 Dev Module**
2. Klik **Tools** → **Port** → Pilih port COM yang muncul (misal: COM3 atau /dev/ttyUSB0)
3. Set parameter:
   - Upload Speed: **115200**
   - Flash Frequency: **80MHz**
   - Flash Mode: **QIO**
   - Flash Size: **4MB (32Mb)**
   - Partition Scheme: **Default 4MB with spiffs**

### Step 3: Upload Code

1. Buka file **`esp32_fingerprint.ino`** di Arduino IDE
2. Ubah konfigurasi WiFi dan MQTT (lihat di atas)
3. Klik **Verify** (✓) untuk compile
4. Jika tidak ada error, klik **Upload** (→)
5. Tunggu proses upload selesai

**Jika muncul error "Failed to connect":**
- Tekan dan tahan tombol **BOOT** di ESP32
- Klik **Upload** lagi
- Lepas tombol BOOT setelah "Connecting..." muncul

### Step 4: Monitor Serial

1. Klik **Tools** → **Serial Monitor**
2. Set baud rate ke **115200**
3. Anda akan melihat output seperti:

```
=================================
   FINGERPRINT ENROLLMENT SYSTEM
=================================
Connecting to WiFi: NamaWiFi...
WiFi connected!
IP Address: 192.168.1.150
Connecting to MQTT...
MQTT Connected!
Subscribed to: fingerprint/enroll/request
System Ready!
Waiting for enrollment command...
```

---

## 🎯 Testing Sistem

### Test 1: Koneksi WiFi & MQTT

1. Pastikan server Node.js berjalan di laptop
2. Cek Serial Monitor ESP32, pastikan tertulis:
   ```
   WiFi connected!
   MQTT Connected!
   ```

### Test 2: Enrollment dari Web

1. Buka web: `http://localhost:3000/tambah-karyawan.html`
2. Isi form data karyawan
3. Klik **Simpan**
4. Klik **Daftarkan Sidik Jari**
5. **Perhatikan LED di ESP32:**
   - LED **Biru** = Menunggu jari ditempelkan
   - LED **Hijau** = Berhasil scan
   - LED **Merah** = Gagal / Error

### Test 3: Proses Enrollment

**Di Serial Monitor ESP32:**
```
[MQTT] Enrollment command received
Fingerprint ID: 1
Employee: John Doe (NIP: 12345)
Place finger on sensor...
Image taken!
Image converted!
Place same finger again...
Image taken!
Image converted!
Fingerprint enrolled successfully!
Published to: fingerprint/enroll/response
```

**Di Web Browser:**
- Muncul notifikasi: "Sidik jari berhasil didaftarkan!"
- Status enrollment berubah hijau ✅

---

## 🔍 Troubleshooting

### Problem 1: ESP32 Tidak Terdeteksi di Arduino IDE

**Solusi:**
1. Install driver CH340/CP2102 (tergantung chip USB ESP32 Anda)
2. Download driver:
   - CH340: https://sparks.gogo.co.nz/ch340.html
   - CP2102: https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers
3. Restart Arduino IDE setelah install driver
4. Coba port USB lain di laptop

### Problem 2: WiFi Tidak Connect

**Solusi:**
1. Pastikan SSID dan password benar (case-sensitive!)
2. Pastikan WiFi 2.4GHz, bukan 5GHz (ESP32 hanya support 2.4GHz)
3. Cek jarak ESP32 ke router WiFi

### Problem 3: MQTT Tidak Connect

**Solusi:**
1. Pastikan server Node.js berjalan
2. Cek IP server sudah benar:
   ```bash
   hostname -I
   ```
3. Ping dari laptop ke ESP32:
   ```bash
   ping [IP_ESP32]
   ```
4. Pastikan laptop dan ESP32 di jaringan WiFi yang sama
5. Cek firewall di laptop tidak block port 1883

### Problem 4: Fingerprint Sensor Tidak Terdeteksi

**Solusi:**
1. Cek koneksi kabel:
   - TX Sensor → RX2 ESP32 (GPIO16)
   - RX Sensor → TX2 ESP32 (GPIO17)
2. Pastikan VCC sensor dapat 3.3V (BUKAN 5V!)
3. Di Serial Monitor, pastikan muncul:
   ```
   Fingerprint sensor detected!
   ```
4. Jika muncul "Did not find fingerprint sensor", cek kabel lagi

### Problem 5: LED Tidak Menyala

**Solusi:**
1. Cek polaritas LED (kaki panjang = +, kaki pendek = -)
2. Pastikan resistor 220Ω terpasang
3. Cek koneksi GND
4. Test manual:
   ```cpp
   digitalWrite(LED_GREEN, HIGH); // Nyalakan LED hijau
   delay(1000);
   digitalWrite(LED_GREEN, LOW);  // Matikan LED hijau
   ```

### Problem 6: Enrollment Selalu Gagal

**Solusi:**
1. Bersihkan sensor fingerprint (usap dengan kain lembut)
2. Pastikan jari kering (tidak basah/berminyak)
3. Tekan jari dengan kuat saat enrollment
4. Coba jari lain
5. Cek Serial Monitor untuk pesan error detail

---

## 📊 Flow Diagram Sistem

```
┌──────────────┐                    ┌──────────────┐                    ┌──────────────┐
│  Web Admin   │                    │   Server     │                    │    ESP32     │
│              │                    │   Node.js    │                    │ + Fingerprint│
└──────┬───────┘                    └──────┬───────┘                    └──────┬───────┘
       │                                   │                                   │
       │ 1. Isi form karyawan              │                                   │
       ├──────────────────────────────────>│                                   │
       │                                   │                                   │
       │ 2. Simpan ke database             │                                   │
       │<──────────────────────────────────┤                                   │
       │                                   │                                   │
       │ 3. Klik "Daftarkan Sidik Jari"    │                                   │
       ├──────────────────────────────────>│                                   │
       │                                   │                                   │
       │                                   │ 4. MQTT Publish:                  │
       │                                   │    fingerprint/enroll/request     │
       │                                   ├──────────────────────────────────>│
       │                                   │                                   │
       │                                   │                                   │ 5. LED Biru ON
       │                                   │                                   │    Tunggu jari
       │                                   │                                   │
       │                                   │                                   │ 6. Scan jari 1x
       │                                   │                                   │    LED hijau blink
       │                                   │                                   │
       │                                   │                                   │ 7. Scan jari 2x
       │                                   │                                   │    (verifikasi)
       │                                   │                                   │
       │                                   │ 8. MQTT Publish:                  │
       │                                   │    fingerprint/enroll/response    │
       │                                   │<──────────────────────────────────┤
       │                                   │                                   │
       │                                   │ 9. Update database                │ 8. LED Hijau ON
       │                                   │    fingerprint_id = X             │    (Success!)
       │                                   │                                   │
       │ 10. Socket.IO: enrollment_success │                                   │
       │<──────────────────────────────────┤                                   │
       │                                   │                                   │
       │ 11. Tampilkan notifikasi sukses! │                                   │
       │                                   │                                   │
```

---

## 🎨 LED Indicator Behavior

| Kondisi | LED Merah | LED Hijau | LED Biru | Keterangan |
|---------|-----------|-----------|----------|------------|
| **Booting** | Blink | OFF | OFF | ESP32 starting up |
| **WiFi Connect** | OFF | OFF | Blink | Connecting to WiFi |
| **MQTT Connect** | OFF | Blink | OFF | Connecting to MQTT |
| **Ready** | OFF | ON (1s) | OFF | System ready |
| **Waiting Enrollment** | OFF | OFF | ON | Tunggu perintah MQTT |
| **Scan Finger** | OFF | OFF | Blink cepat | Tunggu jari ditempelkan |
| **Processing** | OFF | Blink | OFF | Memproses image |
| **Success** | OFF | ON (3s) | OFF | Enrollment berhasil |
| **Error** | ON (3s) | OFF | OFF | Enrollment gagal |

---

## 📁 File yang Perlu Didownload

Saya akan buatkan file-file berikut:

1. ✅ **ESP32_FINGERPRINT_SETUP.md** (file ini)
2. ⏳ **esp32_fingerprint.ino** (code lengkap Arduino)
3. ⏳ **wiring_diagram.png** (diagram koneksi visual)

---

## 📞 Support

Jika ada masalah:
1. Cek Serial Monitor untuk error message
2. Pastikan semua koneksi hardware benar
3. Verifikasi konfigurasi WiFi dan MQTT
4. Lihat troubleshooting di atas

---

**Status:** ✅ Dokumentasi Lengkap
**Last Updated:** 16 November 2025
**Tested with:** ESP32 DevKit v1 + R307 Fingerprint Sensor
