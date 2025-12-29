# Sistem Monitoring Absensi Karyawan

## Gambaran Umum

Sistem Monitoring Absensi Karyawan adalah aplikasi berbasis web untuk mengelola kehadiran karyawan menggunakan teknologi **fingerprint sensor** yang terintegrasi dengan **ESP32**. Sistem ini menyediakan dua portal utama:

1. **Portal Admin** - Untuk manajemen karyawan, payroll, laporan, dan konfigurasi sistem
2. **Portal Karyawan** - Untuk melihat riwayat absensi, slip gaji, dan pengajuan cuti

---

## Fitur Utama

### Portal Admin
- Dashboard dengan statistik real-time
- Manajemen data karyawan (CRUD)
- Manajemen payroll dan penggajian
- Penjadwalan shift kerja
- Manajemen hari libur
- Laporan absensi (harian, mingguan, bulanan)
- Riwayat absensi dengan timeline
- Integrasi backup ke OwnCloud/WebDAV
- Manajemen notifikasi
- Pengaturan sistem

### Portal Karyawan
- Dashboard personal
- Riwayat absensi pribadi
- Slip gaji digital
- Pengajuan cuti online
- Profil karyawan

### Fitur IoT
- Integrasi fingerprint sensor R307/R503
- Komunikasi real-time via MQTT
- Mode enrollment untuk pendaftaran sidik jari
- Deteksi kehadiran otomatis

---

## Teknologi yang Digunakan

| Komponen | Teknologi |
|----------|-----------|
| Backend | Node.js + Express.js |
| Database | PostgreSQL |
| Real-time | Socket.IO |
| IoT Protocol | MQTT (Mosquitto) |
| Hardware | ESP32 + Fingerprint Sensor |
| Frontend | HTML5, CSS3, JavaScript |
| UI Design | Glassmorphism |
| PWA | Service Worker + Web App Manifest |
| Cloud Backup | OwnCloud/WebDAV Integration |

---

## Arsitektur Sistem

```
┌─────────────────┐     MQTT      ┌──────────────────┐
│  ESP32 +        │──────────────▶│  MQTT Broker     │
│  Fingerprint    │               │  (Mosquitto)     │
└─────────────────┘               └────────┬─────────┘
                                           │
                                           ▼
┌─────────────────┐    HTTP/WS    ┌──────────────────┐
│  Web Browser    │◀─────────────▶│  Node.js Server  │
│  (Admin/User)   │               │  (Express.js)    │
└─────────────────┘               └────────┬─────────┘
                                           │
                                           ▼
                                  ┌──────────────────┐
                                  │  PostgreSQL      │
                                  │  Database        │
                                  └──────────────────┘
```

---

## Struktur Database

### Tabel Utama
1. **pegawai** - Data karyawan
2. **absensi** - Rekaman kehadiran
3. **users** - Akun pengguna (admin/karyawan)
4. **payroll** - Data penggajian
5. **pengajuan_cuti** - Pengajuan cuti karyawan
6. **shift** - Jadwal shift kerja
7. **hari_libur** - Daftar hari libur
8. **notifikasi** - Notifikasi sistem
9. **setting_payroll** - Konfigurasi penggajian

---

## Struktur Folder Proyek

```
server-monitoring/
├── server.js              # Entry point aplikasi
├── db.js                  # Konfigurasi database
├── package.json           # Dependencies
├── public/
│   ├── admin/             # Portal admin
│   │   ├── dashboard-glass.html
│   │   ├── manajemen-karyawan-glass.html
│   │   ├── payroll-glass.html
│   │   ├── laporan-glass.html
│   │   ├── riwayat-glass.html
│   │   ├── jadwal-shift-glass.html
│   │   ├── settings-glass.html
│   │   └── ...
│   ├── user/              # Portal karyawan
│   │   ├── user-dashboard.html
│   │   ├── user-absensi.html
│   │   ├── user-gaji.html
│   │   ├── user-cuti.html
│   │   └── ...
│   ├── login.html         # Halaman login
│   ├── manifest.json      # PWA manifest
│   └── service-worker.js  # Service worker
├── dokumentasi/           # Dokumentasi sistem
│   ├── README.md
│   └── diagrams/
└── uploads/               # File upload
```

---

## API Endpoints

### Authentication
- `POST /api/login` - Login pengguna
- `POST /api/register` - Registrasi pengguna baru

### Pegawai
- `GET /api/pegawai` - Daftar semua pegawai
- `GET /api/pegawai/:id` - Detail pegawai
- `POST /api/pegawai` - Tambah pegawai baru
- `PUT /api/pegawai/:id` - Update data pegawai
- `DELETE /api/pegawai/:id` - Hapus pegawai

### Absensi
- `GET /api/absensi` - Riwayat absensi
- `GET /api/absensi/today` - Absensi hari ini
- `POST /api/absensi` - Catat absensi manual
- `GET /api/absensi/stats` - Statistik absensi

### Payroll
- `GET /api/payroll` - Daftar payroll
- `POST /api/payroll/generate` - Generate payroll
- `GET /api/payroll/:id` - Detail payroll

### Cuti
- `GET /api/cuti` - Daftar pengajuan cuti
- `POST /api/cuti` - Ajukan cuti baru
- `PUT /api/cuti/:id/approve` - Setujui cuti
- `PUT /api/cuti/:id/reject` - Tolak cuti

---

## Instalasi

### Prasyarat
- Node.js v16+
- PostgreSQL 13+
- MQTT Broker (Mosquitto)
- ESP32 dengan Fingerprint Sensor

### Langkah Instalasi

1. Clone repository
```bash
git clone https://github.com/username/server-monitoring.git
cd server-monitoring
```

2. Install dependencies
```bash
npm install
```

3. Setup database
```bash
# Buat database PostgreSQL
createdb absensi_db

# Import schema (jika ada)
psql -d absensi_db -f schema.sql
```

4. Konfigurasi environment
```bash
# Edit file konfigurasi sesuai kebutuhan
# Database: db.js
# MQTT: server.js
```

5. Jalankan server
```bash
npm start
```

6. Akses aplikasi
```
http://localhost:3000
```

---

## Dokumentasi Lengkap

- [Diagram Arsitektur](diagrams/arsitektur-sistem.md)
- [Use Case Diagram](diagrams/use-case.md)
- [Entity Relationship Diagram](diagrams/erd.md)
- [Sequence Diagrams](diagrams/sequence-diagrams.md)
- [Flowcharts](diagrams/flowchart.md)

---

## Lisensi

Hak Cipta - Sistem Monitoring Absensi Karyawan
