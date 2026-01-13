# Panduan Pengujian Jaringan Manual
## Sistem Absensi IoT Berbasis Fingerprint

Dokumen ini berisi langkah-langkah pengujian manual untuk mendokumentasikan aspek jaringan sistem.

---

## 1. Pengujian Arsitektur Jaringan

### 1.1 Cek Port yang Aktif

**Command:**
```bash
# Cek semua port yang digunakan sistem
sudo netstat -tlnp | grep -E "3000|1883|5432|8080"

# Atau menggunakan ss (lebih modern)
ss -tlnp | grep -E "3000|1883|5432|8080"
```

**Contoh Output:**
```
tcp   LISTEN  0  511  0.0.0.0:3000   0.0.0.0:*  users:(("node",pid=1234,fd=18))
tcp   LISTEN  0  511  0.0.0.0:1883   0.0.0.0:*  users:(("node",pid=1234,fd=20))
tcp   LISTEN  0  128  127.0.0.1:5432 0.0.0.0:*  users:(("postgres",pid=567,fd=5))
tcp   LISTEN  0  511  0.0.0.0:8080   0.0.0.0:*  users:(("docker-proxy",pid=890,fd=4))
```

**Penjelasan:**
- Port 3000: HTTP Server (Express.js) - untuk web dashboard
- Port 1883: MQTT Broker (Aedes) - untuk komunikasi IoT
- Port 5432: PostgreSQL Database - untuk penyimpanan data
- Port 8080: OwnCloud (Docker) - untuk cloud backup

**Screenshot yang perlu diambil:** Terminal dengan output netstat/ss

---

### 1.2 Cek Koneksi Antar Komponen

**Command:**
```bash
# Test koneksi ke HTTP Server
curl -I http://localhost:3000

# Test koneksi ke MQTT Broker (menggunakan mosquitto client)
mosquitto_pub -h localhost -p 1883 -t "test" -m "hello" -d

# Test koneksi ke PostgreSQL
psql -h localhost -p 5432 -U absensi_user -d db_absensi -c "SELECT 1;"

# Test koneksi ke OwnCloud
curl -I http://localhost:8080
```

**Penjelasan:**
- curl -I: Mengirim HTTP HEAD request untuk cek apakah server merespons
- mosquitto_pub: Client MQTT untuk test publish message
- psql: PostgreSQL client untuk test query

---

### 1.3 Cek Docker Container (OwnCloud)

**Command:**
```bash
# Lihat container yang berjalan
docker ps

# Lihat detail container OwnCloud
docker inspect <container_id> | grep -A 5 "Ports"

# Lihat log OwnCloud
docker logs <container_id> --tail 20
```

**Screenshot yang perlu diambil:** Output docker ps menunjukkan OwnCloud running

---

## 2. Pengujian Protokol Komunikasi

### 2.1 Pengujian MQTT Protocol

#### A. Subscribe ke Topic (Terminal 1)
```bash
# Install mosquitto-clients jika belum ada
sudo apt install mosquitto-clients

# Subscribe ke topic fingerprint
mosquitto_sub -h localhost -p 1883 -t "fingerprint/#" -v
```

#### B. Publish Message (Terminal 2)
```bash
# Simulasi data absensi dari ESP32
mosquitto_pub -h localhost -p 1883 -t "fingerprint/attendance" \
  -m '{"fingerprint_id": 5, "timestamp": "2024-01-15T08:00:00"}'

# Simulasi enrollment response
mosquitto_pub -h localhost -p 1883 -t "fingerprint/enroll/response" \
  -m '{"success": true, "fingerprint_id": 6, "message": "Enrollment berhasil"}'
```

**Penjelasan:**
- `-h localhost`: Host MQTT broker
- `-p 1883`: Port MQTT
- `-t "fingerprint/#"`: Subscribe ke semua topic yang dimulai dengan "fingerprint/"
- `-v`: Verbose mode, tampilkan topic beserta message

**Screenshot yang perlu diambil:**
1. Terminal 1 menunjukkan subscribe aktif
2. Terminal 2 menunjukkan publish berhasil
3. Terminal 1 menerima message dari Terminal 2

---

### 2.2 Pengujian WebSocket (Socket.IO)

#### Menggunakan Browser DevTools

1. Buka browser, akses `http://localhost:3000/admin/dashboard-glass.html`
2. Tekan F12 (DevTools)
3. Buka tab **Network**
4. Filter: **WS** (WebSocket)
5. Refresh halaman

**Yang perlu diamati:**
- Connection upgrade dari HTTP ke WebSocket
- Status: 101 Switching Protocols
- Messages yang dikirim/diterima

**Cara melihat WebSocket messages:**
1. Klik pada koneksi WebSocket di Network tab
2. Pilih tab **Messages**
3. Lihat event yang dikirim (↑) dan diterima (↓)

**Screenshot yang perlu diambil:**
1. Network tab menunjukkan koneksi WebSocket
2. Messages tab menunjukkan event Socket.IO

---

### 2.3 Pengujian HTTP/REST API

#### Menggunakan curl

```bash
# 1. Test Login API
curl -X POST http://localhost:3000/api/login \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "admin123"}' \
  -c cookies.txt -v

# 2. Test GET Pegawai (dengan cookie session)
curl -X GET http://localhost:3000/api/pegawai \
  -b cookies.txt \
  -H "Content-Type: application/json"

# 3. Test GET Absensi
curl -X GET "http://localhost:3000/api/absensi?tanggal=2024-01-15" \
  -b cookies.txt

# 4. Test MQTT Status
curl -X GET http://localhost:3000/api/mqtt-status \
  -b cookies.txt
```

#### Menggunakan Browser DevTools

1. Buka halaman admin
2. F12 → Network tab
3. Filter: **XHR** atau **Fetch**
4. Lakukan aksi (klik menu, submit form)
5. Lihat request/response

**Screenshot yang perlu diambil:**
1. curl output dengan response JSON
2. Network tab dengan request API

---

### 2.4 Pengujian WebDAV Protocol (OwnCloud)

```bash
# 1. Test koneksi ke OwnCloud WebDAV
curl -X PROPFIND http://localhost:8080/remote.php/dav/files/admin/ \
  -u admin:Admin!@34 \
  -H "Depth: 1"

# 2. Test upload file (simulasi backup)
echo "test,data,csv" > /tmp/test-backup.csv
curl -X PUT http://localhost:8080/remote.php/dav/files/admin/Backup-Absensi/test-backup.csv \
  -u admin:Admin!@34 \
  -T /tmp/test-backup.csv

# 3. Verifikasi file terupload
curl -X PROPFIND http://localhost:8080/remote.php/dav/files/admin/Backup-Absensi/ \
  -u admin:Admin!@34 \
  -H "Depth: 1"
```

**Penjelasan:**
- `PROPFIND`: Method WebDAV untuk listing folder
- `-u admin:Admin!@34`: Basic Authentication
- `-T`: Upload file
- `Depth: 1`: Hanya list 1 level folder

**Screenshot yang perlu diambil:**
1. Output PROPFIND menunjukkan folder
2. Output PUT menunjukkan upload berhasil (201 Created)

---

## 3. Pengujian Quality of Service (QoS)

### 3.1 Pengujian Latency MQTT

**Metode: Timestamp Comparison**

```bash
# Terminal 1: Subscribe dengan timestamp
mosquitto_sub -h localhost -p 1883 -t "fingerprint/attendance" -v | while read line; do
  echo "$(date +%s.%N) - Received: $line"
done

# Terminal 2: Publish dengan timestamp
echo "Sending at: $(date +%s.%N)"
mosquitto_pub -h localhost -p 1883 -t "fingerprint/attendance" \
  -m '{"fingerprint_id": 1, "timestamp": "'$(date +%s.%N)'"}'
```

**Cara menghitung latency:**
```
Latency = Waktu_Receive - Waktu_Send
```

Ulangi 10x dan hitung rata-rata.

---

### 3.2 Pengujian Latency HTTP API

```bash
# Menggunakan curl dengan timing
curl -X GET http://localhost:3000/api/pegawai \
  -b cookies.txt \
  -w "\n\nDNS: %{time_namelookup}s\nConnect: %{time_connect}s\nTTFB: %{time_starttransfer}s\nTotal: %{time_total}s\n" \
  -o /dev/null -s

# Atau menggunakan Apache Benchmark
ab -n 100 -c 10 http://localhost:3000/api/pegawai
```

**Penjelasan output curl:**
- `time_namelookup`: Waktu DNS resolution
- `time_connect`: Waktu TCP connection
- `time_starttransfer`: Time To First Byte (TTFB)
- `time_total`: Total waktu request

---

### 3.3 Pengujian Latency WebSocket

**Menggunakan Browser Console:**

```javascript
// Paste di browser console (F12 → Console)
const socket = io();
const startTime = Date.now();

socket.on('connect', () => {
  console.log('WebSocket connected in:', Date.now() - startTime, 'ms');
});

// Test ping-pong latency
socket.emit('ping', Date.now());
socket.on('pong', (serverTime) => {
  const latency = Date.now() - serverTime;
  console.log('WebSocket latency:', latency, 'ms');
});
```

---

### 3.4 Pengujian Packet Loss

```bash
# Test packet loss ke server
ping -c 100 localhost

# Hasil yang diharapkan: 0% packet loss
# 100 packets transmitted, 100 received, 0% packet loss
```

---

### 3.5 Pengujian Throughput

```bash
# Test throughput HTTP menggunakan Apache Benchmark
ab -n 1000 -c 50 http://localhost:3000/api/pegawai

# Hasil akan menunjukkan:
# - Requests per second
# - Time per request
# - Transfer rate
```

**Screenshot yang perlu diambil:**
1. Output ab (Apache Benchmark)
2. Tabel hasil pengujian QoS

---

## 4. Pengujian Keamanan Jaringan

### 4.1 Pengujian CSRF Protection

```bash
# Test 1: Request tanpa Origin header (harus GAGAL)
curl -X POST http://localhost:3000/api/pegawai \
  -H "Content-Type: application/json" \
  -d '{"nama": "Test"}' \
  -b cookies.txt

# Test 2: Request dengan Origin berbeda (harus GAGAL)
curl -X POST http://localhost:3000/api/pegawai \
  -H "Content-Type: application/json" \
  -H "Origin: http://evil-site.com" \
  -d '{"nama": "Test"}' \
  -b cookies.txt

# Test 3: Request dengan Origin yang benar (harus BERHASIL)
curl -X POST http://localhost:3000/api/pegawai \
  -H "Content-Type: application/json" \
  -H "Origin: http://localhost:3000" \
  -H "Referer: http://localhost:3000/admin/" \
  -d '{"nama": "Test", "nip": "12345"}' \
  -b cookies.txt
```

**Yang diharapkan:**
- Test 1 & 2: Response 403 Forbidden (CSRF blocked)
- Test 3: Response 200 OK

---

### 4.2 Pengujian RBAC (Role-Based Access Control)

```bash
# Login sebagai Admin
curl -X POST http://localhost:3000/api/login \
  -H "Content-Type: application/json" \
  -d '{"username": "admin", "password": "admin123"}' \
  -c admin_cookies.txt

# Test akses admin endpoint (harus BERHASIL)
curl -X GET http://localhost:3000/api/pegawai \
  -b admin_cookies.txt

# Login sebagai User biasa
curl -X POST http://localhost:3000/api/login \
  -H "Content-Type: application/json" \
  -d '{"username": "user1", "password": "user123"}' \
  -c user_cookies.txt

# Test akses admin endpoint dengan user biasa (harus GAGAL/terbatas)
curl -X DELETE http://localhost:3000/api/pegawai/1 \
  -b user_cookies.txt
```

---

### 4.3 Pengujian Port Filtering

```bash
# Scan port yang terbuka
nmap -p 3000,1883,5432,8080 localhost

# Cek firewall rules (jika menggunakan ufw)
sudo ufw status verbose

# Cek iptables rules
sudo iptables -L -n
```

---

### 4.4 Pengujian SQL Injection (harus GAGAL)

```bash
# Attempt SQL injection (harus ditolak/tidak berpengaruh)
curl -X POST http://localhost:3000/api/login \
  -H "Content-Type: application/json" \
  -d '{"username": "admin'\'' OR 1=1--", "password": "test"}'

# Sistem menggunakan parameterized query sehingga aman
```

---

## 5. Pengujian Integrasi IoT ke Cloud

### 5.1 End-to-End Test Flow

**Langkah-langkah:**

1. **Buka Dashboard Admin** di browser
   - URL: `http://localhost:3000/admin/dashboard-glass.html`
   - Buka DevTools (F12) → Network tab

2. **Simulasi Scan Fingerprint via MQTT**
   ```bash
   mosquitto_pub -h localhost -p 1883 -t "fingerprint/attendance" \
     -m '{"fingerprint_id": 1, "timestamp": "'$(date -Iseconds)'"}'
   ```

3. **Verifikasi di Dashboard**
   - Data absensi baru muncul di tabel (real-time via WebSocket)
   - Catat waktu dari publish sampai muncul di UI

4. **Verifikasi di Database**
   ```bash
   psql -h localhost -U absensi_user -d db_absensi \
     -c "SELECT * FROM absensi ORDER BY id DESC LIMIT 1;"
   ```

5. **Test Backup ke OwnCloud**
   - Klik tombol "Backup" di halaman Settings
   - Verifikasi file di OwnCloud:
   ```bash
   curl -X PROPFIND http://localhost:8080/remote.php/dav/files/admin/Backup-Absensi/ \
     -u admin:Admin!@34 -H "Depth: 1"
   ```

---

### 5.2 Pengukuran Latency End-to-End

**Script untuk mengukur:**

```bash
#!/bin/bash
# save sebagai: test-latency.sh

echo "=== End-to-End Latency Test ==="
START=$(date +%s.%N)

# Publish MQTT
mosquitto_pub -h localhost -p 1883 -t "fingerprint/attendance" \
  -m '{"fingerprint_id": 99, "timestamp": "'$(date -Iseconds)'"}'

# Wait dan cek database
sleep 1
RESULT=$(psql -h localhost -U absensi_user -d db_absensi -t \
  -c "SELECT COUNT(*) FROM absensi WHERE fingerprint_id = 99;")

END=$(date +%s.%N)
LATENCY=$(echo "$END - $START" | bc)

echo "Latency: ${LATENCY}s"
echo "Data tersimpan: $RESULT record(s)"
```

---

## 6. Tabel Hasil Pengujian (Template)

### 6.1 Hasil Pengujian Port

| Port | Service | Status | Screenshot |
|------|---------|--------|------------|
| 3000 | HTTP Server | ✅ Active | Gambar 4.x |
| 1883 | MQTT Broker | ✅ Active | Gambar 4.x |
| 5432 | PostgreSQL | ✅ Active | Gambar 4.x |
| 8080 | OwnCloud | ✅ Active | Gambar 4.x |

### 6.2 Hasil Pengujian QoS

| Parameter | Target | Hasil | Status |
|-----------|--------|-------|--------|
| Latency MQTT | < 500ms | ___ms | ✅/❌ |
| Latency WebSocket | < 1s | ___ms | ✅/❌ |
| Latency HTTP | < 2s | ___ms | ✅/❌ |
| Packet Loss | < 1% | ___% | ✅/❌ |
| Throughput | Stabil | ___req/s | ✅/❌ |

### 6.3 Hasil Pengujian Keamanan

| Test Case | Expected | Actual | Status |
|-----------|----------|--------|--------|
| CSRF tanpa Origin | 403 Forbidden | ___ | ✅/❌ |
| CSRF Origin salah | 403 Forbidden | ___ | ✅/❌ |
| CSRF Origin benar | 200 OK | ___ | ✅/❌ |
| SQL Injection | Blocked | ___ | ✅/❌ |
| RBAC Admin | Full Access | ___ | ✅/❌ |
| RBAC User | Limited | ___ | ✅/❌ |

---

## 7. Checklist Screenshot untuk Dokumentasi

- [ ] Output `netstat` / `ss` menunjukkan port aktif
- [ ] Output `docker ps` menunjukkan OwnCloud running
- [ ] Terminal MQTT subscribe menerima message
- [ ] Browser DevTools - WebSocket connection
- [ ] Browser DevTools - WebSocket messages
- [ ] Browser DevTools - HTTP/REST API calls
- [ ] Output curl WebDAV PROPFIND
- [ ] Output curl WebDAV PUT (backup)
- [ ] Output Apache Benchmark (throughput test)
- [ ] Dashboard menampilkan data real-time
- [ ] OwnCloud web interface dengan file backup
- [ ] Output pengujian CSRF (blocked vs allowed)

---

## 8. Tips Dokumentasi

1. **Gunakan timestamp** pada setiap screenshot
2. **Beri nomor gambar** sesuai urutan di bab (Gambar 4.1, 4.2, dst)
3. **Tambahkan keterangan** di bawah setiap gambar
4. **Highlight** bagian penting dengan kotak merah
5. **Catat spesifikasi** perangkat yang digunakan untuk testing

**Contoh caption:**
> Gambar 4.15 Hasil Pengujian Latency MQTT menunjukkan rata-rata latency 180ms yang memenuhi target < 500ms

---

## 9. Pengujian Sistem IoT (ESP32 + Fingerprint + OLED)

### 9.1 Arsitektur Hardware IoT

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                         PERANGKAT IOT                                       │
│                                                                             │
│   ┌─────────────────┐      ┌─────────────────┐      ┌─────────────────┐    │
│   │  FINGERPRINT    │      │     ESP32       │      │  OLED DISPLAY   │    │
│   │  SENSOR AS608   │      │   DevKit V1     │      │   SPI 0.96"     │    │
│   │                 │      │                 │      │                 │    │
│   │  VCC  ──────────┼──────┼── 3.3V         │      │  VCC ───────────┼────┤
│   │  GND  ──────────┼──────┼── GND          │      │  GND ───────────┼────┤
│   │  TX   ──────────┼──────┼── GPIO16 (RX)  │      │  SCK ───────────┼── GPIO18
│   │  RX   ──────────┼──────┼── GPIO17 (TX)  │      │  SDA ───────────┼── GPIO23
│   │                 │      │                 │      │  CS  ───────────┼── GPIO5
│   │  Baud: 57600    │      │  WiFi 2.4GHz   │      │  DC  ───────────┼── GPIO4
│   │                 │      │  MQTT Client   │      │  RES ───────────┼── GPIO2
│   └─────────────────┘      └─────────────────┘      └─────────────────┘    │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

### 9.2 Pengujian Koneksi WiFi ESP32

#### A. Monitor Serial ESP32 (PlatformIO)

**Cara 1: Via PlatformIO IDE (VSCode)**
1. Buka **VSCode** dengan project ESP32
2. Klik icon **PlatformIO** di sidebar kiri
3. Pilih **Monitor** atau tekan `Ctrl+Alt+M`
4. Atau buka terminal: `pio device monitor`

**Cara 2: Via Terminal**
```bash
# Di folder esp32/
cd /home/hasan/Documents/Perancangan/Perancangan/server-monitoring/esp32

# Jalankan serial monitor
pio device monitor --baud 115200

# Atau dengan filter timestamp
pio device monitor --baud 115200 --filter time
```

**Cara 3: Via PlatformIO CLI**
```bash
# Upload dan langsung monitor
pio run --target upload && pio device monitor
```

**Output yang diharapkan:**
```
Connecting to WiFi...
WiFi connected!
IP Address: 192.168.1.xxx
Signal Strength (RSSI): -45 dBm
Connecting to MQTT broker...
MQTT Connected!
Subscribed to: fingerprint/enroll/request
Subscribed to: fingerprint/command
Ready for fingerprint scan...
```

**Screenshot yang perlu diambil:**
1. Serial Monitor menunjukkan koneksi WiFi berhasil
2. IP Address yang didapat dari DHCP
3. RSSI (kekuatan sinyal WiFi)

---

### 9.3 Pengujian Komunikasi MQTT ESP32 ↔ Server

#### A. Test ESP32 sebagai Publisher

**Di Server (Terminal 1):**
```bash
# Subscribe untuk menerima data dari ESP32
mosquitto_sub -h localhost -p 1883 -t "fingerprint/#" -v
```

**Di ESP32:**
- Scan jari yang sudah terdaftar
- Atau tekan tombol test (jika ada)

**Output di Terminal Server:**
```
fingerprint/attendance {"fingerprint_id":1,"timestamp":"2024-01-15T08:00:00"}
```

#### B. Test ESP32 sebagai Subscriber

**Di Server (Terminal 2):**
```bash
# Kirim perintah enrollment ke ESP32
mosquitto_pub -h localhost -p 1883 -t "fingerprint/enroll/request" \
  -m '{"pegawai_id":10,"fingerprint_id":5,"nama":"John Doe"}'
```

**Output di Serial Monitor ESP32:**
```
Received enrollment request!
Pegawai ID: 10
Fingerprint ID: 5
Nama: John Doe
Starting enrollment process...
Place your finger on the sensor...
```

**Screenshot yang perlu diambil:**
1. Terminal server menerima data dari ESP32
2. Serial Monitor ESP32 menerima command dari server

---

### 9.4 Pengujian Sensor Fingerprint AS608

#### A. Test Deteksi Jari

**Kode Testing (Upload via PlatformIO):**
```cpp
// Upload ke ESP32: pio run --target upload
#include <Adafruit_Fingerprint.h>

HardwareSerial mySerial(2); // UART2: RX=16, TX=17
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

void setup() {
  Serial.begin(115200);
  mySerial.begin(57600, SERIAL_8N1, 16, 17);

  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Fingerprint sensor detected!");
    Serial.print("Sensor capacity: ");
    Serial.println(finger.capacity);
    Serial.print("Security level: ");
    Serial.println(finger.security_level);
  } else {
    Serial.println("Fingerprint sensor NOT found!");
  }
}

void loop() {
  Serial.println("Place finger on sensor...");

  int result = finger.getImage();
  if (result == FINGERPRINT_OK) {
    Serial.println("Image taken!");

    result = finger.image2Tz();
    if (result == FINGERPRINT_OK) {
      Serial.println("Image converted!");

      result = finger.fingerSearch();
      if (result == FINGERPRINT_OK) {
        Serial.print("Found ID: ");
        Serial.println(finger.fingerID);
        Serial.print("Confidence: ");
        Serial.println(finger.confidence);
      } else {
        Serial.println("Fingerprint not found in database");
      }
    }
  }
  delay(1000);
}
```

**Output Serial Monitor:**
```
Fingerprint sensor detected!
Sensor capacity: 127
Security level: 3
Place finger on sensor...
Image taken!
Image converted!
Found ID: 1
Confidence: 98
```

#### B. Tabel Hasil Pengujian Fingerprint

| Test Case | Input | Expected Output | Actual | Status |
|-----------|-------|-----------------|--------|--------|
| Sensor detection | Power on | "Sensor detected" | ___ | ✅/❌ |
| Scan registered finger | Jari terdaftar | ID + Confidence > 50 | ___ | ✅/❌ |
| Scan unregistered finger | Jari tidak terdaftar | "Not found" | ___ | ✅/❌ |
| Enrollment new finger | Jari baru 2x scan | "Enrollment success" | ___ | ✅/❌ |
| Delete fingerprint | ID yang ada | "Deleted successfully" | ___ | ✅/❌ |

---

### 9.5 Pengujian OLED Display SPI

#### A. Test Tampilan OLED

**Kode Testing (Upload via PlatformIO):**
```cpp
// Upload ke ESP32: pio run --target upload
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_MOSI   23
#define OLED_CLK    18
#define OLED_DC     4
#define OLED_CS     5
#define OLED_RESET  2

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);

void setup() {
  Serial.begin(115200);

  if(!display.begin(SSD1306_SWITCHCAPVCC)) {
    Serial.println("OLED initialization failed!");
    while(1);
  }

  Serial.println("OLED initialized!");

  // Test display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("=== ABSENSI IOT ===");
  display.println();
  display.println("Silakan scan");
  display.println("sidik jari Anda...");
  display.display();
}

void loop() {
  // Test different screens
  delay(3000);

  // Screen: Success
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("BERHASIL!");
  display.setTextSize(1);
  display.println();
  display.println("Nama: John Doe");
  display.println("Jam: 08:00:15");
  display.println("Status: HADIR");
  display.display();

  delay(3000);

  // Screen: Failed
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(0, 0);
  display.println("GAGAL!");
  display.setTextSize(1);
  display.println();
  display.println("Sidik jari tidak");
  display.println("terdaftar!");
  display.display();
}
```

#### B. Tabel Hasil Pengujian OLED

| Test Case | Kondisi | Expected Display | Screenshot |
|-----------|---------|------------------|------------|
| Idle/Ready | Menunggu scan | "Silakan scan sidik jari" | Gambar 4.x |
| Scan Success | Jari terdaftar | Nama + Jam + Status | Gambar 4.x |
| Scan Failed | Jari tidak terdaftar | "Sidik jari tidak terdaftar" | Gambar 4.x |
| Enrollment Step 1 | Proses enrollment | "Letakkan jari..." | Gambar 4.x |
| Enrollment Step 2 | Proses enrollment | "Angkat jari..." | Gambar 4.x |
| Enrollment Success | Selesai enrollment | "Pendaftaran berhasil!" | Gambar 4.x |
| WiFi Connecting | Boot/reconnect | "Menghubungkan WiFi..." | Gambar 4.x |
| MQTT Connected | Koneksi sukses | "Terhubung ke server" | Gambar 4.x |

---

### 9.6 Pengujian End-to-End IoT Flow

#### Skenario 1: Absensi Masuk

**Langkah-langkah:**

1. **Persiapan:**
   - Server running (`npm start`)
   - ESP32 connected ke WiFi
   - Dashboard admin terbuka di browser
   - Serial Monitor terbuka

2. **Eksekusi:**
   - Scan jari yang sudah terdaftar di sensor

3. **Verifikasi:**

   | Checkpoint | Yang Diamati | Expected |
   |------------|--------------|----------|
   | ESP32 Serial | Log scan detected | "Fingerprint ID: X found" |
   | ESP32 Serial | MQTT publish | "Published to fingerprint/attendance" |
   | OLED Display | Feedback visual | "BERHASIL! Nama: XXX" |
   | Server Log | MQTT received | "📥 MQTT Message received" |
   | Server Log | Database insert | "✅ Absensi recorded" |
   | Dashboard | Real-time update | Data baru muncul di tabel |
   | Database | Record tersimpan | Query SELECT menunjukkan data |

4. **Pengukuran Waktu:**
   ```
   T0: Jari menyentuh sensor
   T1: ESP32 mendeteksi (Serial log)
   T2: MQTT published (Serial log)
   T3: Server received (Server log)
   T4: Database inserted (Server log)
   T5: Dashboard updated (Visual)

   Total Latency = T5 - T0
   Target: < 1 detik
   ```

#### Skenario 2: Enrollment Fingerprint Baru

**Langkah-langkah:**

1. **Persiapan:**
   - Buka halaman Tambah Karyawan di admin
   - Isi data karyawan
   - ESP32 siap menerima command

2. **Eksekusi:**
   - Klik tombol "Daftarkan Sidik Jari"

3. **Verifikasi Flow:**

   ```
   Browser                    Server                     ESP32
      │                          │                          │
      │── POST /api/enroll ────►│                          │
      │                          │── MQTT publish ────────►│
      │                          │   fingerprint/enroll/   │
      │                          │   request               │
      │                          │                          │
      │                          │                     [OLED: "Letakkan jari..."]
      │                          │                          │
      │                          │◄── MQTT publish ────────│
      │                          │    fingerprint/enroll/  │
      │                          │    progress {step:1}    │
      │                          │                          │
      │◄── WebSocket ───────────│                          │
      │    enrollment_progress   │                          │
      │                          │                     [User: Scan jari 1x]
      │                          │                          │
      │                          │                     [OLED: "Angkat jari..."]
      │                          │                          │
      │                          │                     [User: Scan jari 2x]
      │                          │                          │
      │                          │◄── MQTT publish ────────│
      │                          │    fingerprint/enroll/  │
      │                          │    response {success}   │
      │                          │                          │
      │◄── WebSocket ───────────│                          │
      │    enrollment_success    │                          │
      │                          │                          │
   [UI: "Berhasil!"]          [DB: Update fp_id]      [OLED: "Berhasil!"]
   ```

---

### 9.7 Pengujian Kualitas Sinyal WiFi

```bash
# Di ESP32, tambahkan kode untuk monitoring RSSI
int rssi = WiFi.RSSI();
Serial.print("Signal Strength: ");
Serial.print(rssi);
Serial.println(" dBm");
```

**Tabel Interpretasi RSSI:**

| RSSI (dBm) | Kualitas | Keterangan |
|------------|----------|------------|
| -30 to -50 | Excellent | Sinyal sangat kuat |
| -50 to -60 | Good | Sinyal baik |
| -60 to -70 | Fair | Sinyal cukup |
| -70 to -80 | Weak | Sinyal lemah, mungkin ada delay |
| < -80 | Poor | Tidak stabil, sering disconnect |

**Screenshot yang perlu diambil:**
- Serial Monitor menunjukkan RSSI
- Posisi ESP32 relatif terhadap router

---

### 9.8 Pengujian Reliabilitas IoT

#### A. Test Reconnect WiFi

1. Matikan router WiFi
2. Amati Serial Monitor ESP32
3. Nyalakan kembali router
4. Verifikasi ESP32 reconnect otomatis

**Expected behavior:**
```
WiFi disconnected!
Reconnecting...
Reconnecting...
WiFi reconnected!
IP: 192.168.1.xxx
MQTT reconnected!
```

#### B. Test Reconnect MQTT

1. Restart server Node.js (`pkill -f node && npm start`)
2. Amati Serial Monitor ESP32
3. Verifikasi ESP32 reconnect ke MQTT broker

#### C. Tabel Hasil Pengujian Reliabilitas

| Test Case | Action | Recovery Time | Status |
|-----------|--------|---------------|--------|
| WiFi disconnect 10 detik | Router off/on | ___ detik | ✅/❌ |
| WiFi disconnect 1 menit | Router off/on | ___ detik | ✅/❌ |
| MQTT broker restart | Server restart | ___ detik | ✅/❌ |
| Power cycle ESP32 | Reset button | ___ detik | ✅/❌ |
| Scan saat reconnecting | Scan jari | Queue/retry | ✅/❌ |

---

### 9.9 Spesifikasi Hardware untuk Dokumentasi

**Tabel Komponen Hardware:**

| No | Komponen | Spesifikasi | Qty | Fungsi |
|----|----------|-------------|-----|--------|
| 1 | ESP32 DevKit V1 | Dual-core 240MHz, WiFi 802.11 b/g/n, 4MB Flash | 1 | Mikrokontroler utama |
| 2 | Fingerprint AS608 | Optical sensor, 500dpi, 127 templates, UART | 1 | Identifikasi sidik jari |
| 3 | OLED SSD1306 | 0.96", 128x64 pixels, SPI interface | 1 | Display feedback |
| 4 | Kabel Jumper | Male-to-male, Male-to-female | ~20 | Koneksi komponen |
| 5 | Breadboard | 830 points | 1 | Prototyping |
| 6 | Kabel USB | Micro USB | 1 | Power & programming |

**Tabel Pin Mapping:**

| ESP32 Pin | Komponen | Fungsi |
|-----------|----------|--------|
| GPIO16 | Fingerprint TX | Serial receive |
| GPIO17 | Fingerprint RX | Serial transmit |
| GPIO18 | OLED SCK | SPI clock |
| GPIO23 | OLED SDA/MOSI | SPI data |
| GPIO5 | OLED CS | Chip select |
| GPIO4 | OLED DC | Data/command |
| GPIO2 | OLED RES | Reset |
| 3.3V | All VCC | Power supply |
| GND | All GND | Ground |

---

### 9.10 Checklist Screenshot IoT

- [ ] Foto rangkaian hardware (ESP32 + Fingerprint + OLED)
- [ ] Serial Monitor: WiFi connected + IP Address
- [ ] Serial Monitor: MQTT connected
- [ ] Serial Monitor: Fingerprint scan detected
- [ ] Serial Monitor: MQTT publish success
- [ ] OLED: Tampilan idle "Silakan scan..."
- [ ] OLED: Tampilan success dengan nama
- [ ] OLED: Tampilan failed
- [ ] OLED: Tampilan enrollment progress
- [ ] Dashboard: Data muncul real-time setelah scan
- [ ] Wireshark: MQTT packet capture (optional, advanced)

