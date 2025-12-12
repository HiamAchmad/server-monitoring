-- ================================================
-- DATABASE SISTEM ABSENSI (PostgreSQL)
-- ================================================
-- Dibuat untuk: Tugas Akhir Sistem Absensi IoT
-- Database: PostgreSQL
-- ================================================

-- Buat database (jika belum ada)
-- Jalankan dengan: psql -U postgres
CREATE DATABASE db_absensi;

-- Koneksi ke database
\c db_absensi;

-- ================================================
-- CREATE ENUM TYPES
-- ================================================
CREATE TYPE status_pegawai AS ENUM ('Aktif', 'Nonaktif');
CREATE TYPE keterangan_absensi AS ENUM ('Hadir', 'Terlambat', 'Izin', 'Sakit');
CREATE TYPE role_user AS ENUM ('Admin', 'User');

-- ================================================
-- TABEL: pegawai
-- Menyimpan data pegawai/karyawan
-- ================================================
CREATE TABLE IF NOT EXISTS pegawai (
    id_pegawai SERIAL PRIMARY KEY,
    nip VARCHAR(50) UNIQUE NOT NULL,
    nama_pegawai VARCHAR(100) NOT NULL,
    posisi VARCHAR(50) DEFAULT 'Karyawan',
    email VARCHAR(100),
    no_telepon VARCHAR(20),
    tanggal_bergabung DATE,
    status status_pegawai DEFAULT 'Aktif',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Komentar untuk kolom
COMMENT ON COLUMN pegawai.nip IS 'Nomor Induk Pegawai';

-- Trigger untuk auto-update updated_at
CREATE OR REPLACE FUNCTION update_updated_at_column()
RETURNS TRIGGER AS $$
BEGIN
    NEW.updated_at = CURRENT_TIMESTAMP;
    RETURN NEW;
END;
$$ language 'plpgsql';

CREATE TRIGGER update_pegawai_updated_at
    BEFORE UPDATE ON pegawai
    FOR EACH ROW
    EXECUTE FUNCTION update_updated_at_column();

-- ================================================
-- TABEL: absensi
-- Menyimpan riwayat absensi pegawai
-- ================================================
CREATE TABLE IF NOT EXISTS absensi (
    id_absensi SERIAL PRIMARY KEY,
    pegawai_id INT NOT NULL,
    waktu_absen VARCHAR(10) NOT NULL,
    keterangan keterangan_absensi DEFAULT 'Hadir',
    timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (pegawai_id) REFERENCES pegawai(id_pegawai) ON DELETE CASCADE
);

-- Komentar untuk kolom
COMMENT ON COLUMN absensi.waktu_absen IS 'Format HH:MM:SS dari ESP32';
COMMENT ON COLUMN absensi.timestamp IS 'Waktu lengkap dari server';

-- Index untuk optimasi
CREATE INDEX idx_timestamp ON absensi(timestamp);
CREATE INDEX idx_pegawai_id ON absensi(pegawai_id);

-- ================================================
-- TABEL: users (untuk login admin web)
-- ================================================
CREATE TABLE IF NOT EXISTS users (
    id_user SERIAL PRIMARY KEY,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL,
    nama_lengkap VARCHAR(100),
    role role_user DEFAULT 'User',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Komentar untuk kolom
COMMENT ON COLUMN users.password IS 'Password di-hash';

-- ================================================
-- TABEL: tugas (untuk upload file tugas karyawan)
-- ================================================
CREATE TABLE IF NOT EXISTS tugas (
    id_tugas SERIAL PRIMARY KEY,
    pegawai_id INT NOT NULL,
    nama_tugas VARCHAR(200) NOT NULL,
    deskripsi TEXT,
    file_name VARCHAR(255) NOT NULL,
    file_path VARCHAR(255) NOT NULL,
    deadline DATE NOT NULL,
    upload_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (pegawai_id) REFERENCES pegawai(id_pegawai) ON DELETE CASCADE
);

-- Komentar untuk kolom
COMMENT ON COLUMN tugas.nama_tugas IS 'Nama/Judul Tugas';
COMMENT ON COLUMN tugas.deskripsi IS 'Deskripsi tugas';
COMMENT ON COLUMN tugas.file_name IS 'Nama file asli';
COMMENT ON COLUMN tugas.file_path IS 'Path file di server';
COMMENT ON COLUMN tugas.deadline IS 'Deadline tugas';
COMMENT ON COLUMN tugas.upload_date IS 'Waktu upload';

-- Index untuk optimasi
CREATE INDEX idx_pegawai_tugas ON tugas(pegawai_id);
CREATE INDEX idx_deadline ON tugas(deadline);

-- ================================================
-- DATA SAMPLE
-- ================================================

-- Insert sample pegawai
INSERT INTO pegawai (nip, nama_pegawai, posisi, email, tanggal_bergabung) VALUES
('20210410700073', 'Achmad Hasan', 'Admin', 'hasan@example.com', '2021-01-15'),
('19451708200025', 'User1', 'Karyawan', 'user1@example.com', '2019-03-20'),
('20220101000001', 'Budi Santoso', 'Karyawan', 'budi@example.com', '2022-01-10'),
('20220101000002', 'Siti Aminah', 'Supervisor', 'siti@example.com', '2022-02-15');

-- Insert sample user admin (password: admin123 - harus di-hash di production!)
-- Catatan: Dalam implementasi nyata, gunakan bcrypt untuk hash password
INSERT INTO users (username, password, nama_lengkap, role) VALUES
('admin', 'admin123', 'Administrator', 'Admin'),
('user', 'user123', 'User Biasa', 'User');

-- Insert sample data absensi
INSERT INTO absensi (pegawai_id, waktu_absen, keterangan, timestamp) VALUES
(1, '08:00:15', 'Hadir', '2025-10-31 08:00:15'),
(2, '08:15:30', 'Hadir', '2025-10-31 08:15:30'),
(3, '09:30:00', 'Terlambat', '2025-10-31 09:30:00'),
(1, '08:05:00', 'Hadir', '2025-10-30 08:05:00'),
(2, '08:10:00', 'Hadir', '2025-10-30 08:10:00');

-- Insert sample tugas
INSERT INTO tugas (pegawai_id, nama_tugas, deskripsi, file_name, file_path, deadline, upload_date) VALUES
(1, 'Laporan Kinerja Bulanan Oktober', 'Laporan kinerja dan pencapaian bulan Oktober 2025', 'laporan-oktober.pdf', 'sample-laporan-oktober.pdf', '2025-11-05', '2025-10-31 10:00:00'),
(2, 'Dokumentasi Sistem', 'Dokumentasi lengkap sistem aplikasi', 'dokumentasi.docx', 'sample-dokumentasi.docx', '2025-11-10', '2025-10-31 11:30:00');

-- ================================================
-- VIEWS (Opsional - untuk memudahkan query)
-- ================================================

-- View untuk laporan absensi lengkap
CREATE OR REPLACE VIEW view_absensi_lengkap AS
SELECT
    a.id_absensi,
    a.pegawai_id,
    p.nip,
    p.nama_pegawai,
    p.posisi,
    a.waktu_absen,
    a.keterangan,
    a.timestamp,
    DATE(a.timestamp) as tanggal,
    a.timestamp::time as jam
FROM absensi a
JOIN pegawai p ON a.pegawai_id = p.id_pegawai
ORDER BY a.timestamp DESC;

-- View untuk statistik absensi per pegawai
CREATE OR REPLACE VIEW view_statistik_pegawai AS
SELECT
    p.id_pegawai,
    p.nip,
    p.nama_pegawai,
    p.posisi,
    COUNT(a.id_absensi) as total_absensi,
    COUNT(CASE WHEN a.keterangan = 'Hadir' THEN 1 END) as hadir,
    COUNT(CASE WHEN a.keterangan = 'Terlambat' THEN 1 END) as terlambat,
    COUNT(CASE WHEN a.keterangan = 'Izin' THEN 1 END) as izin,
    COUNT(CASE WHEN a.keterangan = 'Sakit' THEN 1 END) as sakit
FROM pegawai p
LEFT JOIN absensi a ON p.id_pegawai = a.pegawai_id
GROUP BY p.id_pegawai;

-- View untuk tugas dengan info pegawai
CREATE OR REPLACE VIEW view_tugas_lengkap AS
SELECT
    t.id_tugas,
    t.nama_tugas,
    t.deskripsi,
    t.file_name,
    t.file_path,
    t.deadline,
    t.upload_date,
    p.id_pegawai,
    p.nip,
    p.nama_pegawai,
    p.posisi,
    CASE
        WHEN t.deadline < CURRENT_DATE THEN 'Terlambat'
        WHEN t.deadline = CURRENT_DATE THEN 'Hari Ini'
        ELSE 'Aktif'
    END as status_deadline,
    (t.deadline - CURRENT_DATE) as sisa_hari
FROM tugas t
JOIN pegawai p ON t.pegawai_id = p.id_pegawai
ORDER BY t.deadline ASC;

-- ================================================
-- STORED PROCEDURES (Function)
-- ================================================

-- Function untuk mendapatkan absensi hari ini
CREATE OR REPLACE FUNCTION sp_absensi_hari_ini()
RETURNS TABLE (
    nama_pegawai VARCHAR,
    nip VARCHAR,
    waktu_absen VARCHAR,
    keterangan keterangan_absensi,
    timestamp TIMESTAMP
) AS $$
BEGIN
    RETURN QUERY
    SELECT
        p.nama_pegawai,
        p.nip,
        a.waktu_absen,
        a.keterangan,
        a.timestamp
    FROM absensi a
    JOIN pegawai p ON a.pegawai_id = p.id_pegawai
    WHERE DATE(a.timestamp) = CURRENT_DATE
    ORDER BY a.timestamp DESC;
END;
$$ LANGUAGE plpgsql;

-- ================================================
-- ADDITIONAL INDEXES untuk optimasi query
-- ================================================
CREATE INDEX IF NOT EXISTS idx_absensi_tanggal ON absensi(timestamp);
CREATE INDEX IF NOT EXISTS idx_pegawai_nip ON pegawai(nip);
CREATE INDEX IF NOT EXISTS idx_pegawai_status ON pegawai(status);

-- ================================================
-- SELESAI
-- ================================================
-- Database siap digunakan!
--
-- Cara import:
-- psql -U absensi_user -d db_absensi < database_postgresql.sql
--
-- atau dari psql prompt sebagai superuser:
-- \i /path/to/database_postgresql.sql
-- ================================================
