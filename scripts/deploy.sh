#!/bin/bash

# ================================================
# Deployment Script for Sistem Absensi
# Domain: mitrjaya.my.id
# Path: /var/www/server-monitoring
# ================================================

set -e

PROJECT_DIR="/var/www/server-monitoring"
NGINX_CONF="nginx/absensi.conf"
NGINX_DEST="/etc/nginx/sites-available/mitrjaya.my.id"

echo "🚀 Starting deployment..."

# Create directory if not exists
mkdir -p "$PROJECT_DIR"

# Navigate to project directory
cd "$PROJECT_DIR"

# Pull latest changes
echo "📥 Pulling latest changes from Git..."
git pull origin main || git pull origin master

# Set permissions
echo "🔐 Setting permissions..."
chmod -R 755 /var/www/server-monitoring

# Install dependencies
echo "📦 Installing dependencies..."
npm install --production

# Restart Node.js with PM2
echo "🔄 Restarting Node.js server..."
if pm2 list | grep -q "absensi"; then
    pm2 restart absensi
else
    pm2 start server.js --name "absensi"
fi
pm2 save

# Update NGINX configuration
echo "🔧 Updating NGINX configuration..."
cp "$NGINX_CONF" "$NGINX_DEST"
ln -sf "$NGINX_DEST" /etc/nginx/sites-enabled/

# Test NGINX configuration
echo "🧪 Testing NGINX configuration..."
nginx -t

# Reload NGINX
echo "🔄 Reloading NGINX..."
systemctl reload nginx

echo "✅ Deployment completed successfully!"
echo "🌐 Site: https://mitrjaya.my.id"
