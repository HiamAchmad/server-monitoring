# Flowcharts

## 1. Flowchart Login

```mermaid
flowchart TD
    A([Start]) --> B[Buka Halaman Login]
    B --> C[Input Username & Password]
    C --> D[Klik Login]
    D --> E{Validasi Input}

    E -->|Kosong| F[Tampilkan Error: Field harus diisi]
    F --> C

    E -->|Valid| G[Kirim ke Server]
    G --> H{Cek Username di Database}

    H -->|Tidak Ditemukan| I[Tampilkan Error: User tidak ditemukan]
    I --> C

    H -->|Ditemukan| J{Verifikasi Password}

    J -->|Salah| K[Tampilkan Error: Password salah]
    K --> C

    J -->|Benar| L[Buat Session/Token]
    L --> M{Cek Role User}

    M -->|Admin| N[Redirect ke Admin Dashboard]
    M -->|User| O[Redirect ke User Dashboard]

    N --> P([End])
    O --> P
```

## 2. Flowchart Absensi Fingerprint

```mermaid
flowchart TD
    A([Start]) --> B[Karyawan Scan Fingerprint]
    B --> C[ESP32 Baca Data Sensor]
    C --> D{Fingerprint Terdeteksi?}

    D -->|Tidak| E[LED Merah + Buzzer]
    E --> F[Tunggu 2 detik]
    F --> B

    D -->|Ya| G[Proses Image Fingerprint]
    G --> H{Match dengan Template?}

    H -->|Tidak Cocok| I[LED Merah + Buzzer]
    I --> J[Tampilkan: Tidak Dikenali]
    J --> B

    H -->|Cocok| K[Ambil Fingerprint ID]
    K --> L[Kirim via MQTT ke Server]
    L --> M[Server Terima Data]
    M --> N[Query Pegawai dari DB]
    N --> O{Pegawai Ditemukan?}

    O -->|Tidak| P[Log Error]
    P --> Q([End])

    O -->|Ya| R[Ambil Data Shift]
    R --> S{Sudah Absen Hari Ini?}

    S -->|Ya - Ada jam_masuk| T{Ada jam_keluar?}

    T -->|Tidak| U[Update jam_keluar]
    U --> V[LED Hijau + Buzzer]
    V --> W[Broadcast ke Dashboard]
    W --> Q

    T -->|Ya| X[Tampilkan: Sudah Absen Lengkap]
    X --> Q

    S -->|Tidak| Y[Cek Waktu vs Jam Masuk Shift]
    Y --> Z{Terlambat?}

    Z -->|Ya| AA[Status = Terlambat]
    Z -->|Tidak| AB[Status = Hadir]

    AA --> AC[Insert Record Absensi]
    AB --> AC
    AC --> V
```

## 3. Flowchart Pengajuan Cuti

```mermaid
flowchart TD
    A([Start]) --> B[Karyawan Login]
    B --> C[Buka Menu Pengajuan Cuti]
    C --> D[Isi Form Cuti]
    D --> E[Pilih Tanggal Mulai & Selesai]
    E --> F[Pilih Jenis Cuti]
    F --> G[Isi Alasan]
    G --> H[Klik Submit]

    H --> I{Validasi Form}
    I -->|Invalid| J[Tampilkan Error]
    J --> D

    I -->|Valid| K{Tanggal Valid?}
    K -->|Tanggal sudah lewat| L[Error: Tanggal tidak valid]
    L --> D

    K -->|Valid| M[Simpan ke Database]
    M --> N[Status = Pending]
    N --> O[Kirim Notifikasi ke Admin]
    O --> P[Tampilkan: Pengajuan Berhasil]
    P --> Q([End - Karyawan])

    subgraph Admin["Proses Admin"]
        R([Start Admin]) --> S[Admin Login]
        S --> T[Buka Daftar Pengajuan Cuti]
        T --> U[Review Pengajuan]
        U --> V{Keputusan}

        V -->|Approve| W[Update Status = Approved]
        W --> X[Input Catatan Optional]
        X --> Y[Kirim Notifikasi ke Karyawan]

        V -->|Reject| Z[Update Status = Rejected]
        Z --> AA[Input Alasan Penolakan]
        AA --> Y

        Y --> AB([End - Admin])
    end
```

## 4. Flowchart Generate Payroll

```mermaid
flowchart TD
    A([Start]) --> B[Admin Buka Halaman Payroll]
    B --> C[Pilih Bulan & Tahun]
    C --> D[Klik Generate Payroll]
    D --> E{Payroll Sudah Ada?}

    E -->|Ya| F{Konfirmasi Overwrite?}
    F -->|Tidak| G([End])
    F -->|Ya| H[Hapus Payroll Lama]

    E -->|Tidak| I[Ambil Daftar Pegawai Aktif]
    H --> I

    I --> J[Loop: Untuk Setiap Pegawai]
    J --> K[Hitung Hari Kerja Bulan Ini]
    K --> L[Query Absensi Pegawai]
    L --> M[Hitung Hari Hadir]
    M --> N[Hitung Hari Terlambat]
    N --> O[Hitung Hari Alpha]

    O --> P[Ambil Gaji Pokok]
    P --> Q[Hitung Tunjangan]
    Q --> R[Hitung Potongan Alpha]
    R --> S[Hitung Potongan Terlambat]
    S --> T[Hitung Lembur]
    T --> U[Total = Gaji Pokok + Tunjangan + Lembur - Potongan]

    U --> V[Simpan ke Tabel Payroll]
    V --> W{Masih Ada Pegawai?}
    W -->|Ya| J
    W -->|Tidak| X[Tampilkan Ringkasan]
    X --> Y([End])
```

## 5. Flowchart Enrollment Fingerprint

```mermaid
flowchart TD
    A([Start]) --> B[Admin Buka Detail Karyawan]
    B --> C[Klik Enroll Fingerprint]
    C --> D[Kirim Command ke ESP32 via MQTT]
    D --> E[ESP32 Masuk Mode Enrollment]
    E --> F[LED Kuning Menyala]

    F --> G[Tunggu Scan Jari Pertama]
    G --> H{Jari Terdeteksi?}
    H -->|Timeout 30s| I[Batalkan Enrollment]
    I --> J([End - Gagal])

    H -->|Ya| K[Capture Template 1]
    K --> L[Kirim Progress ke Server]
    L --> M[Tampilkan: Scan Jari Lagi]

    M --> N[Tunggu Scan Jari Kedua]
    N --> O{Jari Terdeteksi?}
    O -->|Timeout 30s| I

    O -->|Ya| P[Capture Template 2]
    P --> Q{Template Match?}
    Q -->|Tidak| R[Error: Jari Tidak Sama]
    R --> G

    Q -->|Ya| S[Gabungkan Template]
    S --> T[Simpan ke Flash ESP32]
    T --> U[Generate Fingerprint ID]
    U --> V[Kirim ke Server via MQTT]
    V --> W[Update Database Pegawai]
    W --> X[LED Hijau + Buzzer Sukses]
    X --> Y[Tampilkan: Enrollment Berhasil]
    Y --> Z([End - Sukses])
```

## 6. Flowchart Backup Data

```mermaid
flowchart TD
    A([Start]) --> B[Admin Buka Settings]
    B --> C[Klik Backup ke OwnCloud]
    C --> D{Konfigurasi OwnCloud Lengkap?}

    D -->|Tidak| E[Tampilkan Error: Konfigurasi tidak lengkap]
    E --> F[Isi URL, Username, Password]
    F --> D

    D -->|Ya| G[Mulai Proses Backup]
    G --> H[Export Data Pegawai]
    H --> I[Export Data Absensi]
    I --> J[Export Data Payroll]
    J --> K[Export Data Cuti]
    K --> L[Export Data Settings]

    L --> M[Compress ke ZIP]
    M --> N[Generate Nama File: backup_YYYY-MM-DD.zip]
    N --> O[Upload ke OwnCloud via WebDAV]

    O --> P{Upload Berhasil?}
    P -->|Tidak| Q[Tampilkan Error]
    Q --> R[Log Error ke Database]
    R --> S([End - Gagal])

    P -->|Ya| T[Log Backup Sukses]
    T --> U[Tampilkan: Backup Berhasil]
    U --> V[Tampilkan Link Download]
    V --> W([End - Sukses])
```

## 7. Flowchart Laporan Absensi

```mermaid
flowchart TD
    A([Start]) --> B[Admin Buka Halaman Laporan]
    B --> C[Pilih Jenis Laporan]

    C --> D{Jenis Laporan}
    D -->|Harian| E[Pilih Tanggal]
    D -->|Mingguan| F[Pilih Minggu]
    D -->|Bulanan| G[Pilih Bulan & Tahun]

    E --> H[Query Data Absensi]
    F --> H
    G --> H

    H --> I[Aggregate Data per Pegawai]
    I --> J[Hitung Statistik]
    J --> K[Format Data Laporan]
    K --> L[Tampilkan Preview]

    L --> M{Aksi Selanjutnya?}
    M -->|Lihat Saja| N([End])
    M -->|Cetak| O[Generate Print View]
    O --> P[window.print]
    P --> N
    M -->|Export PDF| Q[Generate PDF]
    Q --> R[Download File]
    R --> N
    M -->|Export Excel| S[Generate Excel/CSV]
    S --> R
```

## 8. Flowchart Manajemen Shift

```mermaid
flowchart TD
    A([Start]) --> B[Admin Buka Jadwal Shift]
    B --> C{Aksi?}

    C -->|Lihat| D[Tampilkan Daftar Shift]
    D --> E([End])

    C -->|Tambah| F[Klik Tambah Shift]
    F --> G[Isi Form: Nama, Jam Masuk, Jam Keluar]
    G --> H[Input Toleransi Terlambat]
    H --> I{Validasi}
    I -->|Invalid| J[Tampilkan Error]
    J --> G
    I -->|Valid| K[Simpan ke Database]
    K --> L[Refresh Daftar]
    L --> E

    C -->|Edit| M[Pilih Shift]
    M --> N[Edit Data]
    N --> O[Klik Simpan]
    O --> P[Update Database]
    P --> L

    C -->|Hapus| Q[Pilih Shift]
    Q --> R{Ada Pegawai Menggunakan?}
    R -->|Ya| S[Error: Shift Sedang Digunakan]
    S --> E
    R -->|Tidak| T[Konfirmasi Hapus]
    T --> U[Delete dari Database]
    U --> L

    C -->|Assign Pegawai| V[Pilih Pegawai]
    V --> W[Pilih Shift]
    W --> X[Update shift_id Pegawai]
    X --> L
```

## 9. Flowchart Hari Libur

```mermaid
flowchart TD
    A([Start]) --> B[Admin Buka Menu Hari Libur]
    B --> C{Aksi?}

    C -->|Lihat| D[Tampilkan Kalender Libur]
    D --> E([End])

    C -->|Tambah| F[Klik Tambah Hari Libur]
    F --> G[Pilih Tanggal]
    G --> H[Input Nama Hari Libur]
    H --> I[Input Keterangan Optional]
    I --> J{Tanggal Sudah Ada?}
    J -->|Ya| K[Error: Tanggal Sudah Terdaftar]
    K --> G
    J -->|Tidak| L[Simpan ke Database]
    L --> M[Refresh Kalender]
    M --> E

    C -->|Hapus| N[Pilih Hari Libur]
    N --> O[Konfirmasi Hapus]
    O --> P[Delete dari Database]
    P --> M
```

## 10. Flowchart User Dashboard

```mermaid
flowchart TD
    A([Start]) --> B[User Login Berhasil]
    B --> C[Redirect ke Dashboard]
    C --> D[Load Data User]

    D --> E[Query Profil Pegawai]
    E --> F[Query Absensi Hari Ini]
    F --> G[Query Absensi Bulan Ini]
    G --> H[Query Pengajuan Cuti Pending]

    H --> I[Render Dashboard]
    I --> J[Tampilkan Profil]
    J --> K[Tampilkan Status Absensi Hari Ini]
    K --> L[Tampilkan Statistik Bulanan]
    L --> M[Tampilkan Notifikasi]

    M --> N{User Action?}
    N -->|Lihat Absensi| O[Redirect ke Halaman Absensi]
    N -->|Lihat Gaji| P[Redirect ke Halaman Gaji]
    N -->|Ajukan Cuti| Q[Redirect ke Halaman Cuti]
    N -->|Logout| R[Clear Session]
    R --> S[Redirect ke Login]

    O --> T([End])
    P --> T
    Q --> T
    S --> T
```

## 11. Flowchart System Startup

```mermaid
flowchart TD
    A([Server Start]) --> B[Load Environment Variables]
    B --> C[Initialize Database Connection]
    C --> D{DB Connected?}

    D -->|No| E[Log Error]
    E --> F[Retry Connection]
    F --> G{Max Retries?}
    G -->|No| C
    G -->|Yes| H[Exit with Error]

    D -->|Yes| I[Initialize MQTT Client]
    I --> J{MQTT Connected?}
    J -->|No| K[Log Warning: MQTT Offline]
    K --> L[Continue without MQTT]

    J -->|Yes| M[Subscribe to Topics]
    M --> N[Setup MQTT Handlers]

    L --> O[Initialize Express App]
    N --> O
    O --> P[Setup Middleware]
    P --> Q[Setup Routes]
    Q --> R[Initialize Socket.IO]
    R --> S[Start HTTP Server]
    S --> T[Server Ready on Port 3000]
    T --> U([Running])
```

## 12. Flowchart Real-time Updates

```mermaid
flowchart TD
    A([Event Trigger]) --> B{Event Type?}

    B -->|MQTT Absensi| C[Receive MQTT Message]
    C --> D[Parse JSON Data]
    D --> E[Validate Data]
    E --> F[Save to Database]
    F --> G[Emit Socket Event]

    B -->|Socket Connect| H[Client Connects]
    H --> I[Authenticate Token]
    I --> J{Valid?}
    J -->|No| K[Disconnect]
    J -->|Yes| L[Join Room by Role]
    L --> M[Send Initial Data]

    B -->|Data Update| N[Database Updated]
    N --> O[Determine Affected Clients]
    O --> P[Emit to Specific Room]

    G --> Q[Broadcast to Admin Room]
    Q --> R[Update Dashboard Stats]
    R --> S[Update Table/Timeline]
    S --> T[Show Toast Notification]

    M --> U([Client Updated])
    P --> U
    T --> U
```
