-- ================================================
-- MIGRATION: FITUR NOTIFIKASI
-- Tanggal: 2025-12-09
-- ================================================

-- Tabel Notifikasi
CREATE TABLE IF NOT EXISTS notifikasi (
    id_notifikasi SERIAL PRIMARY KEY,
    pegawai_id INT REFERENCES pegawai(id_pegawai) ON DELETE CASCADE,
    judul VARCHAR(100) NOT NULL,
    pesan TEXT NOT NULL,
    tipe VARCHAR(50) DEFAULT 'info', -- info, success, warning, error
    kategori VARCHAR(50) DEFAULT 'sistem', -- sistem, absensi, cuti, payroll, tugas
    is_read BOOLEAN DEFAULT false,
    is_admin BOOLEAN DEFAULT false, -- true = untuk admin, false = untuk user
    link VARCHAR(255), -- link tujuan jika diklik
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Index untuk optimasi query
CREATE INDEX IF NOT EXISTS idx_notifikasi_pegawai ON notifikasi(pegawai_id);
CREATE INDEX IF NOT EXISTS idx_notifikasi_is_read ON notifikasi(is_read);
CREATE INDEX IF NOT EXISTS idx_notifikasi_is_admin ON notifikasi(is_admin);
CREATE INDEX IF NOT EXISTS idx_notifikasi_created ON notifikasi(created_at DESC);

-- Contoh notifikasi awal
INSERT INTO notifikasi (pegawai_id, judul, pesan, tipe, kategori, is_admin) VALUES
(NULL, 'Selamat Datang', 'Sistem notifikasi telah aktif', 'success', 'sistem', true);
