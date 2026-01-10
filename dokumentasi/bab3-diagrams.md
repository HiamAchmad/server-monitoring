# BAB 3 - DIAGRAM UML
## Sistem Absensi IoT Berbasis Fingerprint

---

## Daftar Isi

1. [Use Case Diagram](#1-use-case-diagram)
2. [Activity Diagram](#2-activity-diagram)
3. [Sequence Diagram](#3-sequence-diagram)
4. [Class Diagram](#4-class-diagram)
5. [ERD (Entity Relationship Diagram)](#5-entity-relationship-diagram-erd)
6. [State Diagram](#6-state-diagram)
7. [Component Diagram](#7-component-diagram)
8. [Deployment Diagram](#8-deployment-diagram)
9. [Arsitektur Sistem](#9-arsitektur-sistem)

---

## 1. Use Case Diagram

**Deskripsi:** Menggambarkan interaksi antara aktor (Admin, Karyawan, ESP32) dengan fitur-fitur sistem.

```mermaid
flowchart LR
    subgraph Aktor
        ADMIN((Admin))
        KARYAWAN((Karyawan))
        ESP32((ESP32))
    end

    subgraph Sistem["Sistem Absensi IoT"]
        UC1[Login/Logout]
        UC2[Kelola Karyawan]
        UC3[Kelola Payroll]
        UC4[Kelola Shift]
        UC5[Kelola Cuti]
        UC6[Lihat Laporan]
        UC7[Enroll Fingerprint]
        UC8[Lihat Dashboard]
        UC9[Backup ke OwnCloud]
        UC10[Lihat Absensi]
        UC11[Lihat Slip Gaji]
        UC12[Ajukan Cuti]
        UC13[Scan Fingerprint]
        UC14[Kirim Data Absensi]
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

    KARYAWAN --> UC1
    KARYAWAN --> UC8
    KARYAWAN --> UC10
    KARYAWAN --> UC11
    KARYAWAN --> UC12
    KARYAWAN --> UC13

    ESP32 --> UC13
    ESP32 --> UC14
```

**Keterangan Aktor:**
| Aktor | Deskripsi |
|-------|-----------|
| Admin | Mengelola sistem, karyawan, payroll, dan cuti |
| Karyawan | Melakukan absensi dan melihat data pribadi |
| ESP32 | Perangkat IoT untuk scan fingerprint |

---

## 2. Activity Diagram

### 2.1 Proses Login

**Deskripsi:** Alur proses user login ke sistem.

```mermaid
flowchart TD
    A([Mulai]) --> B[Buka Halaman Login]
    B --> C[Input Username & Password]
    C --> D[Klik Login]
    D --> E{Valid?}
    E -->|Ya| F{Cek Role}
    E -->|Tidak| G[Tampilkan Error]
    G --> C
    F -->|Admin| H[Dashboard Admin]
    F -->|User| I[Dashboard User]
    H --> J([Selesai])
    I --> J
```

---

### 2.2 Proses Absensi Fingerprint

**Deskripsi:** Alur karyawan melakukan absensi menggunakan fingerprint.

```mermaid
flowchart TD
    A([Mulai]) --> B[Letakkan Jari di Sensor]
    B --> C[Sensor Capture Fingerprint]
    C --> D{Fingerprint Cocok?}

    D -->|Tidak| E[OLED: Tidak Terdaftar]
    E --> F([Gagal])

    D -->|Ya| G[OLED: Tampilkan Nama]
    G --> H{Jam Masuk/Pulang?}

    H -->|Masuk| I{Terlambat?}
    I -->|Tidak| J[Status: Hadir]
    I -->|Ya| K[Status: Terlambat]

    H -->|Pulang| L{Sudah Absen Masuk?}
    L -->|Ya| M[Catat Jam Pulang]
    L -->|Tidak| N[OLED: Belum Absen Masuk]
    N --> F

    J --> O[Simpan ke Database]
    K --> O
    M --> O
    O --> P[Kirim via MQTT]
    P --> Q[OLED: Berhasil]
    Q --> R([Sukses])
```

---

### 2.3 Proses Pengajuan Cuti

**Deskripsi:** Alur karyawan mengajukan cuti hingga diproses admin.

```mermaid
flowchart TD
    A([Mulai]) --> B[Buka Menu Cuti]
    B --> C[Isi Form Cuti]
    C --> D[Klik Submit]
    D --> E{Data Valid?}
    E -->|Tidak| F[Tampilkan Error]
    F --> C
    E -->|Ya| G[Simpan - Status: Menunggu]
    G --> H[Notifikasi ke Admin]
    H --> I([Selesai - Karyawan])

    H --> J[Admin Review]
    J --> K{Keputusan}
    K -->|Setuju| L[Status: Disetujui]
    K -->|Tolak| M[Status: Ditolak]
    L --> N[Notifikasi ke Karyawan]
    M --> N
    N --> O([Selesai - Admin])
```

---

### 2.4 Proses Generate Payroll

**Deskripsi:** Alur admin generate payroll bulanan.

```mermaid
flowchart TD
    A([Mulai]) --> B[Pilih Bulan & Tahun]
    B --> C[Klik Generate]
    C --> D[Ambil Data Pegawai Aktif]
    D --> E[Loop Setiap Pegawai]

    E --> F[Hitung Kehadiran]
    F --> G[Hitung Keterlambatan]
    G --> H[Hitung Lembur]
    H --> I[Kalkulasi Gaji]
    I --> J[Simpan Payroll]

    J --> K{Ada Pegawai Lagi?}
    K -->|Ya| E
    K -->|Tidak| L[Tampilkan Hasil]
    L --> M([Selesai])
```

---

### 2.5 Proses Enrollment Fingerprint

**Deskripsi:** Alur pendaftaran sidik jari karyawan baru.

```mermaid
flowchart TD
    A([Mulai]) --> B[Admin Pilih Pegawai]
    B --> C[Klik Enroll]
    C --> D[ESP32 Mode Enrollment]
    D --> E[OLED: Scan Jari]

    E --> F[Scan Jari Pertama]
    F --> G[OLED: Scan Sekali Lagi]
    G --> H[Scan Jari Kedua]
    H --> I{Template Cocok?}

    I -->|Tidak| J[OLED: Gagal]
    J --> E

    I -->|Ya| K[Simpan di Sensor]
    K --> L[Kirim ID ke Server]
    L --> M[Update Database]
    M --> N[OLED: Berhasil]
    N --> O([Selesai])
```

---

## 3. Sequence Diagram

### 3.1 Proses Login

**Deskripsi:** Urutan interaksi saat user login.

```mermaid
flowchart LR
    subgraph Login
    U[User] -->|1-Input| B[Browser]
    B -->|2-POST /api/login| S[Server]
    S -->|3-Query| DB[(Database)]
    DB -->|4-Data User| S
    S -->|5-Token JWT| B
    B -->|6-Redirect| U
    end
```

---

### 3.2 Proses Absensi Fingerprint

**Deskripsi:** Urutan interaksi saat absensi fingerprint.

```mermaid
flowchart LR
    subgraph Absensi
    K[Karyawan] -->|1-Scan| FP[Sensor]
    FP -->|2-Data| ESP[ESP32]
    ESP -->|3-Nama| OLED[OLED]
    ESP -->|4-Publish| MQTT[MQTT]
    MQTT -->|5-Forward| S[Server]
    S -->|6-Query| DB[(Database)]
    S -->|7-Insert| DB
    S -->|8-Emit| WS[WebSocket]
    WS -->|9-Update| A[Admin]
    S -->|10-Response| MQTT
    MQTT -->|11-Forward| ESP
    ESP -->|12-Berhasil| OLED
    end
```

---

### 3.3 Proses Pengajuan Cuti

**Deskripsi:** Urutan interaksi pengajuan cuti.

```mermaid
flowchart LR
    subgraph Cuti
    K[Karyawan] -->|1-Submit| B[Browser]
    B -->|2-POST /api/cuti| S[Server]
    S -->|3-Insert| DB[(Database)]
    S -->|4-Notifikasi| WS[WebSocket]
    WS -->|5-Push| A[Admin]
    S -->|6-Konfirmasi| B
    end
```

---

## 4. Class Diagram

**Deskripsi:** Struktur kelas/entitas sistem beserta atribut dan method.

```mermaid
classDiagram
    class Pegawai {
        +id_pegawai
        +nip
        +nama_pegawai
        +posisi
        +fingerprint_id
        +gaji_pokok
        +create()
        +update()
        +delete()
    }

    class Absensi {
        +id_absensi
        +pegawai_id
        +waktu_absen
        +waktu_keluar
        +keterangan
        +checkIn()
        +checkOut()
    }

    class User {
        +id_user
        +username
        +password
        +role
        +login()
        +logout()
    }

    class PengajuanCuti {
        +id_pengajuan
        +pegawai_id
        +tanggal_mulai
        +tanggal_selesai
        +status
        +create()
        +approve()
        +reject()
    }

    class Payroll {
        +id_payroll
        +pegawai_id
        +bulan
        +tahun
        +total_gaji
        +generate()
        +cetakSlip()
    }

    Pegawai "1" --o "*" Absensi : memiliki
    Pegawai "1" --o "*" PengajuanCuti : mengajukan
    Pegawai "1" --o "*" Payroll : menerima
```

**Keterangan Relasi:**
| Relasi | Deskripsi |
|--------|-----------|
| Pegawai → Absensi | 1 pegawai punya banyak absensi |
| Pegawai → Cuti | 1 pegawai bisa ajukan banyak cuti |
| Pegawai → Payroll | 1 pegawai punya banyak record gaji |

---

## 5. Entity Relationship Diagram (ERD)

**Deskripsi:** Struktur tabel database dan relasi antar tabel.

```mermaid
erDiagram
    PEGAWAI ||--o{ ABSENSI : memiliki
    PEGAWAI ||--o{ PENGAJUAN_CUTI : mengajukan
    PEGAWAI ||--o{ PAYROLL : menerima

    PEGAWAI {
        int id_pegawai PK
        varchar nip
        varchar nama_pegawai
        varchar posisi
        int fingerprint_id
        decimal gaji_pokok
    }

    ABSENSI {
        int id_absensi PK
        int pegawai_id FK
        time waktu_absen
        time waktu_keluar
        varchar keterangan
    }

    USERS {
        int id_user PK
        varchar username
        varchar password
        varchar role
    }

    PENGAJUAN_CUTI {
        int id_pengajuan PK
        int pegawai_id FK
        date tanggal_mulai
        date tanggal_selesai
        varchar status
    }

    PAYROLL {
        int id_payroll PK
        int pegawai_id FK
        int bulan
        int tahun
        decimal total_gaji
    }

    SHIFT {
        int id_shift PK
        varchar nama_shift
        time jam_masuk
        time jam_keluar
    }
```

**Keterangan:**
- **PK** = Primary Key (Kunci Utama)
- **FK** = Foreign Key (Kunci Asing)
- **||--o{** = Relasi One-to-Many (1:N)

---

## 6. State Diagram

### 6.1 Status Pengajuan Cuti

**Deskripsi:** Perubahan status pengajuan cuti.

```mermaid
stateDiagram-v2
    [*] --> Menunggu: Submit
    Menunggu --> Disetujui: Approve
    Menunggu --> Ditolak: Reject
    Disetujui --> [*]
    Ditolak --> [*]
```

---

### 6.2 Status Absensi Harian

**Deskripsi:** Perubahan status absensi karyawan.

```mermaid
stateDiagram-v2
    [*] --> BelumAbsen
    BelumAbsen --> Hadir: Tepat Waktu
    BelumAbsen --> Terlambat: Lewat Toleransi
    BelumAbsen --> Alpha: Tidak Absen
    Hadir --> Pulang: Scan Keluar
    Terlambat --> Pulang: Scan Keluar
    Pulang --> [*]
    Alpha --> [*]
```

---

### 6.3 Status Pegawai

**Deskripsi:** Perubahan status kepegawaian.

```mermaid
stateDiagram-v2
    [*] --> Aktif
    Aktif --> Cuti: Cuti Disetujui
    Cuti --> Aktif: Selesai Cuti
    Aktif --> Nonaktif: Dinonaktifkan
    Nonaktif --> Aktif: Diaktifkan
    Aktif --> Resign: Keluar
    Resign --> [*]
```

---

## 7. Component Diagram

**Deskripsi:** Komponen-komponen sistem dan keterhubungannya.

```mermaid
flowchart TB
    subgraph Client["CLIENT"]
        A[Admin Dashboard]
        U[User Dashboard]
    end

    subgraph Server["SERVER"]
        API[REST API]
        WS[WebSocket]
        MQ[MQTT Handler]
    end

    subgraph Data["DATABASE"]
        DB[(PostgreSQL)]
    end

    subgraph IoT["PERANGKAT IoT"]
        ESP[ESP32]
        FP[Fingerprint]
        OLED[OLED Display]
    end

    subgraph Cloud["CLOUD STORAGE"]
        OC[OwnCloud/WebDAV]
    end

    A --> API
    U --> API
    A <--> WS
    U <--> WS
    API --> DB
    API -->|Backup| OC
    ESP --> FP
    ESP --> OLED
    ESP <--> MQ
    MQ --> API
```

---

## 8. Deployment Diagram

**Deskripsi:** Arsitektur deployment/penempatan sistem.

```mermaid
flowchart TB
    subgraph Cloud["CLOUD"]
        OC[OwnCloud Server]
    end

    subgraph Server["SERVER LOKAL"]
        Node[Node.js]
        MQTT[MQTT Broker]
        PG[(PostgreSQL)]
    end

    subgraph Client["CLIENT DEVICES"]
        PC[Admin PC]
        HP[Mobile/PWA]
    end

    subgraph IoT["IoT DEVICES"]
        ESP[ESP32 + Sensor]
    end

    PC -->|HTTP| Node
    HP -->|HTTP| Node
    ESP -->|MQTT| MQTT
    MQTT --> Node
    Node --> PG
    Node -->|WebDAV| OC
```

---

## 9. Arsitektur Sistem

**Deskripsi:** Gambaran keseluruhan arsitektur sistem.

```mermaid
flowchart TB
    subgraph Frontend["FRONTEND"]
        Admin[Portal Admin]
        User[Portal User]
    end

    subgraph Backend["BACKEND"]
        API[Express.js API]
        Socket[Socket.IO]
        MQTTHandler[MQTT Handler]
    end

    subgraph DB["DATABASE"]
        PG[(PostgreSQL)]
    end

    subgraph IoT["IoT"]
        ESP[ESP32]
        Sensor[Fingerprint]
        Display[OLED]
    end

    subgraph Broker["BROKER"]
        MQTT[MQTT Broker]
    end

    subgraph Cloud["CLOUD"]
        OC[OwnCloud]
    end

    Admin <--> API
    User <--> API
    Admin <--> Socket
    User <--> Socket
    API <--> PG
    API -->|Backup| OC
    MQTTHandler <--> MQTT
    ESP <--> Sensor
    ESP <--> Display
    ESP <--> MQTT
```

---

## Ringkasan Diagram

| No | Diagram | Fungsi |
|----|---------|--------|
| 1 | Use Case | Menunjukkan fitur yang bisa diakses setiap aktor |
| 2 | Activity | Menunjukkan alur proses/aktivitas sistem |
| 3 | Sequence | Menunjukkan urutan interaksi antar komponen |
| 4 | Class | Menunjukkan struktur kelas dan relasi |
| 5 | ERD | Menunjukkan struktur tabel database |
| 6 | State | Menunjukkan perubahan status objek |
| 7 | Component | Menunjukkan komponen sistem |
| 8 | Deployment | Menunjukkan arsitektur penempatan |
| 9 | Arsitektur | Menunjukkan gambaran keseluruhan sistem |
