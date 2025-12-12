# 🖥️ Fitur Absensi Manual via Web

## Tanggal: 6 November 2025

---

## 📋 Ringkasan Fitur

Fitur **Absensi Manual** memungkinkan admin untuk mencatat absensi karyawan (masuk dan keluar) langsung dari web dashboard tanpa perlu menggunakan perangkat fingerprint IoT.

### Fitur Utama:
1. ✅ **Form Absen Masuk** - Catat waktu masuk karyawan
2. ✅ **Form Absen Keluar** - Catat waktu keluar dengan perhitungan otomatis lembur
3. ✅ **Dropdown Karyawan** - Pilih karyawan dari daftar yang sudah terdaftar
4. ✅ **Waktu Real-time** - Input waktu otomatis terisi dengan waktu saat ini
5. ✅ **Notifikasi** - Feedback visual untuk setiap aksi
6. ✅ **Responsive Design** - Tampilan menyesuaikan di desktop dan mobile

---

## 🎨 Tampilan

### Desktop View
```
┌─────────────────────────────────────────────────────────┐
│         ✋ Absensi Manual                                │
├──────────────────────────┬──────────────────────────────┤
│   🟢 Absen Masuk         │   🔴 Absen Keluar            │
├──────────────────────────┼──────────────────────────────┤
│ Pilih Karyawan:          │ Pilih Karyawan:              │
│ [Ahmad Zainudin]▼        │ [Ahmad Zainudin]▼            │
│                          │                              │
│ Waktu Masuk:             │ Waktu Keluar:                │
│ [08:00]                  │ [17:30]                      │
│                          │                              │
│ Keterangan:              │ ℹ️ Info: Durasi kerja dan    │
│ [Hadir]▼                 │   lembur akan dihitung       │
│                          │   otomatis                   │
│                          │                              │
│ [✅ Catat Absen Masuk]   │ [🚪 Catat Absen Keluar]      │
└──────────────────────────┴──────────────────────────────┘
```

### Mobile View
Kedua form ditampilkan vertikal (stack) untuk kenyamanan di layar kecil.

---

## 🔧 File yang Dimodifikasi

### 1. **dashboard-glass.html**
- Menambahkan section "Absensi Manual" di antara chart dan tabel karyawan
- Form absen masuk dan keluar dengan glassmorphism design
- JavaScript handlers: `handleAbsenMasuk()` dan `handleAbsenKeluar()`
- Function helper: `loadEmployeesDropdown()`, `setCurrentTime()`, `showNotification()`

### 2. **server.js**
- Endpoint baru: `POST /absensi/manual`
- Mendukung tipe_absen: "Masuk" dan "Keluar"
- Integrasi dengan `attendance-logic.js`
- Broadcast real-time update via Socket.IO

---

## 🚀 Cara Menggunakan

### Absen Masuk:

1. **Buka Dashboard**: `http://localhost:3000/dashboard-glass.html`
2. **Scroll ke section** "✋ Absensi Manual"
3. **Pilih karyawan** dari dropdown (sebelah kiri/atas - form hijau)
4. **Atur waktu masuk** (default: waktu sekarang)
5. **Pilih keterangan**: Hadir atau Terlambat
6. **Klik "✅ Catat Absen Masuk"**

**Hasil:**
```
✅ Absen Masuk berhasil dicatat!
```

---

### Absen Keluar:

1. **Pilih karyawan** yang sudah absen masuk (sebelah kanan/bawah - form merah)
2. **Atur waktu keluar** (default: waktu sekarang)
3. **Klik "🚪 Catat Absen Keluar"**

**Hasil (jika ada lembur):**
```
✅ Absen Keluar berhasil dicatat!
⏱️ Lembur: 02:30
```

**Hasil (jika tidak lembur):**
```
✅ Absen Keluar berhasil dicatat!
```

---

## 📐 Validasi & Error Handling

### Validasi Form:
- ❌ Karyawan harus dipilih
- ❌ Waktu masuk/keluar wajib diisi
- ❌ Tipe absen harus "Masuk" atau "Keluar"

### Error Handling:

**Error 1: Absen keluar tanpa absen masuk**
```json
{
  "success": false,
  "message": "Tidak ada record absen masuk hari ini"
}
```

**Solusi:** Pastikan karyawan sudah absen masuk di hari yang sama

**Error 2: Koneksi database gagal**
```json
{
  "success": false,
  "message": "Gagal menyimpan absen masuk: Database error"
}
```

**Solusi:** Cek koneksi PostgreSQL

---

## 🔌 API Endpoint

### **POST /absensi/manual**

**Request Body (Absen Masuk):**
```json
{
  "pegawai_id": 1,
  "waktu_absen": "08:00:00",
  "keterangan": "Hadir",
  "tipe_absen": "Masuk"
}
```

**Request Body (Absen Keluar):**
```json
{
  "pegawai_id": 1,
  "waktu_absen": "17:30:00",
  "tipe_absen": "Keluar"
}
```

**Response (Success - Absen Masuk):**
```json
{
  "success": true,
  "message": "Absen masuk berhasil dicatat",
  "data": {
    "id_absensi": 123,
    "pegawai_id": 1,
    "waktu_absen": "08:00:00",
    "keterangan": "Hadir",
    "tipe_absen": "Masuk"
  }
}
```

**Response (Success - Absen Keluar):**
```json
{
  "success": true,
  "message": "Absen keluar berhasil dicatat",
  "data": {
    "id_absensi": 123,
    "waktu_keluar": "17:30:00",
    "durasi_kerja": 570,
    "durasi_lembur": 30,
    "status_lembur": "Ya"
  },
  "overtime": "00:30"
}
```

**Response (Error):**
```json
{
  "success": false,
  "message": "Data tidak lengkap. Mohon isi pegawai_id, waktu_absen, dan tipe_absen"
}
```

---

## 🧪 Testing

### Test Absen Masuk via Web:
1. Login ke dashboard
2. Scroll ke "Absensi Manual"
3. Form kiri (hijau) - Pilih karyawan "Ahmad Zainudin"
4. Waktu: 08:00
5. Keterangan: Hadir
6. Submit → Lihat notifikasi sukses

### Test Absen Keluar via Web:
1. Form kanan (merah) - Pilih karyawan yang sama
2. Waktu: 17:30
3. Submit → Lihat notifikasi dengan info lembur

### Test via curl (Optional):
```bash
# Absen Masuk
curl -X POST http://localhost:3000/absensi/manual \
  -H "Content-Type: application/json" \
  -d '{
    "pegawai_id": 1,
    "waktu_absen": "08:00:00",
    "keterangan": "Hadir",
    "tipe_absen": "Masuk"
  }'

# Absen Keluar
curl -X POST http://localhost:3000/absensi/manual \
  -H "Content-Type: application/json" \
  -d '{
    "pegawai_id": 1,
    "waktu_absen": "17:30:00",
    "tipe_absen": "Keluar"
  }'
```

---

## 🎯 Use Case

### Skenario 1: Karyawan Lupa Tap Fingerprint
**Masalah:** Ahmad lupa tap fingerprint saat masuk pagi
**Solusi:**
1. Admin buka dashboard
2. Catat absen masuk manual untuk Ahmad jam 08:00
3. Data tersimpan seperti absen via IoT

### Skenario 2: Perangkat IoT Error
**Masalah:** Fingerprint sensor rusak
**Solusi:**
1. Gunakan absensi manual di web
2. Semua karyawan tetap bisa absen
3. Data tetap akurat dengan perhitungan lembur otomatis

### Skenario 3: Koreksi Data
**Masalah:** Karyawan pulang jam 17:30 tapi lupa tap keluar
**Solusi:**
1. Admin catat absen keluar manual
2. Sistem otomatis hitung lembur 30 menit
3. Data lengkap untuk payroll

---

## 🔔 Real-time Update

Setiap kali absensi manual dicatat:
1. **Database** langsung ter-update
2. **Socket.IO** broadcast data ke semua client
3. **Dashboard** auto-refresh tanpa reload
4. **Riwayat** menampilkan data terbaru
5. **Notifikasi** muncul di pojok kanan atas

---

## 📊 Integrasi dengan Fitur Lain

### ✅ Dashboard
- Form absensi manual terintegrasi
- Statistik real-time ter-update

### ✅ Riwayat (riwayat-glass.html)
- Data absensi manual muncul di tabel
- Kolom lembur menampilkan perhitungan

### ✅ MQTT IoT
- Data manual dan IoT tersimpan di database yang sama
- Tidak ada perbedaan format

### ✅ OwnCloud Backup
- Data manual ikut ter-backup otomatis
- Schedule arsip tetap berjalan

---

## 🔐 Keamanan

### Login Required
- Hanya user yang sudah login bisa akses
- Session management dengan sessionStorage
- Auto-redirect ke login jika belum login

### Validasi Backend
- Semua input divalidasi di server
- SQL injection prevention (parameterized queries)
- Error handling yang proper

---

## 🎨 Design Features

### Glassmorphism Style:
- ✨ Backdrop blur effect
- 🌈 Gradient borders
- 💎 Glass-like transparency
- 🎯 Smooth animations

### Color Coding:
- **Hijau (Absen Masuk)**: #10b981 - Fresh, start of day
- **Merah (Absen Keluar)**: #ef4444 - End of day
- **Orange (Lembur)**: #ffc107 - Warning/overtime indicator

### Responsive:
- **Desktop**: Side by side (2 columns)
- **Tablet**: Side by side dengan gap lebih kecil
- **Mobile**: Stacked vertically

---

## ⚙️ Konfigurasi

### Default Settings:
```javascript
// Auto-set current time on page load
setCurrentTime();

// Auto-populate employee dropdown
loadEmployeesDropdown();

// Notification duration
setTimeout(() => notification.remove(), 4000); // 4 seconds
```

### Customization:
Untuk mengubah waktu default atau keterangan default, edit di:
```javascript
// dashboard-glass.html line ~338
document.getElementById('waktu_masuk').value = currentTime;

// dashboard-glass.html line ~162
<option value="Hadir" selected>Hadir</option>
```

---

## 🐛 Troubleshooting

### Problem 1: Dropdown karyawan kosong
**Penyebab:** API `/stats/employees` gagal
**Solusi:**
```bash
# Cek server log
# Cek database connection
sudo systemctl status postgresql
```

### Problem 2: Form tidak submit
**Penyebab:** JavaScript error
**Solusi:**
1. Buka browser console (F12)
2. Cek error message
3. Pastikan semua field terisi

### Problem 3: Waktu tidak otomatis terisi
**Penyebab:** `setCurrentTime()` tidak dipanggil
**Solusi:**
- Reload halaman
- Cek browser console
- Pastikan JavaScript enabled

### Problem 4: Notifikasi tidak muncul
**Penyebab:** `showNotification()` function error
**Solusi:**
- Cek CSS animation support
- Cek z-index conflict
- Clear browser cache

---

## 📈 Metrics & Analytics

Data yang ter-capture dari absensi manual:
- Timestamp insert otomatis (NOW())
- User IP (jika diperlukan bisa ditambah)
- Tipe absen (Masuk/Keluar)
- Sumber data (manual vs IoT bisa ditambah column)

---

## 🚀 Future Improvements

### Possible Enhancements:
1. **Bulk Import** - Upload CSV untuk absen massal
2. **History Log** - Track siapa yang input manual
3. **Approval Flow** - Manager approval untuk koreksi
4. **Photo Verification** - Ambil foto saat absen manual
5. **Geolocation** - Record lokasi saat absen manual
6. **QR Code** - Scan QR untuk absen lebih cepat
7. **Mobile App** - Native app dengan camera

---

## ✅ Checklist Implementasi

- [x] Form UI di dashboard
- [x] Dropdown karyawan ter-populate
- [x] Input waktu auto-fill dengan current time
- [x] Handler function untuk submit
- [x] Backend endpoint `/absensi/manual`
- [x] Validasi input
- [x] Error handling
- [x] Socket.IO broadcast
- [x] Notifikasi real-time
- [x] Responsive design
- [x] Dokumentasi

---

## 📞 Support

### Jika ada masalah:

1. **Frontend Issue**
   - Cek browser console (F12)
   - Cek network tab untuk API response
   - Test dengan browser lain

2. **Backend Issue**
   - Cek server log di terminal
   - Cek database connection
   - Cek attendance-logic.js loaded

3. **Data Issue**
   - Query database manual
   - Cek tipe_absen value
   - Cek timestamp format

---

**Status:** ✅ Fully Implemented & Tested
**Integration:** Dashboard, Server, Database
**Last Updated:** 6 November 2025

---

## 🎉 Selamat Menggunakan!

Dengan fitur absensi manual ini, sistem menjadi lebih **fleksibel** dan **reliable**. Karyawan tetap bisa absen meskipun perangkat IoT bermasalah, dan admin punya kontrol penuh untuk koreksi data.

**Enjoy! 🚀**
