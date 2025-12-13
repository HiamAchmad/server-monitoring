#!/bin/bash

# ================================================
# Fix Permission Script for NGINX
# Run this on server: bash fix-permission.sh
# ================================================

echo "🔐 Fixing permissions for NGINX..."

# Fix project directory permission
chmod -R 755 /var/www/server-monitoring
echo "✅ /var/www/server-monitoring permission set to 755 recursively"

# Verify permissions
echo ""
echo "📋 Verifying permissions:"
ls -la /var/www/server-monitoring/ | head -10

# Test NGINX config
echo ""
echo "🧪 Testing NGINX configuration..."
nginx -t

# Reload NGINX
echo ""
echo "🔄 Reloading NGINX..."
systemctl reload nginx

echo ""
echo "✅ Permission fix completed!"
echo ""
echo "🌐 Test URLs:"
echo "   - https://mitrjaya.my.id"
echo "   - https://mitrjaya.my.id/admin/"
echo "   - https://mitrjaya.my.id/user/"
echo "   - https://mitrjaya.my.id/public/css/glass-style.css"
