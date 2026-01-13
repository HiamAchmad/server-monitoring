# BAB 3
# PERANCANGAN DAN IMPLEMENTASI SISTEM

## 3.1 Analisis Kebutuhan Sistem

### 3.1.1 Kebutuhan Fungsional

| No | Kebutuhan | Deskripsi | Prioritas |
|----|-----------|-----------|-----------|
| F1 | Identifikasi Sidik Jari | Sistem dapat mengenali sidik jari pegawai yang terdaftar | Tinggi |
| F2 | Pencatatan Kehadiran | Sistem mencatat waktu masuk dan keluar pegawai secara otomatis | Tinggi |
| F3 | Pendaftaran Sidik Jari | Admin dapat mendaftarkan sidik jari pegawai baru via dashboard | Tinggi |
| F4 | Monitoring Real-time | Dashboard menampilkan data kehadiran secara real-time | Tinggi |
| F5 | Manajemen Pegawai | CRUD data pegawai (NIP, nama, jabatan, departemen) | Tinggi |
| F6 | Manajemen Cuti | Pengajuan dan persetujuan cuti pegawai | Sedang |
| F7 | Perhitungan Payroll | Kalkulasi gaji berdasarkan kehadiran | Sedang |
| F8 | Backup Data | Backup data ke cloud storage (OwnCloud) | Sedang |
| F9 | Laporan | Generate laporan kehadiran bulanan | Sedang |
| F10 | Autentikasi | Login dengan role-based access control | Tinggi |

### 3.1.2 Kebutuhan Non-Fungsional

| No | Kebutuhan | Deskripsi | Target |
|----|-----------|-----------|--------|
| NF1 | **Latency** | Waktu respons dari scan hingga tercatat | < 1 detik |
| NF2 | **Availability** | Ketersediaan sistem | > 99% uptime |
| NF3 | **Reliability** | Keandalan transmisi data (packet loss) | < 1% |
| NF4 | **Scalability** | Jumlah pegawai yang didukung | > 100 pegawai |
| NF5 | **Security** | Perlindungan data dan akses | CSRF, RBAC, Encryption |
| NF6 | **Usability** | Kemudahan penggunaan | Interface intuitif |
| NF7 | **Maintainability** | Kemudahan maintenance | Modular architecture |

---

## 3.2 Perancangan Arsitektur Sistem

### 3.2.1 Arsitektur Sistem Keseluruhan

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                              ARSITEKTUR SISTEM MONITORING KEPEGAWAIAN IoT                        │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

╔═════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                      DEVICE LAYER (IoT)                                          ║
╠═════════════════════════════════════════════════════════════════════════════════════════════════╣
║                                                                                                  ║
║   ┌───────────────────────────────────────────────────────────────────────────────────────────┐  ║
║   │                              ESP32 + FINGERPRINT + OLED                                   │  ║
║   │                                                                                           │  ║
║   │   ┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐               │  ║
║   │   │ Fingerprint │    │    ESP32    │    │    OLED     │    │   LED RGB   │               │  ║
║   │   │   AS608     │───►│  DevKit V1  │───►│  SSD1306    │    │  Indicator  │               │  ║
║   │   │   Sensor    │    │ Controller  │    │  Display    │    │  R/G/B      │               │  ║
║   │   └─────────────┘    └──────┬──────┘    └─────────────┘    └─────────────┘               │  ║
║   │                             │                                                             │  ║
║   │                      ┌──────▼──────┐                                                      │  ║
║   │                      │  WiFi STA   │                                                      │  ║
║   │                      │  802.11bgn  │                                                      │  ║
║   │                      └──────┬──────┘                                                      │  ║
║   └──────────────────────────────│────────────────────────────────────────────────────────────┘  ║
║                                  │                                                               ║
╚══════════════════════════════════│═══════════════════════════════════════════════════════════════╝
                                   │ MQTT (Port 1883)
                                   │ Topic: fingerprint/*
                                   ▼
╔══════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                      NETWORK LAYER                                                ║
╠══════════════════════════════════════════════════════════════════════════════════════════════════╣
║                                                                                                   ║
║   ┌─────────────────────────────────────────────────────────────────────────────────────────┐    ║
║   │                              WiFi Router / Access Point                                  │    ║
║   │                                    2.4 GHz                                               │    ║
║   └─────────────────────────────────────────┬───────────────────────────────────────────────┘    ║
║                                             │                                                     ║
╚═════════════════════════════════════════════│═════════════════════════════════════════════════════╝
                                              │ TCP/IP
                                              ▼
╔══════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                      APPLICATION LAYER                                            ║
╠══════════════════════════════════════════════════════════════════════════════════════════════════╣
║                                                                                                   ║
║   ┌─────────────────────────────────────────────────────────────────────────────────────────┐    ║
║   │                              NODE.JS APPLICATION SERVER                                  │    ║
║   │                                                                                          │    ║
║   │   ┌─────────────┐    ┌─────────────┐    ┌─────────────┐    ┌─────────────┐              │    ║
║   │   │ MQTT Broker │    │ HTTP Server │    │  Socket.IO  │    │  Business   │              │    ║
║   │   │   (Aedes)   │◄──►│  (Express)  │◄──►│  WebSocket  │◄──►│   Logic     │              │    ║
║   │   │  Port:1883  │    │  Port:3000  │    │             │    │             │              │    ║
║   │   └─────────────┘    └─────────────┘    └─────────────┘    └──────┬──────┘              │    ║
║   │                                                                   │                      │    ║
║   └───────────────────────────────────────────────────────────────────┼──────────────────────┘    ║
║                                                                       │                           ║
╚═══════════════════════════════════════════════════════════════════════│═══════════════════════════╝
                                                                        │ PostgreSQL Protocol
                                                                        │ Port: 5432
                                                                        ▼
╔══════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                      DATA LAYER                                                   ║
╠══════════════════════════════════════════════════════════════════════════════════════════════════╣
║                                                                                                   ║
║   ┌─────────────────────────────────────────────────────────────────────────────────────────┐    ║
║   │                              POSTGRESQL DATABASE                                          │    ║
║   │                                   Port: 5432                                              │    ║
║   │                                                                                          │    ║
║   │   ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐          │    ║
║   │   │  users  │  │ pegawai │  │ absensi │  │  cuti   │  │ payroll │  │settings │          │    ║
║   │   └─────────┘  └─────────┘  └─────────┘  └─────────┘  └─────────┘  └─────────┘          │    ║
║   │                                                                                          │    ║
║   └─────────────────────────────────────────────────────────────────────────────────────────┘    ║
║                                                                                                   ║
╚══════════════════════════════════════════════════════════════════════════════════════════════════╝
                                              │ WebDAV (HTTP PUT)
                                              │ Port: 8080
                                              ▼
╔══════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                      CLOUD LAYER                                                  ║
╠══════════════════════════════════════════════════════════════════════════════════════════════════╣
║                                                                                                   ║
║   ┌─────────────────────────────────────────────────────────────────────────────────────────┐    ║
║   │                              OWNCLOUD (Docker Container)                                 │    ║
║   │                                   Port: 8080                                             │    ║
║   │                                                                                          │    ║
║   │   ┌───────────────────────────────────────────────────────────────────────────────┐     │    ║
║   │   │                     /Backup-Absensi/                                          │     │    ║
║   │   │   • Backup-Absensi-2024-01-15_08-00-00.csv                                   │     │    ║
║   │   │   • Backup-Absensi-2024-01-16_08-00-00.csv                                   │     │    ║
║   │   │   • ...                                                                       │     │    ║
║   │   └───────────────────────────────────────────────────────────────────────────────┘     │    ║
║   │                                                                                          │    ║
║   └─────────────────────────────────────────────────────────────────────────────────────────┘    ║
║                                                                                                   ║
╚══════════════════════════════════════════════════════════════════════════════════════════════════╝
```

### 3.2.2 Arsitektur Komunikasi

| Layer | Komponen | Protokol | Port | Fungsi |
|-------|----------|----------|------|--------|
| Device | ESP32 ↔ Server | MQTT | 1883 | Transmisi data fingerprint |
| Network | WiFi | 802.11 b/g/n | - | Konektivitas wireless |
| Application | Browser ↔ Server | HTTP/REST | 3000 | API request/response |
| Application | Browser ↔ Server | WebSocket | 3000 | Real-time notification |
| Data | Server ↔ Database | PostgreSQL | 5432 | Query database |
| Cloud | Server ↔ OwnCloud | WebDAV | 8080 | Backup file |

---

## 3.3 Perangkat Keras (Hardware)

### 3.3.1 Spesifikasi Perangkat IoT

#### A. Daftar Komponen Hardware

| No | Komponen | Spesifikasi | Qty | Fungsi |
|----|----------|-------------|-----|--------|
| 1 | **ESP32 DevKit V1** | Xtensa LX6 Dual-core 240MHz, 520KB SRAM, 4MB Flash, WiFi 802.11 b/g/n, Bluetooth 4.2 | 1 | Mikrokontroler utama |
| 2 | **Fingerprint Sensor R307/AS608** | Optical sensor, 500 DPI, kapasitas 127 template, UART 57600bps, FAR <0.001%, FRR <0.1% | 1 | Identifikasi sidik jari |
| 3 | **OLED Display SSD1306** | 0.96 inch, resolusi 128x64 pixel, interface SPI | 1 | Feedback visual |
| 4 | **LED RGB** | LED 5mm, warna Merah/Hijau/Biru | 3 | Indikator status |
| 5 | **Resistor** | 220Ω 1/4W | 3 | Pembatas arus LED |
| 6 | **Kabel Jumper** | Male-to-Female, Male-to-Male | ~15 | Koneksi komponen |
| 7 | **Breadboard** | 830 points | 1 | Prototyping |
| 8 | **Kabel USB Micro** | USB 2.0 Type-A to Micro-B | 1 | Power & programming |

#### B. Spesifikasi Teknis ESP32

| Parameter | Spesifikasi |
|-----------|-------------|
| **Processor** | Xtensa LX6 Dual-core @ 240 MHz |
| **SRAM** | 520 KB |
| **Flash** | 4 MB (external) |
| **WiFi** | 802.11 b/g/n, 2.4 GHz, WPA/WPA2 |
| **Bluetooth** | v4.2 BR/EDR dan BLE |
| **GPIO** | 34 pin programmable |
| **UART** | 3 port (UART0, UART1, UART2) |
| **Operating Voltage** | 3.3V |
| **Operating Current** | 80mA (average), 200mA (peak WiFi TX) |

#### C. Spesifikasi Teknis Sensor Fingerprint AS608

| Parameter | Spesifikasi |
|-----------|-------------|
| **Tipe Sensor** | Optical |
| **Resolusi** | 500 DPI |
| **Kapasitas Template** | 127 fingerprints |
| **Waktu Pengenalan** | < 1 detik |
| **False Acceptance Rate (FAR)** | < 0.001% |
| **False Rejection Rate (FRR)** | < 0.1% |
| **Baud Rate** | 9600 - 115200 (default 57600) |
| **Tegangan Operasi** | 3.3V - 6V DC |
| **Arus Operasi** | < 120mA |
| **Interface** | UART (TTL) |

### 3.3.2 Skema Rangkaian Hardware

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         SKEMA RANGKAIAN PERANGKAT IoT                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │                         ESP32 DevKit V1                             │   │
│   │                                                                     │   │
│   │   3.3V  ──────────────────┬────────────── VCC Fingerprint          │   │
│   │   GND   ──────────────────┼────────────── GND Fingerprint          │   │
│   │   GPIO16 (RX2) ───────────┼────────────── TX  Fingerprint          │   │
│   │   GPIO17 (TX2) ───────────┼────────────── RX  Fingerprint          │   │
│   │                           │                                         │   │
│   │   GPIO25 ─────────[220Ω]──┼────────────── LED Merah (Error)        │   │
│   │   GPIO26 ─────────[220Ω]──┼────────────── LED Hijau (Success)      │   │
│   │   GPIO27 ─────────[220Ω]──┼────────────── LED Biru  (Process)      │   │
│   │                           │                                         │   │
│   │   3.3V  ──────────────────┼────────────── VCC OLED                 │   │
│   │   GND   ──────────────────┼────────────── GND OLED                 │   │
│   │   GPIO18 ─────────────────┼────────────── SCK OLED (SPI Clock)     │   │
│   │   GPIO23 ─────────────────┼────────────── SDA OLED (MOSI)          │   │
│   │   GPIO5  ─────────────────┼────────────── CS  OLED (Chip Select)   │   │
│   │   GPIO4  ─────────────────┼────────────── DC  OLED (Data/Command)  │   │
│   │   GPIO2  ─────────────────┼────────────── RES OLED (Reset)         │   │
│   │                           │                                         │   │
│   └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 3.3.3 Tabel Pin Mapping

| ESP32 Pin | Komponen | Fungsi | Keterangan |
|-----------|----------|--------|------------|
| GPIO16 | Fingerprint TX | UART2 RX | Serial receive dari sensor |
| GPIO17 | Fingerprint RX | UART2 TX | Serial transmit ke sensor |
| GPIO25 | LED Merah | Digital Output | Indikator error/gagal |
| GPIO26 | LED Hijau | Digital Output | Indikator sukses |
| GPIO27 | LED Biru | Digital Output | Indikator proses |
| GPIO18 | OLED SCK | SPI Clock | Clock signal |
| GPIO23 | OLED SDA | SPI MOSI | Data signal |
| GPIO5 | OLED CS | SPI Chip Select | Pemilihan device |
| GPIO4 | OLED DC | Data/Command | Mode kontrol |
| GPIO2 | OLED RES | Reset | Reset display |
| 3.3V | VCC | Power | Tegangan operasi |
| GND | Ground | Common ground | Referensi ground |

### 3.3.4 Spesifikasi Server

| Komponen | Minimum | Recommended |
|----------|---------|-------------|
| **Processor** | Intel Core i3 / AMD Ryzen 3 | Intel Core i5 / AMD Ryzen 5 |
| **RAM** | 4 GB | 8 GB |
| **Storage** | 50 GB HDD | 100 GB SSD |
| **OS** | Ubuntu 20.04 / Windows 10 | Ubuntu 22.04 / Windows 11 |
| **Network** | 100 Mbps | 1 Gbps |

---

## 3.4 Perangkat Lunak (Software)

### 3.4.1 Software Pengembangan

| No | Software | Versi | Fungsi |
|----|----------|-------|--------|
| 1 | **Visual Studio Code** | Latest | IDE utama pengembangan |
| 2 | **PlatformIO IDE** | Latest | Extension untuk ESP32 development |
| 3 | **Node.js** | ≥ 14.0.0 | Runtime JavaScript server-side |
| 4 | **npm** | ≥ 6.0.0 | Package manager Node.js |
| 5 | **Git** | Latest | Version control |
| 6 | **PostgreSQL** | 15.x | Database server |
| 7 | **Docker** | Latest | Container untuk OwnCloud |

### 3.4.2 Library ESP32 (PlatformIO)

| No | Library | Fungsi |
|----|---------|--------|
| 1 | **Adafruit Fingerprint Sensor Library** | Driver sensor fingerprint AS608 |
| 2 | **PubSubClient** | MQTT client untuk ESP32 |
| 3 | **ArduinoJson** | JSON parsing/serialization |
| 4 | **Adafruit SSD1306** | Driver OLED display |
| 5 | **Adafruit GFX Library** | Graphics primitives |

**Konfigurasi platformio.ini:**
```ini
[env:esp32dev]
platform = espressif32
board = esp32dev
framework = arduino
monitor_speed = 115200
upload_speed = 921600

lib_deps =
    adafruit/Adafruit Fingerprint Sensor Library
    knolleary/PubSubClient
    bblanchon/ArduinoJson
    adafruit/Adafruit SSD1306
    adafruit/Adafruit GFX Library
```

### 3.4.3 Library Server (Node.js)

| No | Package | Versi | Fungsi |
|----|---------|-------|--------|
| 1 | **express** | ^5.1.0 | Web framework HTTP server |
| 2 | **socket.io** | ^4.8.1 | WebSocket real-time communication |
| 3 | **aedes** | ^0.51.3 | Embedded MQTT broker |
| 4 | **mqtt** | ^5.14.1 | MQTT client library |
| 5 | **pg** | ^8.16.3 | PostgreSQL client |
| 6 | **axios** | ^1.13.2 | HTTP client untuk WebDAV |
| 7 | **webdav** | ^4.11.2 | WebDAV client untuk OwnCloud |
| 8 | **dotenv** | ^17.2.3 | Environment variables management |
| 9 | **moment** | ^2.30.1 | Date/time manipulation |
| 10 | **moment-timezone** | ^0.6.0 | Timezone support |
| 11 | **multer** | ^2.0.2 | File upload handling |
| 12 | **node-cron** | ^4.2.1 | Task scheduling |

**Konfigurasi package.json:**
```json
{
  "name": "sistem-absensi-iot",
  "version": "1.0.0",
  "dependencies": {
    "aedes": "^0.51.3",
    "axios": "^1.13.2",
    "dotenv": "^17.2.3",
    "express": "^5.1.0",
    "moment": "^2.30.1",
    "mqtt": "^5.14.1",
    "pg": "^8.16.3",
    "socket.io": "^4.8.1",
    "webdav": "^4.11.2"
  }
}
```

---

## 3.5 Perancangan Jaringan

### 3.5.1 Topologi Jaringan

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         TOPOLOGI JARINGAN SISTEM                             │
└─────────────────────────────────────────────────────────────────────────────┘

                              ┌─────────────────┐
                              │   WiFi Router   │
                              │   (Access Point)│
                              │    2.4 GHz      │
                              └────────┬────────┘
                                       │
           ┌───────────────────────────┼───────────────────────────┐
           │                           │                           │
           ▼                           ▼                           ▼
    ┌─────────────┐             ┌─────────────┐             ┌─────────────┐
    │   ESP32     │             │   SERVER    │             │   CLIENT    │
    │ + Sensor    │             │  (Node.js)  │             │  (Browser)  │
    │ + OLED      │             │             │             │             │
    │             │             │ ┌─────────┐ │             │  Dashboard  │
    │  WiFi STA   │◄───MQTT────►│ │  Aedes  │ │◄──HTTP/WS──►│  Monitoring │
    │  Mode       │    1883     │ │  :1883  │ │    3000     │             │
    └─────────────┘             │ └─────────┘ │             └─────────────┘
                                │             │
                                │ ┌─────────┐ │
                                │ │ Express │ │
                                │ │  :3000  │ │
                                │ └────┬────┘ │
                                └──────┼──────┘
                                       │
                    ┌──────────────────┼──────────────────┐
                    │                  │                  │
                    ▼                  ▼                  ▼
             ┌─────────────┐    ┌─────────────┐    ┌─────────────┐
             │ PostgreSQL  │    │  OwnCloud   │    │  pgAdmin    │
             │   :5432     │    │   :8080     │    │  (Optional) │
             │             │    │  (Docker)   │    │             │
             └─────────────┘    └─────────────┘    └─────────────┘
```

### 3.5.2 Konfigurasi Port

| Port | Protokol | Service | Akses | Keterangan |
|------|----------|---------|-------|------------|
| 3000 | TCP/HTTP | Express + Socket.IO | Internal | Web server & WebSocket |
| 1883 | TCP/MQTT | Aedes MQTT Broker | Internal | IoT communication |
| 5432 | TCP | PostgreSQL | Localhost | Database (local only) |
| 8080 | TCP/HTTP | OwnCloud WebDAV | Internal | Cloud storage |

### 3.5.3 Konfigurasi MQTT

| Parameter | Nilai | Keterangan |
|-----------|-------|------------|
| **Broker** | Aedes (embedded) | Berjalan dalam Node.js |
| **Port** | 1883 | Standard MQTT port |
| **QoS Default** | 1 | At least once delivery |
| **Keep Alive** | 60 detik | Client heartbeat interval |
| **Clean Session** | true | Fresh session setiap koneksi |

**MQTT Topics:**

| Topic | Direction | QoS | Payload | Fungsi |
|-------|-----------|-----|---------|--------|
| `fingerprint/attendance` | ESP32 → Server | 1 | `{fingerprint_id, timestamp}` | Data scan absensi |
| `fingerprint/enroll/request` | Server → ESP32 | 1 | `{pegawai_id, fingerprint_id, nama}` | Perintah enrollment |
| `fingerprint/enroll/response` | ESP32 → Server | 1 | `{success, fingerprint_id, message}` | Hasil enrollment |
| `fingerprint/enroll/progress` | ESP32 → Server | 0 | `{step, message}` | Progress enrollment |
| `fingerprint/command` | Server → ESP32 | 1 | `{command, params}` | Perintah umum |

---

## 3.6 Perancangan Database

### 3.6.1 Entity Relationship Diagram (ERD)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              ERD SISTEM                                      │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌──────────────┐                              ┌──────────────┐
    │    USERS     │                              │   PEGAWAI    │
    ├──────────────┤                              ├──────────────┤
    │ PK id        │                              │ PK id        │
    │    username  │                              │    nip       │
    │    password  │                              │    nama      │
    │    role      │                              │    jabatan   │
    │    created_at│                              │    departemen│
    └──────────────┘                              │    gaji_pokok│
                                                  │    fp_id     │
                                                  │    created_at│
                                                  └──────┬───────┘
                                                         │
                                                         │ 1
                                                         │
                    ┌────────────────────────────────────┼────────────────────────────────────┐
                    │                                    │                                    │
                    ▼ N                                  ▼ N                                  ▼ N
            ┌──────────────┐                      ┌──────────────┐                     ┌──────────────┐
            │   ABSENSI    │                      │     CUTI     │                     │   PAYROLL    │
            ├──────────────┤                      ├──────────────┤                     ├──────────────┤
            │ PK id        │                      │ PK id        │                     │ PK id        │
            │ FK pegawai_id│                      │ FK pegawai_id│                     │ FK pegawai_id│
            │    tanggal   │                      │    jenis_cuti│                     │    bulan     │
            │    jam_masuk │                      │    tgl_mulai │                     │    tahun     │
            │    jam_keluar│                      │    tgl_akhir │                     │    gaji_pokok│
            │    status    │                      │    status    │                     │    tunjangan │
            │    fp_id     │                      │    keterangan│                     │    potongan  │
            │    keterangan│                      │    approved_by│                    │    total     │
            └──────────────┘                      └──────────────┘                     └──────────────┘
```

### 3.6.2 Skema Tabel

**Tabel users:**

| Kolom | Tipe Data | Constraint | Keterangan |
|-------|-----------|------------|------------|
| id | SERIAL | PRIMARY KEY | Auto increment ID |
| username | VARCHAR(50) | UNIQUE, NOT NULL | Username login |
| password | VARCHAR(255) | NOT NULL | Password (bcrypt hash) |
| role | VARCHAR(20) | NOT NULL | 'admin' atau 'user' |
| created_at | TIMESTAMP | DEFAULT NOW() | Waktu dibuat |

**Tabel pegawai:**

| Kolom | Tipe Data | Constraint | Keterangan |
|-------|-----------|------------|------------|
| id | SERIAL | PRIMARY KEY | Auto increment ID |
| nip | VARCHAR(20) | UNIQUE, NOT NULL | Nomor Induk Pegawai |
| nama | VARCHAR(100) | NOT NULL | Nama lengkap |
| jabatan | VARCHAR(50) | | Jabatan/posisi |
| departemen | VARCHAR(50) | | Unit kerja |
| gaji_pokok | DECIMAL(12,2) | | Gaji pokok |
| fp_id | INTEGER | UNIQUE | Fingerprint ID di sensor |
| created_at | TIMESTAMP | DEFAULT NOW() | Waktu dibuat |

**Tabel absensi:**

| Kolom | Tipe Data | Constraint | Keterangan |
|-------|-----------|------------|------------|
| id | SERIAL | PRIMARY KEY | Auto increment ID |
| pegawai_id | INTEGER | FOREIGN KEY | Referensi ke pegawai |
| tanggal | DATE | NOT NULL | Tanggal absensi |
| jam_masuk | TIME | | Waktu clock-in |
| jam_keluar | TIME | | Waktu clock-out |
| status | VARCHAR(20) | | HADIR/TERLAMBAT/IZIN/CUTI/ALPHA |
| fingerprint_id | INTEGER | | ID dari sensor |
| keterangan | TEXT | | Catatan tambahan |

**Tabel cuti:**

| Kolom | Tipe Data | Constraint | Keterangan |
|-------|-----------|------------|------------|
| id | SERIAL | PRIMARY KEY | Auto increment ID |
| pegawai_id | INTEGER | FOREIGN KEY | Referensi ke pegawai |
| jenis_cuti | VARCHAR(50) | NOT NULL | Jenis cuti |
| tgl_mulai | DATE | NOT NULL | Tanggal mulai cuti |
| tgl_akhir | DATE | NOT NULL | Tanggal selesai cuti |
| status | VARCHAR(20) | | PENDING/APPROVED/REJECTED |
| keterangan | TEXT | | Alasan cuti |
| approved_by | INTEGER | | ID admin yang menyetujui |

**Tabel payroll:**

| Kolom | Tipe Data | Constraint | Keterangan |
|-------|-----------|------------|------------|
| id | SERIAL | PRIMARY KEY | Auto increment ID |
| pegawai_id | INTEGER | FOREIGN KEY | Referensi ke pegawai |
| bulan | INTEGER | NOT NULL | Bulan (1-12) |
| tahun | INTEGER | NOT NULL | Tahun |
| gaji_pokok | DECIMAL(12,2) | | Gaji pokok |
| tunjangan | DECIMAL(12,2) | | Total tunjangan |
| potongan | DECIMAL(12,2) | | Total potongan |
| total | DECIMAL(12,2) | | Gaji bersih |

---

## 3.7 Perancangan Alur Sistem

### 3.7.1 Flowchart Absensi

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         FLOWCHART PROSES ABSENSI                             │
└─────────────────────────────────────────────────────────────────────────────┘

                                    ┌─────────┐
                                    │  START  │
                                    └────┬────┘
                                         │
                                         ▼
                                ┌────────────────┐
                                │ Scan Sidik Jari│
                                │   di Sensor    │
                                └────────┬───────┘
                                         │
                                         ▼
                              ┌──────────────────────┐
                              │ Sensor Mendeteksi    │
                              │    Sidik Jari?       │
                              └──────────┬───────────┘
                                         │
                        ┌────────────────┴────────────────┐
                        │ Ya                              │ Tidak
                        ▼                                 ▼
              ┌──────────────────┐              ┌──────────────────┐
              │ Konversi Image   │              │ OLED: "Letakkan  │
              │   ke Template    │              │   Jari Anda"     │
              └────────┬─────────┘              └──────────────────┘
                       │
                       ▼
              ┌──────────────────┐
              │ Cari di Database │
              │    Sensor        │
              └────────┬─────────┘
                       │
                       ▼
              ┌──────────────────┐
              │ Sidik Jari       │
              │   Terdaftar?     │
              └────────┬─────────┘
                       │
          ┌────────────┴────────────┐
          │ Ya                      │ Tidak
          ▼                         ▼
  ┌──────────────────┐      ┌──────────────────┐
  │ Get Fingerprint  │      │ LED Merah ON     │
  │      ID          │      │ OLED: "Tidak     │
  └────────┬─────────┘      │   Terdaftar!"    │
           │                └──────────────────┘
           ▼
  ┌──────────────────┐
  │ Publish MQTT     │
  │ fingerprint/     │
  │ attendance       │
  └────────┬─────────┘
           │
           ▼
  ┌──────────────────┐
  │ Server Menerima  │
  │   Data MQTT      │
  └────────┬─────────┘
           │
           ▼
  ┌──────────────────┐
  │ Query Pegawai    │
  │ by fp_id         │
  └────────┬─────────┘
           │
           ▼
  ┌──────────────────┐
  │ Insert/Update    │
  │ Tabel Absensi    │
  └────────┬─────────┘
           │
           ▼
  ┌──────────────────┐
  │ Emit WebSocket   │
  │ new_attendance   │
  └────────┬─────────┘
           │
           ▼
  ┌──────────────────┐
  │ LED Hijau ON     │
  │ OLED: "Berhasil!"│
  │ Nama + Jam       │
  └────────┬─────────┘
           │
           ▼
      ┌─────────┐
      │   END   │
      └─────────┘
```

### 3.7.2 Flowchart Enrollment

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      FLOWCHART PROSES ENROLLMENT                             │
└─────────────────────────────────────────────────────────────────────────────┘

      DASHBOARD                       SERVER                         ESP32
          │                              │                              │
          │   POST /api/enroll           │                              │
          │─────────────────────────────►│                              │
          │                              │                              │
          │                              │   MQTT: fingerprint/         │
          │                              │   enroll/request             │
          │                              │─────────────────────────────►│
          │                              │                              │
          │                              │                     ┌────────┴────────┐
          │                              │                     │ LED Biru Blink  │
          │                              │                     │ OLED: "Letakkan │
          │                              │                     │   Jari 1..."    │
          │                              │                     └────────┬────────┘
          │                              │                              │
          │                              │                     ┌────────┴────────┐
          │                              │                     │ Capture Image 1 │
          │                              │                     │ Convert to Tz1  │
          │                              │                     └────────┬────────┘
          │                              │                              │
          │                              │   MQTT: fingerprint/         │
          │                              │   enroll/progress            │
          │                              │◄─────────────────────────────│
          │                              │                              │
          │◄─ WebSocket: progress ───────│                              │
          │                              │                     ┌────────┴────────┐
          │                              │                     │ OLED: "Angkat   │
          │                              │                     │   Jari..."      │
          │                              │                     │ "Letakkan 2..." │
          │                              │                     └────────┬────────┘
          │                              │                              │
          │                              │                     ┌────────┴────────┐
          │                              │                     │ Capture Image 2 │
          │                              │                     │ Convert to Tz2  │
          │                              │                     │ Create Model    │
          │                              │                     │ Store to ID     │
          │                              │                     └────────┬────────┘
          │                              │                              │
          │                              │   MQTT: fingerprint/         │
          │                              │   enroll/response            │
          │                              │◄─────────────────────────────│
          │                              │                              │
          │                     ┌────────┴────────┐                     │
          │                     │ Update pegawai  │                     │
          │                     │ SET fp_id = X   │                     │
          │                     └────────┬────────┘                     │
          │                              │                              │
          │◄─ WebSocket: success ────────│                     ┌────────┴────────┐
          │                              │                     │ LED Hijau ON    │
          │                              │                     │ OLED: "Berhasil"│
          │                              │                     └─────────────────┘
          │                              │                              │
```

### 3.7.3 Sequence Diagram Real-time Monitoring

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    SEQUENCE DIAGRAM REAL-TIME MONITORING                     │
└─────────────────────────────────────────────────────────────────────────────┘

    Browser              Server                ESP32               Sensor
       │                    │                    │                    │
       │  HTTP: Load Page   │                    │                    │
       │───────────────────►│                    │                    │
       │                    │                    │                    │
       │◄── HTML + JS ──────│                    │                    │
       │                    │                    │                    │
       │  WebSocket Connect │                    │                    │
       │═══════════════════►│                    │                    │
       │                    │                    │                    │
       │◄═ initial_data ════│                    │                    │
       │   [attendance]     │                    │                    │
       │                    │                    │                    │
       │                    │                    │         ┌──────────┴──────────┐
       │                    │                    │         │ Pegawai scan jari   │
       │                    │                    │         └──────────┬──────────┘
       │                    │                    │                    │
       │                    │                    │◄── Image ──────────│
       │                    │                    │                    │
       │                    │                    │── Match ──────────►│
       │                    │                    │                    │
       │                    │                    │◄── ID + Confidence─│
       │                    │                    │                    │
       │                    │  MQTT: attendance  │                    │
       │                    │◄═══════════════════│                    │
       │                    │                    │                    │
       │                    │──┐                 │                    │
       │                    │  │ Process:        │                    │
       │                    │  │ - Get pegawai   │                    │
       │                    │  │ - Insert absensi│                    │
       │                    │◄─┘                 │                    │
       │                    │                    │                    │
       │◄═ new_attendance ══│                    │                    │
       │   {nama, waktu}    │                    │                    │
       │                    │                    │                    │
       │──┐                 │                    │                    │
       │  │ Update UI:      │                    │                    │
       │  │ - Add row       │                    │                    │
       │  │ - Notification  │                    │                    │
       │◄─┘                 │                    │                    │
       │                    │                    │                    │
```

---

## 3.8 Implementasi Sistem

### 3.8.1 Struktur Direktori Proyek

```
server-monitoring/
├── esp32/
│   ├── platformio.ini              # Konfigurasi PlatformIO
│   └── src/
│       └── esp32_fingerprint.ino   # Source code ESP32
│
├── public/
│   ├── admin/                      # Halaman admin
│   │   ├── dashboard-glass.html
│   │   ├── pegawai.html
│   │   ├── absensi.html
│   │   ├── cuti.html
│   │   ├── payroll.html
│   │   └── settings.html
│   ├── css/                        # Stylesheet
│   ├── js/                         # JavaScript client
│   └── assets/                     # Gambar, icon
│
├── server.js                       # Main server file
├── package.json                    # Node.js dependencies
├── .env                            # Environment variables
├── init-db.sql                     # SQL initialization
│
├── docker-compose.yml              # Docker untuk OwnCloud
│
└── dokumentasi/                    # Dokumentasi
    ├── bab1-pendahuluan.md
    ├── bab2-landasan-teori.md
    ├── bab3-perancangan-sistem.md
    ├── bab4-implementasi.md
    ├── bab5-kesimpulan.md
    └── diagrams/
        ├── topologi-jaringan.md
        └── activity-diagram.md
```

### 3.8.2 Langkah Implementasi

| No | Tahap | Kegiatan | Output |
|----|-------|----------|--------|
| 1 | **Persiapan Environment** | Install Node.js, PostgreSQL, Docker, PlatformIO | Development environment ready |
| 2 | **Setup Database** | Buat database, tabel, user | Database db_absensi |
| 3 | **Setup Server** | Install dependencies, konfigurasi .env | Server Node.js running |
| 4 | **Setup OwnCloud** | Docker compose up | OwnCloud container running |
| 5 | **Rangkai Hardware** | Koneksi ESP32, sensor, OLED, LED | Perangkat IoT terhubung |
| 6 | **Upload Firmware** | Flash kode ke ESP32 via PlatformIO | ESP32 running |
| 7 | **Testing Koneksi** | Test WiFi, MQTT, Database | Semua terhubung |
| 8 | **Testing Fungsional** | Test absensi, enrollment, backup | Fitur berjalan |
| 9 | **Deployment** | Final configuration | Sistem production-ready |

### 3.8.3 Konfigurasi Environment

**.env file:**
```env
# Database Configuration
DB_HOST=localhost
DB_PORT=5432
DB_NAME=db_absensi
DB_USER=absensi_user
DB_PASSWORD=your_password

# Server Configuration
HTTP_PORT=3000
MQTT_PORT=1883

# OwnCloud Configuration
OWNCLOUD_URL=http://localhost:8080/remote.php/dav/files/admin/
OWNCLOUD_USERNAME=admin
OWNCLOUD_PASSWORD=your_owncloud_password
```

---

## 3.9 Pengujian Sistem

### 3.9.1 Rencana Pengujian

| No | Jenis Pengujian | Aspek yang Diuji | Metode |
|----|-----------------|------------------|--------|
| 1 | **Unit Testing** | Fungsi individual | Manual testing |
| 2 | **Integration Testing** | Komunikasi antar komponen | End-to-end test |
| 3 | **Performance Testing** | Latency, throughput | Benchmark |
| 4 | **Security Testing** | CSRF, SQL Injection | Penetration test |
| 5 | **Usability Testing** | Kemudahan penggunaan | User acceptance |

### 3.9.2 Kriteria Keberhasilan

| Parameter | Target | Status |
|-----------|--------|--------|
| Latency MQTT | < 500ms | ☐ |
| Latency WebSocket | < 1s | ☐ |
| Packet Loss | < 1% | ☐ |
| Fingerprint Accuracy | > 95% | ☐ |
| System Availability | > 99% | ☐ |
| CSRF Protection | Blocked | ☐ |
| SQL Injection | Prevented | ☐ |

---

## Referensi Silang

- Bab 1: [Pendahuluan](bab1-pendahuluan.md)
- Bab 2: [Landasan Teori](bab2-landasan-teori.md)
- Bab 4: [Implementasi dan Pengujian](bab4-implementasi.md)
- Bab 5: [Kesimpulan dan Saran](bab5-kesimpulan.md)
- Diagram: [Topologi Jaringan](diagrams/topologi-jaringan.md)
- Panduan: [Pengujian Jaringan](panduan-pengujian-jaringan.md)
