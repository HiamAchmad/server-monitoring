# NGINX Configuration untuk mitrjaya.my.id

## Langkah Instalasi di Server (47.84.67.102)

### 1. Install NGINX

```bash
sudo apt update
sudo apt install nginx -y
```

### 2. Install Certbot untuk SSL

```bash
sudo apt install certbot python3-certbot-nginx -y
```

### 3. Copy Konfigurasi NGINX

```bash
# Copy file dari project ke nginx
sudo cp /home/hasan/Documents/Perancangan/Perancangan/server-monitoring/nginx/absensi.conf /etc/nginx/sites-available/mitrjaya.my.id

# Enable site
sudo ln -s /etc/nginx/sites-available/mitrjaya.my.id /etc/nginx/sites-enabled/

# Hapus default (opsional)
sudo rm /etc/nginx/sites-enabled/default
```

### 4. Dapatkan SSL Certificate

```bash
# Pastikan Cloudflare proxy OFF dulu (DNS only) untuk verifikasi
sudo certbot --nginx -d mitrjaya.my.id -d www.mitrjaya.my.id
```

### 5. Test dan Reload NGINX

```bash
# Test konfigurasi
sudo nginx -t

# Reload
sudo systemctl reload nginx
```

### 6. Jalankan Node.js Server

```bash
cd /home/hasan/Documents/Perancangan/Perancangan/server-monitoring

# Install dependencies
npm install

# Jalankan server (development)
node server.js

# Atau dengan PM2 (production)
npm install -g pm2
pm2 start server.js --name "absensi"
pm2 save
pm2 startup
```

## URL Akses

| URL | Halaman |
|-----|---------|
| https://mitrjaya.my.id | Landing Page |
| https://mitrjaya.my.id/admin/ | Portal Admin |
| https://mitrjaya.my.id/user/ | Portal Karyawan |
| https://api.mitrjaya.my.id | API (opsional) |

## Cloudflare Settings

Pastikan di Cloudflare:
- **SSL/TLS**: Full (strict)
- **Always Use HTTPS**: ON
- **Proxy status**: Proxied (orange cloud)

## Troubleshooting

### SSL Certificate Error

```bash
# Matikan proxy Cloudflare dulu (DNS only)
# Lalu jalankan:
sudo certbot --nginx -d mitrjaya.my.id -d www.mitrjaya.my.id

# Setelah berhasil, nyalakan proxy Cloudflare lagi
```

### 502 Bad Gateway

```bash
# Cek Node.js berjalan
pm2 status

# Atau cek port 3000
netstat -tlnp | grep 3000

# Restart server jika perlu
pm2 restart absensi
```

### Permission Denied

```bash
# Beri akses ke folder project
sudo chmod -R 755 /home/hasan/Documents/Perancangan/Perancangan/server-monitoring

# Tambahkan www-data ke group user
sudo usermod -aG hasan www-data
```

### WebSocket/Socket.IO Error

Pastikan di Cloudflare:
- **Network > WebSockets**: ON

## Firewall

```bash
# Buka port yang diperlukan
sudo ufw allow 80
sudo ufw allow 443
sudo ufw allow 3000  # Untuk akses langsung (opsional)
sudo ufw enable
```
