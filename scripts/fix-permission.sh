#!/bin/bash

# ================================================
# Fix Permission Script for NGINX
# Run this on server: bash fix-permission.sh
# ================================================

echo "🔐 Fixing permissions for NGINX..."

# Fix /root directory permission
chmod 755 /root
echo "✅ /root permission set to 755"

# Fix project directory permission
chmod -R 755 /root/server-monitoring
echo "✅ /root/server-monitoring permission set to 755 recursively"

# Verify permissions
echo ""
echo "📋 Verifying permissions:"
ls -la /root/ | head -5
echo ""
ls -la /root/server-monitoring/ | head -10

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
echo "   - https://mitrjaya.my.id/css/glass-style.css"
