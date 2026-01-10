# Class Diagram

## 1. Class Diagram Sistem Absensi IoT

### 1.1 Class Diagram Keseluruhan

```mermaid
classDiagram
    class Pegawai {
        -int id_pegawai
        -string nip
        -string nama_pegawai
        -string posisi
        -string divisi
        -string email
        -string no_telepon
        -date tanggal_bergabung
        -enum status
        -int fingerprint_id
        -decimal gaji_pokok
        -timestamp created_at
        -timestamp updated_at
        +create()
        +update()
        +delete()
        +getById()
        +getAll()
        +getByFingerprint()
        +updateFingerprint()
    }

    class Absensi {
        -int id_absensi
        -int pegawai_id
        -string waktu_absen
        -time waktu_keluar
        -int durasi_kerja
        -int durasi_lembur
        -string status_lembur
        -enum keterangan
        -timestamp timestamp
        +create()
        +update()
        +getByPegawai()
        +getByDate()
        +getStatistik()
        +checkIn()
        +checkOut()
    }

    class User {
        -int id_user
        -string username
        -string password
        -string nama_lengkap
        -enum role
        -timestamp created_at
        +login()
        +logout()
        +verifyPassword()
        +changePassword()
        +create()
        +getByUsername()
    }

    class Shift {
        -int id_shift
        -string nama_shift
        -time jam_masuk
        -time jam_keluar
        -int toleransi_terlambat
        -time jam_lembur_mulai
        -boolean is_active
        -timestamp created_at
        +create()
        +update()
        +delete()
        +getActive()
        +checkTerlambat()
    }

    class PengajuanCuti {
        -int id_pengajuan
        -int pegawai_id
        -string jenis
        -date tanggal_mulai
        -date tanggal_selesai
        -int jumlah_hari
        -text alasan
        -string status
        -int disetujui_oleh
        -timestamp tanggal_disetujui
        -text komentar_approval
        -timestamp created_at
        +create()
        +approve()
        +reject()
        +getByPegawai()
        +getPending()
        +hitungHari()
    }

    class HariLibur {
        -int id_libur
        -date tanggal
        -string nama_libur
        -string jenis
        -timestamp created_at
        +create()
        +delete()
        +getByBulan()
        +isLibur()
    }

    class Payroll {
        -int id_payroll
        -int pegawai_id
        -int bulan
        -int tahun
        -int total_hadir
        -int total_terlambat
        -int total_izin
        -int total_sakit
        -decimal total_jam_lembur
        -decimal gaji_pokok
        -decimal uang_lembur
        -decimal tunjangan
        -decimal potongan_absen
        -decimal total_gaji
        -timestamp created_at
        +generate()
        +update()
        +getByPegawai()
        +getByPeriode()
        +hitungGaji()
        +cetakSlip()
    }

    class SettingPayroll {
        -int id_setting
        -string nama_setting
        -decimal nilai
        -text keterangan
        -timestamp updated_at
        +get()
        +update()
        +getAll()
    }

    class Notifikasi {
        -int id_notifikasi
        -int pegawai_id
        -string judul
        -text pesan
        -string tipe
        -string kategori
        -boolean is_read
        -boolean is_admin
        -string link
        -timestamp created_at
        +create()
        +markAsRead()
        +getByPegawai()
        +getForAdmin()
        +delete()
    }

    class Tugas {
        -int id_tugas
        -int pegawai_id
        -string nama_tugas
        -text deskripsi
        -string file_name
        -string file_path
        -date deadline
        -timestamp upload_date
        +create()
        +delete()
        +getByPegawai()
        +uploadFile()
    }

    %% Relationships
    Pegawai "1" --o "*" Absensi : memiliki
    Pegawai "1" --o "*" Payroll : menerima
    Pegawai "1" --o "*" PengajuanCuti : mengajukan
    Pegawai "1" --o "*" Notifikasi : menerima
    Pegawai "1" --o "*" Tugas : mengerjakan
    Pegawai "1" --o "*" PengajuanCuti : menyetujui
    Payroll "*" --> "1" SettingPayroll : menggunakan
```

---

### 1.2 Class Diagram per Module

#### A. Module Autentikasi

```mermaid
classDiagram
    class AuthController {
        +login(username, password)
        +logout()
        +verifyToken(token)
        +changePassword(oldPwd, newPwd)
    }

    class User {
        -int id_user
        -string username
        -string password
        -string nama_lengkap
        -enum role
        +verifyPassword()
        +generateToken()
    }

    class Session {
        -string token
        -int user_id
        -timestamp expires_at
        +create()
        +validate()
        +destroy()
    }

    AuthController --> User : authenticates
    AuthController --> Session : manages
    User "1" --o "1" Session : has
```

#### B. Module Absensi IoT

```mermaid
classDiagram
    class AbsensiController {
        +recordMasuk(fingerprint_id)
        +recordKeluar(fingerprint_id)
        +getAbsensiHarian()
        +getStatistik()
    }

    class MQTTHandler {
        -string broker_url
        -array topics
        +subscribe(topic)
        +publish(topic, message)
        +handleMessage(topic, payload)
    }

    class FingerprintService {
        +verifyFingerprint(id)
        +enrollFingerprint(pegawai_id)
        +deleteFingerprint(id)
    }

    class Pegawai {
        -int fingerprint_id
        +getByFingerprint()
    }

    class Absensi {
        +checkIn()
        +checkOut()
    }

    class Shift {
        +checkTerlambat()
        +getJamKerja()
    }

    AbsensiController --> MQTTHandler : uses
    AbsensiController --> Pegawai : validates
    AbsensiController --> Absensi : records
    AbsensiController --> Shift : checks
    MQTTHandler --> FingerprintService : communicates
```

#### C. Module Payroll

```mermaid
classDiagram
    class PayrollController {
        +generatePayroll(bulan, tahun)
        +getPayrollList()
        +updatePayroll(id)
        +cetakSlip(id)
    }

    class PayrollService {
        +hitungGajiPokok()
        +hitungLembur()
        +hitungTunjangan()
        +hitungPotongan()
        +hitungTotal()
    }

    class Payroll {
        -decimal gaji_pokok
        -decimal uang_lembur
        -decimal tunjangan
        -decimal potongan_absen
        -decimal total_gaji
        +generate()
        +update()
    }

    class SettingPayroll {
        +getTarifLembur()
        +getPotonganTerlambat()
        +getTunjangan()
    }

    class Absensi {
        +getRekapBulanan()
    }

    PayrollController --> PayrollService : uses
    PayrollController --> Payroll : manages
    PayrollService --> SettingPayroll : reads
    PayrollService --> Absensi : calculates from
```

#### D. Module Cuti

```mermaid
classDiagram
    class CutiController {
        +ajukanCuti(data)
        +approveCuti(id)
        +rejectCuti(id, komentar)
        +getCutiPending()
        +getCutiByPegawai(id)
    }

    class PengajuanCuti {
        -string status
        +create()
        +approve()
        +reject()
        +hitungJumlahHari()
    }

    class NotifikasiService {
        +kirimNotifikasiAdmin()
        +kirimNotifikasiPegawai()
    }

    class HariLibur {
        +isLibur(tanggal)
        +getLiburRange(start, end)
    }

    CutiController --> PengajuanCuti : manages
    CutiController --> NotifikasiService : notifies
    PengajuanCuti --> HariLibur : excludes
```

---

## 2. Class Diagram dengan Atribut Lengkap

### 2.1 Entity Classes

```mermaid
classDiagram
    class Pegawai {
        <<Entity>>
        -id_pegawai: int
        -nip: varchar(50)
        -nama_pegawai: varchar(100)
        -posisi: varchar(50)
        -divisi: varchar(50)
        -email: varchar(100)
        -no_telepon: varchar(20)
        -tanggal_bergabung: date
        -status: StatusPegawai
        -fingerprint_id: int
        -gaji_pokok: decimal(12,2)
        -created_at: timestamp
        -updated_at: timestamp
    }

    class Absensi {
        <<Entity>>
        -id_absensi: int
        -pegawai_id: int
        -waktu_absen: varchar(10)
        -waktu_keluar: time
        -durasi_kerja: int
        -durasi_lembur: int
        -status_lembur: varchar(20)
        -keterangan: KeteranganAbsensi
        -timestamp: timestamp
    }

    class User {
        <<Entity>>
        -id_user: int
        -username: varchar(50)
        -password: varchar(255)
        -nama_lengkap: varchar(100)
        -role: RoleUser
        -created_at: timestamp
    }

    class Shift {
        <<Entity>>
        -id_shift: int
        -nama_shift: varchar(50)
        -jam_masuk: time
        -jam_keluar: time
        -toleransi_terlambat: int
        -jam_lembur_mulai: time
        -is_active: boolean
        -created_at: timestamp
    }

    class PengajuanCuti {
        <<Entity>>
        -id_pengajuan: int
        -pegawai_id: int
        -jenis: varchar(50)
        -tanggal_mulai: date
        -tanggal_selesai: date
        -jumlah_hari: int
        -alasan: text
        -status: StatusCuti
        -disetujui_oleh: int
        -tanggal_disetujui: timestamp
        -komentar_approval: text
        -created_at: timestamp
    }

    class Payroll {
        <<Entity>>
        -id_payroll: int
        -pegawai_id: int
        -bulan: int
        -tahun: int
        -total_hadir: int
        -total_terlambat: int
        -total_izin: int
        -total_sakit: int
        -total_jam_lembur: decimal(10,2)
        -gaji_pokok: decimal(12,2)
        -uang_lembur: decimal(12,2)
        -tunjangan: decimal(12,2)
        -potongan_absen: decimal(12,2)
        -total_gaji: decimal(12,2)
        -created_at: timestamp
    }
```

### 2.2 Enumeration Classes

```mermaid
classDiagram
    class StatusPegawai {
        <<enumeration>>
        Aktif
        Nonaktif
    }

    class KeteranganAbsensi {
        <<enumeration>>
        Hadir
        Terlambat
        Izin
        Sakit
    }

    class RoleUser {
        <<enumeration>>
        Admin
        User
    }

    class StatusCuti {
        <<enumeration>>
        Menunggu
        Disetujui
        Ditolak
    }

    class TipeNotifikasi {
        <<enumeration>>
        info
        success
        warning
        error
    }

    class KategoriNotifikasi {
        <<enumeration>>
        sistem
        absensi
        cuti
        payroll
    }

    class JenisHariLibur {
        <<enumeration>>
        Nasional
        Cuti_Bersama
    }
```

---

## 3. Service Classes

```mermaid
classDiagram
    class PegawaiService {
        +create(data): Pegawai
        +update(id, data): Pegawai
        +delete(id): boolean
        +getById(id): Pegawai
        +getAll(): Pegawai[]
        +getByFingerprint(fp_id): Pegawai
        +search(keyword): Pegawai[]
    }

    class AbsensiService {
        +recordMasuk(pegawai_id, waktu): Absensi
        +recordKeluar(pegawai_id, waktu): Absensi
        +getByPegawai(pegawai_id): Absensi[]
        +getByTanggal(tanggal): Absensi[]
        +getStatistik(pegawai_id, bulan, tahun): object
        +checkTerlambat(waktu, shift): boolean
    }

    class PayrollService {
        +generate(bulan, tahun): Payroll[]
        +generateForPegawai(pegawai_id, bulan, tahun): Payroll
        +hitungGaji(absensi, setting): object
        +cetakSlip(payroll_id): PDF
    }

    class CutiService {
        +ajukan(data): PengajuanCuti
        +approve(id, admin_id): PengajuanCuti
        +reject(id, admin_id, komentar): PengajuanCuti
        +hitungHariKerja(mulai, selesai): int
        +getPending(): PengajuanCuti[]
    }

    class NotifikasiService {
        +create(data): Notifikasi
        +markAsRead(id): boolean
        +getUnread(pegawai_id): Notifikasi[]
        +broadcast(pesan): boolean
    }

    class AuthService {
        +login(username, password): Token
        +logout(token): boolean
        +verifyToken(token): User
        +hashPassword(password): string
    }
```

---

## 4. Controller Classes

```mermaid
classDiagram
    class PegawaiController {
        -pegawaiService: PegawaiService
        +index(req, res)
        +show(req, res)
        +store(req, res)
        +update(req, res)
        +destroy(req, res)
    }

    class AbsensiController {
        -absensiService: AbsensiService
        +index(req, res)
        +store(req, res)
        +getStatistik(req, res)
        +exportPDF(req, res)
    }

    class PayrollController {
        -payrollService: PayrollService
        +index(req, res)
        +generate(req, res)
        +update(req, res)
        +cetakSlip(req, res)
    }

    class CutiController {
        -cutiService: CutiService
        +index(req, res)
        +store(req, res)
        +approve(req, res)
        +reject(req, res)
    }

    class AuthController {
        -authService: AuthService
        +login(req, res)
        +logout(req, res)
        +me(req, res)
    }

    PegawaiController --> PegawaiService
    AbsensiController --> AbsensiService
    PayrollController --> PayrollService
    CutiController --> CutiService
    AuthController --> AuthService
```

---

## 5. Middleware & Utility Classes

```mermaid
classDiagram
    class AuthMiddleware {
        +verifyToken(req, res, next)
        +isAdmin(req, res, next)
        +isUser(req, res, next)
    }

    class ValidationMiddleware {
        +validatePegawai(req, res, next)
        +validateAbsensi(req, res, next)
        +validateCuti(req, res, next)
    }

    class MQTTClient {
        -client: mqtt.Client
        -topics: string[]
        +connect()
        +subscribe(topic)
        +publish(topic, message)
        +onMessage(callback)
    }

    class WebSocketServer {
        -io: SocketIO
        +emit(event, data)
        +broadcast(room, event, data)
        +joinRoom(socket, room)
    }

    class DatabasePool {
        -pool: pg.Pool
        +query(sql, params)
        +getClient()
        +releaseClient(client)
    }

    AuthMiddleware --> AuthService : uses
    MQTTClient --> AbsensiService : triggers
    WebSocketServer --> NotifikasiService : broadcasts
```

---

## 6. Tabel Ringkasan Class

| Class | Tipe | Deskripsi | Relasi |
|-------|------|-----------|--------|
| Pegawai | Entity | Data master karyawan | 1:N dengan Absensi, Payroll, Cuti, Tugas |
| Absensi | Entity | Record kehadiran harian | N:1 dengan Pegawai |
| User | Entity | Akun login sistem | - |
| Shift | Entity | Konfigurasi jam kerja | - |
| PengajuanCuti | Entity | Pengajuan cuti/izin | N:1 dengan Pegawai |
| Payroll | Entity | Data penggajian bulanan | N:1 dengan Pegawai |
| HariLibur | Entity | Kalender hari libur | - |
| Notifikasi | Entity | Notifikasi sistem | N:1 dengan Pegawai |
| Tugas | Entity | Upload file tugas | N:1 dengan Pegawai |
| SettingPayroll | Entity | Konfigurasi payroll | - |
| PegawaiService | Service | Business logic pegawai | - |
| AbsensiService | Service | Business logic absensi | - |
| PayrollService | Service | Business logic payroll | - |
| AuthService | Service | Business logic autentikasi | - |
| MQTTClient | Utility | Koneksi ke MQTT broker | - |
| WebSocketServer | Utility | Real-time communication | - |

---

*Dokumentasi Class Diagram - Sistem Absensi IoT Berbasis Fingerprint*
