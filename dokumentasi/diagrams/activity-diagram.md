# Activity Diagram

## 1. Activity Diagram Login

### 1.1 Login Admin

```mermaid
flowchart TD
    Start([Mulai]) --> A[Buka Halaman Login Admin]
    A --> B[Tampilkan Form Login]
    B --> C[Input Username & Password]
    C --> D[Klik Tombol Login]
    D --> E{Validasi Input}

    E -->|Kosong| F[Tampilkan Pesan Error]
    F --> C

    E -->|Terisi| G[Kirim Request ke Server]
    G --> H{Cek Username di Database}

    H -->|Tidak Ditemukan| I[Tampilkan 'Username tidak ditemukan']
    I --> C

    H -->|Ditemukan| J{Verifikasi Password}

    J -->|Salah| K[Tampilkan 'Password salah']
    K --> C

    J -->|Benar| L[Buat Session]
    L --> M[Simpan Token di Storage]
    M --> N[Redirect ke Dashboard Admin]
    N --> End([Selesai])
```

### 1.2 Login Karyawan

```mermaid
flowchart TD
    Start([Mulai]) --> A[Buka Halaman Login User]
    A --> B[Tampilkan Form Login]
    B --> C[Input NIP & Password]
    C --> D[Klik Tombol Login]
    D --> E{Validasi Input}

    E -->|Kosong| F[Tampilkan Pesan Error]
    F --> C

    E -->|Terisi| G[Kirim Request ke Server]
    G --> H{Cek NIP di Database}

    H -->|Tidak Ditemukan| I[Tampilkan 'NIP tidak terdaftar']
    I --> C

    H -->|Ditemukan| J{Cek Status Pegawai}

    J -->|Tidak Aktif| K[Tampilkan 'Akun tidak aktif']
    K --> C

    J -->|Aktif| L{Verifikasi Password}

    L -->|Salah| M[Tampilkan 'Password salah']
    M --> C

    L -->|Benar| N[Buat Session]
    N --> O[Simpan Data User di Storage]
    O --> P[Redirect ke Dashboard User]
    P --> End([Selesai])
```

---

## 2. Activity Diagram Absensi Fingerprint

### 2.1 Absensi Masuk

```mermaid
flowchart TD
    Start([Mulai]) --> A[Karyawan Tempelkan Jari]
    A --> B[Sensor Capture Sidik Jari]
    B --> C{Kualitas Image OK?}

    C -->|Tidak| D[Tampilkan 'Coba Lagi']
    D --> A

    C -->|Ya| E[Proses Fingerprint Matching]
    E --> F{Sidik Jari Cocok?}

    F -->|Tidak| G[LED Merah + Buzzer]
    G --> H[Tampilkan 'Tidak Dikenali']
    H --> End1([Selesai - Gagal])

    F -->|Ya| I[Ambil Fingerprint ID]
    I --> J[Kirim Data via MQTT]
    J --> K[Server Terima Data]
    K --> L[Cari Pegawai by Fingerprint ID]
    L --> M{Pegawai Ditemukan?}

    M -->|Tidak| N[Log Error]
    N --> End2([Selesai - Error])

    M -->|Ya| O{Sudah Absen Hari Ini?}

    O -->|Ya - Belum Keluar| P[Proses Absen Keluar]
    P --> Q[Update Waktu Keluar]
    Q --> R[Hitung Durasi Kerja]
    R --> S{Durasi > 8 Jam?}
    S -->|Ya| T[Set Status Lembur]
    S -->|Tidak| U[Set Status Normal]
    T --> V[Simpan ke Database]
    U --> V

    O -->|Belum| W[Ambil Data Shift]
    W --> X{Jam Sekarang > Jam Masuk + Toleransi?}

    X -->|Ya| Y[Set Status Terlambat]
    X -->|Tidak| Z[Set Status Hadir]

    Y --> AA[Simpan Absensi Masuk]
    Z --> AA
    AA --> V

    V --> AB[LED Hijau + Buzzer]
    AB --> AC[Broadcast Socket.IO]
    AC --> AD[Buat Notifikasi]
    AD --> End3([Selesai - Sukses])
```

### 2.2 Flowchart Sederhana Absensi

```mermaid
flowchart LR
    A[Scan Jari] --> B{Cocok?}
    B -->|Ya| C{Sudah Absen?}
    B -->|Tidak| D[Gagal]
    C -->|Belum| E[Absen Masuk]
    C -->|Sudah| F[Absen Keluar]
    E --> G[Sukses]
    F --> G
```

---

## 3. Activity Diagram Enrollment Fingerprint

```mermaid
flowchart TD
    Start([Mulai]) --> A[Admin Buka Halaman Tambah Karyawan]
    A --> B[Isi Data Karyawan]
    B --> C[Klik 'Mulai Enrollment']
    C --> D[Kirim Request ke Server]
    D --> E[Server Generate Fingerprint ID]
    E --> F[Server Kirim MQTT ke ESP32]
    F --> G[ESP32 Masuk Mode Enrollment]
    G --> H[Tampilkan 'Tempelkan Jari...']

    H --> I[Karyawan Scan Jari Pertama]
    I --> J{Capture Berhasil?}

    J -->|Tidak| K[Tampilkan 'Coba Lagi']
    K --> I

    J -->|Ya| L[Simpan Template 1]
    L --> M[Kirim Progress ke Server]
    M --> N[Tampilkan 'Scan Jari Sekali Lagi']

    N --> O[Karyawan Scan Jari Kedua]
    O --> P{Capture Berhasil?}

    P -->|Tidak| Q[Tampilkan 'Coba Lagi']
    Q --> O

    P -->|Ya| R{Template 1 & 2 Cocok?}

    R -->|Tidak| S[Tampilkan 'Jari Tidak Sama']
    S --> T[Reset Enrollment]
    T --> I

    R -->|Ya| U[Gabung & Simpan Template]
    U --> V[Kirim Response ke Server]
    V --> W[Server Update Database]
    W --> X[Broadcast Socket.IO]
    X --> Y[Tampilkan 'Enrollment Berhasil']
    Y --> Z[Admin Simpan Data Karyawan]
    Z --> End([Selesai])
```

---

## 4. Activity Diagram Pengajuan Cuti

### 4.1 Karyawan Mengajukan Cuti

```mermaid
flowchart TD
    Start([Mulai]) --> A[Login ke Portal Karyawan]
    A --> B[Buka Menu Pengajuan Izin]
    B --> C[Tampilkan Form Pengajuan]
    C --> D[Pilih Jenis Cuti]
    D --> E[Pilih Tanggal Mulai]
    E --> F[Pilih Tanggal Selesai]
    F --> G[Sistem Hitung Jumlah Hari]
    G --> H[Input Alasan]
    H --> I{Upload Dokumen?}

    I -->|Ya| J[Upload File Pendukung]
    I -->|Tidak| K[Skip Upload]

    J --> L[Klik Submit]
    K --> L

    L --> M{Validasi Form}

    M -->|Tidak Valid| N[Tampilkan Error]
    N --> D

    M -->|Valid| O[Kirim ke Server]
    O --> P[Simpan ke Database]
    P --> Q[Set Status = Menunggu]
    Q --> R[Buat Notifikasi Admin]
    R --> S[Tampilkan 'Pengajuan Terkirim']
    S --> End([Selesai])
```

### 4.2 Admin Approval Cuti

```mermaid
flowchart TD
    Start([Mulai]) --> A[Admin Login]
    A --> B[Buka Menu Cuti/Izin]
    B --> C[Tampilkan Daftar Pengajuan]
    C --> D[Filter Status = Menunggu]
    D --> E[Pilih Pengajuan]
    E --> F[Lihat Detail Pengajuan]
    F --> G{Keputusan Admin}

    G -->|Setujui| H[Klik Tombol Approve]
    H --> I[Input Komentar Optional]
    I --> J[Update Status = Disetujui]
    J --> K[Update Sisa Cuti Karyawan]
    K --> L[Buat Notifikasi ke Karyawan]
    L --> M[Tampilkan 'Berhasil Disetujui']

    G -->|Tolak| N[Klik Tombol Reject]
    N --> O[Input Alasan Penolakan]
    O --> P[Update Status = Ditolak]
    P --> Q[Buat Notifikasi ke Karyawan]
    Q --> R[Tampilkan 'Pengajuan Ditolak']

    M --> End([Selesai])
    R --> End
```

---

## 5. Activity Diagram Generate Payroll

```mermaid
flowchart TD
    Start([Mulai]) --> A[Admin Buka Menu Payroll]
    A --> B[Pilih Bulan & Tahun]
    B --> C[Klik Generate Payroll]
    C --> D[Kirim Request ke Server]
    D --> E[Ambil Daftar Pegawai Aktif]
    E --> F[Loop: Untuk Setiap Pegawai]

    F --> G[Hitung Total Hari Kerja]
    G --> H[Hitung Hari Hadir]
    H --> I[Hitung Hari Terlambat]
    I --> J[Hitung Hari Tidak Hadir]
    J --> K[Hitung Total Jam Lembur]

    K --> L[Ambil Gaji Pokok]
    L --> M[Hitung Uang Lembur]
    M --> N[Hitung Tunjangan]
    N --> O[Hitung Potongan Terlambat]
    O --> P[Hitung Potongan Tidak Hadir]

    P --> Q[Total = Gaji Pokok + Tunjangan + Lembur - Potongan]
    Q --> R[Simpan ke Tabel Payroll]

    R --> S{Masih Ada Pegawai?}
    S -->|Ya| F
    S -->|Tidak| T[Tampilkan Hasil Generate]

    T --> U[Admin Review Payroll]
    U --> V{Approve?}

    V -->|Ya| W[Set Status = Diproses]
    W --> X[Kirim Notifikasi ke Karyawan]
    X --> End([Selesai])

    V -->|Edit| Y[Edit Manual Payroll]
    Y --> U
```

---

## 6. Activity Diagram Kelola Karyawan

### 6.1 Tambah Karyawan

```mermaid
flowchart TD
    Start([Mulai]) --> A[Admin Buka Menu Tambah Karyawan]
    A --> B[Tampilkan Form Input]
    B --> C[Input NIP]
    C --> D[Input Nama Lengkap]
    D --> E[Pilih Posisi/Jabatan]
    E --> F[Input Email]
    F --> G[Input No. Telepon]
    G --> H[Pilih Divisi]
    H --> I[Input Gaji Pokok]
    I --> J{Upload Foto?}

    J -->|Ya| K[Pilih File Foto]
    J -->|Tidak| L[Skip Foto]

    K --> M{Enrollment Fingerprint?}
    L --> M

    M -->|Ya| N[Proses Enrollment]
    N --> O{Enrollment Berhasil?}
    O -->|Tidak| P[Tampilkan Error]
    P --> M
    O -->|Ya| Q[Klik Simpan]

    M -->|Tidak| Q

    Q --> R{Validasi Data}
    R -->|Error| S[Tampilkan Pesan Error]
    S --> C

    R -->|Valid| T[Simpan ke Database]
    T --> U[Tampilkan 'Berhasil Ditambahkan']
    U --> End([Selesai])
```

### 6.2 Edit Karyawan

```mermaid
flowchart TD
    Start([Mulai]) --> A[Admin Buka Manajemen Karyawan]
    A --> B[Cari/Pilih Karyawan]
    B --> C[Klik Tombol Edit]
    C --> D[Tampilkan Form dengan Data Existing]
    D --> E[Edit Data yang Diperlukan]
    E --> F[Klik Simpan]
    F --> G{Validasi Data}

    G -->|Error| H[Tampilkan Error]
    H --> E

    G -->|Valid| I[Update Database]
    I --> J[Tampilkan 'Berhasil Diupdate']
    J --> End([Selesai])
```

### 6.3 Hapus Karyawan

```mermaid
flowchart TD
    Start([Mulai]) --> A[Admin Pilih Karyawan]
    A --> B[Klik Tombol Hapus]
    B --> C[Tampilkan Dialog Konfirmasi]
    C --> D{Konfirmasi?}

    D -->|Batal| E[Tutup Dialog]
    E --> End1([Selesai])

    D -->|Ya, Hapus| F{Ada Data Terkait?}

    F -->|Ya| G[Hapus Data Absensi]
    G --> H[Hapus Data Cuti]
    H --> I[Hapus Data Payroll]
    I --> J[Hapus Fingerprint dari Sensor]
    J --> K[Hapus Data Pegawai]

    F -->|Tidak| K

    K --> L[Tampilkan 'Berhasil Dihapus']
    L --> End2([Selesai])
```

---

## 7. Activity Diagram Lihat Laporan

```mermaid
flowchart TD
    Start([Mulai]) --> A[Admin Buka Menu Laporan]
    A --> B[Pilih Jenis Laporan]
    B --> C{Jenis Laporan}

    C -->|Absensi Bulanan| D1[Pilih Bulan & Tahun]
    C -->|Keterlambatan| D2[Pilih Periode]
    C -->|Lembur| D3[Pilih Periode]
    C -->|Cuti| D4[Pilih Periode]

    D1 --> E[Klik Generate Laporan]
    D2 --> E
    D3 --> E
    D4 --> E

    E --> F[Kirim Request ke Server]
    F --> G[Query Database]
    G --> H[Proses & Agregasi Data]
    H --> I[Tampilkan Laporan di Web]

    I --> J{Aksi Selanjutnya}

    J -->|Print| K[Buka Dialog Print]
    K --> L[Cetak Laporan]
    L --> End([Selesai])

    J -->|Export Excel| M[Generate File Excel]
    M --> N[Download File]
    N --> End

    J -->|Export PDF| O[Generate File PDF]
    O --> P[Download File]
    P --> End

    J -->|Selesai| End
```

---

## 8. Activity Diagram Backup OwnCloud

```mermaid
flowchart TD
    Start([Mulai]) --> A[Admin Buka Menu Settings]
    A --> B[Pilih Tab Backup]
    B --> C[Klik Backup ke OwnCloud]
    C --> D[Tampilkan Progress]

    D --> E[Ambil Semua Data dari Database]
    E --> F[Konversi ke Format JSON]
    F --> G[Compress Data]
    G --> H[Generate Nama File dengan Timestamp]

    H --> I{Cek Koneksi OwnCloud}

    I -->|Gagal| J[Tampilkan Error Koneksi]
    J --> End1([Selesai - Gagal])

    I -->|Berhasil| K[Upload File ke OwnCloud]
    K --> L{Upload Berhasil?}

    L -->|Tidak| M[Tampilkan Error Upload]
    M --> N{Retry?}
    N -->|Ya| K
    N -->|Tidak| End2([Selesai - Gagal])

    L -->|Ya| O[Simpan Log Backup]
    O --> P[Tampilkan 'Backup Berhasil']
    P --> Q[Tampilkan Link File]
    Q --> End3([Selesai - Sukses])
```

---

## 9. Activity Diagram Notifikasi Real-time

```mermaid
flowchart TD
    Start([Mulai]) --> A[Event Terjadi]
    A --> B{Jenis Event}

    B -->|Absensi Baru| C1[Buat Notif untuk Admin]
    B -->|Pengajuan Cuti| C2[Buat Notif untuk Admin]
    B -->|Approval Cuti| C3[Buat Notif untuk Karyawan]
    B -->|Payroll Ready| C4[Buat Notif untuk Karyawan]

    C1 --> D[Simpan ke Database]
    C2 --> D
    C3 --> D
    C4 --> D

    D --> E[Emit via Socket.IO]
    E --> F{Penerima Online?}

    F -->|Ya| G[Tampilkan Toast Notification]
    G --> H[Update Badge Counter]
    H --> I[Update Dropdown Notifikasi]

    F -->|Tidak| J[Notifikasi Tersimpan]
    J --> K[Tampilkan Saat User Login]

    I --> End([Selesai])
    K --> End
```

---

## 10. Ringkasan Activity Diagram

| No | Activity Diagram | Aktor | Deskripsi |
|----|------------------|-------|-----------|
| 1 | Login Admin | Admin | Proses autentikasi admin |
| 2 | Login Karyawan | Karyawan | Proses autentikasi karyawan |
| 3 | Absensi Masuk | Karyawan, ESP32 | Proses clock-in via fingerprint |
| 4 | Enrollment Fingerprint | Admin, Karyawan, ESP32 | Pendaftaran sidik jari baru |
| 5 | Pengajuan Cuti | Karyawan | Mengajukan cuti/izin |
| 6 | Approval Cuti | Admin | Menyetujui/menolak cuti |
| 7 | Generate Payroll | Admin | Membuat data penggajian |
| 8 | Tambah Karyawan | Admin | Menambah data karyawan baru |
| 9 | Edit Karyawan | Admin | Mengubah data karyawan |
| 10 | Hapus Karyawan | Admin | Menghapus data karyawan |
| 11 | Lihat Laporan | Admin | Melihat dan export laporan |
| 12 | Backup OwnCloud | Admin | Backup data ke cloud |
| 13 | Notifikasi | Sistem | Pengiriman notifikasi real-time |

---

*Dokumentasi Activity Diagram - Sistem Absensi IoT Berbasis Fingerprint*
