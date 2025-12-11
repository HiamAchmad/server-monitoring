# 🚀 INTEGRASI POSTGRESQL - JALANKAN INI

## Status: Database belum di-setup

Untuk mengintegrasikan PostgreSQL dengan sistem absensi, ikuti langkah berikut:

---

## ⚡ CARA CEPAT (Copy-Paste 1 Command)

Buka terminal di folder ini dan jalankan:

```bash
bash integrate_postgresql.sh
```

**Catatan:** Script akan meminta password sudo untuk setup database.

---

## 📋 Atau Manual Step-by-Step

Jika prefer manual, jalankan satu per satu:

### 1. Setup Database & User

```bash
sudo -u postgres psql -f setup_db.sql
```

### 2. Import Schema

```bash
sudo -u postgres psql -d db_absensi -f database_postgresql.sql
```

### 3. Test Koneksi

```bash
PGPASSWORD=absensi_password psql -h localhost -U absensi_user -d db_absensi -c "SELECT COUNT(*) FROM pegawai;"
```

Jika berhasil, akan muncul angka (jumlah pegawai).

---

## 🧪 Test Integration

Setelah database ter-setup, test server:

```bash
# Start server
node server.js
```

**Expected output:**
```
Penjadwalan arsip telah diatur.
🚀 MQTT Broker berjalan pada port 1883
🚀 Server absensi berjalan pada port 3000
✅ Terhubung ke database PostgreSQL.
   Server time: 2025-11-04 ...
```

Jika muncul error "🔴 Error menghubungkan ke database", berarti setup database belum berhasil.

---

## 🔍 Troubleshooting

### Problem: "role 'absensi_user' does not exist"

**Solution:**
```bash
sudo -u postgres psql -c "CREATE USER absensi_user WITH PASSWORD 'absensi_password';"
```

### Problem: "database 'db_absensi' does not exist"

**Solution:**
```bash
sudo -u postgres psql -c "CREATE DATABASE db_absensi OWNER absensi_user;"
```

### Problem: "permission denied"

**Solution:**
```bash
sudo -u postgres psql -d db_absensi -c "GRANT ALL ON SCHEMA public TO absensi_user;"
```

---

## ✅ Verification Checklist

Setelah setup, verify dengan:

```bash
# 1. Check database exists
sudo -u postgres psql -l | grep db_absensi

# 2. Check user exists
sudo -u postgres psql -c "\du" | grep absensi_user

# 3. Check tables
PGPASSWORD=absensi_password psql -h localhost -U absensi_user -d db_absensi -c "\dt"

# 4. Check data
PGPASSWORD=absensi_password psql -h localhost -U absensi_user -d db_absensi -c "SELECT * FROM pegawai;"
```

Semua command harus berhasil tanpa error.

---

## 📚 Dokumentasi Lengkap

Setelah setup berhasil, baca dokumentasi:
- [POSTGRESQL_MIGRATION_COMPLETE.md](POSTGRESQL_MIGRATION_COMPLETE.md) - Complete guide
- [SETUP_POSTGRESQL.md](SETUP_POSTGRESQL.md) - Setup details
- [README_MQTT.md](../README_MQTT.md) - MQTT implementation

---

## 🎯 Quick Start After Setup

```bash
# 1. Start server
node server.js

# 2. Test MQTT
node test_mqtt.js

# 3. Open browser
# http://localhost:3000/dashboard.html
```

---

**Status Saat Ini:**
- ✅ PostgreSQL installed and auto-starts
- ✅ server.js converted to PostgreSQL
- ✅ database_postgresql.sql ready
- ⏳ Database needs to be setup (run the command above)

**Setelah database ter-setup, sistem siap digunakan!** 🚀
