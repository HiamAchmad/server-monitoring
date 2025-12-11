-- ================================================
-- MIGRATION: Tambah Field untuk Submission Tugas
-- ================================================
-- Tanggal: 10 November 2025
-- Deskripsi: Menambahkan field untuk tracking submission tugas dari karyawan
-- ================================================

\c db_absensi;

-- Tambah kolom untuk submission dari user
ALTER TABLE tugas ADD COLUMN IF NOT EXISTS status VARCHAR(20) DEFAULT 'Pending';
ALTER TABLE tugas ADD COLUMN IF NOT EXISTS file_submit_path VARCHAR(255);
ALTER TABLE tugas ADD COLUMN IF NOT EXISTS tanggal_submit TIMESTAMP;
ALTER TABLE tugas ADD COLUMN IF NOT EXISTS catatan_submit TEXT;

-- Tambah comment untuk kolom baru
COMMENT ON COLUMN tugas.status IS 'Status tugas: Pending, Selesai, Terlambat';
COMMENT ON COLUMN tugas.file_submit_path IS 'Path file hasil pekerjaan yang di-submit user';
COMMENT ON COLUMN tugas.tanggal_submit IS 'Waktu user submit hasil tugas';
COMMENT ON COLUMN tugas.catatan_submit IS 'Catatan dari user saat submit tugas';

-- Update existing records ke status Pending
UPDATE tugas SET status = 'Pending' WHERE status IS NULL;

-- Create index untuk optimasi query by status
CREATE INDEX IF NOT EXISTS idx_tugas_status ON tugas(status);

-- Verifikasi kolom sudah ditambahkan
SELECT column_name, data_type, column_default
FROM information_schema.columns
WHERE table_name = 'tugas'
ORDER BY ordinal_position;

-- Tampilkan struktur table terbaru
\d tugas;

SELECT '✅ Migration berhasil! Field submission telah ditambahkan ke tabel tugas.' AS status;
