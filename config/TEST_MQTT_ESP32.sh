#!/bin/bash

# ============================================
# SCRIPT TESTING MQTT UNTUK ESP32
# ============================================
# Script ini untuk test MQTT connection
# sebelum menggunakan ESP32 fisik
#
# Requirement: mosquitto-clients
# Install: sudo apt-get install mosquitto-clients

echo "╔════════════════════════════════════════════╗"
echo "║   TEST MQTT CONNECTION UNTUK ESP32         ║"
echo "╚════════════════════════════════════════════╝"
echo ""

# ============================================
# KONFIGURASI
# ============================================
MQTT_HOST="localhost"
MQTT_PORT="1883"
MQTT_TOPIC="absensi/data"

# ============================================
# 1. TEST MQTT BROKER
# ============================================
echo "📡 Testing MQTT Broker..."
echo "   Host: $MQTT_HOST"
echo "   Port: $MQTT_PORT"
echo ""

# Check if mosquitto-clients installed
if ! command -v mosquitto_pub &> /dev/null; then
    echo "❌ mosquitto-clients tidak terinstall!"
    echo ""
    echo "Install dengan command:"
    echo "   sudo apt-get install mosquitto-clients"
    echo ""
    exit 1
fi

# ============================================
# 2. SUBSCRIBE TEST (background)
# ============================================
echo "🔊 Starting subscriber (untuk melihat data yang masuk)..."
echo "   Tekan Ctrl+C untuk stop"
echo ""

mosquitto_sub -h $MQTT_HOST -p $MQTT_PORT -t "$MQTT_TOPIC" &
SUB_PID=$!
sleep 2

# ============================================
# 3. PUBLISH TEST DATA
# ============================================
echo ""
echo "📤 Sending test data..."
echo ""

# Test 1: Hadir
echo "Test 1: Absensi Hadir (John Doe)"
mosquitto_pub -h $MQTT_HOST -p $MQTT_PORT -t "$MQTT_TOPIC" \
  -m '{"pegawai_id":1,"waktu_absen":"08:00:00","keterangan":"Hadir"}'
sleep 2

# Test 2: Terlambat
echo "Test 2: Absensi Terlambat (Jane Smith)"
mosquitto_pub -h $MQTT_HOST -p $MQTT_PORT -t "$MQTT_TOPIC" \
  -m '{"pegawai_id":2,"waktu_absen":"08:15:30","keterangan":"Terlambat"}'
sleep 2

# Test 3: Izin
echo "Test 3: Absensi Izin (Ahmad)"
mosquitto_pub -h $MQTT_HOST -p $MQTT_PORT -t "$MQTT_TOPIC" \
  -m '{"pegawai_id":3,"waktu_absen":"08:30:00","keterangan":"Izin"}'
sleep 2

echo ""
echo "✅ Test selesai!"
echo ""
echo "Cek:"
echo "1. Terminal server - harus ada log MQTT message"
echo "2. Website (halaman Riwayat) - data harus muncul real-time"
echo ""

# Stop subscriber
kill $SUB_PID 2>/dev/null

echo "Tekan Enter untuk keluar..."
read
