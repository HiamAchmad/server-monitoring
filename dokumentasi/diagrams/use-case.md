# Use Case Diagram

## 1. Use Case Diagram Keseluruhan

```mermaid
flowchart LR
    subgraph Actors
        ADMIN((Admin))
        KARYAWAN((Karyawan))
        ESP32((ESP32/Sensor))
    end

    subgraph System["Sistem Monitoring Absensi"]
        subgraph Auth["Autentikasi"]
            UC1[Login]
            UC2[Logout]
            UC3[Ganti Password]
        end

        subgraph AdminUC["Manajemen Admin"]
            UC4[Kelola Karyawan]
            UC5[Kelola Payroll]
            UC6[Kelola Shift]
            UC7[Kelola Hari Libur]
            UC8[Lihat Laporan]
            UC9[Lihat Riwayat]
            UC10[Kelola Settings]
            UC11[Backup Data]
            UC12[Kelola Cuti]
            UC13[Lihat Dashboard]
            UC14[Enroll Fingerprint]
        end

        subgraph UserUC["Fitur Karyawan"]
            UC15[Lihat Dashboard]
            UC16[Lihat Absensi]
            UC17[Lihat Slip Gaji]
            UC18[Ajukan Cuti]
            UC19[Lihat Profil]
        end

        subgraph IoTUC["Fitur IoT"]
            UC20[Scan Fingerprint]
            UC21[Kirim Data Absensi]
            UC22[Mode Enrollment]
        end
    end

    ADMIN --> UC1
    ADMIN --> UC2
    ADMIN --> UC3
    ADMIN --> UC4
    ADMIN --> UC5
    ADMIN --> UC6
    ADMIN --> UC7
    ADMIN --> UC8
    ADMIN --> UC9
    ADMIN --> UC10
    ADMIN --> UC11
    ADMIN --> UC12
    ADMIN --> UC13
    ADMIN --> UC14

    KARYAWAN --> UC1
    KARYAWAN --> UC2
    KARYAWAN --> UC15
    KARYAWAN --> UC16
    KARYAWAN --> UC17
    KARYAWAN --> UC18
    KARYAWAN --> UC19
    KARYAWAN --> UC20

    ESP32 --> UC20
    ESP32 --> UC21
    ESP32 --> UC22
```

## 2. Deskripsi Use Case

### 2.1 Autentikasi

| Use Case | Aktor | Deskripsi |
|----------|-------|-----------|
| UC1 - Login | Admin, Karyawan | Pengguna memasukkan username dan password untuk masuk ke sistem |
| UC2 - Logout | Admin, Karyawan | Pengguna keluar dari sistem |
| UC3 - Ganti Password | Admin | Admin mengganti password akun |

### 2.2 Manajemen Admin

| Use Case | Aktor | Deskripsi |
|----------|-------|-----------|
| UC4 - Kelola Karyawan | Admin | CRUD data karyawan (tambah, edit, hapus, lihat) |
| UC5 - Kelola Payroll | Admin | Generate dan kelola data penggajian |
| UC6 - Kelola Shift | Admin | Atur jadwal shift kerja karyawan |
| UC7 - Kelola Hari Libur | Admin | Tambah dan kelola daftar hari libur |
| UC8 - Lihat Laporan | Admin | Lihat laporan absensi harian/mingguan/bulanan |
| UC9 - Lihat Riwayat | Admin | Lihat riwayat absensi semua karyawan |
| UC10 - Kelola Settings | Admin | Konfigurasi sistem (perusahaan, jam kerja, MQTT, dll) |
| UC11 - Backup Data | Admin | Backup data ke OwnCloud/WebDAV |
| UC12 - Kelola Cuti | Admin | Approve/reject pengajuan cuti karyawan |
| UC13 - Lihat Dashboard | Admin | Melihat statistik dan overview sistem |
| UC14 - Enroll Fingerprint | Admin | Mendaftarkan sidik jari karyawan baru |

### 2.3 Fitur Karyawan

| Use Case | Aktor | Deskripsi |
|----------|-------|-----------|
| UC15 - Lihat Dashboard | Karyawan | Melihat ringkasan absensi pribadi |
| UC16 - Lihat Absensi | Karyawan | Melihat riwayat absensi pribadi |
| UC17 - Lihat Slip Gaji | Karyawan | Melihat slip gaji bulanan |
| UC18 - Ajukan Cuti | Karyawan | Mengajukan permohonan cuti |
| UC19 - Lihat Profil | Karyawan | Melihat dan update profil pribadi |

### 2.4 Fitur IoT

| Use Case | Aktor | Deskripsi |
|----------|-------|-----------|
| UC20 - Scan Fingerprint | ESP32, Karyawan | Karyawan scan sidik jari untuk absensi |
| UC21 - Kirim Data Absensi | ESP32 | ESP32 mengirim data absensi ke server via MQTT |
| UC22 - Mode Enrollment | ESP32 | ESP32 masuk mode pendaftaran sidik jari baru |

---

## 3. Use Case Detail

### UC4 - Kelola Karyawan

```mermaid
flowchart TB
    ADMIN((Admin))

    subgraph UC4["Kelola Karyawan"]
        UC4_1[Lihat Daftar Karyawan]
        UC4_2[Tambah Karyawan]
        UC4_3[Edit Karyawan]
        UC4_4[Hapus Karyawan]
        UC4_5[Import CSV]
        UC4_6[Cari Karyawan]
    end

    ADMIN --> UC4_1
    ADMIN --> UC4_2
    ADMIN --> UC4_3
    ADMIN --> UC4_4
    ADMIN --> UC4_5
    ADMIN --> UC4_6

    UC4_2 -->|include| UC4_1
    UC4_3 -->|include| UC4_1
    UC4_4 -->|include| UC4_1
```

### UC5 - Kelola Payroll

```mermaid
flowchart TB
    ADMIN((Admin))

    subgraph UC5["Kelola Payroll"]
        UC5_1[Lihat Daftar Payroll]
        UC5_2[Generate Payroll]
        UC5_3[Edit Payroll]
        UC5_4[Cetak Slip Gaji]
        UC5_5[Setting Payroll]
    end

    ADMIN --> UC5_1
    ADMIN --> UC5_2
    ADMIN --> UC5_3
    ADMIN --> UC5_4
    ADMIN --> UC5_5

    UC5_2 -->|include| UC5_1
    UC5_4 -->|extend| UC5_1
```

### UC8 - Lihat Laporan

```mermaid
flowchart TB
    ADMIN((Admin))

    subgraph UC8["Lihat Laporan"]
        UC8_1[Laporan Harian]
        UC8_2[Laporan Mingguan]
        UC8_3[Laporan Bulanan]
        UC8_4[Export PDF]
        UC8_5[Export Excel]
        UC8_6[Cetak Laporan]
    end

    ADMIN --> UC8_1
    ADMIN --> UC8_2
    ADMIN --> UC8_3

    UC8_4 -->|extend| UC8_1
    UC8_4 -->|extend| UC8_2
    UC8_4 -->|extend| UC8_3
    UC8_5 -->|extend| UC8_1
    UC8_5 -->|extend| UC8_2
    UC8_5 -->|extend| UC8_3
    UC8_6 -->|extend| UC8_1
    UC8_6 -->|extend| UC8_2
    UC8_6 -->|extend| UC8_3
```

### UC18 - Ajukan Cuti

```mermaid
flowchart TB
    KARYAWAN((Karyawan))
    ADMIN((Admin))

    subgraph UC18["Pengajuan Cuti"]
        UC18_1[Isi Form Cuti]
        UC18_2[Pilih Tanggal]
        UC18_3[Pilih Jenis Cuti]
        UC18_4[Submit Pengajuan]
        UC18_5[Lihat Status]
        UC18_6[Review Cuti]
        UC18_7[Approve Cuti]
        UC18_8[Reject Cuti]
    end

    KARYAWAN --> UC18_1
    KARYAWAN --> UC18_5

    UC18_1 --> UC18_2
    UC18_1 --> UC18_3
    UC18_2 --> UC18_4
    UC18_3 --> UC18_4

    ADMIN --> UC18_6
    UC18_6 --> UC18_7
    UC18_6 --> UC18_8
```

---

## 4. Skenario Use Case

### Skenario UC1 - Login

**Use Case:** Login
**Aktor:** Admin / Karyawan
**Precondition:** Pengguna memiliki akun terdaftar
**Postcondition:** Pengguna berhasil masuk ke sistem

| Langkah | Aktor | Sistem |
|---------|-------|--------|
| 1 | Buka halaman login | Tampilkan form login |
| 2 | Masukkan username | - |
| 3 | Masukkan password | - |
| 4 | Klik tombol Login | Validasi kredensial |
| 5 | - | Jika valid: redirect ke dashboard |
| 5a | - | Jika tidak valid: tampilkan error |

### Skenario UC4 - Tambah Karyawan

**Use Case:** Tambah Karyawan
**Aktor:** Admin
**Precondition:** Admin sudah login
**Postcondition:** Data karyawan baru tersimpan

| Langkah | Aktor | Sistem |
|---------|-------|--------|
| 1 | Klik tombol "Tambah Karyawan" | Tampilkan form tambah karyawan |
| 2 | Isi data karyawan (NIP, Nama, dll) | - |
| 3 | Upload foto (opsional) | - |
| 4 | Klik tombol Simpan | Validasi data |
| 5 | - | Simpan ke database |
| 6 | - | Tampilkan notifikasi sukses |

### Skenario UC20 - Scan Fingerprint

**Use Case:** Scan Fingerprint
**Aktor:** Karyawan, ESP32
**Precondition:** Sidik jari sudah terdaftar
**Postcondition:** Data absensi tercatat

| Langkah | Aktor | Sistem |
|---------|-------|--------|
| 1 | Letakkan jari pada sensor | ESP32 membaca sidik jari |
| 2 | - | Cocokkan dengan database |
| 3 | - | Jika cocok: ambil data karyawan |
| 4 | - | Kirim data via MQTT |
| 5 | - | Server terima dan simpan absensi |
| 6 | - | Broadcast ke dashboard real-time |
| 7 | - | Tampilkan notifikasi di web |

---

## 5. Matriks Aktor vs Use Case

| Use Case | Admin | Karyawan | ESP32 |
|----------|:-----:|:--------:|:-----:|
| Login | ✓ | ✓ | - |
| Logout | ✓ | ✓ | - |
| Ganti Password | ✓ | - | - |
| Kelola Karyawan | ✓ | - | - |
| Kelola Payroll | ✓ | - | - |
| Kelola Shift | ✓ | - | - |
| Kelola Hari Libur | ✓ | - | - |
| Lihat Laporan | ✓ | - | - |
| Lihat Riwayat | ✓ | - | - |
| Kelola Settings | ✓ | - | - |
| Backup Data | ✓ | - | - |
| Kelola Cuti | ✓ | - | - |
| Lihat Dashboard | ✓ | ✓ | - |
| Enroll Fingerprint | ✓ | - | ✓ |
| Lihat Absensi | - | ✓ | - |
| Lihat Slip Gaji | - | ✓ | - |
| Ajukan Cuti | - | ✓ | - |
| Lihat Profil | - | ✓ | - |
| Scan Fingerprint | - | ✓ | ✓ |
| Kirim Data Absensi | - | - | ✓ |
| Mode Enrollment | - | - | ✓ |
