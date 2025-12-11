# 📘 Dokumentasi Sistem Absensi IoT dengan Fingerprint - Tugas Akhir

## 📋 Ringkasan Sistem

**Judul:** Sistem Absensi Karyawan Berbasis IoT dengan Integrasi Fingerprint Sensor dan Progressive Web Application (PWA)

**Deskripsi:** Sistem absensi modern yang mengintegrasikan perangkat IoT (ESP32 + Fingerprint Sensor) dengan aplikasi web berbasis PWA untuk mengelola kehadiran karyawan secara real-time, manajemen tugas, dan enrollment sidik jari.

---

## 🏗️ Arsitektur Sistem

### 1. **Komponen Hardware (IoT)**
- **ESP32 Microcontroller** - Device utama untuk pemrosesan
- **Fingerprint Sensor (R307/AS608)** - Pembaca sidik jari karyawan
- **OLED Display** - Tampilan feedback untuk user
- **WiFi Module** - Koneksi ke server via MQTT

### 2. **Backend Server (Node.js)**
- **Express.js** - Web framework
- **PostgreSQL** - Database relasional
- **MQTT Broker (Aedes)** - Protocol komunikasi IoT
- **Socket.IO** - Real-time communication
- **WebDAV (OwnCloud)** - Cloud storage untuk file

### 3. **Frontend (Progressive Web App)**
- **Vanilla JavaScript** - Tanpa framework untuk performa optimal
- **Service Worker** - Offline capability
- **Glassmorphism UI** - Modern design dengan blur effect
- **Responsive Design** - Support mobile & desktop

---

## 🎯 Fitur Utama Sistem

### A. **Sistem Absensi**

#### 1. Absen Masuk & Keluar
- **Clock In** - Karyawan scan sidik jari saat datang
- **Clock Out** - Karyawan scan sidik jari saat pulang
- **Perhitungan Durasi Kerja** - Otomatis dihitung (menit)
- **Deteksi Lembur** - Otomatis detect jika > 8 jam atau keluar > 17:00
- **Status Real-time** - Update langsung via Socket.IO

#### 2. Validasi dan Verifikasi
- **Validasi Sidik Jari** - 1:N matching di sensor
- **Threshold Akurasi** - Confidence score untuk keamanan
- **Anti-duplicate** - Tidak bisa absen 2x di hari yang sama
- **Log Timestamp** - Setiap transaksi tercatat dengan waktu presisi

---

### B. **Manajemen Karyawan**

#### 1. Registrasi Karyawan Baru
- **Form Input Data:**
  - NIP (Nomor Induk Pegawai) - Unique
  - Nama Lengkap
  - Posisi (Karyawan/Supervisor/Manager/Admin)
  - Email
  - No. Telepon
  - Tanggal Bergabung
  - Status (Aktif/Non-Aktif)

#### 2. Enrollment Sidik Jari (IoT Integration)
- **Flow Enrollment:**
  1. Admin input data karyawan via web
  2. Server assign fingerprint_id otomatis (1-127)
  3. Server kirim MQTT command ke ESP32:
     ```json
     Topic: "fingerprint/enroll/request"
     Payload: {
       "command": "ENROLL",
       "fingerprint_id": 12,
       "pegawai_id": 5,
       "nip": "20210410700073",
       "nama": "Achmad Hasan"
     }
     ```
  4. ESP32 meminta user tempelkan jari 2x
  5. Sensor menyimpan template sidik jari
  6. ESP32 kirim response ke server:
     ```json
     Topic: "fingerprint/enroll/response"
     Payload: {
       "success": true,
       "fingerprint_id": 12,
       "pegawai_id": 5,
       "message": "Sidik jari berhasil didaftarkan"
     }
     ```
  7. Server update database dan broadcast ke client
  8. Admin dapat notifikasi real-time via Socket.IO

#### 3. Data Karyawan
- **CRUD Operations** - Create, Read, Update, Delete
- **Pencarian & Filter** - Berdasarkan nama, NIP, posisi
- **Export Data** - Ke format Excel/CSV (future)

---

### C. **Manajemen Tugas (Task Management)**

#### 1. Sistem Submission Tugas Bebas
Karyawan dapat submit hasil pekerjaan tanpa dibatasi tugas yang sudah diberikan.

**Fitur:**
- **Input Nama Tugas** - Bebas (tidak dari dropdown)
- **Upload File** - PDF, DOC, XLS, PPT, ZIP, Gambar (max 10MB)
- **Catatan/Deskripsi** - Optional
- **Timestamp** - Otomatis tercatat
- **Storage** - Lokal (uploads/) + OwnCloud backup

**Endpoint:**
```javascript
POST /tugas/submit-free
Body: {
  nama_tugas: "Laporan Harian 10 Nov",
  pegawai_id: 1,
  catatan: "Dokumentasi kegiatan hari ini",
  file: [binary]
}
```

#### 2. History Submission
- **View Personal** - Karyawan lihat history submission sendiri
- **View All (Admin)** - Admin lihat semua submission
- **Download File** - Admin & karyawan bisa download
- **Delete** - Admin bisa hapus submission

#### 3. Database Schema
```sql
CREATE TABLE tugas_submissions (
    id_submission SERIAL PRIMARY KEY,
    pegawai_id INTEGER REFERENCES pegawai(id_pegawai),
    nama_tugas VARCHAR(255) NOT NULL,
    catatan TEXT,
    file_path VARCHAR(255) NOT NULL,
    tanggal_submit TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

---

### D. **Progressive Web App (PWA)**

#### 1. Karakteristik PWA
- **Installable** - Bisa di-install ke home screen (Android/iOS)
- **Offline-First** - Tetap berfungsi tanpa koneksi internet
- **Responsive** - Adaptif untuk semua ukuran layar
- **Fast Loading** - Service Worker caching
- **Native-like** - Fullscreen, tanpa address bar

#### 2. Service Worker Strategy
```javascript
// Static Cache - Install time
const STATIC_CACHE = [
  '/user-login.html',
  '/user-dashboard.html',
  '/glass-style.css',
  '/icons/icon-192x192.png'
];

// Runtime Cache - On demand
- API responses (GET requests only)
- Images & assets
- Fonts

// Network-First Strategy
- API endpoints (/absensi, /pegawai, /submissions)
- Dynamic content

// Cache-First Strategy
- Static assets (HTML, CSS, JS)
- Images & icons
```

#### 3. Manifest.json
```json
{
  "name": "Sistem Absensi IoT",
  "short_name": "Absensi",
  "start_url": "/user-login.html",
  "display": "standalone",
  "theme_color": "#667eea",
  "background_color": "#0f0f23",
  "icons": [
    {
      "src": "/icons/icon-192x192.png",
      "sizes": "192x192",
      "type": "image/png"
    },
    {
      "src": "/icons/icon-512x512.png",
      "sizes": "512x512",
      "type": "image/png"
    }
  ]
}
```

---

## 🔐 Keamanan Sistem

### 1. Autentikasi & Autorisasi
- **Session-based Auth** - sessionStorage untuk user state
- **Role-based Access** - Admin vs User permissions
- **Password Validation** - (Minimal, bisa ditingkatkan)

### 2. Data Security
- **SQL Injection Prevention** - Parameterized queries
- **XSS Protection** - Input sanitization
- **CSRF Protection** - (Bisa ditambahkan)
- **File Upload Validation** - Type & size checking

### 3. Network Security
- **MQTT over TCP** - (Bisa upgrade ke TLS)
- **HTTP** - (Bisa upgrade ke HTTPS)
- **Firewall Rules** - Port 3000 & 1883

---

## 📊 Database Schema

### 1. Table: pegawai
```sql
CREATE TABLE pegawai (
    id_pegawai SERIAL PRIMARY KEY,
    nip VARCHAR(50) UNIQUE NOT NULL,
    nama_pegawai VARCHAR(100) NOT NULL,
    posisi VARCHAR(50) DEFAULT 'Karyawan',
    email VARCHAR(100),
    no_telepon VARCHAR(20),
    tanggal_bergabung DATE,
    status VARCHAR(20) DEFAULT 'Aktif',
    fingerprint_id INTEGER UNIQUE,  -- NEW: Link ke sensor
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

**Indexes:**
- `idx_pegawai_nip` - Faster lookup by NIP
- `idx_pegawai_fingerprint` - Faster lookup by fingerprint_id

### 2. Table: absensi
```sql
CREATE TABLE absensi (
    id_absensi SERIAL PRIMARY KEY,
    pegawai_id INTEGER REFERENCES pegawai(id_pegawai),
    waktu_absen TIME NOT NULL,
    waktu_keluar TIME,                 -- Waktu clock out
    durasi_kerja INTEGER DEFAULT 0,    -- Menit
    durasi_lembur INTEGER DEFAULT 0,   -- Menit
    status_lembur VARCHAR(20) DEFAULT 'Tidak',
    tipe_absen VARCHAR(10) DEFAULT 'Masuk',  -- Masuk/Keluar
    keterangan VARCHAR(50) DEFAULT 'Hadir',
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

### 3. Table: tugas_submissions
```sql
CREATE TABLE tugas_submissions (
    id_submission SERIAL PRIMARY KEY,
    pegawai_id INTEGER REFERENCES pegawai(id_pegawai),
    nama_tugas VARCHAR(255) NOT NULL,
    catatan TEXT,
    file_path VARCHAR(255) NOT NULL,
    tanggal_submit TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);
```

**Indexes:**
- `idx_submissions_pegawai` - Faster filtering by pegawai
- `idx_submissions_tanggal` - Faster sorting by date

---

## 🌐 API Endpoints

### A. Pegawai Management

#### 1. Get All Pegawai
```http
GET /pegawai
Response: {
  "success": true,
  "data": [
    {
      "id_pegawai": 1,
      "nip": "20210410700073",
      "nama_pegawai": "Achmad Hasan",
      "posisi": "Admin",
      "fingerprint_id": 12
    }
  ]
}
```

#### 2. Add New Pegawai
```http
POST /pegawai/add
Body: {
  "nip": "20220101000003",
  "nama_pegawai": "John Doe",
  "posisi": "Karyawan",
  "email": "john@example.com",
  "no_telepon": "08123456789",
  "tanggal_bergabung": "2024-01-15"
}
Response: {
  "success": true,
  "message": "Pegawai berhasil ditambahkan",
  "data": { ... }
}
```

#### 3. Enroll Fingerprint
```http
POST /fingerprint/enroll
Body: {
  "pegawai_id": 5
}
Response: {
  "success": true,
  "message": "Perintah enrollment dikirim ke ESP32",
  "data": {
    "fingerprint_id": 12,
    "pegawai_id": 5,
    "nama": "John Doe"
  }
}
```

### B. Absensi

#### 1. Get Absensi History
```http
GET /absensi?limit=50
Response: {
  "success": true,
  "data": [
    {
      "id_absensi": 1,
      "nama_pegawai": "Achmad Hasan",
      "waktu_absen": "08:00:00",
      "waktu_keluar": "17:30:00",
      "durasi_kerja": 570,
      "durasi_lembur": 30,
      "status_lembur": "Ya"
    }
  ]
}
```

#### 2. Submit Absensi (via HTTP - alternative)
```http
POST /absensi
Body: {
  "pegawai_id": 1,
  "waktu_absen": "08:00:00",
  "keterangan": "Hadir",
  "tipe_absen": "Masuk"
}
```

### C. Tugas/Submissions

#### 1. Submit Tugas (Free Text)
```http
POST /tugas/submit-free
Content-Type: multipart/form-data
Body: {
  "nama_tugas": "Laporan Harian",
  "pegawai_id": 1,
  "catatan": "Dokumentasi kegiatan",
  "file": [binary]
}
```

#### 2. Get Submissions (User)
```http
GET /submissions/pegawai/:pegawai_id
Response: {
  "success": true,
  "data": [ ... ]
}
```

#### 3. Get All Submissions (Admin)
```http
GET /submissions/all
Response: {
  "success": true,
  "data": [
    {
      "id_submission": 1,
      "nama_tugas": "Laporan Harian 10 Nov",
      "nama_pegawai": "Achmad Hasan",
      "nip": "20210410700073",
      "posisi": "Admin",
      "tanggal_submit": "2025-11-10T10:30:00"
    }
  ]
}
```

#### 4. Delete Submission
```http
DELETE /submissions/:id_submission
Response: {
  "success": true,
  "message": "Submission berhasil dihapus"
}
```

---

## 📡 MQTT Protocol

### 1. Topics

#### Absensi Data (ESP32 → Server)
```
Topic: absensi/data
Payload: {
  "pegawai_id": 1,
  "waktu_absen": "08:00:00",
  "keterangan": "Hadir",
  "tipe_absen": "Masuk"  // Masuk atau Keluar
}
```

#### Enrollment Request (Server → ESP32)
```
Topic: fingerprint/enroll/request
Payload: {
  "command": "ENROLL",
  "fingerprint_id": 12,
  "pegawai_id": 5,
  "nip": "20220101000003",
  "nama": "John Doe"
}
```

#### Enrollment Response (ESP32 → Server)
```
Topic: fingerprint/enroll/response
Payload: {
  "success": true,
  "fingerprint_id": 12,
  "pegawai_id": 5,
  "message": "Sidik jari berhasil didaftarkan"
}
```

### 2. QoS Level
- **QoS 0** - At most once (default)
- **QoS 1** - At least once (recommended untuk enrollment)

---

## 🎨 User Interface

### A. User (Karyawan)

#### 1. Login Page
- **URL:** `/user-login.html`
- **Input:** NIP, Password
- **Features:**
  - Remember me checkbox
  - PWA install prompt
  - Offline indicator

#### 2. Dashboard
- **URL:** `/user-dashboard.html`
- **Widgets:**
  - Status kehadiran hari ini
  - Total kehadiran bulan ini
  - Statistik lembur
  - Quick actions

#### 3. Tugas Saya
- **URL:** `/user-tugas.html`
- **Features:**
  - Form submit tugas (nama, catatan, file)
  - History submission
  - Download file
  - Filter & search

### B. Admin

#### 1. Dashboard
- **URL:** `/dashboard-glass.html`
- **Widgets:**
  - Total karyawan
  - Kehadiran hari ini
  - Statistik lembur
  - Recent activities

#### 2. Riwayat Absensi
- **URL:** `/riwayat-glass.html`
- **Features:**
  - Table semua absensi
  - Filter by tanggal, karyawan
  - Export to Excel
  - Real-time updates

#### 3. Manajemen Tugas
- **URL:** `/manajemen-glass.html`
- **Features:**
  - List semua submission
  - Download file
  - Delete submission
  - Filter by karyawan

#### 4. Tambah Karyawan
- **URL:** `/tambah-karyawan.html`
- **Features:**
  - Form input data karyawan
  - Tombol "Daftarkan Sidik Jari"
  - Real-time enrollment status
  - Success/error notifications

---

## 🔄 Real-time Communication (Socket.IO)

### Events

#### 1. data_update (Server → Client)
Broadcast setiap ada absensi baru:
```javascript
socket.on('data_update', (data) => {
  // Update UI dengan data absensi terbaru
  console.log('New attendance:', data);
});
```

#### 2. enrollment_success (Server → Client)
Broadcast ketika enrollment berhasil:
```javascript
socket.on('enrollment_success', (data) => {
  // {
  //   success: true,
  //   pegawai_id: 5,
  //   fingerprint_id: 12,
  //   message: "Sidik jari berhasil didaftarkan"
  // }
});
```

#### 3. enrollment_error (Server → Client)
Broadcast ketika enrollment gagal:
```javascript
socket.on('enrollment_error', (data) => {
  // {
  //   success: false,
  //   pegawai_id: 5,
  //   message: "Gagal mendeteksi sidik jari"
  // }
});
```

---

## 🚀 Deployment & Installation

### 1. Requirements
```bash
# Software
- Node.js v16+
- PostgreSQL 12+
- MQTT Broker (Aedes built-in)

# Hardware
- ESP32
- Fingerprint Sensor (R307/AS608)
- OLED Display (optional)
```

### 2. Installation Steps

#### Backend Setup
```bash
# Clone project
cd /path/to/project

# Install dependencies
npm install

# Setup database
sudo -u postgres psql
CREATE DATABASE db_absensi;
CREATE USER absensi_user WITH PASSWORD 'absensi_password';
GRANT ALL PRIVILEGES ON DATABASE db_absensi TO absensi_user;
\q

# Run migrations
sudo -u postgres psql -d db_absensi -f database_postgresql.sql
sudo -u postgres psql -d db_absensi -f ALTER_TABLE_ABSEN_KELUAR_LEMBUR.sql
sudo -u postgres psql -d db_absensi -f migration_add_fingerprint.sql
sudo -u postgres psql -d db_absensi -f migration_tugas_free_submissions.sql

# Start server
node server.js
```

#### Frontend Access
```
User Login: http://localhost:3000/user-login.html
Admin Login: http://localhost:3000/index-glass.html
```

#### ESP32 Setup
```cpp
// Configure WiFi
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// Configure MQTT
const char* mqtt_server = "192.168.1.XXX";  // Server IP
const int mqtt_port = 1883;
```

### 3. Network Configuration

#### Firewall (Linux)
```bash
# Allow port 3000 (HTTP)
sudo ufw allow 3000/tcp

# Allow port 1883 (MQTT)
sudo ufw allow 1883/tcp

# Check status
sudo ufw status
```

#### Access from Mobile
```
# Get server IP
hostname -I

# Access from mobile (same WiFi)
http://192.168.1.XX:3000/user-login.html
```

---

## 📈 Kelebihan Sistem

### 1. **Teknologi Modern**
- ✅ IoT Integration dengan MQTT protocol
- ✅ Progressive Web App (installable, offline-capable)
- ✅ Real-time updates dengan Socket.IO
- ✅ Glassmorphism UI design yang modern

### 2. **Skalabilitas**
- ✅ Support multiple ESP32 devices
- ✅ Database PostgreSQL yang robust
- ✅ Modular architecture (easy to extend)
- ✅ RESTful API design

### 3. **User Experience**
- ✅ Interface intuitif dan responsive
- ✅ Real-time feedback pada enrollment
- ✅ Mobile-first design
- ✅ Offline capability

### 4. **Keamanan**
- ✅ Biometric authentication (fingerprint)
- ✅ SQL injection prevention
- ✅ Session management
- ✅ Role-based access control

### 5. **Fungsionalitas Lengkap**
- ✅ Absen masuk & keluar
- ✅ Perhitungan lembur otomatis
- ✅ Manajemen tugas dengan upload file
- ✅ Enrollment fingerprint via web
- ✅ History & reporting

---

## 🔧 Pengembangan Lebih Lanjut (Future Work)

### 1. Keamanan
- [ ] Implement HTTPS (SSL/TLS)
- [ ] MQTT over TLS
- [ ] JWT Authentication
- [ ] Password hashing (bcrypt)
- [ ] CSRF protection

### 2. Fitur Tambahan
- [ ] Export to Excel/PDF
- [ ] Email notification
- [ ] Face recognition (alternative to fingerprint)
- [ ] GPS location tracking
- [ ] Shift management
- [ ] Leave/cuti management

### 3. Analytics & Reporting
- [ ] Dashboard analytics
- [ ] Attendance report per bulan
- [ ] Overtime report
- [ ] Performance metrics
- [ ] Grafik & chart

### 4. Integrasi
- [ ] WhatsApp notification via API
- [ ] Google Calendar sync
- [ ] Payroll integration
- [ ] HR system integration

### 5. Performance
- [ ] Database indexing optimization
- [ ] Query caching
- [ ] Load balancing
- [ ] CDN untuk static assets

---

## 📚 Teknologi & Library yang Digunakan

### Backend
```json
{
  "express": "^4.18.2",
  "pg": "^8.11.0",
  "aedes": "^0.50.0",
  "mqtt": "^5.0.0",
  "socket.io": "^4.6.1",
  "multer": "^1.4.5-lts.1",
  "moment-timezone": "^0.5.43",
  "webdav": "^5.3.0"
}
```

### Frontend
- Vanilla JavaScript (No framework)
- Socket.IO Client
- Service Worker API
- Fetch API
- LocalStorage/SessionStorage API

### Database
- PostgreSQL 12+
- pg (node-postgres driver)

### IoT
- ESP32 Arduino Framework
- Adafruit Fingerprint Library
- PubSubClient (MQTT)
- WiFi Library

---

## 📞 Troubleshooting

### Problem 1: ESP32 tidak connect ke MQTT
**Solution:**
```cpp
// Check WiFi connection
Serial.println(WiFi.localIP());

// Check MQTT connection
if (!mqttClient.connected()) {
  reconnectMQTT();
}
```

### Problem 2: Fingerprint enrollment gagal
**Solution:**
- Pastikan jari bersih dan kering
- Tempelkan jari dengan posisi yang sama 2x
- Cek kualitas sensor
- Cek wiring ESP32 ↔ Sensor

### Problem 3: PWA tidak install di HP
**Solution:**
- Clear browser cache
- Unregister old service worker
- Hard refresh (Ctrl+Shift+R)
- Check manifest.json valid

### Problem 4: Database connection error
**Solution:**
```bash
# Check PostgreSQL service
sudo systemctl status postgresql

# Check database exists
sudo -u postgres psql -l

# Grant permissions
sudo -u postgres psql -d db_absensi
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO absensi_user;
```

---

## 📄 Lisensi & Credits

**Developed by:** Achmad Hasan
**Institution:** [Your University Name]
**Year:** 2025
**Tugas Akhir:** Sistem Absensi Karyawan Berbasis IoT dengan Integrasi Fingerprint dan PWA

---

## 🎓 Referensi untuk Tugas Akhir

### Paper & Journal
1. IoT-based Attendance System using MQTT Protocol
2. Progressive Web Apps: A New Approach to Mobile Applications
3. Biometric Authentication in Enterprise Systems
4. Real-time Data Communication with Socket.IO

### Books
1. "IoT Systems: Architecture and Design Principles"
2. "Modern Web Development with Node.js"
3. "PostgreSQL: Up and Running"
4. "Building Progressive Web Apps"

### Documentation
- Node.js Official Docs
- PostgreSQL Documentation
- ESP32 Arduino Core Documentation
- MQTT Protocol Specification v3.1.1
- Socket.IO Documentation
- Service Worker API (MDN)

---

**Dokumentasi ini dapat digunakan sebagai referensi untuk Bab 3 (Perancangan Sistem) dan Bab 4 (Implementasi & Pengujian) pada Tugas Akhir Anda.**

**Good luck dengan tugas akhir Anda! 🚀**
