# 🚀 CARA START WEBSITE - SUPER MUDAH!

## Status Saat Ini:
- ✅ PostgreSQL: **Installed & Running**
- ✅ Server Code: **Ready (PostgreSQL integrated)**
- ✅ Dependencies: **Installed**
- ✅ OwnCloud: **Running (port 8080)**
- ⏳ Database: **Belum di-setup** (perlu 1 command)

---

## 🎯 JALANKAN WEBSITE (1 COMMAND):

**Buka terminal, copy-paste command ini:**

```bash
cd /home/hasan/Documents/Perancangan-20251031T151707Z-1-001/Perancangan/server-monitoring && sudo -u postgres psql -c "CREATE USER absensi_user WITH PASSWORD 'absensi_password';" && sudo -u postgres psql -c "CREATE DATABASE db_absensi OWNER absensi_user;" && sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE db_absensi TO absensi_user;" && sudo -u postgres psql -d db_absensi -c "GRANT ALL ON SCHEMA public TO absensi_user;" && sudo -u postgres psql -d db_absensi -c "ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO absensi_user;" && sudo -u postgres psql -d db_absensi -f database_postgresql.sql && node server.js
```

**Tunggu sampai muncul:**
```
✅ Terhubung ke database PostgreSQL.
🚀 Server absensi berjalan pada port 3000
```

**Lalu buka browser:**
- http://localhost:3000/dashboard.html

**SELESAI!** 🎉

---

## 📋 Atau Cara Bertahap (Jika Prefer Step-by-Step):

### Step 1: Setup Database (Copy-Paste Ini)
```bash
cd /home/hasan/Documents/Perancangan-20251031T151707Z-1-001/Perancangan/server-monitoring

sudo -u postgres psql << 'EOF'
CREATE USER absensi_user WITH PASSWORD 'absensi_password';
CREATE DATABASE db_absensi OWNER absensi_user;
GRANT ALL PRIVILEGES ON DATABASE db_absensi TO absensi_user;
\c db_absensi
GRANT ALL ON SCHEMA public TO absensi_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO absensi_user;
\q
EOF
```

### Step 2: Import Data (Copy-Paste Ini)
```bash
sudo -u postgres psql -d db_absensi -f database_postgresql.sql
```

### Step 3: Start Server
```bash
node server.js
```

---

## 🌐 Akses Website:

Setelah server running, buka:

| Page | URL | Fungsi |
|------|-----|--------|
| **Dashboard** | http://localhost:3000/dashboard.html | Admin dashboard dengan statistik real-time |
| **Manajemen Tugas** | http://localhost:3000/manajemen.html | Upload & manage tugas karyawan |
| **Riwayat Absensi** | http://localhost:3000/riwayat.html | Lihat history absensi |
| **OwnCloud** | http://localhost:8080 | Cloud backup (admin/admin) |

---

## ✅ Verification:

Server berhasil jika Anda melihat:
```
Penjadwalan arsip telah diatur.
🚀 MQTT Broker berjalan pada port 1883
🚀 Server absensi berjalan pada port 3000
✅ Terhubung ke database PostgreSQL.
   Server time: 2025-11-04 ...
```

---

## 🔧 Troubleshooting:

### "Port 3000 already in use"
```bash
pkill -f "node.*server.js"
node server.js
```

### "Database already exists"
```bash
sudo -u postgres psql -c "DROP DATABASE db_absensi;"
sudo -u postgres psql -c "DROP USER absensi_user;"
# Lalu jalankan setup lagi
```

### "Cannot connect to PostgreSQL"
```bash
sudo systemctl start postgresql
sudo systemctl enable postgresql
```

---

## 📊 Fitur Website:

✅ **Real-time Dashboard** - Update otomatis via Socket.IO
✅ **MQTT Protocol** - Terima data dari ESP32
✅ **Employee Statistics** - Total absensi per karyawan
✅ **Task Management** - Upload & track tugas
✅ **Auto Backup** - Otomatis ke OwnCloud
✅ **Monthly Archive** - Auto-archive setiap bulan
✅ **PostgreSQL** - Auto-start, tidak perlu manual!

---

## 🧪 Test MQTT:

Setelah server running, test MQTT client:
```bash
node test_mqtt.js
```

Akan muncul di dashboard secara real-time!

---

## 📁 File Penting:

- **server.js** - Main server (PostgreSQL integrated) ✅
- **database_postgresql.sql** - Database schema ✅
- **test_mqtt.js** - MQTT test client ✅
- **public/** - Website files (dashboard, manajemen, riwayat) ✅

---

## 💡 Tips:

1. **Server harus running terus** - Jangan close terminal
2. **Buka di browser modern** - Chrome, Firefox, Edge
3. **MQTT port 1883** - Untuk ESP32 connection
4. **Auto-backup aktif** - File tugas otomatis ke OwnCloud

---

**Silakan copy-paste command di atas ke terminal, dan website langsung bisa dibuka!** 🚀

---

**Need Help?**
- Baca: [POSTGRESQL_MIGRATION_COMPLETE.md](POSTGRESQL_MIGRATION_COMPLETE.md)
- MQTT Guide: [README_MQTT.md](../README_MQTT.md)
- Quick Commands: [JALANKAN_SEKARANG.txt](JALANKAN_SEKARANG.txt)
