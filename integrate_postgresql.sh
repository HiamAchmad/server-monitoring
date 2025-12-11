#!/bin/bash
# Integrasi PostgreSQL untuk Sistem Absensi
# Jalankan dengan: bash integrate_postgresql.sh

set -e  # Exit on error

echo "================================================"
echo "🔧 INTEGRASI POSTGRESQL - SISTEM ABSENSI"
echo "================================================"
echo ""

# Get script directory
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
cd "$SCRIPT_DIR"

# Check if PostgreSQL is running
echo "1. Checking PostgreSQL service..."
if systemctl is-active --quiet postgresql; then
    echo "   ✅ PostgreSQL is running"
else
    echo "   ❌ PostgreSQL is not running"
    echo "   Starting PostgreSQL..."
    sudo systemctl start postgresql
    echo "   ✅ PostgreSQL started"
fi

echo ""
echo "2. Creating database and user..."
sudo -u postgres psql -f setup_db.sql

echo ""
echo "3. Importing database schema..."
sudo -u postgres psql -d db_absensi -f database_postgresql.sql 2>&1 | grep -E "INSERT|CREATE|ERROR" || echo "   ✅ Schema imported"

echo ""
echo "4. Testing database connection..."
PGPASSWORD=absensi_password psql -h localhost -U absensi_user -d db_absensi -c "\dt" > /dev/null 2>&1
if [ $? -eq 0 ]; then
    echo "   ✅ Connection successful!"

    echo ""
    echo "5. Checking database contents..."
    PGPASSWORD=absensi_password psql -h localhost -U absensi_user -d db_absensi << 'EOSQL'
\echo '   Tables in database:'
\dt
\echo ''
\echo '   Pegawai data:'
SELECT id_pegawai, nip, nama_pegawai, posisi FROM pegawai;
\echo ''
\echo '   Total records:'
SELECT
    (SELECT COUNT(*) FROM pegawai) as total_pegawai,
    (SELECT COUNT(*) FROM absensi) as total_absensi,
    (SELECT COUNT(*) FROM tugas) as total_tugas;
EOSQL
else
    echo "   ❌ Connection failed!"
    exit 1
fi

echo ""
echo "================================================"
echo "✅ INTEGRASI POSTGRESQL SELESAI!"
echo "================================================"
echo ""
echo "📊 Database Info:"
echo "   Host: localhost"
echo "   Port: 5432"
echo "   Database: db_absensi"
echo "   User: absensi_user"
echo "   Password: absensi_password"
echo ""
echo "🚀 Next steps:"
echo "   1. Start server: node server.js"
echo "   2. Access dashboard: http://localhost:3000/dashboard.html"
echo "   3. Test MQTT: node test_mqtt.js"
echo ""
echo "📝 Documentation:"
echo "   - POSTGRESQL_MIGRATION_COMPLETE.md"
echo "   - SETUP_POSTGRESQL.md"
echo ""
