-- Setup PostgreSQL Database untuk Sistem Absensi
-- Jalankan dengan: sudo -u postgres psql -f setup_db.sql

-- Create user if not exists
DO $$
BEGIN
    IF NOT EXISTS (SELECT FROM pg_catalog.pg_user WHERE usename = 'absensi_user') THEN
        CREATE USER absensi_user WITH PASSWORD 'absensi_password';
        RAISE NOTICE 'User absensi_user berhasil dibuat';
    ELSE
        RAISE NOTICE 'User absensi_user sudah ada';
    END IF;
END
$$;

-- Create database if not exists
SELECT 'CREATE DATABASE db_absensi OWNER absensi_user'
WHERE NOT EXISTS (SELECT FROM pg_database WHERE datname = 'db_absensi')\gexec

-- Grant privileges
GRANT ALL PRIVILEGES ON DATABASE db_absensi TO absensi_user;

\echo 'Database setup complete!'
\echo 'Connecting to db_absensi...'
\c db_absensi

-- Grant schema privileges
GRANT ALL ON SCHEMA public TO absensi_user;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO absensi_user;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO absensi_user;

-- Set default privileges for future objects
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO absensi_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON SEQUENCES TO absensi_user;

\echo 'Privileges granted successfully!'
\echo 'Ready to import schema...'
