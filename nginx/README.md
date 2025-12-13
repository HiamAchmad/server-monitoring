# NGINX Configuration untuk mitrjaya.my.id

## Server Information

- **Server IP**: 47.84.67.102
- **Domain**: mitrjaya.my.id
- **Project Path**: /var/www/server-monitoring

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

### 3. Setup Project Directory

```bash
# Buat folder project
mkdir -p /var/www/server-monitoring

# Set permission
chmod -R 755 /var/www/server-monitoring
```

### 4. Copy Konfigurasi NGINX

```bash
# Copy file dari project ke nginx
cp /var/www/server-monitoring/nginx/absensi.conf /etc/nginx/sites-available/mitrjaya.my.id

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
cd /var/www/server-monitoring

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

## Troubleshooting

### SSL Certificate Error

```bash
# Matikan proxy Cloudflare dulu (DNS only)
# Lalu jalankan:
certbot --nginx -d mitrjaya.my.id -d www.mitrjaya.my.id

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
# Beri akses ke project folder
chmod -R 755 /var/www/server-monitoring

# Restart NGINX
systemctl restart nginx
```

### WebSocket/Socket.IO Error

Pastikan di Cloudflare:
- **Network > WebSockets**: ON

## Firewall

```bash
# Buka port yang diperlukan
ufw allow 80
ufw allow 443
ufw enable
```

## File Structure

```
/var/www/server-monitoring/
├── index.html              # Landing page
├── server.js               # Node.js server
├── package.json
├── public/
│   ├── admin/              # Admin pages
│   ├── user/               # User pages
│   ├── css/                # Stylesheets
│   ├── js/                 # JavaScript
│   └── icons/              # PWA icons
├── uploads/                # Uploaded files
└── nginx/
    └── absensi.conf        # NGINX config
```
