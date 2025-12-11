# PostgreSQL Setup Guide - Sistem Absensi

## Status: PostgreSQL Sudah Terinstall dan Auto-Start! ✅

PostgreSQL sudah berjalan di sistem Anda dan sudah dikonfigurasi untuk **auto-start saat boot**. Anda tidak perlu lagi menjalankan manual seperti MySQL!

## Langkah Setup Database (Hanya Perlu 1x)

Jalankan perintah berikut di terminal:

```bash
cd /home/hasan/Documents/Perancangan-20251031T151707Z-1-001/Perancangan/server-monitoring

# 1. Login ke PostgreSQL sebagai superuser
sudo -u postgres psql

# 2. Jalankan perintah SQL berikut (copy-paste semua):
CREATE USER absensi_user WITH PASSWORD 'absensi_password';
CREATE DATABASE db_absensi OWNER absensi_user;
GRANT ALL PRIVILEGES ON DATABASE db_absensi TO absensi_user;
\c db_absensi
GRANT ALL ON SCHEMA public TO absensi_user;
\q

# 3. Import schema database
sudo -u postgres psql -d db_absensi -f database_postgresql.sql

# 4. Verifikasi koneksi (pastikan berhasil)
PGPASSWORD=absensi_password psql -h localhost -U absensi_user -d db_absensi -c "SELECT COUNT(*) FROM pegawai;"
```

Jika langkah 4 menampilkan angka (jumlah pegawai), maka setup berhasil! ✅

## Start Server

Setelah database ter-setup, jalankan server:

```bash
cd /home/hasan/Documents/Perancangan-20251031T151707Z-1-001/Perancangan/server-monitoring
node server.js
```

## Konfigurasi Database (Sudah Diatur di server.js)

```javascript
Host: localhost
Port: 5432
User: absensi_user
Password: absensi_password
Database: db_absensi
```

## Keuntungan PostgreSQL vs MySQL

✅ **Auto-start saat boot** - Tidak perlu `sudo systemctl start` lagi!
✅ **Connection pooling** - Lebih efisien
✅ **MVCC** - Multi-Version Concurrency Control untuk performa lebih baik
✅ **JSON support** - Native JSON data type
✅ **Better concurrent connections** - Handle banyak client simultan

## Cek Status PostgreSQL

```bash
# Cek apakah berjalan
systemctl status postgresql

# Pastikan enabled (auto-start)
systemctl is-enabled postgresql  # Harus output: enabled
```

## Troubleshooting

### Problem: "role does not exist"
**Solution:**
```bash
sudo -u postgres psql -c "CREATE USER absensi_user WITH PASSWORD 'absensi_password';"
```

### Problem: "database does not exist"
**Solution:**
```bash
sudo -u postgres psql -c "CREATE DATABASE db_absensi OWNER absensi_user;"
```

### Problem: "permission denied"
**Solution:**
```bash
sudo -u postgres psql -d db_absensi -c "GRANT ALL ON SCHEMA public TO absensi_user;"
```

### Problem: Cannot connect from Node.js
**Solution:** Check [pg_hba.conf](file:///etc/postgresql/15/main/pg_hba.conf):
```bash
# Should have this line:
host    all             all             127.0.0.1/32            md5
```

Restart PostgreSQL jika mengubah config:
```bash
sudo systemctl restart postgresql
```

## Backup Database (Opsional)

```bash
# Backup
pg_dump -h localhost -U absensi_user db_absensi > backup_$(date +%Y%m%d).sql

# Restore
psql -h localhost -U absensi_user -d db_absensi < backup_20251104.sql
```

## Akses Database via Command Line

```bash
PGPASSWORD=absensi_password psql -h localhost -U absensi_user -d db_absensi
```

Atau tambahkan ke [~/.pgpass](file:///home/hasan/.pgpass) untuk login tanpa password:
```bash
echo "localhost:5432:db_absensi:absensi_user:absensi_password" >> ~/.pgpass
chmod 600 ~/.pgpass
```

## Database GUI Tools (Opsional)

Jika ingin GUI untuk manage database:

1. **pgAdmin 4** - Official PostgreSQL GUI
   ```bash
   # Install
   sudo apt install pgadmin4
   ```

2. **DBeaver** - Universal database tool
   ```bash
   sudo snap install dbeaver-ce
   ```

3. **VS Code Extension** - PostgreSQL extension by Chris Kolkman

## Summary

- ✅ PostgreSQL sudah terinstall
- ✅ Auto-start enabled (tidak perlu manual start)
- ✅ server.js sudah diupdate untuk PostgreSQL
- ✅ database_postgresql.sql sudah siap diimport
- ⏳ **Tinggal jalankan SQL commands di atas untuk setup database**

Setelah setup, server akan otomatis connect ke PostgreSQL setiap kali dijalankan!
