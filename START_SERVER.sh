#!/bin/bash
# Quick Server Start
# Jalankan dengan: bash START_SERVER.sh

cd "$(dirname "$0")"

echo "================================================"
echo "🚀 STARTING SERVER"
echo "================================================"
echo ""

# Check if database exists
echo "Checking database..."
PGPASSWORD=absensi_password psql -h localhost -U absensi_user -d db_absensi -c "SELECT 1" > /dev/null 2>&1

if [ $? -ne 0 ]; then
    echo "❌ Database not ready!"
    echo ""
    echo "Please run setup first:"
    echo "   bash RESET_AND_BUILD.sh"
    echo ""
    exit 1
fi

echo "✅ Database ready"
echo ""
echo "Starting server..."
echo ""
echo "================================================"
echo "Server akan berjalan di:"
echo "   🌐 Dashboard: http://localhost:3000/dashboard.html"
echo "   📋 Manajemen: http://localhost:3000/manajemen.html"
echo "   📊 Riwayat:   http://localhost:3000/riwayat.html"
echo "   📡 MQTT:      port 1883"
echo "   ☁️  OwnCloud: http://localhost:8080"
echo ""
echo "Tekan Ctrl+C untuk stop server"
echo "================================================"
echo ""

# Start server
node server.js
