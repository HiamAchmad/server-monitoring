# BAB 2
# LANDASAN TEORI

## 2.1 Sistem Monitoring dan Administrasi Kepegawaian

### 2.1.1 Definisi Sistem Monitoring Kepegawaian

Sistem monitoring kepegawaian adalah suatu sistem terintegrasi yang berfungsi untuk memantau, mengelola, dan mengadministrasikan seluruh aspek yang berkaitan dengan sumber daya manusia dalam suatu organisasi. Sistem ini tidak hanya mencakup pencatatan kehadiran, tetapi juga meliputi pengelolaan data pegawai, pemantauan kinerja, manajemen cuti, perhitungan remunerasi, dan pelaporan administratif.

Menurut Laudon & Laudon (2020), sistem informasi manajemen kepegawaian modern harus mampu mengintegrasikan berbagai fungsi administratif dalam satu platform yang terpadu, meliputi:

1. **Manajemen Data Pegawai** - Pengelolaan informasi dasar pegawai seperti identitas, jabatan, dan riwayat kepegawaian
2. **Monitoring Kehadiran** - Pemantauan real-time terhadap aktivitas kehadiran pegawai
3. **Administrasi Cuti dan Izin** - Pengelolaan pengajuan, persetujuan, dan pencatatan cuti
4. **Manajemen Remunerasi** - Perhitungan gaji, tunjangan, dan potongan berdasarkan data kehadiran
5. **Pelaporan dan Analitik** - Penyediaan laporan untuk pengambilan keputusan manajemen

### 2.1.2 Komponen Administrasi Kepegawaian

Administrasi kepegawaian mencakup serangkaian proses yang saling terkait:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    KOMPONEN ADMINISTRASI KEPEGAWAIAN                        │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌───────────────┐    ┌───────────────┐    ┌───────────────┐              │
│   │  DATA MASTER  │    │  KEHADIRAN    │    │     CUTI      │              │
│   │   PEGAWAI     │    │  & ABSENSI    │    │   & IZIN      │              │
│   │               │    │               │    │               │              │
│   │ • NIP         │    │ • Jam Masuk   │    │ • Jenis Cuti  │              │
│   │ • Nama        │    │ • Jam Keluar  │    │ • Saldo Cuti  │              │
│   │ • Jabatan     │    │ • Status      │    │ • Pengajuan   │              │
│   │ • Departemen  │    │ • Lembur      │    │ • Persetujuan │              │
│   │ • Golongan    │    │ • Keterlambatan│   │ • Riwayat     │              │
│   └───────┬───────┘    └───────┬───────┘    └───────┬───────┘              │
│           │                    │                    │                       │
│           └────────────────────┼────────────────────┘                       │
│                                │                                            │
│                       ┌────────▼────────┐                                   │
│                       │    PAYROLL      │                                   │
│                       │  (Penggajian)   │                                   │
│                       │                 │                                   │
│                       │ • Gaji Pokok    │                                   │
│                       │ • Tunjangan     │                                   │
│                       │ • Potongan      │                                   │
│                       │ • Slip Gaji     │                                   │
│                       └────────┬────────┘                                   │
│                                │                                            │
│                       ┌────────▼────────┐                                   │
│                       │    LAPORAN      │                                   │
│                       │  & MONITORING   │                                   │
│                       │                 │                                   │
│                       │ • Rekap Absensi │                                   │
│                       │ • Statistik     │                                   │
│                       │ • Dashboard     │                                   │
│                       │ • Export Data   │                                   │
│                       └─────────────────┘                                   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.1.3 Integrasi IoT dalam Sistem Monitoring Kepegawaian

Penerapan teknologi Internet of Things (IoT) dalam sistem monitoring kepegawaian memberikan keunggulan signifikan dalam hal akurasi, efisiensi, dan keandalan data. Perangkat IoT seperti sensor biometrik yang terhubung ke jaringan memungkinkan:

1. **Otomatisasi Pengumpulan Data** - Data kehadiran dikumpulkan secara otomatis tanpa intervensi manual
2. **Real-time Monitoring** - Status kehadiran dapat dipantau secara langsung dari dashboard
3. **Validasi Identitas** - Penggunaan biometrik mencegah kecurangan absensi (buddy punching)
4. **Integrasi Sistem** - Data dari perangkat IoT langsung terintegrasi dengan sistem administrasi

---

## 2.2 Internet of Things (IoT)

### 2.2.1 Definisi dan Konsep IoT

Internet of Things (IoT) adalah konsep dimana objek fisik (things) dilengkapi dengan sensor, perangkat lunak, dan teknologi lainnya untuk terhubung dan bertukar data dengan perangkat dan sistem lain melalui internet (Ashton, 2009). Dalam konteks sistem monitoring kepegawaian, IoT memungkinkan perangkat sensor sidik jari untuk berkomunikasi langsung dengan server pusat.

### 2.2.2 Arsitektur IoT

Arsitektur IoT umumnya terdiri dari empat layer utama:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         ARSITEKTUR IoT 4-LAYER                              │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│                           APPLICATION LAYER                                  │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  Dashboard Monitoring │ API Services │ Business Logic │ Data Analytics ││
│  └─────────────────────────────────────────────────────────────────────────┘│
│                                    ▲                                         │
└────────────────────────────────────┼─────────────────────────────────────────┘
                                     │
┌────────────────────────────────────┼─────────────────────────────────────────┐
│                           PROCESSING LAYER                                   │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  Database │ Message Broker │ Data Processing │ Cloud Storage            ││
│  └─────────────────────────────────────────────────────────────────────────┘│
│                                    ▲                                         │
└────────────────────────────────────┼─────────────────────────────────────────┘
                                     │
┌────────────────────────────────────┼─────────────────────────────────────────┐
│                           NETWORK LAYER                                      │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  WiFi 802.11 │ MQTT Protocol │ TCP/IP │ HTTP/WebSocket                  ││
│  └─────────────────────────────────────────────────────────────────────────┘│
│                                    ▲                                         │
└────────────────────────────────────┼─────────────────────────────────────────┘
                                     │
┌────────────────────────────────────┼─────────────────────────────────────────┐
│                           PERCEPTION LAYER                                   │
│  ┌─────────────────────────────────────────────────────────────────────────┐│
│  │  ESP32 Microcontroller │ Fingerprint Sensor │ OLED Display │ LED RGB    ││
│  └─────────────────────────────────────────────────────────────────────────┘│
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘
```

**Penjelasan setiap layer:**

| Layer | Fungsi | Komponen dalam Sistem |
|-------|--------|----------------------|
| **Perception Layer** | Mengumpulkan data dari lingkungan fisik | ESP32, Sensor Fingerprint AS608, OLED |
| **Network Layer** | Mentransmisikan data antar perangkat | WiFi, MQTT, TCP/IP |
| **Processing Layer** | Memproses dan menyimpan data | Server Node.js, PostgreSQL, MQTT Broker |
| **Application Layer** | Menyajikan informasi kepada pengguna | Dashboard, API, Laporan |

### 2.2.3 Karakteristik Sistem IoT

Sistem IoT yang baik harus memiliki karakteristik berikut:

1. **Connectivity** - Kemampuan perangkat untuk terhubung ke jaringan
2. **Sensing** - Kemampuan mengumpulkan data dari lingkungan
3. **Intelligence** - Kemampuan memproses data secara lokal atau remote
4. **Actuation** - Kemampuan memberikan respons (feedback visual/audio)
5. **Security** - Perlindungan terhadap akses tidak sah

---

## 2.3 Mikrokontroler ESP32

### 2.3.1 Spesifikasi ESP32

ESP32 adalah mikrokontroler dengan kemampuan WiFi dan Bluetooth terintegrasi yang diproduksi oleh Espressif Systems. Mikrokontroler ini sangat populer untuk aplikasi IoT karena performa tinggi dan konsumsi daya yang rendah.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           ESP32 DevKit V1                                    │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │                         BLOCK DIAGRAM                               │   │
│   │                                                                     │   │
│   │    ┌──────────────────────────────────────────────────────────┐    │   │
│   │    │                    ESP32 WROOM-32                        │    │   │
│   │    │  ┌────────────┐  ┌────────────┐  ┌────────────────────┐  │    │   │
│   │    │  │ Xtensa LX6 │  │ Xtensa LX6 │  │   SRAM 520KB       │  │    │   │
│   │    │  │  Core 0    │  │  Core 1    │  │   Flash 4MB        │  │    │   │
│   │    │  │  240MHz    │  │  240MHz    │  │                    │  │    │   │
│   │    │  └────────────┘  └────────────┘  └────────────────────┘  │    │   │
│   │    │                                                          │    │   │
│   │    │  ┌────────────┐  ┌────────────┐  ┌────────────────────┐  │    │   │
│   │    │  │   WiFi     │  │ Bluetooth  │  │      GPIO          │  │    │   │
│   │    │  │ 802.11bgn  │  │  v4.2 BLE  │  │    34 Pins         │  │    │   │
│   │    │  │  2.4GHz    │  │            │  │  UART/SPI/I2C      │  │    │   │
│   │    │  └────────────┘  └────────────┘  └────────────────────┘  │    │   │
│   │    └──────────────────────────────────────────────────────────┘    │   │
│   │                                                                     │   │
│   └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Spesifikasi Teknis ESP32:**

| Parameter | Spesifikasi |
|-----------|-------------|
| CPU | Dual-core Xtensa LX6 @ 240 MHz |
| SRAM | 520 KB |
| Flash | 4 MB (external) |
| WiFi | 802.11 b/g/n, 2.4 GHz |
| Bluetooth | v4.2 BR/EDR dan BLE |
| GPIO | 34 pin programmable |
| UART | 3 port (UART0, UART1, UART2) |
| SPI | 4 interface |
| I2C | 2 interface |
| ADC | 12-bit, 18 channel |
| Operating Voltage | 3.3V |
| Operating Current | 80mA (average) |

### 2.3.2 Antarmuka UART pada ESP32

Universal Asynchronous Receiver/Transmitter (UART) adalah protokol komunikasi serial yang digunakan untuk berkomunikasi dengan sensor fingerprint. ESP32 memiliki 3 port UART:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         ESP32 UART CONFIGURATION                             │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌─────────────┐         ┌─────────────┐         ┌─────────────┐          │
│   │   UART0     │         │   UART1     │         │   UART2     │          │
│   │             │         │             │         │             │          │
│   │ TX: GPIO1   │         │ TX: GPIO10  │         │ TX: GPIO17  │◄── Used  │
│   │ RX: GPIO3   │         │ RX: GPIO9   │         │ RX: GPIO16  │◄── Used  │
│   │             │         │             │         │             │          │
│   │ (USB Debug) │         │ (Flash)     │         │ (Sensor)    │          │
│   └─────────────┘         └─────────────┘         └─────────────┘          │
│                                                                             │
│   Konfigurasi UART2 untuk Fingerprint Sensor:                              │
│   • Baud Rate: 57600 bps                                                   │
│   • Data Bits: 8                                                           │
│   • Stop Bits: 1                                                           │
│   • Parity: None (8N1)                                                     │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

### 2.3.3 WiFi pada ESP32

ESP32 mendukung standar WiFi 802.11 b/g/n pada frekuensi 2.4 GHz. Dalam mode Station (STA), ESP32 dapat terhubung ke Access Point dan berkomunikasi dengan server melalui protokol TCP/IP.

**Parameter WiFi:**

| Parameter | Nilai | Keterangan |
|-----------|-------|------------|
| Standard | 802.11 b/g/n | Kompatibel dengan router modern |
| Frequency | 2.4 GHz | Single band |
| Security | WPA/WPA2/WPA3 | Mendukung enkripsi |
| Max Data Rate | 150 Mbps | 802.11n mode |
| Range | 50-100 meter | Tergantung lingkungan |

---

## 2.4 Sensor Fingerprint

### 2.4.1 Sensor Fingerprint AS608/R307

Sensor fingerprint AS608 (juga dikenal sebagai R307) adalah sensor biometrik optik yang digunakan untuk menangkap dan memproses gambar sidik jari. Sensor ini memiliki processor DSP internal yang mampu melakukan proses enrollment dan matching secara mandiri.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      FINGERPRINT SENSOR AS608/R307                           │
├─────────────────────────────────────────────────────────────────────────────┤
│                                                                             │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │                                                                     │   │
│   │   ┌─────────────┐    ┌─────────────────┐    ┌─────────────────┐    │   │
│   │   │   OPTICAL   │    │      DSP        │    │    FLASH        │    │   │
│   │   │   SENSOR    │───►│   PROCESSOR     │◄──►│   MEMORY        │    │   │
│   │   │   500 DPI   │    │   Matching      │    │   127 Templates │    │   │
│   │   └─────────────┘    └────────┬────────┘    └─────────────────┘    │   │
│   │                               │                                     │   │
│   │                        ┌──────▼──────┐                              │   │
│   │                        │    UART     │                              │   │
│   │                        │  Interface  │                              │   │
│   │                        │   57600bps  │                              │   │
│   │                        └─────────────┘                              │   │
│   │                                                                     │   │
│   └─────────────────────────────────────────────────────────────────────┘   │
│                                                                             │
│   Pinout:                                                                   │
│   ┌────────┬────────────────────────────────────────────────────────────┐   │
│   │  Pin   │  Fungsi                                                    │   │
│   ├────────┼────────────────────────────────────────────────────────────┤   │
│   │  VCC   │  Power Supply (3.3V - 5V)                                  │   │
│   │  GND   │  Ground                                                    │   │
│   │  TX    │  Serial Transmit (ke RX mikrokontroler)                    │   │
│   │  RX    │  Serial Receive (dari TX mikrokontroler)                   │   │
│   └────────┴────────────────────────────────────────────────────────────┘   │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Spesifikasi Teknis AS608:**

| Parameter | Spesifikasi |
|-----------|-------------|
| Tipe Sensor | Optical |
| Resolusi | 500 DPI |
| Kapasitas Template | 127 fingerprints |
| Waktu Pengenalan | < 1 detik |
| False Acceptance Rate (FAR) | < 0.001% |
| False Rejection Rate (FRR) | < 0.1% |
| Baud Rate | 9600 - 115200 (default 57600) |
| Tegangan Operasi | 3.3V - 6V DC |
| Arus Operasi | < 120mA |
| Dimensi Jendela | 18mm x 22mm |

### 2.4.2 Proses Pengenalan Sidik Jari

Proses pengenalan sidik jari terdiri dari dua tahap utama: **Enrollment** dan **Matching**.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    PROSES ENROLLMENT (PENDAFTARAN)                           │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐
    │ CAPTURE │    │ CAPTURE │    │ CREATE  │    │ STORE   │    │ SUCCESS │
    │ IMAGE 1 │───►│ IMAGE 2 │───►│ MODEL   │───►│TEMPLATE │───►│   ID    │
    │         │    │         │    │         │    │         │    │         │
    │ Scan 1x │    │ Scan 2x │    │ Combine │    │ Flash   │    │ Return  │
    └─────────┘    └─────────┘    └─────────┘    └─────────┘    └─────────┘


┌─────────────────────────────────────────────────────────────────────────────┐
│                    PROSES MATCHING (PENCOCOKAN)                              │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌─────────┐    ┌─────────┐    ┌─────────┐    ┌─────────┐
    │ CAPTURE │    │ CONVERT │    │ SEARCH  │    │ RESULT  │
    │  IMAGE  │───►│   TO    │───►│DATABASE │───►│ ID +    │
    │         │    │ TEMPLATE│    │         │    │CONFIDENCE│
    │ Scan 1x │    │ img2Tz  │    │ 1:N     │    │         │
    └─────────┘    └─────────┘    └─────────┘    └─────────┘
```

### 2.4.3 Keamanan Biometrik

Sidik jari merupakan salah satu metode biometrik yang paling banyak digunakan karena:

1. **Uniqueness** - Setiap sidik jari bersifat unik, bahkan pada kembar identik
2. **Permanence** - Pola sidik jari tidak berubah sepanjang hidup
3. **Collectability** - Mudah diambil dengan sensor optik atau kapasitif
4. **Performance** - Akurasi tinggi dengan FAR dan FRR yang rendah
5. **Acceptability** - Diterima secara luas oleh masyarakat

---

## 2.5 Protokol MQTT (Message Queuing Telemetry Transport)

### 2.5.1 Definisi dan Karakteristik MQTT

MQTT adalah protokol messaging berbasis publish/subscribe yang dirancang untuk komunikasi machine-to-machine (M2M) dan Internet of Things (IoT). Protokol ini dikembangkan oleh IBM pada tahun 1999 dan menjadi standar ISO (ISO/IEC 20922:2016).

**Karakteristik MQTT:**

| Karakteristik | Penjelasan |
|---------------|------------|
| **Lightweight** | Header minimal 2 bytes, cocok untuk bandwidth terbatas |
| **Publish/Subscribe** | Arsitektur loosely coupled, publisher tidak perlu tahu subscriber |
| **Quality of Service** | Tiga level QoS (0, 1, 2) untuk keandalan berbeda |
| **Retained Messages** | Broker menyimpan pesan terakhir untuk subscriber baru |
| **Last Will and Testament** | Notifikasi otomatis jika client disconnect tidak normal |
| **Keep Alive** | Mekanisme heartbeat untuk deteksi koneksi |

### 2.5.2 Arsitektur Publish/Subscribe

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                    MQTT PUBLISH/SUBSCRIBE ARCHITECTURE                       │
└─────────────────────────────────────────────────────────────────────────────┘

                              ┌─────────────────────┐
                              │     MQTT BROKER     │
                              │      (Aedes)        │
                              │     Port: 1883      │
                              └──────────┬──────────┘
                                         │
              ┌──────────────────────────┼──────────────────────────┐
              │                          │                          │
              ▼                          ▼                          ▼
    ┌─────────────────┐        ┌─────────────────┐        ┌─────────────────┐
    │   PUBLISHER     │        │   SUBSCRIBER    │        │ PUB + SUB       │
    │   (ESP32)       │        │   (Server)      │        │ (Server)        │
    │                 │        │                 │        │                 │
    │ PUBLISH:        │        │ SUBSCRIBE:      │        │ PUB: enroll/req │
    │ • attendance    │        │ • attendance    │        │ SUB: enroll/res │
    │ • enroll/resp   │        │ • enroll/+      │        │                 │
    └─────────────────┘        └─────────────────┘        └─────────────────┘


┌─────────────────────────────────────────────────────────────────────────────┐
│                         MESSAGE FLOW EXAMPLE                                 │
└─────────────────────────────────────────────────────────────────────────────┘

    ESP32                          BROKER                         SERVER
      │                              │                              │
      │──── CONNECT ────────────────►│                              │
      │◄─── CONNACK ─────────────────│                              │
      │                              │                              │
      │                              │◄──── SUBSCRIBE ──────────────│
      │                              │      fingerprint/#           │
      │                              │───── SUBACK ────────────────►│
      │                              │                              │
      │──── PUBLISH ────────────────►│                              │
      │     fingerprint/attendance   │───── PUBLISH ───────────────►│
      │     {fingerprint_id: 5}      │      fingerprint/attendance  │
      │                              │      {fingerprint_id: 5}     │
      │                              │                              │
```

### 2.5.3 MQTT Quality of Service (QoS)

MQTT menyediakan tiga level Quality of Service:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                           MQTT QoS LEVELS                                    │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│  QoS 0 - AT MOST ONCE (Fire and Forget)                                     │
│  ═══════════════════════════════════════                                     │
│                                                                              │
│     Publisher ──────── PUBLISH ────────────────────────────► Broker         │
│                                                                              │
│  • Tidak ada acknowledgment                                                  │
│  • Pesan mungkin hilang                                                      │
│  • Paling cepat, overhead minimal                                            │
│  • Cocok untuk: sensor data yang sering diupdate, progress notification     │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│  QoS 1 - AT LEAST ONCE (Acknowledged Delivery)                               │
│  ═══════════════════════════════════════════════                             │
│                                                                              │
│     Publisher ──────── PUBLISH ────────────────────────────► Broker         │
│     Publisher ◄─────── PUBACK ─────────────────────────────  Broker         │
│                                                                              │
│  • Broker mengirim acknowledgment                                            │
│  • Pesan pasti terkirim, mungkin duplikat                                    │
│  • Trade-off antara kecepatan dan keandalan                                  │
│  • Cocok untuk: data absensi, enrollment response                           │
└─────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────┐
│  QoS 2 - EXACTLY ONCE (Assured Delivery)                                     │
│  ═══════════════════════════════════════                                     │
│                                                                              │
│     Publisher ──────── PUBLISH ────────────────────────────► Broker         │
│     Publisher ◄─────── PUBREC ─────────────────────────────  Broker         │
│     Publisher ──────── PUBREL ────────────────────────────► Broker         │
│     Publisher ◄─────── PUBCOMP ────────────────────────────  Broker         │
│                                                                              │
│  • 4-way handshake untuk memastikan exactly once delivery                   │
│  • Tidak ada duplikasi, tidak ada kehilangan                                 │
│  • Paling lambat, overhead tertinggi                                         │
│  • Cocok untuk: transaksi finansial, data kritikal                          │
└─────────────────────────────────────────────────────────────────────────────┘
```

**Perbandingan QoS Level:**

| Aspek | QoS 0 | QoS 1 | QoS 2 |
|-------|-------|-------|-------|
| Delivery Guarantee | At most once | At least once | Exactly once |
| Possible Duplicates | No | Yes | No |
| Message Loss | Possible | No | No |
| Overhead | Minimal | Low | High |
| Performance | Fastest | Medium | Slowest |
| Use Case | Non-critical data | Important data | Critical data |

### 2.5.4 MQTT Topic Structure

Topic dalam MQTT menggunakan struktur hierarki yang dipisahkan dengan tanda slash (/):

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         MQTT TOPIC HIERARCHY                                 │
└─────────────────────────────────────────────────────────────────────────────┘

                            fingerprint
                                 │
            ┌────────────────────┼────────────────────┐
            │                    │                    │
        attendance            enroll              command
            │                    │
            │         ┌──────────┼──────────┐
            │         │          │          │
            │      request   response   progress


Topic Examples:
• fingerprint/attendance         → Data absensi
• fingerprint/enroll/request     → Perintah enrollment
• fingerprint/enroll/response    → Hasil enrollment
• fingerprint/enroll/progress    → Progress enrollment
• fingerprint/command            → Perintah umum

Wildcard Subscription:
• fingerprint/#                  → Subscribe ke semua topic fingerprint
• fingerprint/enroll/+           → Subscribe ke semua sub-topic enroll
```

### 2.5.5 MQTT Broker

MQTT Broker adalah komponen sentral yang menerima semua pesan dari publisher dan mendistribusikannya ke subscriber yang sesuai. Dalam sistem ini digunakan **Aedes**, sebuah MQTT broker yang berjalan di atas Node.js.

**Keunggulan Aedes:**

1. **Lightweight** - Dapat berjalan dalam proses yang sama dengan aplikasi Node.js
2. **Embedded** - Tidak memerlukan instalasi terpisah
3. **Programmable** - Dapat dikonfigurasi dan diperluas dengan JavaScript
4. **Event-driven** - Mendukung event handling untuk monitoring

---

## 2.6 Jaringan Komputer untuk IoT

### 2.6.1 Model TCP/IP

Komunikasi dalam sistem IoT menggunakan stack protokol TCP/IP. Berikut adalah layer yang terlibat:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         TCP/IP PROTOCOL STACK                                │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────────────────────────────────────────────────────────┐
    │                      APPLICATION LAYER                               │
    │  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐  ┌─────────┐   │
    │  │  HTTP   │  │  MQTT   │  │ WebSocket│  │ WebDAV  │  │   DNS   │   │
    │  │ :3000   │  │ :1883   │  │  :3000  │  │  :8080  │  │  :53    │   │
    │  └─────────┘  └─────────┘  └─────────┘  └─────────┘  └─────────┘   │
    └───────────────────────────────┬─────────────────────────────────────┘
                                    │
    ┌───────────────────────────────┼─────────────────────────────────────┐
    │                      TRANSPORT LAYER                                 │
    │  ┌─────────────────────────────────────────────────────────────┐    │
    │  │                         TCP                                  │    │
    │  │  • Connection-oriented    • Reliable delivery                │    │
    │  │  • Flow control           • Error checking                   │    │
    │  └─────────────────────────────────────────────────────────────┘    │
    └───────────────────────────────┬─────────────────────────────────────┘
                                    │
    ┌───────────────────────────────┼─────────────────────────────────────┐
    │                      INTERNET LAYER                                  │
    │  ┌─────────────────────────────────────────────────────────────┐    │
    │  │                         IP                                   │    │
    │  │  • Addressing (IPv4)      • Routing                          │    │
    │  │  • Fragmentation          • Best-effort delivery             │    │
    │  └─────────────────────────────────────────────────────────────┘    │
    └───────────────────────────────┬─────────────────────────────────────┘
                                    │
    ┌───────────────────────────────┼─────────────────────────────────────┐
    │                      NETWORK ACCESS LAYER                            │
    │  ┌─────────────────────────────────────────────────────────────┐    │
    │  │              WiFi 802.11 / Ethernet                          │    │
    │  │  • MAC Addressing         • Frame transmission               │    │
    │  │  • Error detection        • Physical signaling               │    │
    │  └─────────────────────────────────────────────────────────────┘    │
    └─────────────────────────────────────────────────────────────────────┘
```

### 2.6.2 Topologi Jaringan

Sistem monitoring kepegawaian IoT menggunakan topologi **Star** dengan server sebagai central node:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         NETWORK TOPOLOGY                                     │
└─────────────────────────────────────────────────────────────────────────────┘

                              ┌─────────────┐
                              │   ROUTER    │
                              │  (WiFi AP)  │
                              │  2.4 GHz    │
                              └──────┬──────┘
                                     │
           ┌─────────────────────────┼─────────────────────────┐
           │                         │                         │
           ▼                         ▼                         ▼
    ┌─────────────┐           ┌─────────────┐           ┌─────────────┐
    │   ESP32     │           │   SERVER    │           │   CLIENT    │
    │ + Sensor    │           │  (Node.js)  │           │  (Browser)  │
    │ + OLED      │           │             │           │             │
    │             │           │  • HTTP     │           │  Dashboard  │
    │  WiFi STA   │           │  • MQTT     │           │  Monitoring │
    │             │           │  • Socket   │           │             │
    └─────────────┘           └──────┬──────┘           └─────────────┘
                                     │
                              ┌──────┴──────┐
                              │             │
                              ▼             ▼
                       ┌───────────┐  ┌───────────┐
                       │PostgreSQL │  │ OwnCloud  │
                       │  :5432    │  │  :8080    │
                       └───────────┘  └───────────┘
```

### 2.6.3 Quality of Service (QoS) Jaringan

Parameter QoS yang penting dalam sistem monitoring IoT:

| Parameter | Definisi | Target | Cara Pengukuran |
|-----------|----------|--------|-----------------|
| **Latency** | Waktu tempuh paket dari sumber ke tujuan | < 500ms | Timestamp comparison |
| **Throughput** | Jumlah data yang berhasil ditransfer per satuan waktu | Stabil | Bytes/second |
| **Packet Loss** | Persentase paket yang hilang | < 1% | Packet counter |
| **Jitter** | Variasi delay antar paket | < 50ms | Delay variance |
| **Availability** | Persentase waktu sistem dapat diakses | > 99% | Uptime monitoring |

---

## 2.7 Protokol Pendukung

### 2.7.1 WebSocket (Socket.IO)

WebSocket adalah protokol komunikasi yang menyediakan kanal komunikasi full-duplex melalui koneksi TCP tunggal. Socket.IO adalah library yang mengimplementasikan WebSocket dengan fallback mechanism.

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                      WEBSOCKET COMMUNICATION                                 │
└─────────────────────────────────────────────────────────────────────────────┘

    Traditional HTTP:
    ┌────────┐                              ┌────────┐
    │ Client │ ───── Request ──────────────►│ Server │
    │        │ ◄──── Response ──────────────│        │
    │        │                              │        │
    │        │ ───── Request ──────────────►│        │
    │        │ ◄──── Response ──────────────│        │
    └────────┘                              └────────┘
    (Half-duplex, request-response pattern)


    WebSocket:
    ┌────────┐                              ┌────────┐
    │ Client │ ═════ HTTP Upgrade ═════════►│ Server │
    │        │ ◄════ 101 Switching ═════════│        │
    │        │                              │        │
    │        │ ◄═══════════════════════════►│        │
    │        │      Bidirectional           │        │
    │        │      Full-duplex             │        │
    │        │      Real-time               │        │
    └────────┘                              └────────┘
    (Full-duplex, event-driven communication)
```

**Keunggulan WebSocket untuk Real-time Dashboard:**

1. **Low Latency** - Tidak ada overhead HTTP request/response
2. **Bidirectional** - Server dapat push data ke client tanpa request
3. **Persistent Connection** - Koneksi tetap terbuka selama sesi
4. **Event-driven** - Model pemrograman yang intuitif

### 2.7.2 HTTP/REST API

REST (Representational State Transfer) adalah gaya arsitektur untuk membangun web services. Digunakan untuk operasi CRUD pada sistem administrasi kepegawaian.

| Method | Endpoint | Fungsi |
|--------|----------|--------|
| GET | /api/pegawai | Mengambil daftar pegawai |
| POST | /api/pegawai | Menambah pegawai baru |
| PUT | /api/pegawai/:id | Mengupdate data pegawai |
| DELETE | /api/pegawai/:id | Menghapus pegawai |
| GET | /api/absensi | Mengambil data absensi |
| POST | /api/login | Autentikasi pengguna |

### 2.7.3 WebDAV Protocol

Web Distributed Authoring and Versioning (WebDAV) adalah ekstensi HTTP yang memungkinkan operasi file pada remote server. Digunakan untuk backup data ke cloud storage (OwnCloud).

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         WebDAV OPERATIONS                                    │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌────────────────────────────────────────────────────────────────────────┐
    │  Method     │  HTTP     │  Fungsi                                      │
    ├─────────────┼───────────┼──────────────────────────────────────────────┤
    │  PROPFIND   │  -        │  Get properties of resource (list folder)   │
    │  MKCOL      │  -        │  Create collection (folder)                 │
    │  GET        │  GET      │  Download file                              │
    │  PUT        │  PUT      │  Upload file                                │
    │  DELETE     │  DELETE   │  Delete resource                            │
    │  COPY       │  -        │  Copy resource                              │
    │  MOVE       │  -        │  Move resource                              │
    └─────────────┴───────────┴──────────────────────────────────────────────┘
```

---

## 2.8 Keamanan Sistem

### 2.8.1 Aspek Keamanan IoT

Keamanan dalam sistem IoT mencakup beberapa aspek:

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         SECURITY CONCERNS IN IoT                             │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────────────────────────────────────────────────────────┐
    │                      DEVICE SECURITY                                 │
    │  • Firmware integrity          • Physical tampering protection      │
    │  • Secure boot                 • Hardware security module           │
    └─────────────────────────────────────────────────────────────────────┘
                                    │
    ┌─────────────────────────────────────────────────────────────────────┐
    │                      NETWORK SECURITY                                │
    │  • Encryption (TLS/SSL)        • Firewall configuration             │
    │  • VPN for remote access       • Network segmentation               │
    └─────────────────────────────────────────────────────────────────────┘
                                    │
    ┌─────────────────────────────────────────────────────────────────────┐
    │                      APPLICATION SECURITY                            │
    │  • Authentication (RBAC)       • Input validation                   │
    │  • CSRF protection             • SQL injection prevention           │
    │  • Session management          • XSS prevention                     │
    └─────────────────────────────────────────────────────────────────────┘
                                    │
    ┌─────────────────────────────────────────────────────────────────────┐
    │                      DATA SECURITY                                   │
    │  • Password hashing (bcrypt)   • Environment variables              │
    │  • Database access control     • Backup encryption                  │
    └─────────────────────────────────────────────────────────────────────┘
```

### 2.8.2 CSRF Protection

Cross-Site Request Forgery (CSRF) adalah serangan yang memaksa pengguna untuk melakukan aksi yang tidak diinginkan pada aplikasi web dimana mereka sudah terautentikasi. Proteksi dilakukan dengan memvalidasi Origin dan Referer header.

### 2.8.3 Role-Based Access Control (RBAC)

RBAC membatasi akses sistem berdasarkan peran pengguna:

| Role | Permission | Akses Menu |
|------|------------|------------|
| **Admin** | Full CRUD, Approve, Backup | Semua menu |
| **User** | Read own, Create request | Dashboard personal, Pengajuan |

---

## 2.9 Platform Pengembangan

### 2.9.1 PlatformIO

PlatformIO adalah ecosystem open-source untuk pengembangan embedded system yang terintegrasi dengan VSCode. Keunggulan dibanding Arduino IDE:

| Fitur | PlatformIO | Arduino IDE |
|-------|------------|-------------|
| Library Management | Otomatis per-project | Global, manual |
| Multi-board Support | Built-in | Manual config |
| IntelliSense | Full support | Limited |
| Debugging | Integrated | Tidak ada |
| Version Control | Git-friendly | Kurang friendly |
| CI/CD | Supported | Tidak ada |

**Struktur Project PlatformIO:**

```
esp32/
├── platformio.ini          # Konfigurasi project
├── src/
│   └── esp32_fingerprint.ino   # Source code utama
├── lib/                    # Library lokal
├── include/                # Header files
└── .pio/                   # Build output
```

### 2.9.2 Node.js Runtime

Node.js adalah JavaScript runtime yang memungkinkan eksekusi JavaScript di sisi server. Dipilih karena:

1. **Event-driven** - Cocok untuk aplikasi real-time
2. **Non-blocking I/O** - Efisien untuk concurrent connections
3. **NPM Ecosystem** - Package manager terbesar
4. **Full-stack JavaScript** - Satu bahasa untuk frontend dan backend

---

## 2.10 Database PostgreSQL

### 2.10.1 Pengertian PostgreSQL

PostgreSQL adalah sistem manajemen basis data relasional (RDBMS) open-source yang terkenal dengan keandalan, integritas data, dan fitur-fitur canggihnya. PostgreSQL mendukung standar SQL dan menyediakan banyak fitur tambahan seperti stored procedures, triggers, dan tipe data kompleks.

**Keunggulan PostgreSQL:**

| Fitur | Keterangan |
|-------|------------|
| **ACID Compliant** | Atomicity, Consistency, Isolation, Durability |
| **MVCC** | Multi-Version Concurrency Control untuk performa tinggi |
| **Extensible** | Mendukung custom types, functions, operators |
| **JSON Support** | Native JSON dan JSONB untuk data semi-structured |
| **Full-text Search** | Built-in full-text search capability |
| **Replication** | Streaming replication untuk high availability |

### 2.10.2 Arsitektur Database Sistem

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         DATABASE ARCHITECTURE                                │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────────────────────────────────────────────────────────────┐
    │                         PostgreSQL Server                                │
    │                           Port: 5432                                     │
    │  ┌───────────────────────────────────────────────────────────────────┐  │
    │  │                      Database: db_absensi                         │  │
    │  │                                                                   │  │
    │  │   ┌─────────────┐   ┌─────────────┐   ┌─────────────────────┐    │  │
    │  │   │   users     │   │  pegawai    │   │      absensi        │    │  │
    │  │   │             │   │             │   │                     │    │  │
    │  │   │ • id        │   │ • id        │   │ • id                │    │  │
    │  │   │ • username  │   │ • nip       │   │ • pegawai_id (FK)   │    │  │
    │  │   │ • password  │   │ • nama      │   │ • tanggal           │    │  │
    │  │   │ • role      │   │ • jabatan   │   │ • jam_masuk         │    │  │
    │  │   │             │   │ • fp_id     │   │ • jam_keluar        │    │  │
    │  │   └─────────────┘   └──────┬──────┘   │ • status            │    │  │
    │  │                            │          │ • fingerprint_id    │    │  │
    │  │                            └──────────┼─────────────────────┘    │  │
    │  │                                       │                          │  │
    │  │   ┌─────────────┐   ┌─────────────┐   │   ┌─────────────────┐    │  │
    │  │   │    cuti     │   │   payroll   │◄──┘   │     settings    │    │  │
    │  │   │             │   │             │       │                 │    │  │
    │  │   │ • id        │   │ • id        │       │ • key           │    │  │
    │  │   │ • pegawai_id│   │ • pegawai_id│       │ • value         │    │  │
    │  │   │ • tgl_mulai │   │ • bulan     │       │                 │    │  │
    │  │   │ • tgl_selesai│  │ • gaji_pokok│       │                 │    │  │
    │  │   │ • status    │   │ • tunjangan │       │                 │    │  │
    │  │   │ • keterangan│   │ • potongan  │       │                 │    │  │
    │  │   └─────────────┘   └─────────────┘       └─────────────────┘    │  │
    │  │                                                                   │  │
    │  └───────────────────────────────────────────────────────────────────┘  │
    │                                                                         │
    └─────────────────────────────────────────────────────────────────────────┘
```

### 2.10.3 Entity Relationship Diagram (ERD)

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              ERD SISTEM                                      │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌──────────────┐          ┌──────────────┐          ┌──────────────┐
    │    USERS     │          │   PEGAWAI    │          │   ABSENSI    │
    ├──────────────┤          ├──────────────┤          ├──────────────┤
    │ PK id        │          │ PK id        │◄────────┐│ PK id        │
    │    username  │          │    nip       │         ││ FK pegawai_id│────┐
    │    password  │          │    nama      │         │└──────────────┘    │
    │    role      │          │    jabatan   │         │                    │
    │    created_at│          │    departemen│         │                    │
    └──────────────┘          │    gaji_pokok│         │                    │
                              │    fp_id     │─────────┼────────────────────┤
                              │    created_at│         │                    │
                              └──────────────┘         │                    │
                                     │                 │                    │
                                     │ 1               │                    │
                                     │                 │                    │
                    ┌────────────────┼────────────────┐│                    │
                    │                │                ││                    │
                    ▼ N              ▼ N              ▼│N                   │
            ┌──────────────┐  ┌──────────────┐  ┌──────────────┐           │
            │     CUTI     │  │   PAYROLL    │  │   ABSENSI    │◄──────────┘
            ├──────────────┤  ├──────────────┤  ├──────────────┤
            │ PK id        │  │ PK id        │  │    tanggal   │
            │ FK pegawai_id│  │ FK pegawai_id│  │    jam_masuk │
            │    jenis_cuti│  │    bulan     │  │    jam_keluar│
            │    tgl_mulai │  │    tahun     │  │    status    │
            │    tgl_akhir │  │    gaji_pokok│  │    keterangan│
            │    status    │  │    tunjangan │  │    fp_id     │
            │    keterangan│  │    potongan  │  └──────────────┘
            └──────────────┘  │    total     │
                              └──────────────┘

    Relasi:
    • PEGAWAI (1) ──────► (N) ABSENSI    : Satu pegawai memiliki banyak absensi
    • PEGAWAI (1) ──────► (N) CUTI       : Satu pegawai memiliki banyak cuti
    • PEGAWAI (1) ──────► (N) PAYROLL    : Satu pegawai memiliki banyak payroll
```

### 2.10.4 Skema Tabel Utama

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
| status | VARCHAR(20) | | HADIR/IZIN/CUTI/ALPHA |
| fingerprint_id | INTEGER | | ID dari sensor |
| keterangan | TEXT | | Catatan tambahan |

### 2.10.5 Koneksi Database

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         DATABASE CONNECTION FLOW                             │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────┐                              ┌─────────────────┐
    │   Node.js       │                              │   PostgreSQL    │
    │   Application   │                              │    Server       │
    └────────┬────────┘                              └────────┬────────┘
             │                                                │
             │  ══════ Connection Pool (pg module) ═════════► │
             │         host: localhost                        │
             │         port: 5432                             │
             │         database: db_absensi                   │
             │         user: absensi_user                     │
             │         password: ********                     │
             │                                                │
             │  ──────── SQL Query ─────────────────────────► │
             │           SELECT * FROM pegawai                │
             │                                                │
             │  ◄─────── Result Set ──────────────────────── │
             │           [{id:1, nip:'001', nama:'John'}]     │
             │                                                │
    └────────▼────────┘                              └────────▼────────┘

    Connection String:
    postgresql://absensi_user:password@localhost:5432/db_absensi
```

### 2.10.6 Keamanan Database

| Aspek | Implementasi |
|-------|--------------|
| **Authentication** | Username + Password |
| **Authorization** | Role-based privileges |
| **Encryption** | Password hashing (bcrypt) sebelum disimpan |
| **SQL Injection Prevention** | Parameterized queries |
| **Access Control** | Port 5432 hanya localhost |
| **Backup** | Export CSV ke OwnCloud |

---

## 2.11 Cloud Storage Integration

### 2.11.1 OwnCloud

OwnCloud adalah platform cloud storage self-hosted yang menyediakan fitur file synchronization dan sharing. Keunggulan menggunakan OwnCloud:

1. **Self-hosted** - Data tetap di server lokal
2. **WebDAV Support** - Protokol standar untuk akses file
3. **Docker Deployment** - Mudah di-deploy dan dikelola
4. **Open Source** - Bebas biaya lisensi

### 2.11.2 Arsitektur Backup

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         BACKUP ARCHITECTURE                                  │
└─────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────┐           ┌─────────────┐           ┌─────────────┐
    │  DATABASE   │           │   SERVER    │           │  OWNCLOUD   │
    │ PostgreSQL  │──Query───►│  Node.js    │──WebDAV──►│   Docker    │
    │             │           │             │   PUT     │             │
    │  Raw Data   │           │  CSV Export │           │  Backup     │
    └─────────────┘           └─────────────┘           └─────────────┘

    Backup File Format: Backup-Absensi-YYYY-MM-DD_HH-mm-ss.csv
```

---

## Referensi

1. Ashton, K. (2009). That 'Internet of Things' Thing. RFID Journal.
2. Espressif Systems. (2023). ESP32 Technical Reference Manual.
3. OASIS. (2019). MQTT Version 5.0 Specification.
4. Laudon, K. C., & Laudon, J. P. (2020). Management Information Systems: Managing the Digital Firm.
5. Banks, A., & Gupta, R. (2014). MQTT Version 3.1.1. OASIS Standard.
6. Fielding, R. T. (2000). Architectural Styles and the Design of Network-based Software Architectures. Doctoral dissertation.
7. The PostgreSQL Global Development Group. (2023). PostgreSQL 15 Documentation.

---

**Referensi Silang:**
- Bab 1: [Pendahuluan](bab1-pendahuluan.md)
- Bab 3: [Perancangan Sistem](bab3-diagrams.md)
- Bab 4: [Implementasi dan Pengujian](bab4-implementasi.md)
- Bab 5: [Kesimpulan dan Saran](bab5-kesimpulan.md)
