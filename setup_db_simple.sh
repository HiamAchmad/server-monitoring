#!/bin/bash
# Simple Database Setup
echo "Setting up database..."
sudo -u postgres psql -c "DROP DATABASE IF EXISTS db_absensi;" 2>/dev/null
sudo -u postgres psql -c "DROP USER IF EXISTS absensi_user;" 2>/dev/null
sudo -u postgres psql -c "CREATE USER absensi_user WITH PASSWORD 'absensi_password';"
sudo -u postgres psql -c "CREATE DATABASE db_absensi OWNER absensi_user;"
sudo -u postgres psql -c "GRANT ALL PRIVILEGES ON DATABASE db_absensi TO absensi_user;"
sudo -u postgres psql -d db_absensi -c "GRANT ALL ON SCHEMA public TO absensi_user;"
sudo -u postgres psql -d db_absensi -c "ALTER DEFAULT PRIVILEGES IN SCHEMA public GRANT ALL ON TABLES TO absensi_user;"
sudo -u postgres psql -d db_absensi -f database_postgresql.sql > /dev/null
echo "✅ Database setup complete!"
echo ""
echo "Now start server with: node server.js"
