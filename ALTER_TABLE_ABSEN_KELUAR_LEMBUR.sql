-- ============================================
-- ALTER TABLE: Tambah Kolom Absen Keluar & Lembur
-- ============================================
-- Tanggal: 6 November 2025
-- Purpose: Menambahkan fitur absen keluar dan perhitungan lembur

-- Gunakan database yang sesuai
\c db_absensi

-- 1. Tambah kolom waktu_keluar (waktu check out)
ALTER TABLE absensi
ADD COLUMN IF NOT EXISTS waktu_keluar TIME;

-- 2. Tambah kolom durasi_kerja (dalam menit)
ALTER TABLE absensi
ADD COLUMN IF NOT EXISTS durasi_kerja INTEGER DEFAULT 0;

-- 3. Tambah kolom durasi_lembur (dalam menit)
ALTER TABLE absensi
ADD COLUMN IF NOT EXISTS durasi_lembur INTEGER DEFAULT 0;

-- 4. Tambah kolom status_lembur
ALTER TABLE absensi
ADD COLUMN IF NOT EXISTS status_lembur VARCHAR(20) DEFAULT 'Tidak';

-- 5. Tambah kolom tipe_absen (Masuk/Keluar)
ALTER TABLE absensi
ADD COLUMN IF NOT EXISTS tipe_absen VARCHAR(10) DEFAULT 'Masuk';

-- Tampilkan struktur table yang sudah diupdate
\d absensi

-- Query untuk cek data
SELECT * FROM absensi ORDER BY id_absensi DESC LIMIT 5;

-- Info
SELECT 'Database berhasil diupdate!' as status;
SELECT 'Kolom baru:' as info;
SELECT '- waktu_keluar (TIME)' as kolom_1;
SELECT '- durasi_kerja (INTEGER - menit)' as kolom_2;
SELECT '- durasi_lembur (INTEGER - menit)' as kolom_3;
SELECT '- status_lembur (VARCHAR)' as kolom_4;
SELECT '- tipe_absen (VARCHAR - Masuk/Keluar)' as kolom_5;
