#!/bin/bash

# ============================================
# Script Update Database untuk Absen Keluar & Lembur
# ============================================

echo "╔════════════════════════════════════════════╗"
echo "║   UPDATE DATABASE - ABSEN KELUAR & LEMBUR ║"
echo "╚════════════════════════════════════════════╝"
echo ""

echo "🔧 Menambahkan kolom baru ke tabel absensi..."
echo ""

sudo -u postgres psql -d db_absensi <<EOF
-- Tambah kolom waktu_keluar
ALTER TABLE absensi ADD COLUMN IF NOT EXISTS waktu_keluar TIME;

-- Tambah kolom durasi_kerja (dalam menit)
ALTER TABLE absensi ADD COLUMN IF NOT EXISTS durasi_kerja INTEGER DEFAULT 0;

-- Tambah kolom durasi_lembur (dalam menit)
ALTER TABLE absensi ADD COLUMN IF NOT EXISTS durasi_lembur INTEGER DEFAULT 0;

-- Tambah kolom status_lembur
ALTER TABLE absensi ADD COLUMN IF NOT EXISTS status_lembur VARCHAR(20) DEFAULT 'Tidak';

-- Tambah kolom tipe_absen (Masuk/Keluar)
ALTER TABLE absensi ADD COLUMN IF NOT EXISTS tipe_absen VARCHAR(10) DEFAULT 'Masuk';

-- Tampilkan struktur tabel
\echo ''
\echo '✅ Kolom-kolom baru berhasil ditambahkan!'
\echo ''
\echo '📊 Struktur tabel absensi sekarang:'
\d absensi

-- Tampilkan contoh data
\echo ''
\echo '📋 Sample data (5 terakhir):'
SELECT id_absensi, pegawai_id, waktu_absen, waktu_keluar, tipe_absen, durasi_kerja, durasi_lembur, status_lembur
FROM absensi
ORDER BY id_absensi DESC
LIMIT 5;

\echo ''
\echo '✅ Database berhasil diupdate!'
\echo ''
EOF

echo ""
echo "════════════════════════════════════════════"
echo "✅ Selesai!"
echo ""
echo "Kolom baru yang ditambahkan:"
echo "  • waktu_keluar (TIME)"
echo "  • durasi_kerja (INTEGER - menit)"
echo "  • durasi_lembur (INTEGER - menit)"
echo "  • status_lembur (VARCHAR)"
echo "  • tipe_absen (VARCHAR - Masuk/Keluar)"
echo ""
echo "🚀 Selanjutnya: Restart server Node.js"
echo "════════════════════════════════════════════"
