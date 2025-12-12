-- Fix waktu_absen column type issue
-- Change VARCHAR(10) to TIMESTAMP to store full datetime properly

-- Drop views that depend on waktu_absen column
DROP VIEW IF EXISTS view_absensi_lengkap;
DROP VIEW IF EXISTS view_statistik_pegawai;
DROP VIEW IF EXISTS view_tugas_lengkap;

-- Alter waktu_absen column from VARCHAR(10) to TIMESTAMP
-- First, we need to handle existing data
ALTER TABLE absensi ALTER COLUMN waktu_absen TYPE TIMESTAMP USING
  CASE
    WHEN waktu_absen ~ '^\d{4}-\d{2}-\d{2}T' THEN waktu_absen::TIMESTAMP
    ELSE CURRENT_TIMESTAMP
  END;

-- Also enlarge tipe_absen just to be safe
ALTER TABLE absensi ALTER COLUMN tipe_absen TYPE VARCHAR(20);

-- Recreate view_absensi_lengkap
CREATE VIEW view_absensi_lengkap AS
SELECT
    a.id_absensi,
    a.pegawai_id,
    a.waktu_absen,
    a.waktu_keluar,
    a.keterangan,
    a.tipe_absen,
    a.durasi_kerja,
    a.durasi_lembur,
    a.status_lembur,
    a.timestamp,
    p.nip,
    p.nama_pegawai,
    p.posisi,
    p.email,
    p.no_telepon,
    p.status
FROM absensi a
JOIN pegawai p ON a.pegawai_id = p.id_pegawai
ORDER BY a.timestamp DESC;

-- Recreate view_statistik_pegawai
CREATE VIEW view_statistik_pegawai AS
SELECT
    p.id_pegawai,
    p.nip,
    p.nama_pegawai,
    p.posisi,
    count(a.id_absensi) AS total_absensi,
    count(
        CASE
            WHEN a.keterangan = 'Hadir'::keterangan_absensi THEN 1
            ELSE NULL::integer
        END) AS hadir,
    count(
        CASE
            WHEN a.keterangan = 'Terlambat'::keterangan_absensi THEN 1
            ELSE NULL::integer
        END) AS terlambat,
    count(
        CASE
            WHEN a.keterangan = 'Izin'::keterangan_absensi THEN 1
            ELSE NULL::integer
        END) AS izin,
    count(
        CASE
            WHEN a.keterangan = 'Sakit'::keterangan_absensi THEN 1
            ELSE NULL::integer
        END) AS sakit
FROM pegawai p
LEFT JOIN absensi a ON p.id_pegawai = a.pegawai_id
GROUP BY p.id_pegawai;

-- Recreate view_tugas_lengkap
CREATE VIEW view_tugas_lengkap AS
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
        WHEN t.deadline < CURRENT_DATE THEN 'Terlambat'::text
        WHEN t.deadline = CURRENT_DATE THEN 'Hari Ini'::text
        ELSE 'Aktif'::text
    END AS status_deadline,
    t.deadline - CURRENT_DATE AS sisa_hari
FROM tugas t
JOIN pegawai p ON t.pegawai_id = p.id_pegawai
ORDER BY t.deadline;

-- Verify the changes
\d absensi
