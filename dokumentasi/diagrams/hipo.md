# HIPO (Hierarchy plus Input-Process-Output)

## 1. Diagram Hierarki Sistem

```mermaid
flowchart TB
    subgraph Level0["Level 0 - Sistem Utama"]
        H0["0.0 Sistem Absensi IoT<br/>Berbasis Fingerprint"]
    end

    subgraph Level1["Level 1 - Modul Utama"]
        H1["1.0 Modul<br/>Autentikasi"]
        H2["2.0 Modul<br/>Manajemen Karyawan"]
        H3["3.0 Modul<br/>Absensi"]
        H4["4.0 Modul<br/>Cuti/Izin"]
        H5["5.0 Modul<br/>Payroll"]
        H6["6.0 Modul<br/>Laporan"]
        H7["7.0 Modul<br/>Pengaturan"]
        H8["8.0 Modul<br/>IoT/Fingerprint"]
    end

    H0 --> H1
    H0 --> H2
    H0 --> H3
    H0 --> H4
    H0 --> H5
    H0 --> H6
    H0 --> H7
    H0 --> H8
```

---

## 2. Diagram Hierarki Detail

### 2.1 Modul Autentikasi (1.0)

```mermaid
flowchart TB
    H1["1.0 Modul Autentikasi"]

    H1_1["1.1 Login Admin"]
    H1_2["1.2 Login Karyawan"]
    H1_3["1.3 Logout"]
    H1_4["1.4 Ganti Password"]
    H1_5["1.5 Reset Password"]

    H1 --> H1_1
    H1 --> H1_2
    H1 --> H1_3
    H1 --> H1_4
    H1 --> H1_5
```

### 2.2 Modul Manajemen Karyawan (2.0)

```mermaid
flowchart TB
    H2["2.0 Modul Manajemen Karyawan"]

    H2_1["2.1 Lihat Data Karyawan"]
    H2_2["2.2 Tambah Karyawan"]
    H2_3["2.3 Edit Karyawan"]
    H2_4["2.4 Hapus Karyawan"]
    H2_5["2.5 Import CSV"]
    H2_6["2.6 Upload Foto"]
    H2_7["2.7 Kelola Divisi"]

    H2 --> H2_1
    H2 --> H2_2
    H2 --> H2_3
    H2 --> H2_4
    H2 --> H2_5
    H2 --> H2_6
    H2 --> H2_7
```

### 2.3 Modul Absensi (3.0)

```mermaid
flowchart TB
    H3["3.0 Modul Absensi"]

    H3_1["3.1 Rekam Absensi Masuk"]
    H3_2["3.2 Rekam Absensi Keluar"]
    H3_3["3.3 Absensi Manual"]
    H3_4["3.4 Lihat Riwayat Absensi"]
    H3_5["3.5 Statistik Harian"]
    H3_6["3.6 Hitung Lembur"]
    H3_7["3.7 Export Data"]

    H3 --> H3_1
    H3 --> H3_2
    H3 --> H3_3
    H3 --> H3_4
    H3 --> H3_5
    H3 --> H3_6
    H3 --> H3_7
```

### 2.4 Modul Cuti/Izin (4.0)

```mermaid
flowchart TB
    H4["4.0 Modul Cuti/Izin"]

    H4_1["4.1 Ajukan Cuti"]
    H4_2["4.2 Lihat Status Pengajuan"]
    H4_3["4.3 Approve Cuti"]
    H4_4["4.4 Reject Cuti"]
    H4_5["4.5 Hapus Pengajuan"]
    H4_6["4.6 Upload Dokumen"]

    H4 --> H4_1
    H4 --> H4_2
    H4 --> H4_3
    H4 --> H4_4
    H4 --> H4_5
    H4 --> H4_6
```

### 2.5 Modul Payroll (5.0)

```mermaid
flowchart TB
    H5["5.0 Modul Payroll"]

    H5_1["5.1 Generate Payroll"]
    H5_2["5.2 Lihat Payroll"]
    H5_3["5.3 Edit Payroll"]
    H5_4["5.4 Cetak Slip Gaji"]
    H5_5["5.5 Setting Tarif"]
    H5_6["5.6 Hitung Potongan"]
    H5_7["5.7 Hitung Tunjangan"]

    H5 --> H5_1
    H5 --> H5_2
    H5 --> H5_3
    H5 --> H5_4
    H5 --> H5_5
    H5 --> H5_6
    H5 --> H5_7
```

### 2.6 Modul Laporan (6.0)

```mermaid
flowchart TB
    H6["6.0 Modul Laporan"]

    H6_1["6.1 Laporan Absensi Bulanan"]
    H6_2["6.2 Laporan Keterlambatan"]
    H6_3["6.3 Laporan Cuti"]
    H6_4["6.4 Laporan Lembur"]
    H6_5["6.5 Export Excel"]
    H6_6["6.6 Export PDF"]

    H6 --> H6_1
    H6 --> H6_2
    H6 --> H6_3
    H6 --> H6_4
    H6 --> H6_5
    H6 --> H6_6
```

### 2.7 Modul Pengaturan (7.0)

```mermaid
flowchart TB
    H7["7.0 Modul Pengaturan"]

    H7_1["7.1 Kelola Shift"]
    H7_2["7.2 Kelola Hari Libur"]
    H7_3["7.3 Kelola Jadwal Kerja"]
    H7_4["7.4 Backup OwnCloud"]
    H7_5["7.5 Setting Sistem"]
    H7_6["7.6 Notifikasi"]

    H7 --> H7_1
    H7 --> H7_2
    H7 --> H7_3
    H7 --> H7_4
    H7 --> H7_5
    H7 --> H7_6
```

### 2.8 Modul IoT/Fingerprint (8.0)

```mermaid
flowchart TB
    H8["8.0 Modul IoT/Fingerprint"]

    H8_1["8.1 Enrollment Fingerprint"]
    H8_2["8.2 Scan Fingerprint"]
    H8_3["8.3 Koneksi MQTT"]
    H8_4["8.4 Kirim Data Absensi"]
    H8_5["8.5 Reset Fingerprint"]
    H8_6["8.6 Status Perangkat"]

    H8 --> H8_1
    H8 --> H8_2
    H8 --> H8_3
    H8 --> H8_4
    H8 --> H8_5
    H8 --> H8_6
```

---

## 3. Tabel IPO (Input-Process-Output)

### 3.1 Modul Autentikasi (1.0)

| No | Fungsi | Input | Process | Output |
|----|--------|-------|---------|--------|
| 1.1 | Login Admin | Username, Password | Validasi kredensial di tabel `users` | Session admin, redirect dashboard |
| 1.2 | Login Karyawan | NIP, Password | Validasi kredensial di tabel `pegawai` | Session user, data pegawai |
| 1.3 | Logout | Session ID | Hapus session dari storage | Redirect ke halaman login |
| 1.4 | Ganti Password | Password lama, Password baru | Validasi password lama, update password | Status berhasil/gagal |
| 1.5 | Reset Password | ID Pegawai | Generate password default (NIP) | Password ter-reset |

### 3.2 Modul Manajemen Karyawan (2.0)

| No | Fungsi | Input | Process | Output |
|----|--------|-------|---------|--------|
| 2.1 | Lihat Data Karyawan | - | Query SELECT dari tabel `pegawai` | List data karyawan |
| 2.2 | Tambah Karyawan | NIP, Nama, Posisi, Email, dll | INSERT ke tabel `pegawai` | Data karyawan baru |
| 2.3 | Edit Karyawan | ID, Data update | UPDATE tabel `pegawai` | Data karyawan terupdate |
| 2.4 | Hapus Karyawan | ID Pegawai | DELETE dari tabel `pegawai` | Status penghapusan |
| 2.5 | Import CSV | File CSV | Parse CSV, batch INSERT | Jumlah data terimport |
| 2.6 | Upload Foto | ID Pegawai, File foto | Simpan file, update path di DB | URL foto tersimpan |
| 2.7 | Kelola Divisi | Nama divisi | CRUD tabel `divisi` | Data divisi |

### 3.3 Modul Absensi (3.0)

| No | Fungsi | Input | Process | Output |
|----|--------|-------|---------|--------|
| 3.1 | Rekam Absensi Masuk | Fingerprint ID, Timestamp | Cari pegawai, cek status, INSERT absensi | Data absensi masuk |
| 3.2 | Rekam Absensi Keluar | Fingerprint ID, Timestamp | Update waktu_keluar, hitung durasi | Data absensi keluar, durasi kerja |
| 3.3 | Absensi Manual | ID Pegawai, Waktu, Keterangan | INSERT/UPDATE tabel `absensi` | Data absensi manual |
| 3.4 | Lihat Riwayat Absensi | Filter (tanggal, pegawai) | Query JOIN pegawai & absensi | List riwayat absensi |
| 3.5 | Statistik Harian | Tanggal | COUNT & GROUP BY keterangan | Jumlah hadir, terlambat, izin |
| 3.6 | Hitung Lembur | ID Absensi, Waktu keluar | Bandingkan dengan jam shift | Durasi lembur (menit) |
| 3.7 | Export Data | Filter, Format | Query data, generate file | File Excel/CSV |

### 3.4 Modul Cuti/Izin (4.0)

| No | Fungsi | Input | Process | Output |
|----|--------|-------|---------|--------|
| 4.1 | Ajukan Cuti | ID Pegawai, Jenis, Tanggal, Alasan | INSERT ke `pengajuan_cuti` | Data pengajuan, notifikasi admin |
| 4.2 | Lihat Status Pengajuan | ID Pegawai | Query pengajuan by pegawai | List pengajuan & status |
| 4.3 | Approve Cuti | ID Pengajuan, Komentar | UPDATE status = 'Disetujui' | Status terupdate, notifikasi user |
| 4.4 | Reject Cuti | ID Pengajuan, Komentar | UPDATE status = 'Ditolak' | Status terupdate, notifikasi user |
| 4.5 | Hapus Pengajuan | ID Pengajuan | DELETE dari `pengajuan_cuti` | Status penghapusan |
| 4.6 | Upload Dokumen | ID Pengajuan, File | Simpan file, update path | URL dokumen |

### 3.5 Modul Payroll (5.0)

| No | Fungsi | Input | Process | Output |
|----|--------|-------|---------|--------|
| 5.1 | Generate Payroll | Bulan, Tahun | Hitung kehadiran, lembur, potongan | Data payroll per pegawai |
| 5.2 | Lihat Payroll | Filter (bulan, tahun, pegawai) | Query tabel `payroll` | List data payroll |
| 5.3 | Edit Payroll | ID Payroll, Data update | UPDATE tabel `payroll` | Payroll terupdate |
| 5.4 | Cetak Slip Gaji | ID Payroll | Generate PDF slip gaji | File PDF slip gaji |
| 5.5 | Setting Tarif | Nama setting, Nilai | UPDATE `setting_payroll` | Tarif terupdate |
| 5.6 | Hitung Potongan | Total terlambat, Absen | Kalkulasi berdasarkan tarif | Nominal potongan |
| 5.7 | Hitung Tunjangan | ID Pegawai | Ambil tunjangan dari setting | Nominal tunjangan |

### 3.6 Modul Laporan (6.0)

| No | Fungsi | Input | Process | Output |
|----|--------|-------|---------|--------|
| 6.1 | Laporan Absensi Bulanan | Bulan, Tahun | Query & agregasi data absensi | Rekap absensi bulanan |
| 6.2 | Laporan Keterlambatan | Bulan, Tahun | Filter keterangan = 'Terlambat' | List keterlambatan |
| 6.3 | Laporan Cuti | Bulan, Tahun | Query `pengajuan_cuti` | Rekap pengajuan cuti |
| 6.4 | Laporan Lembur | Bulan, Tahun | Filter status_lembur = 'Ya' | Rekap jam lembur |
| 6.5 | Export Excel | Data laporan | Generate file XLSX | File Excel |
| 6.6 | Export PDF | Data laporan | Generate file PDF | File PDF |

### 3.7 Modul Pengaturan (7.0)

| No | Fungsi | Input | Process | Output |
|----|--------|-------|---------|--------|
| 7.1 | Kelola Shift | Data shift (nama, jam) | CRUD tabel `shift` | Data shift |
| 7.2 | Kelola Hari Libur | Tanggal, Nama libur | CRUD tabel `hari_libur` | Data hari libur |
| 7.3 | Kelola Jadwal Kerja | Template jadwal mingguan | CRUD tabel `jadwal_kerja` | Data jadwal |
| 7.4 | Backup OwnCloud | Konfigurasi WebDAV | Upload database ke cloud | Status backup |
| 7.5 | Setting Sistem | Key-value setting | UPDATE `settings` | Konfigurasi tersimpan |
| 7.6 | Notifikasi | Event trigger | INSERT `notifikasi`, emit Socket.IO | Notifikasi real-time |

### 3.8 Modul IoT/Fingerprint (8.0)

| No | Fungsi | Input | Process | Output |
|----|--------|-------|---------|--------|
| 8.1 | Enrollment Fingerprint | ID Pegawai | Kirim MQTT command, simpan template | Fingerprint ID tersimpan |
| 8.2 | Scan Fingerprint | Data sidik jari | Verifikasi 1:N di sensor | Fingerprint ID match |
| 8.3 | Koneksi MQTT | Config broker | Connect ke Aedes broker | Status koneksi |
| 8.4 | Kirim Data Absensi | Fingerprint ID, Timestamp | Publish ke topic MQTT | Data absensi terkirim |
| 8.5 | Reset Fingerprint | ID Pegawai | Hapus template di sensor & DB | Fingerprint ter-reset |
| 8.6 | Status Perangkat | - | Query connected clients | List ESP32 terhubung |

---

## 4. Diagram IPO Visual

### 4.1 IPO Login Karyawan (1.2)

```mermaid
flowchart LR
    subgraph Input
        I1[NIP]
        I2[Password]
    end

    subgraph Process
        P1[Validasi Format NIP]
        P2[Query Database Pegawai]
        P3[Verifikasi Password]
        P4[Buat Session]
    end

    subgraph Output
        O1[Session Token]
        O2[Data Pegawai]
        O3[Redirect Dashboard]
    end

    I1 --> P1
    I2 --> P1
    P1 --> P2
    P2 --> P3
    P3 --> P4
    P4 --> O1
    P4 --> O2
    P4 --> O3
```

### 4.2 IPO Rekam Absensi Masuk (3.1)

```mermaid
flowchart LR
    subgraph Input
        I1[Fingerprint ID]
        I2[Timestamp]
    end

    subgraph Process
        P1[Cari Pegawai by Fingerprint]
        P2[Cek Absensi Hari Ini]
        P3[Tentukan Status<br/>Hadir/Terlambat]
        P4[INSERT ke Database]
        P5[Broadcast Socket.IO]
        P6[Buat Notifikasi]
    end

    subgraph Output
        O1[Data Absensi Tersimpan]
        O2[Update Dashboard Real-time]
        O3[Notifikasi Admin]
    end

    I1 --> P1
    I2 --> P1
    P1 --> P2
    P2 --> P3
    P3 --> P4
    P4 --> P5
    P5 --> P6
    P4 --> O1
    P5 --> O2
    P6 --> O3
```

### 4.3 IPO Generate Payroll (5.1)

```mermaid
flowchart LR
    subgraph Input
        I1[Bulan]
        I2[Tahun]
        I3[ID Pegawai<br/>atau Semua]
    end

    subgraph Process
        P1[Ambil Data Kehadiran]
        P2[Hitung Total Hadir]
        P3[Hitung Keterlambatan]
        P4[Hitung Jam Lembur]
        P5[Ambil Gaji Pokok]
        P6[Hitung Tunjangan]
        P7[Hitung Potongan]
        P8[Kalkulasi Total Gaji]
        P9[INSERT/UPDATE Payroll]
    end

    subgraph Output
        O1[Data Payroll]
        O2[Slip Gaji]
        O3[Notifikasi Karyawan]
    end

    I1 --> P1
    I2 --> P1
    I3 --> P1
    P1 --> P2
    P2 --> P3
    P3 --> P4
    P4 --> P5
    P5 --> P6
    P6 --> P7
    P7 --> P8
    P8 --> P9
    P9 --> O1
    P9 --> O2
    P9 --> O3
```

### 4.4 IPO Enrollment Fingerprint (8.1)

```mermaid
flowchart LR
    subgraph Input
        I1[ID Pegawai]
        I2[Nama Pegawai]
        I3[NIP]
    end

    subgraph Process
        P1[Generate Fingerprint ID]
        P2[Kirim MQTT Command<br/>ke ESP32]
        P3[ESP32 Capture<br/>Sidik Jari 2x]
        P4[ESP32 Simpan Template]
        P5[ESP32 Kirim Response]
        P6[Update Database Pegawai]
        P7[Broadcast Socket.IO]
    end

    subgraph Output
        O1[Fingerprint ID Tersimpan]
        O2[Status Enrollment]
        O3[Notifikasi Real-time]
    end

    I1 --> P1
    I2 --> P2
    I3 --> P2
    P1 --> P2
    P2 --> P3
    P3 --> P4
    P4 --> P5
    P5 --> P6
    P6 --> P7
    P6 --> O1
    P7 --> O2
    P7 --> O3
```

### 4.5 IPO Ajukan Cuti (4.1)

```mermaid
flowchart LR
    subgraph Input
        I1[ID Pegawai]
        I2[Jenis Cuti]
        I3[Tanggal Mulai]
        I4[Tanggal Selesai]
        I5[Alasan]
        I6[Dokumen Pendukung]
    end

    subgraph Process
        P1[Validasi Tanggal]
        P2[Hitung Jumlah Hari]
        P3[Cek Sisa Cuti]
        P4[Upload Dokumen]
        P5[INSERT Pengajuan]
        P6[Buat Notifikasi Admin]
        P7[Emit Socket.IO]
    end

    subgraph Output
        O1[Data Pengajuan]
        O2[Status: Menunggu]
        O3[Notifikasi ke Admin]
    end

    I1 --> P1
    I2 --> P1
    I3 --> P1
    I4 --> P1
    I5 --> P5
    I6 --> P4
    P1 --> P2
    P2 --> P3
    P3 --> P4
    P4 --> P5
    P5 --> P6
    P6 --> P7
    P5 --> O1
    P5 --> O2
    P7 --> O3
```

---

## 5. Ringkasan Hierarki Sistem

```
0.0 Sistem Absensi IoT Berbasis Fingerprint
│
├── 1.0 Modul Autentikasi
│   ├── 1.1 Login Admin
│   ├── 1.2 Login Karyawan
│   ├── 1.3 Logout
│   ├── 1.4 Ganti Password
│   └── 1.5 Reset Password
│
├── 2.0 Modul Manajemen Karyawan
│   ├── 2.1 Lihat Data Karyawan
│   ├── 2.2 Tambah Karyawan
│   ├── 2.3 Edit Karyawan
│   ├── 2.4 Hapus Karyawan
│   ├── 2.5 Import CSV
│   ├── 2.6 Upload Foto
│   └── 2.7 Kelola Divisi
│
├── 3.0 Modul Absensi
│   ├── 3.1 Rekam Absensi Masuk
│   ├── 3.2 Rekam Absensi Keluar
│   ├── 3.3 Absensi Manual
│   ├── 3.4 Lihat Riwayat Absensi
│   ├── 3.5 Statistik Harian
│   ├── 3.6 Hitung Lembur
│   └── 3.7 Export Data
│
├── 4.0 Modul Cuti/Izin
│   ├── 4.1 Ajukan Cuti
│   ├── 4.2 Lihat Status Pengajuan
│   ├── 4.3 Approve Cuti
│   ├── 4.4 Reject Cuti
│   ├── 4.5 Hapus Pengajuan
│   └── 4.6 Upload Dokumen
│
├── 5.0 Modul Payroll
│   ├── 5.1 Generate Payroll
│   ├── 5.2 Lihat Payroll
│   ├── 5.3 Edit Payroll
│   ├── 5.4 Cetak Slip Gaji
│   ├── 5.5 Setting Tarif
│   ├── 5.6 Hitung Potongan
│   └── 5.7 Hitung Tunjangan
│
├── 6.0 Modul Laporan
│   ├── 6.1 Laporan Absensi Bulanan
│   ├── 6.2 Laporan Keterlambatan
│   ├── 6.3 Laporan Cuti
│   ├── 6.4 Laporan Lembur
│   ├── 6.5 Export Excel
│   └── 6.6 Export PDF
│
├── 7.0 Modul Pengaturan
│   ├── 7.1 Kelola Shift
│   ├── 7.2 Kelola Hari Libur
│   ├── 7.3 Kelola Jadwal Kerja
│   ├── 7.4 Backup OwnCloud
│   ├── 7.5 Setting Sistem
│   └── 7.6 Notifikasi
│
└── 8.0 Modul IoT/Fingerprint
    ├── 8.1 Enrollment Fingerprint
    ├── 8.2 Scan Fingerprint
    ├── 8.3 Koneksi MQTT
    ├── 8.4 Kirim Data Absensi
    ├── 8.5 Reset Fingerprint
    └── 8.6 Status Perangkat
```

---

## 6. Keterangan

### Penjelasan HIPO
**HIPO (Hierarchy plus Input-Process-Output)** adalah teknik dokumentasi yang menggambarkan:
1. **Hierarki** - Struktur modul sistem dari level tertinggi ke terendah
2. **IPO** - Alur data dari Input, melalui Process, menghasilkan Output

### Kegunaan HIPO
- Memudahkan pemahaman struktur sistem
- Dokumentasi untuk maintenance dan pengembangan
- Acuan untuk testing dan debugging
- Komunikasi dengan stakeholder

### Teknologi Terkait
| Komponen | Teknologi |
|----------|-----------|
| Backend | Node.js + Express 5.1 |
| Database | PostgreSQL |
| Real-time | Socket.IO |
| IoT Communication | MQTT (Aedes Broker) |
| Frontend | HTML/CSS/JS (Glassmorphism) |
| Hardware | ESP32 + Fingerprint Sensor |

---

*Dokumentasi HIPO - Sistem Absensi IoT Berbasis Fingerprint*
*Dibuat: Januari 2026*
