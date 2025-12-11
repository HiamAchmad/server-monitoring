-- Fix Permissions untuk User absensi_user
-- Jalankan dengan: sudo -u postgres psql -d db_absensi -f fix_permissions.sql

-- Grant akses ke schema public
GRANT ALL ON SCHEMA public TO absensi_user;

-- Grant akses ke semua tabel
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO absensi_user;

-- Grant akses ke semua sequences (termasuk tugas_id_tugas_seq)
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO absensi_user;

-- Grant akses untuk tabel dan sequence yang akan dibuat di masa depan
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO absensi_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON SEQUENCES TO absensi_user;

-- Verifikasi permissions
\dp tugas
\dp tugas_id_tugas_seq

-- Selesai
\echo 'Permissions berhasil diperbaiki!'
