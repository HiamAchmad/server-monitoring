#!/bin/bash

# ================================================
# Deployment Script for Sistem Absensi
# Domain: mitrjaya.my.id
# ================================================

set -e

PROJECT_DIR="/root/server-monitoring"
NGINX_CONF="nginx/absensi.conf"
NGINX_DEST="/etc/nginx/sites-available/mitrjaya.my.id"

echo "🚀 Starting deployment..."

# Navigate to project directory
cd "$PROJECT_DIR"

# Pull latest changes
echo "📥 Pulling latest changes from Git..."
git pull origin main || git pull origin master

# Fix permissions for NGINX (www-data user)
echo "🔐 Setting permissions..."
chmod 755 /root
chmod -R 755 /root/server-monitoring

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

# Update NGINX configuration
echo "🔧 Updating NGINX configuration..."
sudo cp "$NGINX_CONF" "$NGINX_DEST"

# Test NGINX configuration
echo "🧪 Testing NGINX configuration..."
sudo nginx -t

# Reload NGINX
echo "🔄 Reloading NGINX..."
sudo systemctl reload nginx

echo "✅ Deployment completed successfully!"
echo "🌐 Site: https://mitrjaya.my.id"
