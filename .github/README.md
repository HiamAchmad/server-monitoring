# CI/CD Setup - Auto Deploy ke Server

## Tambahkan GitHub Secrets

Buka **GitHub Repository** > **Settings** > **Secrets and variables** > **Actions** > **New repository secret**

Tambahkan 3 secrets berikut:

| Secret Name | Value |
|-------------|-------|
| `SERVER_HOST` | `47.84.67.102` |
| `SERVER_USER` | `root` |
| `SERVER_PASSWORD` | `Admin#1234` |

## Cara Menambahkan Secret

1. Buka repository GitHub Anda
2. Klik **Settings** (tab di atas)
3. Di sidebar kiri, klik **Secrets and variables** > **Actions**
4. Klik tombol **New repository secret**
5. Masukkan:
   - **Name**: `SERVER_HOST`
   - **Secret**: `47.84.67.102`
6. Klik **Add secret**
7. Ulangi untuk `SERVER_USER` dan `SERVER_PASSWORD`

## Workflow

Setiap kali push ke branch `main` atau `master`:

```
Git Push → GitHub Actions → SSH ke Server → Update Code → Restart PM2 → Reload NGINX
```

## Persyaratan di Server

Pastikan di server (47.84.67.102) sudah terinstall:

```bash
# Install Node.js
curl -fsSL https://deb.nodesource.com/setup_20.x | bash -
apt install -y nodejs

# Install PM2
npm install -g pm2

# Install NGINX
apt install -y nginx

# Install Certbot (untuk SSL)
apt install -y certbot python3-certbot-nginx
```

## Setup SSL di Server (Pertama Kali)

```bash
# Matikan Cloudflare proxy dulu (grey cloud)
certbot --nginx -d mitrjaya.my.id -d www.mitrjaya.my.id

# Setelah sukses, nyalakan proxy lagi (orange cloud)
```

## Test Deployment

```bash
git add .
git commit -m "test deploy"
git push origin main
```

Cek progress: **GitHub** > **Actions**

## URL Setelah Deploy

- https://mitrjaya.my.id - Landing Page
- https://mitrjaya.my.id/admin/ - Portal Admin
- https://mitrjaya.my.id/user/ - Portal Karyawan
