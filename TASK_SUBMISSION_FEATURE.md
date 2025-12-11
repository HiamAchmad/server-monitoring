# 📤 Task Submission Feature - Implementasi Lengkap

**Tanggal:** 10 November 2025
**Status:** ⚠️ PERLU MIGRASI DATABASE

## 📋 Ringkasan Perubahan

Sistem absensi telah diperbarui dengan fitur **Task Submission** yang memungkinkan:
- **Admin**: Hanya bisa melihat tugas yang di-submit karyawan (view-only)
- **Karyawan**: Bisa upload hasil pekerjaan/tugas mereka

---

## ✅ Perubahan yang Sudah Dilakukan

### 1. **manajemen-glass.html** (Admin - View Only) ✅

**File:** `public/manajemen-glass.html`

**Perubahan:**
- ❌ Removed: Form upload tugas (admin tidak bisa upload lagi)
- ✅ Added: Filter section (by karyawan, search, status)
- ✅ Changed: Display submitted tasks from users
- ✅ Updated: Page title "Daftar Tugas Karyawan"

**Fitur Admin:**
- Melihat daftar tugas yang di-submit karyawan
- Filter berdasarkan karyawan, search, dan status
- Download file hasil pekerjaan karyawan
- Hapus tugas yang sudah di-submit

---

### 2. **user-tugas.html** (User - Upload Capable) ✅

**File:** `public/user-tugas.html`

**Perubahan:**
- ✅ Added: Upload form section
- ✅ Added: Task selection dropdown
- ✅ Added: Description/notes textarea
- ✅ Added: Drag & drop file upload
- ✅ Added: File preview
- ✅ Added: Upload progress bar
- ✅ Kept: Display assigned tasks (view existing)

**Fitur User:**
- Melihat tugas yang diberikan admin
- Memilih tugas yang ingin di-submit
- Upload file hasil pekerjaan (PDF, DOC, XLS, PPT, ZIP, RAR, JPG, PNG)
- Menambah catatan/deskripsi saat submit
- Tracking progress upload
- Filter tugas (Semua, Aktif, Selesai)

---

### 3. **Backend Endpoint** ✅

**File:** `server.js`

**Endpoint Baru:**
```javascript
POST /tugas/upload
```

**Functionality:**
- Accept: `tugas_id`, `pegawai_id`, `catatan`, `file`
- Validate: File size (max 10MB), task ownership
- Process: Upload file to `/uploads`
- Update: Task record with submission info
- Cloud: Auto-backup to OwnCloud `/Tugas-Submit/`
- Response: Success/error with task data

**Endpoint di server.js:646**

---

## ⚠️ **PERLU DILAKUKAN: Database Migration**

### **PENTING:** Tabel `tugas` perlu ditambahkan 4 field baru!

**File Migration:** `migration_tugas_submission.sql`

**Field yang Perlu Ditambahkan:**

| Field | Type | Description |
|-------|------|-------------|
| `status` | VARCHAR(20) | Status tugas: Pending, Selesai, Terlambat |
| `file_submit_path` | VARCHAR(255) | Path file hasil pekerjaan dari user |
| `tanggal_submit` | TIMESTAMP | Waktu user submit hasil tugas |
| `catatan_submit` | TEXT | Catatan dari user saat submit |

---

## 🔧 Cara Menjalankan Migration

### **Opsi 1: Manual via psql (RECOMMENDED)**

```bash
# 1. Login sebagai postgres superuser
sudo -u postgres psql

# 2. Connect ke database
\c db_absensi

# 3. Tambah kolom satu per satu
ALTER TABLE tugas ADD COLUMN IF NOT EXISTS status VARCHAR(20) DEFAULT 'Pending';
ALTER TABLE tugas ADD COLUMN IF NOT EXISTS file_submit_path VARCHAR(255);
ALTER TABLE tugas ADD COLUMN IF NOT EXISTS tanggal_submit TIMESTAMP;
ALTER TABLE tugas ADD COLUMN IF NOT EXISTS catatan_submit TEXT;

# 4. Update existing records
UPDATE tugas SET status = 'Pending' WHERE status IS NULL;

# 5. Create index
CREATE INDEX IF NOT EXISTS idx_tugas_status ON tugas(status);

# 6. Verifikasi
\d tugas

# 7. Exit
\q
```

### **Opsi 2: Via SQL File**

```bash
# Jalankan migration file
sudo -u postgres psql -d db_absensi -f migration_tugas_submission.sql
```

---

## 🧪 Cara Testing

### **1. Test Upload dari User**

```bash
# Login sebagai user di user-login.html
# Masuk ke halaman user-tugas.html
# Pilih tugas dari dropdown
# Upload file
# Submit
```

**Expected Result:**
- ✅ File terupload ke `/uploads/`
- ✅ Database updated dengan submission info
- ✅ Status berubah menjadi "Selesai"
- ✅ File ter-backup ke OwnCloud `/Tugas-Submit/`

### **2. Test View dari Admin**

```bash
# Login sebagai admin di index-glass.html
# Masuk ke halaman manajemen-glass.html
# Lihat daftar tugas yang di-submit
```

**Expected Result:**
- ✅ Menampilkan tugas dengan status "Selesai"
- ✅ Menampilkan info submit date
- ✅ Bisa download file hasil pekerjaan
- ✅ Filter berfungsi

---

## 📁 File Structure

```
server-monitoring/
├── public/
│   ├── manajemen-glass.html     ✅ UPDATED (Admin view-only)
│   ├── user-tugas.html           ✅ UPDATED (User upload)
│   └── ...
├── uploads/                      📁 User submitted files
├── server.js                     ✅ UPDATED (New endpoint)
├── migration_tugas_submission.sql ⚠️ PERLU DIJALANKAN
└── TASK_SUBMISSION_FEATURE.md   📄 Dokumentasi ini
```

---

## 🔄 Workflow

### **Before (Old):**
```
Admin → Upload Task → Assign to Employee
Employee → View Task → Download File
                     ↓
                 (NO SUBMISSION)
```

### **After (New):**
```
Admin → Upload Task → Assign to Employee
                         ↓
Employee → View Task → Download File
                     ↓
              Upload Results (NEW!)
                     ↓
Admin → View Submissions → Download Results
                         ↓
                    Evaluate Work
```

---

## 🔐 Security

**Validations Implemented:**
- ✅ File size limit: 10MB
- ✅ File type check: PDF, DOC, XLS, PPT, TXT, ZIP, RAR, JPG, PNG
- ✅ Ownership check: User can only submit their own tasks
- ✅ Task existence check
- ✅ File cleanup on error

**OwnCloud Integration:**
- Admin uploads → `/Tugas-Karyawan/`
- User submissions → `/Tugas-Submit/`

---

## 📊 Database Schema (Updated)

```sql
CREATE TABLE tugas (
    id_tugas SERIAL PRIMARY KEY,
    pegawai_id INT NOT NULL,
    nama_tugas VARCHAR(200) NOT NULL,
    deskripsi TEXT,
    file_name VARCHAR(255) NOT NULL,        -- Admin's original file
    file_path VARCHAR(255) NOT NULL,        -- Admin's original path
    deadline DATE NOT NULL,
    upload_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,

    -- NEW FIELDS (PERLU DITAMBAHKAN)
    status VARCHAR(20) DEFAULT 'Pending',   -- NEW
    file_submit_path VARCHAR(255),          -- NEW (User's submitted file)
    tanggal_submit TIMESTAMP,               -- NEW
    catatan_submit TEXT,                    -- NEW

    FOREIGN KEY (pegawai_id) REFERENCES pegawai(id_pegawai) ON DELETE CASCADE
);
```

---

## ❗ TODO Next Steps

1. ✅ ~~Revisi manajemen-glass.html (admin view-only)~~
2. ✅ ~~Update user-tugas.html (user upload)~~
3. ✅ ~~Buat endpoint POST /tugas/upload~~
4. ⚠️ **JALANKAN MIGRATION DATABASE** (manual required)
5. ⏳ Buat halaman user-jadwal.html
6. ⏳ Buat halaman user-profil.html
7. ⏳ Fix navigasi dan link di user dashboard

---

## 🐛 Known Issues

**Issue:** Migration requires postgres superuser
**Solution:** Run migration manually as shown above

**Issue:** Server perlu restart setelah migration
**Solution:**
```bash
# Restart server
pm2 restart server
# or
kill -9 <pid> && node server.js
```

---

## 📞 Contact

Jika ada pertanyaan atau issue, hubungi developer.

---

**Generated by:** Claude Code
**Last Updated:** 10 November 2025
