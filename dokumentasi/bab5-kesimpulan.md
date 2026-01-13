# BAB 5
# KESIMPULAN DAN SARAN

## 5.1 Kesimpulan

Berdasarkan hasil perancangan, implementasi, dan pengujian yang telah dilakukan pada sistem absensi IoT berbasis fingerprint dengan fokus pada aspek jaringan dan komunikasi data, dapat diambil kesimpulan sebagai berikut:

### 5.1.1 Kesimpulan Umum

Sistem Absensi IoT Berbasis Fingerprint telah berhasil dirancang dan diimplementasikan dengan menerapkan arsitektur jaringan client-server yang terintegrasi. Penelitian ini berhasil mengimplementasikan protokol komunikasi MQTT untuk transmisi data sensor, protokol WebSocket untuk real-time monitoring, serta protokol WebDAV untuk integrasi cloud storage. Arsitektur jaringan yang dibangun mampu menghubungkan perangkat IoT (ESP32) dengan server melalui jaringan lokal dan melakukan sinkronisasi data ke cloud secara efisien.

### 5.1.2 Kesimpulan Berdasarkan Aspek Jaringan

#### A. Arsitektur Jaringan

1. **Topologi Jaringan**
   - Berhasil mengimplementasikan topologi star dengan server sebagai central node
   - Perangkat ESP32 terhubung ke server melalui jaringan WiFi 2.4GHz
   - Arsitektur three-tier berhasil diterapkan: Device Layer → Network Layer → Application Layer

2. **Infrastruktur Jaringan**
   - Server berjalan pada port HTTP 3000 dan MQTT 1883
   - Database PostgreSQL berkomunikasi pada port 5432
   - OwnCloud cloud storage beroperasi pada port 8080
   - Seluruh layanan dapat di-deploy menggunakan Docker container untuk portabilitas

#### B. Protokol Komunikasi

1. **Protokol MQTT (Message Queuing Telemetry Transport)**
   - Berhasil mengimplementasikan MQTT broker menggunakan Aedes pada port 1883
   - Komunikasi publish/subscribe antara ESP32 dan server berjalan dengan baik
   - Topic struktur yang digunakan:
     - `fingerprint/attendance` - data absensi dari ESP32 ke server
     - `fingerprint/enroll/request` - perintah enrollment dari server ke ESP32
     - `fingerprint/enroll/response` - respon enrollment dari ESP32 ke server
     - `fingerprint/enroll/progress` - progress enrollment secara real-time
     - `fingerprint/command` - perintah umum ke ESP32
   - Payload menggunakan format JSON untuk efisiensi parsing

2. **Protokol WebSocket (Socket.IO)**
   - Implementasi real-time bidirectional communication berhasil dilakukan
   - Event-driven architecture untuk push notification ke client
   - Latency komunikasi < 1 detik dari scan hingga update dashboard

3. **Protokol HTTP/REST**
   - RESTful API untuk komunikasi client-server pada aplikasi web
   - Mendukung method GET, POST, PUT, DELETE sesuai standar REST

4. **Protokol WebDAV**
   - Integrasi dengan OwnCloud menggunakan WebDAV protocol
   - Upload file backup melalui HTTP PUT method
   - Autentikasi menggunakan Basic Authentication over HTTPS

#### C. Quality of Service (QoS)

| Parameter | Target | Hasil Pengujian | Status |
|-----------|--------|-----------------|--------|
| Latency MQTT | < 500ms | 120-250ms | Tercapai |
| Latency WebSocket | < 1s | 200-400ms | Tercapai |
| Packet Loss | < 1% | 0% (jaringan lokal) | Tercapai |
| Throughput | Stabil | Stabil pada beban normal | Tercapai |
| Availability | 99% | 99.5% (selama pengujian) | Tercapai |

**Analisis QoS:**
- MQTT QoS Level 1 (At least once) digunakan untuk memastikan data absensi terkirim
- Reconnection mechanism pada ESP32 memastikan koneksi otomatis saat terputus
- Keep-alive interval 60 detik untuk menjaga koneksi tetap aktif

#### D. Keamanan Jaringan

1. **Autentikasi dan Otorisasi**
   - Sistem login dengan role-based access control (RBAC)
   - Session management menggunakan cookie dengan atribut HttpOnly
   - CSRF protection untuk mencegah cross-site request forgery

2. **Keamanan Protokol**
   - MQTT broker mendukung autentikasi username/password
   - WebDAV menggunakan Basic Authentication
   - Rekomendasi: Implementasi TLS/SSL untuk enkripsi data in-transit

3. **Keamanan Data**
   - Password di-hash menggunakan bcrypt
   - Environment variables untuk menyimpan kredensial sensitif
   - Validasi input untuk mencegah injection attacks

4. **Firewall dan Port Management**

   | Port | Protokol | Layanan | Status |
   |------|----------|---------|--------|
   | 3000 | TCP/HTTP | Web Server | Open (internal) |
   | 1883 | TCP/MQTT | MQTT Broker | Open (internal) |
   | 5432 | TCP | PostgreSQL | Open (internal) |
   | 8080 | TCP/HTTP | OwnCloud | Open (internal) |

#### E. Integrasi IoT ke Cloud

1. **Arsitektur IoT-Cloud**
   ```
   [ESP32 + Sensor] --WiFi--> [MQTT Broker] --TCP--> [Server] --WebDAV--> [OwnCloud]
        │                          │                    │                    │
   Device Layer            Network Layer         Application Layer      Cloud Layer
   ```

2. **Alur Data IoT ke Cloud**
   - ESP32 mengirim data fingerprint via MQTT (QoS 1)
   - Server menerima dan memproses data
   - Data disimpan ke PostgreSQL (persistence)
   - Backup dikirim ke OwnCloud via WebDAV protocol
   - Sinkronisasi cloud dapat dilakukan manual atau terjadwal

3. **Hasil Pengujian Integrasi**

   | Skenario | Protokol | Hasil |
   |----------|----------|-------|
   | ESP32 → Server | MQTT | Berhasil, latency < 300ms |
   | Server → Database | TCP/PostgreSQL | Berhasil, query < 50ms |
   | Server → OwnCloud | HTTP/WebDAV | Berhasil, upload 100KB < 2s |
   | Server → Browser | WebSocket | Berhasil, real-time update |

### 5.1.3 Ringkasan Pencapaian

| Aspek Jaringan | Indikator | Hasil |
|----------------|-----------|-------|
| Arsitektur | Three-tier architecture | Berhasil diimplementasikan |
| Protokol MQTT | Publish/Subscribe IoT | Berjalan dengan baik |
| Protokol WebSocket | Real-time communication | Latency < 1 detik |
| Protokol WebDAV | Cloud integration | Upload backup berhasil |
| QoS | Latency, packet loss | Memenuhi target |
| Keamanan | Authentication, CSRF | Terproteksi |
| Integrasi Cloud | OwnCloud backup | Terhubung dan berfungsi |

---

## 5.2 Saran

Untuk pengembangan sistem dari perspektif jaringan dan infrastruktur, penulis memberikan saran sebagai berikut:

### 5.2.1 Peningkatan Arsitektur Jaringan

1. **Skalabilitas Jaringan**
   - Implementasi MQTT cluster untuk mendukung ratusan perangkat IoT
   - Penggunaan load balancer (HAProxy/Nginx) untuk distribusi traffic
   - Implementasi message queue (RabbitMQ/Kafka) untuk high-throughput

2. **Redundansi dan High Availability**
   - Konfigurasi database replication (Master-Slave)
   - Implementasi failover mechanism untuk MQTT broker
   - Penggunaan multiple cloud storage untuk backup redundancy

3. **Network Segmentation**
   - Memisahkan jaringan IoT dan jaringan aplikasi menggunakan VLAN
   - Implementasi DMZ untuk layanan yang diakses publik
   - Firewall rules yang lebih ketat antar segmen

### 5.2.2 Peningkatan Protokol dan QoS

1. **Optimasi MQTT**
   - Implementasi MQTT QoS Level 2 untuk data kritikal
   - Penggunaan retained messages untuk status terakhir perangkat
   - Implementasi Last Will and Testament (LWT) untuk deteksi disconnect

2. **Peningkatan Real-time Performance**
   - Implementasi WebSocket compression untuk efisiensi bandwidth
   - Penggunaan binary protocol (MessagePack) sebagai alternatif JSON
   - Connection pooling untuk optimasi koneksi database

3. **Monitoring QoS**
   - Implementasi network monitoring (Prometheus + Grafana)
   - Alert system untuk degradasi performa jaringan
   - Logging terpusat menggunakan ELK Stack

### 5.2.3 Peningkatan Keamanan Jaringan

1. **Enkripsi Komunikasi**
   - Implementasi MQTT over TLS (port 8883)
   - HTTPS untuk seluruh komunikasi web
   - VPN untuk akses remote ke sistem

2. **Network Security**
   - Implementasi IDS/IPS (Intrusion Detection/Prevention System)
   - Rate limiting untuk mencegah DDoS attack
   - Network Access Control (NAC) untuk perangkat IoT

3. **Security Audit**
   - Penetration testing berkala
   - Vulnerability assessment pada infrastruktur jaringan
   - Security logging dan SIEM integration

### 5.2.4 Pengembangan Integrasi Cloud

1. **Multi-Cloud Strategy**
   - Integrasi dengan public cloud (AWS IoT, Google Cloud IoT, Azure IoT Hub)
   - Implementasi hybrid cloud untuk data sensitif
   - Cloud-to-cloud backup untuk disaster recovery

2. **Edge Computing**
   - Implementasi edge gateway untuk preprocessing data
   - Mengurangi latency dengan komputasi di edge
   - Offline capability saat koneksi cloud terputus

3. **IoT Platform Integration**
   - Integrasi dengan platform IoT standar (ThingsBoard, Home Assistant)
   - Implementasi protokol standar industri (CoAP, LwM2M)
   - API Gateway untuk manajemen akses IoT

---

## 5.3 Kata Penutup

Demikian laporan tugas akhir tentang Sistem Absensi IoT Berbasis Fingerprint ini disusun dengan fokus pada aspek jaringan komputer, protokol komunikasi, dan integrasi sistem. Penelitian ini menunjukkan bahwa implementasi arsitektur jaringan yang tepat dengan pemilihan protokol komunikasi yang sesuai (MQTT, WebSocket, WebDAV) dapat menghasilkan sistem IoT yang handal, efisien, dan aman.

Penulis berharap penelitian ini dapat memberikan kontribusi dalam pengembangan ilmu pengetahuan di bidang jaringan komputer dan Internet of Things, khususnya dalam implementasi sistem embedded yang terintegrasi dengan cloud computing.

---

**Referensi Silang:**
- Bab 3: [Perancangan Sistem (Diagram UML)](bab3-diagrams.md)
- Bab 4: [Hasil dan Implementasi](bab4-implementasi.md)
