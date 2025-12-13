# NGINX Configuration untuk mitrjaya.my.id

## Server Information

- **Server IP**: 47.84.67.102
- **Domain**: mitrjaya.my.id
- **Project Path**: /root/server-monitoring

## Langkah Instalasi di Server

### 1. Install NGINX

```bash
apt update
apt install nginx -y
```

### 2. Install Certbot untuk SSL

```bash
apt install certbot python3-certbot-nginx -y
```

### 3. Set Permission untuk Folder /root/

**PENTING**: NGINX berjalan sebagai user `www-data`, perlu akses ke `/root/`

```bash
# Beri akses execute ke folder /root/
chmod 755 /root

# Beri akses ke project folder
chmod -R 755 /root/server-monitoring

# Verifikasi permission
ls -la /root/
ls -la /root/server-monitoring/
```

### 4. Copy Konfigurasi NGINX

```bash
# Copy file dari project ke nginx
cp /root/server-monitoring/nginx/absensi.conf /etc/nginx/sites-available/mitrjaya.my.id

# Enable site
ln -sf /etc/nginx/sites-available/mitrjaya.my.id /etc/nginx/sites-enabled/

# Hapus default (opsional)
rm -f /etc/nginx/sites-enabled/default
```

### 5. Dapatkan SSL Certificate

```bash
# Pastikan Cloudflare proxy OFF dulu (DNS only) untuk verifikasi
certbot --nginx -d mitrjaya.my.id -d www.mitrjaya.my.id
```

### 6. Test dan Reload NGINX

```bash
# Test konfigurasi
nginx -t

# Reload
systemctl reload nginx
```

### 7. Jalankan Node.js Server

```bash
cd /root/server-monitoring

# Install dependencies
npm install

# Jalankan dengan PM2 (production)
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

### Permission Denied / 403 Forbidden

```bash
# Beri akses ke folder /root/
chmod 755 /root

# Beri akses ke project folder
chmod -R 755 /root/server-monitoring

# Restart NGINX
systemctl restart nginx
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
