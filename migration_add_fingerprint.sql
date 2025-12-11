-- Migration: Add fingerprint_id to pegawai table
-- Untuk integrasi fingerprint sensor enrollment

-- Tambah kolom fingerprint_id
ALTER TABLE pegawai ADD COLUMN IF NOT EXISTS fingerprint_id INTEGER UNIQUE;

-- Tambah comment
COMMENT ON COLUMN pegawai.fingerprint_id IS 'ID sidik jari dari fingerprint sensor (unique per pegawai)';

-- Index untuk faster query
CREATE INDEX IF NOT EXISTS idx_pegawai_fingerprint ON pegawai(fingerprint_id);

-- Grant permissions
GRANT ALL PRIVILEGES ON TABLE pegawai TO absensi_user;
