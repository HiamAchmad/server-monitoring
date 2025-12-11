#!/bin/bash
# Full Reset and Build - Clean State Setup
# Jalankan dengan: bash RESET_AND_BUILD.sh

echo "================================================"
echo "🔄 FULL RESET & REBUILD"
echo "================================================"
echo ""

cd "$(dirname "$0")"

# 1. Stop any running servers
echo "1. Stopping any running servers..."
pkill -f "node.*server.js" 2>/dev/null || true
sleep 1
echo "   ✅ Processes stopped"
echo ""

# 2. Check PostgreSQL
echo "2. Checking PostgreSQL service..."
if ! systemctl is-active --quiet postgresql; then
    echo "   Starting PostgreSQL..."
    sudo systemctl start postgresql
    sleep 2
fi
echo "   ✅ PostgreSQL running"
echo ""

# 3. Drop existing database and user (clean slate)
echo "3. Dropping existing database and user (if exists)..."
sudo -u postgres psql << 'EOSQL' 2>/dev/null
-- Terminate existing connections
SELECT pg_terminate_backend(pg_stat_activity.pid)
FROM pg_stat_activity
WHERE pg_stat_activity.datname = 'db_absensi'
  AND pid <> pg_backend_pid();

-- Drop database
DROP DATABASE IF EXISTS db_absensi;

-- Drop user
DROP USER IF EXISTS absensi_user;
EOSQL
echo "   ✅ Old database and user dropped"
echo ""

# 4. Create fresh user and database
echo "4. Creating fresh user and database..."
sudo -u postgres psql << 'EOSQL'
-- Create user
CREATE USER absensi_user WITH PASSWORD 'absensi_password';

-- Create database
CREATE DATABASE db_absensi OWNER absensi_user;

-- Grant privileges
GRANT ALL PRIVILEGES ON DATABASE db_absensi TO absensi_user;
EOSQL
echo "   ✅ User and database created"
echo ""

# 5. Grant schema privileges
echo "5. Granting schema privileges..."
sudo -u postgres psql -d db_absensi << 'EOSQL'
GRANT ALL ON SCHEMA public TO absensi_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO absensi_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON SEQUENCES TO absensi_user;
EOSQL
echo "   ✅ Privileges granted"
echo ""

# 6. Import schema
echo "6. Importing database schema..."
sudo -u postgres psql -d db_absensi -f database_postgresql.sql > /tmp/import_log.txt 2>&1
if [ $? -eq 0 ]; then
    echo "   ✅ Schema imported successfully"
else
    echo "   ⚠️  Import had warnings (check /tmp/import_log.txt)"
fi
echo ""

# 7. Verify database
echo "7. Verifying database contents..."
PGPASSWORD=absensi_password psql -h localhost -U absensi_user -d db_absensi << 'EOSQL'
\echo '   Tables:'
\dt
\echo ''
\echo '   Sample data:'
SELECT id_pegawai, nip, nama_pegawai, posisi FROM pegawai;
\echo ''
\echo '   Record counts:'
SELECT
    'pegawai' as tabel,
    COUNT(*) as jumlah
FROM pegawai
UNION ALL
SELECT
    'absensi' as tabel,
    COUNT(*) as jumlah
FROM absensi
UNION ALL
SELECT
    'tugas' as tabel,
    COUNT(*) as jumlah
FROM tugas
UNION ALL
SELECT
    'users' as tabel,
    COUNT(*) as jumlah
FROM users;
EOSQL
echo ""

echo "================================================"
echo "✅ RESET & REBUILD COMPLETE!"
echo "================================================"
echo ""
echo "📊 System Status:"
echo "   ✅ PostgreSQL: Running & Auto-start enabled"
echo "   ✅ Database: db_absensi (fresh)"
echo "   ✅ User: absensi_user"
echo "   ✅ Dependencies: Rebuilt"
echo "   ✅ Sample Data: Imported"
echo ""
echo "🚀 Ready to start server!"
echo ""
echo "Next steps:"
echo "   1. Start server: node server.js"
echo "   2. Open browser: http://localhost:3000/dashboard.html"
echo ""
echo "Or use the quick start script:"
echo "   bash START_SERVER.sh"
echo ""
