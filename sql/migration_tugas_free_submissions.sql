-- Migration: Create table for free task submissions
-- Karyawan bisa submit tugas bebas tanpa harus link ke tugas yang sudah ada

CREATE TABLE IF NOT EXISTS tugas_submissions (
    id_submission SERIAL PRIMARY KEY,
    pegawai_id INTEGER NOT NULL REFERENCES pegawai(id_pegawai) ON DELETE CASCADE,
    nama_tugas VARCHAR(255) NOT NULL,
    catatan TEXT,
    file_path VARCHAR(255) NOT NULL,
    tanggal_submit TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

-- Index untuk faster query
CREATE INDEX IF NOT EXISTS idx_submissions_pegawai ON tugas_submissions(pegawai_id);
CREATE INDEX IF NOT EXISTS idx_submissions_tanggal ON tugas_submissions(tanggal_submit);

-- Grant permissions
GRANT ALL PRIVILEGES ON TABLE tugas_submissions TO absensi_user;
GRANT USAGE, SELECT ON SEQUENCE tugas_submissions_id_submission_seq TO absensi_user;
