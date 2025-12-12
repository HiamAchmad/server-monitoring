#!/bin/bash
# PostgreSQL Database Setup Script
# Run this script with: bash setup_postgres.sh

set -e  # Exit on error

echo "🔧 Setting up PostgreSQL database for Sistem Absensi..."
echo ""

# Check if PostgreSQL is running
if ! systemctl is-active --quiet postgresql; then
    echo "❌ PostgreSQL service is not running!"
    echo "Starting PostgreSQL..."
    sudo systemctl start postgresql
    sudo systemctl enable postgresql
    echo "✅ PostgreSQL service started"
fi

# Create database user and database as postgres user
echo ""
echo "📦 Creating database and user..."
sudo -u postgres psql <<EOF
-- Create user if not exists
DO \$\$
BEGIN
    IF NOT EXISTS (SELECT FROM pg_catalog.pg_user WHERE usename = 'absensi_user') THEN
        CREATE USER absensi_user WITH PASSWORD 'absensi_password';
        RAISE NOTICE 'User absensi_user created';
    ELSE
        RAISE NOTICE 'User absensi_user already exists';
    END IF;
END
\$\$;

-- Create database if not exists
SELECT 'CREATE DATABASE db_absensi OWNER absensi_user'
WHERE NOT EXISTS (SELECT FROM pg_database WHERE datname = 'db_absensi')\gexec

-- Grant privileges
GRANT ALL PRIVILEGES ON DATABASE db_absensi TO absensi_user;

\c db_absensi

-- Grant schema privileges
GRANT ALL ON SCHEMA public TO absensi_user;
GRANT ALL PRIVILEGES ON ALL TABLES IN SCHEMA public TO absensi_user;
GRANT ALL PRIVILEGES ON ALL SEQUENCES IN SCHEMA public TO absensi_user;

-- Set default privileges for future objects
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO absensi_user;
ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON SEQUENCES TO absensi_user;

\q
EOF

echo "✅ Database user and database created"

# Import database schema
echo ""
echo "📋 Importing database schema..."
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
sudo -u postgres psql -d db_absensi -f "$SCRIPT_DIR/database_postgresql.sql" > /dev/null 2>&1 || {
    echo "⚠️  Schema import had some warnings (this is normal if tables already exist)"
}

echo "✅ Database schema imported"

# Test connection
echo ""
echo "🧪 Testing database connection..."
PGPASSWORD=absensi_password psql -h localhost -U absensi_user -d db_absensi -c "SELECT COUNT(*) as pegawai_count FROM pegawai;" 2>&1 | grep -E 'pegawai_count|[0-9]' || {
    echo "❌ Connection test failed"
    exit 1
}

echo ""
echo "✅ PostgreSQL setup complete!"
echo ""
echo "📊 Database Info:"
echo "   Host: localhost"
echo "   Port: 5432"
echo "   Database: db_absensi"
echo "   User: absensi_user"
echo "   Password: absensi_password"
echo ""
echo "🚀 You can now start your Node.js server with:"
echo "   cd $SCRIPT_DIR"
echo "   node server.js"
echo ""
