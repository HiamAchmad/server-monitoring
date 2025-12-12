-- ================================================
-- DATABASE SISTEM ABSENSI
-- ================================================
-- Dibuat untuk: Tugas Akhir Sistem Absensi IoT
-- Database: MySQL/MariaDB
-- ================================================

-- Buat database (jika belum ada)
CREATE DATABASE IF NOT EXISTS db_absensi;
USE db_absensi;

-- ================================================
-- TABEL: pegawai
-- Menyimpan data pegawai/karyawan
-- ================================================
CREATE TABLE IF NOT EXISTS pegawai (
    id_pegawai INT PRIMARY KEY AUTO_INCREMENT,
    nip VARCHAR(50) UNIQUE NOT NULL COMMENT 'Nomor Induk Pegawai',
    nama_pegawai VARCHAR(100) NOT NULL,
    posisi VARCHAR(50) DEFAULT 'Karyawan',
    email VARCHAR(100),
    no_telepon VARCHAR(20),
    tanggal_bergabung DATE,
    status ENUM('Aktif', 'Nonaktif') DEFAULT 'Aktif',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ================================================
-- TABEL: absensi
-- Menyimpan riwayat absensi pegawai
-- ================================================
CREATE TABLE IF NOT EXISTS absensi (
    id_absensi INT PRIMARY KEY AUTO_INCREMENT,
    pegawai_id INT NOT NULL,
    waktu_absen VARCHAR(10) NOT NULL COMMENT 'Format HH:MM:SS dari ESP32',
    keterangan ENUM('Hadir', 'Terlambat', 'Izin', 'Sakit') DEFAULT 'Hadir',
    timestamp DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT 'Waktu lengkap dari server',
    FOREIGN KEY (pegawai_id) REFERENCES pegawai(id_pegawai) ON DELETE CASCADE,
    INDEX idx_timestamp (timestamp),
    INDEX idx_pegawai_id (pegawai_id)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ================================================
-- TABEL: users (untuk login admin web)
-- ================================================
CREATE TABLE IF NOT EXISTS users (
    id_user INT PRIMARY KEY AUTO_INCREMENT,
    username VARCHAR(50) UNIQUE NOT NULL,
    password VARCHAR(255) NOT NULL COMMENT 'Password di-hash',
    nama_lengkap VARCHAR(100),
    role ENUM('Admin', 'User') DEFAULT 'User',
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

-- ================================================
-- TABEL: tugas (untuk upload file tugas karyawan)
-- ================================================
CREATE TABLE IF NOT EXISTS tugas (
    id_tugas INT PRIMARY KEY AUTO_INCREMENT,
    pegawai_id INT NOT NULL,
    nama_tugas VARCHAR(200) NOT NULL COMMENT 'Nama/Judul Tugas',
    deskripsi TEXT COMMENT 'Deskripsi tugas',
    file_name VARCHAR(255) NOT NULL COMMENT 'Nama file asli',
    file_path VARCHAR(255) NOT NULL COMMENT 'Path file di server',
    deadline DATE NOT NULL COMMENT 'Deadline tugas',
    upload_date DATETIME DEFAULT CURRENT_TIMESTAMP COMMENT 'Waktu upload',
    FOREIGN KEY (pegawai_id) REFERENCES pegawai(id_pegawai) ON DELETE CASCADE,
    INDEX idx_pegawai_tugas (pegawai_id),
    INDEX idx_deadline (deadline)
) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_unicode_ci;

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
    TIME(a.timestamp) as jam
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
        WHEN t.deadline < CURDATE() THEN 'Terlambat'
        WHEN t.deadline = CURDATE() THEN 'Hari Ini'
        ELSE 'Aktif'
    END as status_deadline,
    DATEDIFF(t.deadline, CURDATE()) as sisa_hari
FROM tugas t
JOIN pegawai p ON t.pegawai_id = p.id_pegawai
ORDER BY t.deadline ASC;

-- ================================================
-- STORED PROCEDURES (Opsional)
-- ================================================

-- Procedure untuk mendapatkan absensi hari ini
DELIMITER //
CREATE PROCEDURE IF NOT EXISTS sp_absensi_hari_ini()
BEGIN
    SELECT
        p.nama_pegawai,
        p.nip,
        a.waktu_absen,
        a.keterangan,
        a.timestamp
    FROM absensi a
    JOIN pegawai p ON a.pegawai_id = p.id_pegawai
    WHERE DATE(a.timestamp) = CURDATE()
    ORDER BY a.timestamp DESC;
END //
DELIMITER ;

-- ================================================
-- INDEXES untuk optimasi query
-- ================================================
CREATE INDEX idx_absensi_tanggal ON absensi(timestamp);
CREATE INDEX idx_pegawai_nip ON pegawai(nip);
CREATE INDEX idx_pegawai_status ON pegawai(status);

-- ================================================
-- SELESAI
-- ================================================
-- Database siap digunakan!
--
-- Cara import:
-- mysql -u root -p < database.sql
--
-- atau dari MySQL prompt:
-- source /path/to/database.sql
-- ================================================
