# BAB 4
# HASIL DAN IMPLEMENTASI

## 4.1 Implementasi Sistem

Pada bab ini akan dibahas hasil implementasi dari perancangan sistem yang telah dibuat pada bab sebelumnya. Implementasi meliputi implementasi database, backend, frontend, perangkat IoT, dan deployment sistem.

---

### 4.1.1 Implementasi Database

Database sistem absensi IoT diimplementasikan menggunakan PostgreSQL. Berikut adalah struktur tabel yang telah dibuat:

**Gambar 4.1 Struktur Database di pgAdmin**
```
[Screenshot pgAdmin - Daftar Tabel]
```

Tabel-tabel yang diimplementasikan meliputi:

| No | Nama Tabel | Jumlah Kolom | Deskripsi |
|----|------------|--------------|-----------|
| 1 | pegawai | 12 | Data master karyawan |
| 2 | absensi | 9 | Record kehadiran harian |
| 3 | users | 5 | Akun login sistem |
| 4 | shift | 7 | Konfigurasi jam kerja |
| 5 | pengajuan_cuti | 11 | Pengajuan cuti karyawan |
| 6 | payroll | 14 | Data penggajian bulanan |
| 7 | hari_libur | 4 | Kalender hari libur |
| 8 | notifikasi | 9 | Notifikasi sistem |
| 9 | setting_payroll | 4 | Konfigurasi gaji |

**Gambar 4.2 Relasi Antar Tabel**
```
[Screenshot ERD dari pgAdmin/DBeaver]
```

---

### 4.1.2 Implementasi Backend

Backend sistem diimplementasikan menggunakan Node.js dengan framework Express.js. Berikut adalah struktur folder project:

```
server-monitoring/
├── server.js           # Entry point aplikasi
├── db.js               # Konfigurasi database
├── attendance-logic.js # Logika absensi
├── package.json        # Dependencies
├── .env                # Environment variables
├── public/             # Frontend files
│   ├── admin/          # Portal admin
│   └── user/           # Portal karyawan
├── uploads/            # File upload
└── sql/                # Database schema
```

**API Endpoint yang Diimplementasikan:**

| No | Method | Endpoint | Fungsi |
|----|--------|----------|--------|
| 1 | POST | /api/login | Autentikasi user |
| 2 | GET | /api/pegawai | Daftar pegawai |
| 3 | POST | /api/pegawai | Tambah pegawai |
| 4 | PUT | /api/pegawai/:id | Update pegawai |
| 5 | DELETE | /api/pegawai/:id | Hapus pegawai |
| 6 | GET | /api/absensi | Daftar absensi |
| 7 | GET | /api/absensi/statistik | Statistik absensi |
| 8 | POST | /api/payroll/generate | Generate payroll |
| 9 | GET | /api/cuti | Daftar pengajuan cuti |
| 10 | POST | /api/cuti | Ajukan cuti |
| 11 | PUT | /api/cuti/:id/approve | Approve cuti |
| 12 | POST | /api/backup-owncloud | Backup ke OwnCloud |

**Konfigurasi Server:**
- Port HTTP: 3000
- Port MQTT: 1883
- Database: PostgreSQL (port 5432)

---

### 4.1.3 Implementasi Frontend

Frontend diimplementasikan menggunakan HTML5, CSS3, dan JavaScript dengan desain Glassmorphism. Sistem juga mendukung Progressive Web App (PWA).

#### A. Halaman Login

**Gambar 4.3 Halaman Login**
```
[Screenshot halaman login]
```

Halaman login menampilkan form input username dan password. Setelah login berhasil, user akan diarahkan ke dashboard sesuai dengan role (admin/user).

#### B. Dashboard Admin

**Gambar 4.4 Dashboard Admin**
```
[Screenshot dashboard admin]
```

Dashboard admin menampilkan:
- Statistik kehadiran hari ini
- Grafik absensi mingguan
- Daftar absensi terbaru (real-time)
- Status koneksi perangkat IoT

#### C. Manajemen Karyawan

**Gambar 4.5 Halaman Manajemen Karyawan**
```
[Screenshot daftar karyawan]
```

**Gambar 4.6 Form Tambah Karyawan**
```
[Screenshot form tambah karyawan]
```

Fitur manajemen karyawan meliputi:
- Menampilkan daftar karyawan
- Menambah karyawan baru
- Mengedit data karyawan
- Menghapus karyawan
- Enrollment fingerprint

#### D. Manajemen Absensi

**Gambar 4.7 Halaman Manajemen Absensi**
```
[Screenshot manajemen absensi]
```

Halaman ini menampilkan:
- Daftar absensi dengan filter tanggal
- Status kehadiran (Hadir, Terlambat, Izin, Sakit)
- Durasi kerja dan lembur
- Export ke PDF/Excel

#### E. Manajemen Payroll

**Gambar 4.8 Halaman Payroll**
```
[Screenshot halaman payroll]
```

**Gambar 4.9 Slip Gaji**
```
[Screenshot slip gaji]
```

Fitur payroll meliputi:
- Generate payroll bulanan
- Kalkulasi otomatis (gaji pokok, lembur, tunjangan, potongan)
- Cetak slip gaji

#### F. Manajemen Cuti

**Gambar 4.10 Halaman Manajemen Cuti (Admin)**
```
[Screenshot manajemen cuti admin]
```

Admin dapat:
- Melihat daftar pengajuan cuti
- Approve/reject pengajuan
- Memberikan komentar

#### G. Halaman Settings

**Gambar 4.11 Halaman Settings**
```
[Screenshot halaman settings]
```

Halaman settings meliputi:
- Pengaturan perusahaan
- Konfigurasi jam kerja
- Setting payroll
- Koneksi OwnCloud

#### H. Dashboard Karyawan

**Gambar 4.12 Dashboard Karyawan**
```
[Screenshot dashboard karyawan]
```

Dashboard karyawan menampilkan:
- Ringkasan kehadiran bulan ini
- Status absensi hari ini
- Pengajuan cuti terbaru

#### I. Riwayat Absensi Karyawan

**Gambar 4.13 Riwayat Absensi Karyawan**
```
[Screenshot riwayat absensi user]
```

#### J. Pengajuan Cuti Karyawan

**Gambar 4.14 Form Pengajuan Cuti**
```
[Screenshot form pengajuan cuti]
```

---

### 4.1.4 Implementasi Perangkat IoT

Perangkat IoT diimplementasikan menggunakan ESP32 dengan komponen sebagai berikut:

**Tabel 4.1 Komponen Hardware**

| No | Komponen | Jumlah | Fungsi |
|----|----------|--------|--------|
| 1 | ESP32 DevKit V1 | 1 | Mikrokontroler utama |
| 2 | Fingerprint Sensor AS608 | 1 | Scan sidik jari |
| 3 | OLED Display SPI 0.96" | 1 | Menampilkan informasi |
| 4 | Kabel Jumper | Secukupnya | Koneksi komponen |
| 5 | Breadboard | 1 | Prototyping |

**Gambar 4.15 Skema Wiring**
```
ESP32          Fingerprint AS608
------         -----------------
3.3V    <-->   VCC (Merah)
GND     <-->   GND (Hitam)
GPIO16  <-->   TX (Kuning)
GPIO17  <-->   RX (Hijau)

ESP32          OLED SPI
------         --------
3.3V    <-->   VCC
GND     <-->   GND
GPIO18  <-->   SCK
GPIO23  <-->   SDA
GPIO5   <-->   CS
GPIO4   <-->   DC
GPIO2   <-->   RES
```

**Gambar 4.16 Foto Rangkaian Hardware**
```
[Foto rangkaian ESP32 + Fingerprint + OLED]
```

**Gambar 4.17 Tampilan OLED Saat Scan Berhasil**
```
[Foto tampilan OLED - "Selamat Datang, [Nama]"]
```

**Gambar 4.18 Tampilan OLED Saat Scan Gagal**
```
[Foto tampilan OLED - "Fingerprint Tidak Terdaftar"]
```

**Alur Kerja Perangkat IoT:**

1. Karyawan meletakkan jari di sensor fingerprint
2. ESP32 membaca dan mencocokkan template
3. Jika cocok, data dikirim ke server via MQTT
4. OLED menampilkan hasil (berhasil/gagal)
5. Server menyimpan data ke database
6. Dashboard admin ter-update secara real-time

---

### 4.1.5 Implementasi Deployment

Sistem di-deploy menggunakan Docker untuk memudahkan instalasi dan portabilitas.

**Gambar 4.19 Docker Compose Configuration**

```yaml
version: "3.8"
services:
  app:
    build: .
    container_name: server_monitoring_app
    ports:
      - "3000:3000"
      - "1883:1883"
    depends_on:
      - postgres

  postgres:
    image: postgres:15-alpine
    container_name: postgres_absensi
    environment:
      POSTGRES_USER: absensi_user
      POSTGRES_PASSWORD: absensi_password
      POSTGRES_DB: db_absensi
```

**Gambar 4.20 Docker Container Running**
```
[Screenshot docker ps atau Docker Desktop]
```

**Cara Menjalankan Sistem:**

```bash
# Clone repository
git clone [repository-url]

# Jalankan dengan Docker
docker-compose up -d

# Atau tanpa Docker
npm install
npm start
```

---

### 4.1.6 Implementasi Backup OwnCloud

Sistem terintegrasi dengan OwnCloud untuk backup data absensi.

**Gambar 4.21 Halaman Setting OwnCloud**
```
[Screenshot setting OwnCloud di aplikasi]
```

**Gambar 4.22 Hasil Backup di OwnCloud**
```
[Screenshot folder backup di OwnCloud]
```

File backup disimpan dalam format CSV dengan nama:
```
Backup-Absensi-YYYY-MM-DD_HH-mm-ss.csv
```

---

## 4.2 Hasil Pengujian

### 4.2.1 Pengujian Fungsional (Black Box Testing)

Pengujian fungsional dilakukan untuk memastikan setiap fitur berjalan sesuai dengan yang diharapkan.

**Tabel 4.2 Hasil Pengujian Login**

| No | Skenario | Input | Output Diharapkan | Hasil | Status |
|----|----------|-------|-------------------|-------|--------|
| 1 | Login dengan data valid | Username: admin, Password: benar | Redirect ke dashboard admin | Sesuai | Berhasil |
| 2 | Login dengan password salah | Username: admin, Password: salah | Pesan error "Invalid credentials" | Sesuai | Berhasil |
| 3 | Login dengan username kosong | Username: kosong | Pesan error validasi | Sesuai | Berhasil |

**Tabel 4.3 Hasil Pengujian Manajemen Karyawan**

| No | Skenario | Input | Output Diharapkan | Hasil | Status |
|----|----------|-------|-------------------|-------|--------|
| 1 | Tambah karyawan | Data lengkap | Karyawan tersimpan | Sesuai | Berhasil |
| 2 | Edit karyawan | Data yang diubah | Data ter-update | Sesuai | Berhasil |
| 3 | Hapus karyawan | Klik delete + konfirmasi | Karyawan terhapus | Sesuai | Berhasil |
| 4 | Tambah tanpa NIP | NIP kosong | Pesan error validasi | Sesuai | Berhasil |

**Tabel 4.4 Hasil Pengujian Absensi Fingerprint**

| No | Skenario | Input | Output Diharapkan | Hasil | Status |
|----|----------|-------|-------------------|-------|--------|
| 1 | Scan fingerprint terdaftar | Jari terdaftar | Absensi tercatat, OLED tampil nama | Sesuai | Berhasil |
| 2 | Scan fingerprint tidak terdaftar | Jari tidak terdaftar | OLED tampil "Tidak Terdaftar" | Sesuai | Berhasil |
| 3 | Scan tepat waktu | Scan sebelum batas toleransi | Status: Hadir | Sesuai | Berhasil |
| 4 | Scan terlambat | Scan lewat batas toleransi | Status: Terlambat | Sesuai | Berhasil |

**Tabel 4.5 Hasil Pengujian Pengajuan Cuti**

| No | Skenario | Input | Output Diharapkan | Hasil | Status |
|----|----------|-------|-------------------|-------|--------|
| 1 | Ajukan cuti | Data cuti lengkap | Status: Menunggu | Sesuai | Berhasil |
| 2 | Approve cuti | Klik approve | Status: Disetujui | Sesuai | Berhasil |
| 3 | Reject cuti | Klik reject + komentar | Status: Ditolak | Sesuai | Berhasil |

**Tabel 4.6 Hasil Pengujian Payroll**

| No | Skenario | Input | Output Diharapkan | Hasil | Status |
|----|----------|-------|-------------------|-------|--------|
| 1 | Generate payroll | Pilih bulan/tahun | Payroll ter-generate | Sesuai | Berhasil |
| 2 | Cetak slip gaji | Klik cetak | PDF slip gaji | Sesuai | Berhasil |

**Tabel 4.7 Hasil Pengujian Backup OwnCloud**

| No | Skenario | Input | Output Diharapkan | Hasil | Status |
|----|----------|-------|-------------------|-------|--------|
| 1 | Backup manual | Klik backup | File CSV terupload | Sesuai | Berhasil |
| 2 | Cek status OwnCloud | Klik cek koneksi | Status online/offline | Sesuai | Berhasil |

---

### 4.2.2 Pengujian Integrasi

**Tabel 4.8 Hasil Pengujian Integrasi**

| No | Komponen | Skenario | Hasil | Status |
|----|----------|----------|-------|--------|
| 1 | ESP32 → MQTT | Kirim data absensi | Data diterima server | Berhasil |
| 2 | MQTT → Server | Forward message | Server memproses data | Berhasil |
| 3 | Server → PostgreSQL | Insert absensi | Data tersimpan | Berhasil |
| 4 | Server → WebSocket | Broadcast update | Dashboard ter-update real-time | Berhasil |
| 5 | Server → OwnCloud | Upload backup | File tersimpan di cloud | Berhasil |

---

### 4.2.3 Pengujian Real-time

**Gambar 4.23 Pengujian Real-time Dashboard**
```
[Screenshot dashboard sebelum dan sesudah scan fingerprint]
```

Pengujian menunjukkan bahwa ketika karyawan melakukan scan fingerprint:
1. Data langsung muncul di dashboard admin dalam waktu < 1 detik
2. Notifikasi browser muncul (jika diaktifkan)
3. Statistik kehadiran ter-update otomatis

---

## 4.3 Pembahasan

### 4.3.1 Analisis Hasil Implementasi

Berdasarkan hasil pengujian, sistem absensi IoT berbasis fingerprint telah berhasil diimplementasikan dengan fitur-fitur sebagai berikut:

1. **Autentikasi**: Sistem login dengan role-based access (admin/user)
2. **Manajemen Data**: CRUD karyawan, shift, hari libur
3. **Absensi IoT**: Scan fingerprint via ESP32 dengan feedback OLED
4. **Real-time**: Update dashboard menggunakan WebSocket
5. **Payroll**: Generate gaji otomatis berdasarkan kehadiran
6. **Cuti**: Pengajuan dan approval cuti online
7. **Backup**: Integrasi dengan OwnCloud via WebDAV
8. **PWA**: Aplikasi dapat diakses seperti native app

### 4.3.2 Kelebihan Sistem

| No | Kelebihan | Penjelasan |
|----|-----------|------------|
| 1 | Real-time Monitoring | Dashboard ter-update otomatis saat ada absensi baru |
| 2 | Integrasi IoT | Absensi menggunakan fingerprint lebih akurat |
| 3 | Cloud Backup | Data aman dengan backup ke OwnCloud |
| 4 | Responsive Design | Dapat diakses dari desktop maupun mobile |
| 5 | PWA Support | Dapat diinstall sebagai aplikasi |
| 6 | Otomasi Payroll | Perhitungan gaji otomatis berdasarkan kehadiran |

### 4.3.3 Keterbatasan Sistem

| No | Keterbatasan | Penjelasan |
|----|--------------|------------|
| 1 | Koneksi Internet | Sistem membutuhkan koneksi internet untuk sinkronisasi |
| 2 | Single Sensor | Saat ini hanya mendukung 1 perangkat fingerprint |
| 3 | Kapasitas Fingerprint | Sensor AS608 maksimal 127 template |

---

## 4.4 Kesimpulan Bab

Implementasi sistem absensi IoT berbasis fingerprint telah berhasil dilakukan sesuai dengan perancangan pada bab sebelumnya. Hasil pengujian menunjukkan bahwa seluruh fitur berfungsi dengan baik dan memenuhi kebutuhan sistem.
