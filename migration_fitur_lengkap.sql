-- ================================================
-- MIGRATION: FITUR LENGKAP SISTEM ABSENSI
-- Tanggal: 2025-11-27
-- ================================================

-- ================================================
-- 1. UPDATE TABEL PEGAWAI (Tambah kolom)
-- ================================================
ALTER TABLE pegawai ADD COLUMN IF NOT EXISTS divisi VARCHAR(50);
ALTER TABLE pegawai ADD COLUMN IF NOT EXISTS foto VARCHAR(255);
ALTER TABLE pegawai ADD COLUMN IF NOT EXISTS jadwal_kerja_id INT;
ALTER TABLE pegawai ADD COLUMN IF NOT EXISTS gaji_pokok DECIMAL(12,2) DEFAULT 0;
ALTER TABLE pegawai ADD COLUMN IF NOT EXISTS password VARCHAR(255);

-- ================================================
-- 2. TABEL DIVISI
-- ================================================
CREATE TABLE IF NOT EXISTS divisi (
    id_divisi SERIAL PRIMARY KEY,
    nama_divisi VARCHAR(100) NOT NULL,
    deskripsi TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Insert default divisi
INSERT INTO divisi (nama_divisi, deskripsi) VALUES
('IT', 'Divisi Teknologi Informasi'),
('HRD', 'Divisi Human Resource Development'),
('Finance', 'Divisi Keuangan'),
('Marketing', 'Divisi Pemasaran'),
('Operasional', 'Divisi Operasional'),
('Umum', 'Divisi Umum')
ON CONFLICT DO NOTHING;

-- ================================================
-- 3. TABEL SHIFT KERJA
-- ================================================
CREATE TABLE IF NOT EXISTS shift (
    id_shift SERIAL PRIMARY KEY,
    nama_shift VARCHAR(50) NOT NULL,
    jam_masuk TIME NOT NULL,
    jam_keluar TIME NOT NULL,
    toleransi_terlambat INT DEFAULT 15, -- menit
    jam_lembur_mulai TIME,
    is_active BOOLEAN DEFAULT true,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Insert default shift
INSERT INTO shift (nama_shift, jam_masuk, jam_keluar, toleransi_terlambat, jam_lembur_mulai) VALUES
('Pagi', '08:00', '17:00', 15, '18:00'),
('Sore', '14:00', '22:00', 15, '23:00'),
('Malam', '22:00', '06:00', 15, '07:00')
ON CONFLICT DO NOTHING;

-- ================================================
-- 4. TABEL JADWAL KERJA
-- ================================================
CREATE TABLE IF NOT EXISTS jadwal_kerja (
    id_jadwal SERIAL PRIMARY KEY,
    nama_jadwal VARCHAR(100) NOT NULL,
    shift_senin INT REFERENCES shift(id_shift),
    shift_selasa INT REFERENCES shift(id_shift),
    shift_rabu INT REFERENCES shift(id_shift),
    shift_kamis INT REFERENCES shift(id_shift),
    shift_jumat INT REFERENCES shift(id_shift),
    shift_sabtu INT REFERENCES shift(id_shift),
    shift_minggu INT REFERENCES shift(id_shift),
    is_active BOOLEAN DEFAULT true,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Insert default jadwal
INSERT INTO jadwal_kerja (nama_jadwal, shift_senin, shift_selasa, shift_rabu, shift_kamis, shift_jumat) VALUES
('Reguler (Senin-Jumat)', 1, 1, 1, 1, 1)
ON CONFLICT DO NOTHING;

-- ================================================
-- 5. TABEL HARI LIBUR
-- ================================================
CREATE TABLE IF NOT EXISTS hari_libur (
    id_libur SERIAL PRIMARY KEY,
    tanggal DATE NOT NULL UNIQUE,
    nama_libur VARCHAR(100) NOT NULL,
    jenis VARCHAR(50) DEFAULT 'Nasional', -- Nasional, Cuti Bersama
    is_recurring BOOLEAN DEFAULT false, -- Untuk libur tetap tiap tahun
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Insert sample hari libur 2025
INSERT INTO hari_libur (tanggal, nama_libur, jenis) VALUES
('2025-01-01', 'Tahun Baru', 'Nasional'),
('2025-03-29', 'Hari Raya Nyepi', 'Nasional'),
('2025-03-31', 'Idul Fitri', 'Nasional'),
('2025-04-01', 'Idul Fitri', 'Nasional'),
('2025-04-18', 'Jumat Agung', 'Nasional'),
('2025-05-01', 'Hari Buruh', 'Nasional'),
('2025-05-12', 'Hari Raya Waisak', 'Nasional'),
('2025-05-29', 'Kenaikan Isa Almasih', 'Nasional'),
('2025-06-01', 'Hari Lahir Pancasila', 'Nasional'),
('2025-06-07', 'Idul Adha', 'Nasional'),
('2025-06-27', 'Tahun Baru Islam', 'Nasional'),
('2025-08-17', 'Hari Kemerdekaan', 'Nasional'),
('2025-09-05', 'Maulid Nabi', 'Nasional'),
('2025-12-25', 'Natal', 'Nasional')
ON CONFLICT DO NOTHING;

-- ================================================
-- 6. TABEL PENGAJUAN CUTI/IZIN
-- ================================================
CREATE TYPE status_pengajuan AS ENUM ('Menunggu', 'Disetujui', 'Ditolak');
CREATE TYPE jenis_cuti AS ENUM ('Cuti Tahunan', 'Cuti Sakit', 'Izin Khusus', 'Cuti Melahirkan', 'Tanpa Keterangan');

CREATE TABLE IF NOT EXISTS pengajuan_cuti (
    id_pengajuan SERIAL PRIMARY KEY,
    pegawai_id INT NOT NULL REFERENCES pegawai(id_pegawai) ON DELETE CASCADE,
    jenis jenis_cuti NOT NULL,
    tanggal_mulai DATE NOT NULL,
    tanggal_selesai DATE NOT NULL,
    jumlah_hari INT,
    alasan TEXT,
    dokumen VARCHAR(255), -- Surat sakit dll
    status status_pengajuan DEFAULT 'Menunggu',
    disetujui_oleh INT REFERENCES pegawai(id_pegawai),
    tanggal_disetujui TIMESTAMP,
    komentar_approval TEXT,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Trigger untuk hitung jumlah hari
CREATE OR REPLACE FUNCTION hitung_jumlah_hari_cuti()
RETURNS TRIGGER AS $$
BEGIN
    NEW.jumlah_hari = (NEW.tanggal_selesai - NEW.tanggal_mulai) + 1;
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ LANGUAGE plpgsql;

CREATE TRIGGER trigger_hitung_hari_cuti
    BEFORE INSERT OR UPDATE ON pengajuan_cuti
    FOR EACH ROW
    EXECUTE FUNCTION hitung_jumlah_hari_cuti();

-- ================================================
-- 7. TABEL PAYROLL/PENGGAJIAN
-- ================================================
CREATE TABLE IF NOT EXISTS payroll (
    id_payroll SERIAL PRIMARY KEY,
    pegawai_id INT NOT NULL REFERENCES pegawai(id_pegawai) ON DELETE CASCADE,
    bulan INT NOT NULL,
    tahun INT NOT NULL,

    -- Kehadiran
    total_hari_kerja INT DEFAULT 0,
    total_hadir INT DEFAULT 0,
    total_terlambat INT DEFAULT 0,
    total_izin INT DEFAULT 0,
    total_sakit INT DEFAULT 0,
    total_tanpa_keterangan INT DEFAULT 0,

    -- Jam
    total_jam_kerja DECIMAL(10,2) DEFAULT 0,
    total_jam_lembur DECIMAL(10,2) DEFAULT 0,

    -- Gaji
    gaji_pokok DECIMAL(12,2) DEFAULT 0,
    uang_lembur DECIMAL(12,2) DEFAULT 0,
    tunjangan DECIMAL(12,2) DEFAULT 0,
    potongan_terlambat DECIMAL(12,2) DEFAULT 0,
    potongan_absen DECIMAL(12,2) DEFAULT 0,
    potongan_lain DECIMAL(12,2) DEFAULT 0,
    total_gaji DECIMAL(12,2) DEFAULT 0,

    keterangan TEXT,
    status VARCHAR(20) DEFAULT 'Draft', -- Draft, Diproses, Selesai
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    UNIQUE(pegawai_id, bulan, tahun)
);

-- ================================================
-- 8. TABEL SETTING PAYROLL
-- ================================================
CREATE TABLE IF NOT EXISTS setting_payroll (
    id_setting SERIAL PRIMARY KEY,
    nama_setting VARCHAR(100) NOT NULL UNIQUE,
    nilai DECIMAL(12,2) NOT NULL,
    keterangan TEXT,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Insert default setting payroll
INSERT INTO setting_payroll (nama_setting, nilai, keterangan) VALUES
('tarif_lembur_per_jam', 25000, 'Tarif lembur per jam'),
('potongan_terlambat_per_menit', 1000, 'Potongan keterlambatan per menit'),
('potongan_tanpa_keterangan', 100000, 'Potongan per hari tanpa keterangan'),
('tunjangan_makan', 500000, 'Tunjangan makan per bulan'),
('tunjangan_transport', 300000, 'Tunjangan transportasi per bulan')
ON CONFLICT (nama_setting) DO NOTHING;

-- ================================================
-- 9. INDEX UNTUK OPTIMASI
-- ================================================
CREATE INDEX IF NOT EXISTS idx_pengajuan_pegawai ON pengajuan_cuti(pegawai_id);
CREATE INDEX IF NOT EXISTS idx_pengajuan_status ON pengajuan_cuti(status);
CREATE INDEX IF NOT EXISTS idx_pengajuan_tanggal ON pengajuan_cuti(tanggal_mulai, tanggal_selesai);
CREATE INDEX IF NOT EXISTS idx_payroll_bulan ON payroll(bulan, tahun);
CREATE INDEX IF NOT EXISTS idx_payroll_pegawai ON payroll(pegawai_id);
CREATE INDEX IF NOT EXISTS idx_hari_libur ON hari_libur(tanggal);

-- ================================================
-- 10. VIEW UNTUK LAPORAN
-- ================================================

-- View rekap absensi bulanan
CREATE OR REPLACE VIEW view_rekap_bulanan AS
SELECT
    p.id_pegawai,
    p.nip,
    p.nama_pegawai,
    p.posisi,
    p.divisi,
    EXTRACT(MONTH FROM a.timestamp) as bulan,
    EXTRACT(YEAR FROM a.timestamp) as tahun,
    COUNT(*) as total_absen,
    COUNT(CASE WHEN a.keterangan = 'Hadir' THEN 1 END) as hadir,
    COUNT(CASE WHEN a.keterangan = 'Terlambat' THEN 1 END) as terlambat,
    COUNT(CASE WHEN a.keterangan = 'Izin' THEN 1 END) as izin,
    COUNT(CASE WHEN a.keterangan = 'Sakit' THEN 1 END) as sakit,
    SUM(COALESCE(EXTRACT(EPOCH FROM (a.waktu_keluar::time - a.waktu_absen::time))/3600, 0)) as total_jam
FROM pegawai p
LEFT JOIN absensi a ON p.id_pegawai = a.pegawai_id
GROUP BY p.id_pegawai, p.nip, p.nama_pegawai, p.posisi, p.divisi,
         EXTRACT(MONTH FROM a.timestamp), EXTRACT(YEAR FROM a.timestamp);

-- View rekap lembur
CREATE OR REPLACE VIEW view_rekap_lembur AS
SELECT
    p.id_pegawai,
    p.nip,
    p.nama_pegawai,
    p.posisi,
    EXTRACT(MONTH FROM a.timestamp) as bulan,
    EXTRACT(YEAR FROM a.timestamp) as tahun,
    COUNT(CASE WHEN a.status_lembur = 'Ya' THEN 1 END) as jumlah_hari_lembur,
    SUM(COALESCE(EXTRACT(EPOCH FROM a.durasi_lembur)/3600, 0)) as total_jam_lembur
FROM pegawai p
LEFT JOIN absensi a ON p.id_pegawai = a.pegawai_id
WHERE a.status_lembur = 'Ya'
GROUP BY p.id_pegawai, p.nip, p.nama_pegawai, p.posisi,
         EXTRACT(MONTH FROM a.timestamp), EXTRACT(YEAR FROM a.timestamp);

-- ================================================
-- SELESAI MIGRATION
-- ================================================
-- Jalankan: psql -U absensi_user -d db_absensi < migration_fitur_lengkap.sql