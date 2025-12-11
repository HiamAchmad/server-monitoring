#!/bin/bash
# Setup PostgreSQL dan Start Server
# Jalankan dengan: bash SETUP_SEKARANG.sh

echo "================================================"
echo "🚀 SETUP DATABASE & START SERVER"
echo "================================================"
echo ""

cd "$(dirname "$0")"

echo "Step 1: Checking PostgreSQL..."
if ! systemctl is-active --quiet postgresql; then
    echo "   Starting PostgreSQL..."
    sudo systemctl start postgresql
fi
echo "   ✅ PostgreSQL running"
echo ""

echo "Step 2: Creating database user and database..."
echo "   (Masukkan password sudo jika diminta)"
echo ""

sudo -u postgres psql << 'EOSQL'
-- Create user
DO $$
BEGIN
    IF NOT EXISTS (SELECT FROM pg_user WHERE usename = 'absensi_user') THEN
        CREATE USER absensi_user WITH PASSWORD 'absensi_password';
        RAISE NOTICE 'User created: absensi_user';
    ELSE
        RAISE NOTICE 'User already exists: absensi_user';
    END IF;
END $$;

-- Create database
SELECT 'CREATE DATABASE db_absensi OWNER absensi_user'
WHERE NOT EXISTS (SELECT FROM pg_database WHERE datname = 'db_absensi')\gexec

-- Grant privileges
GRANT ALL PRIVILEGES ON DATABASE db_absensi TO absensi_user;
EOSQL

echo ""
echo "Step 3: Granting schema privileges..."
sudo -u postgres psql -d db_absensi << 'EOSQL'
GRANT ALL ON SCHEMA public TO absensi_user;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO absensi_user;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO absensi_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO absensi_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON SEQUENCES TO absensi_user;
EOSQL

echo ""
echo "Step 4: Importing database schema..."
sudo -u postgres psql -d db_absensi -f database_postgresql.sql > /dev/null 2>&1
echo "   ✅ Schema imported"

echo ""
echo "Step 5: Verifying database..."
PGPASSWORD=absensi_password psql -h localhost -U absensi_user -d db_absensi -c "SELECT id_pegawai, nama_pegawai FROM pegawai;" 2>&1 | head -10

echo ""
echo "================================================"
echo "✅ DATABASE SETUP COMPLETE!"
echo "================================================"
echo ""
echo "Step 6: Starting server..."
echo ""
echo "   Server akan berjalan di:"
echo "   - MQTT Broker: port 1883"
echo "   - HTTP Server: port 3000"
echo "   - Dashboard: http://localhost:3000/dashboard.html"
echo ""
echo "   Tekan Ctrl+C untuk stop server"
echo ""
echo "================================================"
echo ""

# Start server
node server.js
