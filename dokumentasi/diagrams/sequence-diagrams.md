# Sequence Diagrams

## 1. Proses Login

```mermaid
sequenceDiagram
    participant U as User
    participant B as Browser
    participant S as Server
    participant DB as Database

    U->>B: Buka halaman login
    B->>S: GET /login.html
    S-->>B: Tampilkan form login

    U->>B: Input username & password
    U->>B: Klik tombol Login
    B->>S: POST /api/login {username, password}

    S->>DB: SELECT * FROM users WHERE username = ?
    DB-->>S: Return user data

    alt Password valid
        S->>S: Verify password hash
        S->>S: Create session/token
        S-->>B: 200 OK {token, role, user_data}
        B->>B: Simpan token di localStorage

        alt Role = admin
            B->>B: Redirect ke /admin/dashboard-glass.html
        else Role = user
            B->>B: Redirect ke /user/user-dashboard.html
        end
    else Password tidak valid
        S-->>B: 401 Unauthorized {error: "Invalid credentials"}
        B->>U: Tampilkan pesan error
    end
```

## 2. Proses Absensi via Fingerprint

```mermaid
sequenceDiagram
    participant K as Karyawan
    participant FP as Fingerprint Sensor
    participant ESP as ESP32
    participant MQTT as MQTT Broker
    participant S as Server
    participant DB as Database
    participant WS as WebSocket
    participant A as Admin Browser

    K->>FP: Letakkan jari pada sensor
    FP->>ESP: Kirim data fingerprint
    ESP->>ESP: Proses fingerprint image
    ESP->>ESP: Match dengan template tersimpan

    alt Fingerprint cocok
        ESP->>ESP: Ambil fingerprint_id
        ESP->>MQTT: Publish "absensi/masuk" {fingerprint_id, timestamp}
        MQTT->>S: Forward message

        S->>DB: SELECT * FROM pegawai WHERE fingerprint_id = ?
        DB-->>S: Return pegawai data

        S->>DB: SELECT * FROM shift WHERE id = pegawai.shift_id
        DB-->>S: Return shift data

        S->>S: Tentukan status (hadir/terlambat)
        S->>DB: INSERT INTO absensi (pegawai_id, tanggal, jam_masuk, status)
        DB-->>S: Success

        S->>WS: Emit "attendance-update" {pegawai, absensi}
        WS->>A: Broadcast ke semua client
        A->>A: Update dashboard real-time

        S-->>ESP: Publish "absensi/response" {success: true, nama}
        ESP->>FP: Tampilkan LED hijau + buzzer
    else Fingerprint tidak cocok
        ESP->>FP: Tampilkan LED merah + buzzer
        ESP->>MQTT: Publish "absensi/failed"
    end
```

## 3. Proses Absensi Pulang

```mermaid
sequenceDiagram
    participant K as Karyawan
    participant FP as Fingerprint Sensor
    participant ESP as ESP32
    participant MQTT as MQTT Broker
    participant S as Server
    participant DB as Database

    K->>FP: Scan fingerprint
    FP->>ESP: Kirim data
    ESP->>MQTT: Publish "absensi/keluar" {fingerprint_id, timestamp}
    MQTT->>S: Forward message

    S->>DB: SELECT * FROM pegawai WHERE fingerprint_id = ?
    DB-->>S: Return pegawai data

    S->>DB: SELECT * FROM absensi WHERE pegawai_id = ? AND tanggal = TODAY
    DB-->>S: Return absensi record

    alt Sudah ada record jam masuk
        S->>DB: UPDATE absensi SET jam_keluar = ? WHERE id = ?
        DB-->>S: Success
        S-->>ESP: Publish response {success: true}
    else Belum ada record
        S-->>ESP: Publish response {error: "Belum absen masuk"}
    end
```

## 4. Proses Enrollment Fingerprint

```mermaid
sequenceDiagram
    participant A as Admin
    participant B as Browser
    participant S as Server
    participant MQTT as MQTT Broker
    participant ESP as ESP32
    participant FP as Fingerprint Sensor
    participant K as Karyawan

    A->>B: Klik "Enroll Fingerprint" untuk pegawai
    B->>S: POST /api/enrollment/start {pegawai_id}
    S->>S: Generate enrollment session
    S->>MQTT: Publish "fingerprint/enroll" {pegawai_id, mode: "start"}
    MQTT->>ESP: Forward message

    ESP->>ESP: Masuk mode enrollment
    ESP->>FP: Aktifkan LED enrollment

    S-->>B: 200 OK {message: "Enrollment mode aktif"}
    B->>A: Tampilkan "Silakan scan jari..."

    K->>FP: Scan jari pertama
    FP->>ESP: Capture template 1
    ESP->>MQTT: Publish "fingerprint/progress" {step: 1}
    MQTT->>S: Forward
    S->>B: Emit "enrollment-progress" {step: 1}
    B->>A: Tampilkan "Scan jari lagi..."

    K->>FP: Scan jari kedua
    FP->>ESP: Capture template 2
    ESP->>ESP: Gabung template & simpan
    ESP->>MQTT: Publish "fingerprint/complete" {fingerprint_id}
    MQTT->>S: Forward

    S->>DB: UPDATE pegawai SET fingerprint_id = ? WHERE id = ?
    DB-->>S: Success

    S->>B: Emit "enrollment-complete" {success: true}
    B->>A: Tampilkan "Enrollment berhasil!"
```

## 5. Proses Pengajuan Cuti

```mermaid
sequenceDiagram
    participant K as Karyawan
    participant B as Browser
    participant S as Server
    participant DB as Database
    participant N as Notification
    participant A as Admin

    K->>B: Buka halaman pengajuan cuti
    B->>S: GET /user/user-cuti.html
    S-->>B: Tampilkan form cuti

    K->>B: Isi form (tanggal, jenis, alasan)
    K->>B: Klik Submit
    B->>S: POST /api/cuti {pegawai_id, tanggal_mulai, tanggal_selesai, jenis, alasan}

    S->>S: Validasi data
    S->>DB: INSERT INTO pengajuan_cuti (...)
    DB-->>S: Return cuti_id

    S->>DB: INSERT INTO notifikasi (judul, pesan, tipe)
    Note right of S: Notifikasi untuk admin

    S-->>B: 201 Created {id, status: "pending"}
    B->>K: Tampilkan "Pengajuan berhasil!"

    Note over A: Admin membuka dashboard
    A->>B: Buka daftar pengajuan cuti
    B->>S: GET /api/cuti?status=pending
    S->>DB: SELECT * FROM pengajuan_cuti WHERE status = 'pending'
    DB-->>S: Return list cuti
    S-->>B: Return cuti list
    B->>A: Tampilkan daftar pengajuan

    A->>B: Klik Approve
    B->>S: PUT /api/cuti/:id/approve {catatan}
    S->>DB: UPDATE pengajuan_cuti SET status = 'approved' WHERE id = ?
    DB-->>S: Success

    S->>DB: INSERT INTO notifikasi (pegawai_id, judul, pesan)
    Note right of S: Notifikasi untuk karyawan

    S-->>B: 200 OK
    B->>A: Tampilkan "Cuti disetujui"
```

## 6. Proses Generate Payroll

```mermaid
sequenceDiagram
    participant A as Admin
    participant B as Browser
    participant S as Server
    participant DB as Database

    A->>B: Buka halaman Payroll
    A->>B: Pilih bulan dan tahun
    A->>B: Klik "Generate Payroll"
    B->>S: POST /api/payroll/generate {bulan, tahun}

    S->>DB: SELECT * FROM pegawai WHERE status = 'aktif'
    DB-->>S: Return list pegawai

    loop Untuk setiap pegawai
        S->>DB: SELECT COUNT(*) FROM absensi WHERE pegawai_id = ? AND bulan = ? AND status = 'hadir'
        DB-->>S: Return hari_hadir

        S->>DB: SELECT COUNT(*) FROM absensi WHERE pegawai_id = ? AND bulan = ? AND status = 'terlambat'
        DB-->>S: Return hari_terlambat

        S->>DB: SELECT COUNT(*) FROM hari_libur WHERE MONTH(tanggal) = ?
        DB-->>S: Return hari_libur

        S->>S: Hitung gaji<br/>- Gaji pokok<br/>- Tunjangan<br/>- Potongan (alpha, terlambat)<br/>- Lembur<br/>- Total

        S->>DB: INSERT INTO payroll (pegawai_id, bulan, tahun, ...)
        DB-->>S: Success
    end

    S-->>B: 200 OK {message: "Payroll berhasil digenerate", count: n}
    B->>A: Tampilkan hasil
```

## 7. Proses Backup ke OwnCloud

```mermaid
sequenceDiagram
    participant A as Admin
    participant B as Browser
    participant S as Server
    participant DB as Database
    participant OC as OwnCloud/WebDAV

    A->>B: Klik "Backup ke OwnCloud"
    B->>S: POST /api/backup/owncloud

    S->>DB: SELECT * FROM pegawai
    S->>DB: SELECT * FROM absensi
    S->>DB: SELECT * FROM payroll
    DB-->>S: Return all data

    S->>S: Format data ke JSON/CSV
    S->>S: Compress data

    S->>OC: PUT /remote.php/webdav/backup/backup_YYYY-MM-DD.zip
    Note right of S: Autentikasi WebDAV

    alt Backup berhasil
        OC-->>S: 201 Created
        S->>DB: INSERT INTO backup_log (tanggal, status, size)
        S-->>B: 200 OK {message: "Backup berhasil", url: "..."}
        B->>A: Tampilkan "Backup berhasil!"
    else Backup gagal
        OC-->>S: Error (401/500)
        S-->>B: 500 Error {message: "Backup gagal"}
        B->>A: Tampilkan error
    end
```

## 8. Proses Real-time Dashboard Update

```mermaid
sequenceDiagram
    participant A as Admin Browser
    participant WS as WebSocket (Socket.IO)
    participant S as Server
    participant MQTT as MQTT Broker
    participant ESP as ESP32

    Note over A,S: Koneksi WebSocket
    A->>WS: connect()
    WS->>S: Connection established
    S->>A: Emit "connected"

    A->>WS: Join room "admin-dashboard"
    S->>S: Add to room

    Note over ESP,S: Event Absensi Masuk
    ESP->>MQTT: Publish attendance data
    MQTT->>S: Forward message
    S->>S: Process & save to DB
    S->>WS: Emit to room "admin-dashboard"
    WS->>A: "attendance-new" {pegawai, timestamp, status}
    A->>A: Update statistik<br/>Update table<br/>Show notification

    Note over ESP,S: Event Status Sensor
    ESP->>MQTT: Publish sensor status
    MQTT->>S: Forward
    S->>WS: Emit "sensor-status"
    WS->>A: {online: true, lastPing: "..."}
    A->>A: Update connection indicator
```

## 9. Proses Cetak Laporan

```mermaid
sequenceDiagram
    participant A as Admin
    participant B as Browser
    participant S as Server
    participant DB as Database

    A->>B: Pilih periode laporan
    A->>B: Klik "Cetak Laporan"
    B->>S: GET /api/laporan?periode=bulanan&bulan=12&tahun=2024

    S->>DB: SELECT p.*, a.* FROM pegawai p<br/>JOIN absensi a ON p.id = a.pegawai_id<br/>WHERE MONTH(a.tanggal) = ? AND YEAR(a.tanggal) = ?
    DB-->>S: Return data laporan

    S->>S: Aggregate data per pegawai
    S->>S: Calculate statistics
    S-->>B: Return formatted data

    B->>B: Render laporan HTML
    B->>B: window.print()
    B->>A: Dialog print browser
```

## 10. Proses CRUD Karyawan

```mermaid
sequenceDiagram
    participant A as Admin
    participant B as Browser
    participant S as Server
    participant DB as Database
    participant FS as File System

    Note over A,DB: CREATE - Tambah Karyawan
    A->>B: Isi form karyawan + foto
    B->>S: POST /api/pegawai (multipart/form-data)
    S->>FS: Simpan foto ke /uploads/
    FS-->>S: Return foto path
    S->>DB: INSERT INTO pegawai (nip, nama, foto, ...)
    DB-->>S: Return new id
    S-->>B: 201 Created {id, ...}

    Note over A,DB: READ - Lihat Karyawan
    A->>B: Buka halaman manajemen karyawan
    B->>S: GET /api/pegawai
    S->>DB: SELECT * FROM pegawai
    DB-->>S: Return list
    S-->>B: Return pegawai list

    Note over A,DB: UPDATE - Edit Karyawan
    A->>B: Edit data karyawan
    B->>S: PUT /api/pegawai/:id {nama, jabatan, ...}
    S->>DB: UPDATE pegawai SET ... WHERE id = ?
    DB-->>S: Success
    S-->>B: 200 OK

    Note over A,DB: DELETE - Hapus Karyawan
    A->>B: Klik hapus + konfirmasi
    B->>S: DELETE /api/pegawai/:id
    S->>DB: DELETE FROM pegawai WHERE id = ?
    Note right of S: Cascade delete absensi, payroll, dll
    DB-->>S: Success
    S-->>B: 200 OK
```
