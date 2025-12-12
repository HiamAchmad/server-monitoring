# NGINX Configuration untuk Sistem Absensi

## Prasyarat

1. NGINX terinstall
2. Node.js server berjalan di port 3000

## Instalasi

### 1. Install NGINX (jika belum)

```bash
# Ubuntu/Debian
sudo apt update
sudo apt install nginx

# CentOS/RHEL
sudo yum install nginx
```

### 2. Copy Konfigurasi

```bash
# Copy file konfigurasi
sudo cp absensi.conf /etc/nginx/sites-available/

# Buat symbolic link
sudo ln -s /etc/nginx/sites-available/absensi.conf /etc/nginx/sites-enabled/

# Hapus default config (opsional)
sudo rm /etc/nginx/sites-enabled/default
```

### 3. Test dan Reload

```bash
# Test konfigurasi
sudo nginx -t

# Reload NGINX
sudo systemctl reload nginx
```

## Konfigurasi untuk Production

### Menggunakan Domain

Edit `absensi.conf`:

```nginx
server_name absensi.example.com;
```

### Menggunakan SSL (HTTPS)

1. Install Certbot:
```bash
sudo apt install certbot python3-certbot-nginx
```

2. Dapatkan sertifikat:
```bash
sudo certbot --nginx -d absensi.example.com
```

3. Uncomment bagian SSL di `absensi.conf`

## Struktur URL

| URL | Deskripsi |
|-----|-----------|
| `/` | Landing page (pilih portal) |
| `/admin/` | Portal Admin |
| `/admin/index-glass.html` | Login Admin |
| `/admin/dashboard-glass.html` | Dashboard Admin |
| `/user/` | Portal Karyawan |
| `/user/user-login.html` | Login Karyawan |
| `/user/user-dashboard.html` | Dashboard Karyawan |
| `/api/*` | API endpoints |

## Troubleshooting

### Permission Denied

```bash
# Pastikan NGINX bisa membaca folder public
sudo chmod -R 755 /home/hasan/Documents/Perancangan/Perancangan/server-monitoring/public

# Atau tambahkan user nginx ke group
sudo usermod -aG hasan www-data
```

### 502 Bad Gateway

```bash
# Pastikan Node.js server berjalan
cd /home/hasan/Documents/Perancangan/Perancangan/server-monitoring
node server.js

# Cek apakah port 3000 aktif
netstat -tlnp | grep 3000
```

### WebSocket Tidak Berfungsi

Pastikan konfigurasi `location /socket.io/` ada dan benar.

## Menjalankan dengan PM2 (Production)

```bash
# Install PM2
npm install -g pm2

# Jalankan server
pm2 start server.js --name "absensi-server"

# Auto-start saat boot
pm2 startup
pm2 save
```
