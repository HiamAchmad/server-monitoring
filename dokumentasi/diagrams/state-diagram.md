# State Diagram

## 1. State Diagram Pengajuan Cuti

```mermaid
stateDiagram-v2
    [*] --> Draft: Karyawan mulai mengisi form

    Draft --> Menunggu: Submit pengajuan
    Draft --> [*]: Batalkan

    Menunggu --> Disetujui: Admin approve
    Menunggu --> Ditolak: Admin reject
    Menunggu --> Menunggu: Masih dalam review

    Disetujui --> Aktif: Tanggal cuti dimulai
    Ditolak --> [*]: Selesai

    Aktif --> Selesai: Tanggal cuti berakhir
    Selesai --> [*]

    note right of Menunggu
        Status default saat pengajuan
        baru dibuat
    end note

    note right of Disetujui
        Admin memberikan approval
        dengan komentar (opsional)
    end note
```

### Tabel State Pengajuan Cuti

| State | Deskripsi | Transisi Keluar | Trigger |
|-------|-----------|-----------------|---------|
| Draft | Karyawan mengisi form | Menunggu, Batal | Submit, Cancel |
| Menunggu | Menunggu approval admin | Disetujui, Ditolak | Admin action |
| Disetujui | Cuti telah disetujui | Aktif | Tanggal mulai tercapai |
| Ditolak | Cuti ditolak admin | End | - |
| Aktif | Karyawan sedang cuti | Selesai | Tanggal selesai tercapai |
| Selesai | Periode cuti selesai | End | - |

---

## 2. State Diagram Absensi Harian

```mermaid
stateDiagram-v2
    [*] --> BelumAbsen: Hari kerja dimulai

    BelumAbsen --> SudahMasuk: Scan fingerprint masuk
    BelumAbsen --> Alpha: Tidak ada scan sampai akhir hari

    SudahMasuk --> Hadir: Waktu masuk <= jam masuk + toleransi
    SudahMasuk --> Terlambat: Waktu masuk > jam masuk + toleransi

    Hadir --> SudahPulang: Scan fingerprint keluar
    Terlambat --> SudahPulang: Scan fingerprint keluar

    Hadir --> PulangOtomatis: Tidak scan keluar (auto close)
    Terlambat --> PulangOtomatis: Tidak scan keluar (auto close)

    SudahPulang --> Lembur: Jam keluar > jam lembur mulai
    SudahPulang --> TidakLembur: Jam keluar <= jam lembur mulai

    PulangOtomatis --> TidakLembur

    Lembur --> [*]: Record selesai
    TidakLembur --> [*]: Record selesai
    Alpha --> [*]: Record selesai

    note right of BelumAbsen
        State awal setiap
        hari kerja
    end note

    note left of Alpha
        Pegawai tidak hadir
        tanpa keterangan
    end note
```

### Tabel State Absensi Harian

| State | Deskripsi | Kondisi | Hasil |
|-------|-----------|---------|-------|
| BelumAbsen | Belum ada aktivitas | Hari kerja dimulai | - |
| SudahMasuk | Fingerprint masuk terdeteksi | Scan berhasil | Record waktu_absen |
| Hadir | Masuk tepat waktu | waktu <= jam_masuk + toleransi | keterangan = 'Hadir' |
| Terlambat | Masuk lewat waktu | waktu > jam_masuk + toleransi | keterangan = 'Terlambat' |
| SudahPulang | Fingerprint keluar terdeteksi | Scan berhasil | Record waktu_keluar |
| Lembur | Bekerja melebihi jam normal | waktu_keluar > jam_lembur | status_lembur = 'Ya' |
| TidakLembur | Pulang normal | waktu_keluar <= jam_lembur | status_lembur = 'Tidak' |
| Alpha | Tidak hadir tanpa keterangan | Tidak ada scan | keterangan = 'Alpha' |

---

## 3. State Diagram Status Pegawai

```mermaid
stateDiagram-v2
    [*] --> Baru: Pegawai didaftarkan

    Baru --> BelumEnroll: Data disimpan
    BelumEnroll --> Aktif: Fingerprint enrolled

    Aktif --> Cuti: Pengajuan cuti disetujui
    Cuti --> Aktif: Masa cuti selesai

    Aktif --> Nonaktif: Di-nonaktifkan admin
    Nonaktif --> Aktif: Diaktifkan kembali

    Aktif --> Resign: Keluar dari perusahaan
    Nonaktif --> Resign: Resign saat nonaktif

    Resign --> [*]

    note right of BelumEnroll
        Pegawai sudah terdaftar
        tapi belum bisa absen
    end note

    note right of Aktif
        Pegawai dapat melakukan
        absensi fingerprint
    end note
```

### Tabel State Status Pegawai

| State | Deskripsi | Dapat Absen | Dapat Login |
|-------|-----------|-------------|-------------|
| Baru | Baru didaftarkan | Tidak | Tidak |
| BelumEnroll | Belum enrollment fingerprint | Tidak | Ya (jika ada akun) |
| Aktif | Status normal aktif bekerja | Ya | Ya |
| Cuti | Sedang dalam masa cuti | Tidak | Ya |
| Nonaktif | Di-suspend/nonaktifkan | Tidak | Tidak |
| Resign | Sudah keluar | Tidak | Tidak |

---

## 4. State Diagram Proses Enrollment Fingerprint

```mermaid
stateDiagram-v2
    [*] --> Idle: Sistem standby

    Idle --> ModeEnrollment: Admin trigger enrollment

    ModeEnrollment --> ScanPertama: Tunggu scan jari
    ScanPertama --> CaptureTemplate1: Jari terdeteksi
    CaptureTemplate1 --> ScanKedua: Template 1 tersimpan

    ScanKedua --> CaptureTemplate2: Jari terdeteksi
    CaptureTemplate2 --> Verifikasi: Template 2 tersimpan

    Verifikasi --> Sukses: Template cocok
    Verifikasi --> Gagal: Template tidak cocok

    Gagal --> ScanPertama: Ulangi proses

    Sukses --> SimpanDatabase: Fingerprint ID generated
    SimpanDatabase --> Selesai: Data tersimpan

    Selesai --> Idle: Kembali ke mode normal

    ModeEnrollment --> Timeout: Tidak ada aktivitas 60 detik
    Timeout --> Idle: Reset ke standby

    note right of ModeEnrollment
        ESP32 masuk mode
        enrollment, LED kuning
    end note

    note right of Sukses
        LED hijau + buzzer
        Template berhasil disimpan
    end note

    note right of Gagal
        LED merah + buzzer
        Perlu scan ulang
    end note
```

### Tabel State Enrollment

| State | Deskripsi | LED Indicator | Durasi Max |
|-------|-----------|---------------|------------|
| Idle | Standby normal | Hijau steady | - |
| ModeEnrollment | Mode pendaftaran aktif | Kuning blink | 60 detik |
| ScanPertama | Menunggu scan pertama | Kuning | 30 detik |
| CaptureTemplate1 | Memproses template 1 | Kuning fast blink | 2 detik |
| ScanKedua | Menunggu scan kedua | Kuning | 30 detik |
| CaptureTemplate2 | Memproses template 2 | Kuning fast blink | 2 detik |
| Verifikasi | Membandingkan template | Kuning | 1 detik |
| Sukses | Enrollment berhasil | Hijau + buzzer | 3 detik |
| Gagal | Enrollment gagal | Merah + buzzer | 3 detik |
| Timeout | Waktu habis | Merah | 3 detik |

---

## 5. State Diagram Proses Absensi Fingerprint

```mermaid
stateDiagram-v2
    [*] --> Ready: Sensor aktif

    Ready --> Scanning: Jari terdeteksi
    Scanning --> Matching: Capture selesai

    Matching --> Found: Fingerprint cocok
    Matching --> NotFound: Tidak cocok

    NotFound --> Ready: Reset (LED merah)

    Found --> CheckJam: Cek waktu saat ini

    CheckJam --> JamMasuk: 06:00 - 12:00
    CheckJam --> JamKeluar: 12:01 - 23:59

    JamMasuk --> RecordMasuk: Catat absensi masuk
    JamKeluar --> CekSudahMasuk: Cek record hari ini

    CekSudahMasuk --> RecordKeluar: Sudah ada record masuk
    CekSudahMasuk --> ErrorBelumMasuk: Belum ada record

    RecordMasuk --> Sukses: Data tersimpan
    RecordKeluar --> Sukses: Data tersimpan

    ErrorBelumMasuk --> Ready: Reset (LED merah)
    Sukses --> Ready: Reset (LED hijau)

    note right of Found
        Menampilkan nama pegawai
        di layar ESP32
    end note

    note left of NotFound
        "Fingerprint tidak terdaftar"
        LED merah 2x blink
    end note
```

### Tabel State Absensi Fingerprint

| State | Deskripsi | Output ESP32 | MQTT Topic |
|-------|-----------|--------------|------------|
| Ready | Siap menerima scan | LED hijau steady | - |
| Scanning | Proses capture image | LED biru blink | - |
| Matching | Pencocokan template | LED kuning | - |
| Found | Fingerprint ditemukan | Tampil nama | fingerprint/attendance |
| NotFound | Tidak ditemukan | LED merah, buzzer | fingerprint/failed |
| RecordMasuk | Catat jam masuk | LED hijau, buzzer | fingerprint/checkin |
| RecordKeluar | Catat jam keluar | LED hijau, buzzer | fingerprint/checkout |
| Sukses | Proses selesai | LED hijau 3 detik | fingerprint/success |

---

## 6. State Diagram Sesi User

```mermaid
stateDiagram-v2
    [*] --> Guest: Buka aplikasi

    Guest --> Authenticating: Submit login
    Authenticating --> Authenticated: Kredensial valid
    Authenticating --> Guest: Kredensial invalid

    Authenticated --> AdminSession: role = admin
    Authenticated --> UserSession: role = user

    AdminSession --> Expired: Token expired
    UserSession --> Expired: Token expired

    AdminSession --> Guest: Logout
    UserSession --> Guest: Logout

    Expired --> Guest: Redirect ke login

    note right of Authenticated
        Token JWT disimpan
        di localStorage
    end note

    note left of Expired
        Token expired setelah
        24 jam (configurable)
    end note
```

### Tabel State Sesi User

| State | Deskripsi | Akses Halaman | Token Status |
|-------|-----------|---------------|--------------|
| Guest | Belum login | /login.html saja | Tidak ada |
| Authenticating | Proses login | - | - |
| Authenticated | Login berhasil | Redirect sesuai role | Valid |
| AdminSession | Sesi admin aktif | /admin/* | Valid |
| UserSession | Sesi user aktif | /user/* | Valid |
| Expired | Token kedaluwarsa | - | Expired |

---

## 7. State Diagram Generate Payroll

```mermaid
stateDiagram-v2
    [*] --> Idle: Halaman payroll dibuka

    Idle --> SelectPeriode: Admin pilih bulan/tahun
    SelectPeriode --> CheckExisting: Klik Generate

    CheckExisting --> ConfirmOverwrite: Data sudah ada
    CheckExisting --> StartGenerate: Data belum ada

    ConfirmOverwrite --> StartGenerate: Admin konfirmasi
    ConfirmOverwrite --> Idle: Admin batalkan

    StartGenerate --> FetchPegawai: Ambil data pegawai aktif
    FetchPegawai --> LoopPegawai: Iterasi setiap pegawai

    LoopPegawai --> HitungAbsensi: Hitung rekap absensi
    HitungAbsensi --> HitungGaji: Kalkulasi gaji
    HitungGaji --> SimpanPayroll: Insert/Update record

    SimpanPayroll --> NextPegawai: Pegawai selanjutnya
    NextPegawai --> LoopPegawai: Masih ada pegawai
    NextPegawai --> Selesai: Semua pegawai selesai

    Selesai --> Idle: Tampilkan hasil

    state HitungGaji {
        [*] --> GajiPokok
        GajiPokok --> Tunjangan
        Tunjangan --> Lembur
        Lembur --> Potongan
        Potongan --> TotalGaji
        TotalGaji --> [*]
    }

    note right of LoopPegawai
        Progress ditampilkan
        di UI (x dari n pegawai)
    end note
```

### Tabel State Generate Payroll

| State | Deskripsi | Progress | Database Action |
|-------|-----------|----------|-----------------|
| Idle | Menunggu input | 0% | - |
| SelectPeriode | Pilih periode | 0% | - |
| CheckExisting | Cek data existing | 5% | SELECT payroll |
| StartGenerate | Mulai generate | 10% | - |
| FetchPegawai | Ambil list pegawai | 15% | SELECT pegawai |
| LoopPegawai | Iterasi pegawai | 15-95% | - |
| HitungAbsensi | Hitung rekap | - | SELECT absensi |
| HitungGaji | Kalkulasi gaji | - | SELECT setting |
| SimpanPayroll | Simpan record | - | INSERT/UPDATE payroll |
| Selesai | Proses selesai | 100% | - |

---

## 8. State Diagram Notifikasi

```mermaid
stateDiagram-v2
    [*] --> Created: Notifikasi dibuat

    Created --> Delivered: Broadcast via WebSocket
    Delivered --> Unread: Tersimpan di database

    Unread --> Read: User klik notifikasi
    Read --> Archived: Setelah 30 hari

    Archived --> Deleted: Admin hapus / cleanup
    Deleted --> [*]

    Unread --> Deleted: User hapus manual
    Read --> Deleted: User hapus manual

    note right of Created
        Trigger: absensi baru,
        cuti approved, payroll, dll
    end note

    note left of Delivered
        Real-time push ke
        browser via Socket.IO
    end note
```

### Tabel State Notifikasi

| State | Deskripsi | is_read | Umur Data |
|-------|-----------|---------|-----------|
| Created | Baru dibuat | false | 0 |
| Delivered | Sudah terkirim | false | < 1 detik |
| Unread | Belum dibaca | false | - |
| Read | Sudah dibaca | true | - |
| Archived | Diarsipkan | true | > 30 hari |
| Deleted | Dihapus | - | - |

---

*Dokumentasi State Diagram - Sistem Absensi IoT Berbasis Fingerprint*
