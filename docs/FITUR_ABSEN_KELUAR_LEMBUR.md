# 🚀 Fitur Baru: Absen Keluar & Waktu Lembur

## Tanggal: 6 November 2025

---

## 📋 Ringkasan Fitur

Sistem absensi sekarang mendukung:
1. ✅ **Absen Masuk** - Clock in (sudah ada)
2. ✅ **Absen Keluar** - Clock out (BARU!)
3. ✅ **Perhitungan Durasi Kerja** - Otomatis dihitung
4. ✅ **Perhitungan Lembur** - Otomatis detect jika > 8 jam atau keluar > 17:00
5. ✅ **Status Lembur** - Ya/Tidak

---

## 🔧 File yang Sudah Dibuat/Dimodifikasi

### 1. **ALTER_TABLE_ABSEN_KELUAR_LEMBUR.sql**
SQL script untuk update database schema.

### 2. **attendance-logic.js** (BARU!)
Logika perhitungan durasi kerja dan lembur.

### 3. **server.js** (DIUPDATE)
- Support tipe_absen (Masuk/Keluar)
- Integrasi dengan attendance-logic.js
- Broadcasting data lengkap via Socket.IO

---

## 📊 Schema Database Baru

### Kolom Baru di Table `absensi`:

| Kolom | Tipe | Default | Deskripsi |
|-------|------|---------|-----------|
| `waktu_keluar` | TIME | NULL | Waktu check out |
| `durasi_kerja` | INTEGER | 0 | Durasi kerja dalam menit |
| `durasi_lembur` | INTEGER | 0 | Durasi lembur dalam menit |
| `status_lembur` | VARCHAR(20) | 'Tidak' | Status lembur: Ya/Tidak |
| `tipe_absen` | VARCHAR(10) | 'Masuk' | Tipe: Masuk/Keluar |

---

## 🚀 Cara Implementasi

### **STEP 1: Update Database** (WAJIB!)

```bash
# Masuk ke PostgreSQL
sudo -u postgres psql -d db_absensi

# Jalankan SQL commands
ALTER TABLE absensi ADD COLUMN IF NOT EXISTS waktu_keluar TIME;
ALTER TABLE absensi ADD COLUMN IF NOT EXISTS durasi_kerja INTEGER DEFAULT 0;
ALTER TABLE absensi ADD COLUMN IF NOT EXISTS durasi_lembur INTEGER DEFAULT 0;
ALTER TABLE absensi ADD COLUMN IF NOT EXISTS status_lembur VARCHAR(20) DEFAULT 'Tidak';
ALTER TABLE absensi ADD COLUMN IF NOT EXISTS tipe_absen VARCHAR(10) DEFAULT 'Masuk';

# Verifikasi
\d absensi

# Keluar
\q
```

**Atau jalankan file SQL:**
```bash
cd /home/hasan/Documents/Perancangan-20251031T151707Z-1-001/Perancangan/server-monitoring
sudo -u postgres psql -d db_absensi -f ALTER_TABLE_ABSEN_KELUAR_LEMBUR.sql
```

---

### **STEP 2: Restart Server**

```bash
# Stop server yang sedang running
# (tekan Ctrl+C di terminal server atau gunakan pkill)

# Start server lagi
cd /home/hasan/Documents/Perancangan-20251031T151707Z-1-001/Perancangan/server-monitoring
node server.js
```

Server akan otomatis load file `attendance-logic.js` yang baru.

---

### **STEP 3: Test via MQTT**

#### Test Absen Masuk:
```bash
mosquitto_pub -h localhost -p 1883 -t "absensi/data" \
  -m '{"pegawai_id":1,"waktu_absen":"08:00:00","keterangan":"Hadir","tipe_absen":"Masuk"}'
```

#### Test Absen Keluar:
```bash
mosquitto_pub -h localhost -p 1883 -t "absensi/data" \
  -m '{"pegawai_id":1,"waktu_absen":"17:30:00","tipe_absen":"Keluar"}'
```

#### Cek di Server Log:
```
✅ Absen MASUK berhasil disimpan
✅ Absen KELUAR berhasil diupdate
   Durasi Kerja: 09:30
   Durasi Lembur: 00:30
   Status Lembur: Ya
```

---

## 📐 Logika Perhitungan

### 1. **Durasi Kerja**
```
Durasi Kerja (menit) = Waktu Keluar - Waktu Masuk
```

**Contoh:**
- Masuk: 08:00
- Keluar: 17:00
- Durasi: 540 menit = 9 jam

### 2. **Durasi Lembur**

Lembur dihitung jika salah satu kondisi terpenuhi:

**Kondisi A: Keluar setelah jam 17:00**
```
Lembur = Waktu Keluar - 17:00
```

**Kondisi B: Total durasi kerja > 8 jam (480 menit)**
```
Lembur = Durasi Kerja - 480 menit
```

**Ambil yang lebih besar dari kedua kondisi.**

**Contoh 1:**
- Masuk: 08:00
- Keluar: 18:00
- Durasi Kerja: 10 jam = 600 menit
- Lembur (A): 18:00 - 17:00 = 1 jam = 60 menit
- Lembur (B): 600 - 480 = 120 menit
- **Lembur Final: 120 menit = 2 jam** ✅

**Contoh 2:**
- Masuk: 07:00
- Keluar: 17:00
- Durasi Kerja: 10 jam = 600 menit
- Lembur (A): 17:00 - 17:00 = 0 menit
- Lembur (B): 600 - 480 = 120 menit
- **Lembur Final: 120 menit = 2 jam** ✅

**Contoh 3:**
- Masuk: 08:00
- Keluar: 16:00
- Durasi Kerja: 8 jam = 480 menit
- Lembur (A): 0 (belum lewat 17:00)
- Lembur (B): 0 (tidak lebih dari 8 jam)
- **Lembur Final: 0** ❌ (Status: Tidak)

---

## 🎨 Update Frontend (Halaman Riwayat)

### Kolom Tabel Baru:

| Nama | NIP | Masuk | Keluar | Durasi | Lembur | Status Lembur | Keterangan |
|------|-----|-------|--------|--------|--------|---------------|------------|
| Ahmad | NIP001 | 08:00 | 17:30 | 09:30 | 00:30 | ✅ Ya | Hadir |

### Data Socket.IO yang Diterima:

```javascript
socket.on('data_update', (data) => {
    // data.waktu_absen - Waktu masuk
    // data.waktu_keluar - Waktu keluar (null jika belum)
    // data.durasi_kerja - Durasi dalam menit
    // data.durasi_lembur - Lembur dalam menit
    // data.status_lembur - "Ya" atau "Tidak"
    // data.tipe_absen - "Masuk" atau "Keluar"
});
```

### Helper Function untuk Format:

```javascript
function formatDurasi(menit) {
    if (!menit) return '-';
    const jam = Math.floor(menit / 60);
    const sisa_menit = menit % 60;
    return `${String(jam).padStart(2, '0')}:${String(sisa_menit).padStart(2, '0')}`;
}

// Contoh:
formatDurasi(570);  // "09:30"
formatDurasi(30);   // "00:30"
formatDurasi(0);    // "00:00"
```

---

## 🤖 Update ESP32 Code (Absensi_MQTT.ino)

### Tambah Variabel Mode:

```cpp
// Mode absensi: Masuk atau Keluar
String modeAbsensi = "Masuk"; // Default: Masuk
```

### Tambah Tombol untuk Toggle Mode:

```cpp
#define BUTTON_MODE 17  // Tombol untuk ganti mode

void setup() {
    pinMode(BUTTON_MODE, INPUT_PULLUP);
    // ...
}

void loop() {
    // Cek tombol mode
    if (digitalRead(BUTTON_MODE) == LOW) {
        delay(200); // Debounce
        if (modeAbsensi == "Masuk") {
            modeAbsensi = "Keluar";
            Serial.println("Mode: KELUAR");
            display.clearDisplay();
            display.println("Mode: KELUAR");
            display.display();
        } else {
            modeAbsensi = "Masuk";
            Serial.println("Mode: MASUK");
            display.clearDisplay();
            display.println("Mode: MASUK");
            display.display();
        }
        delay(500);
    }

    // ... scan fingerprint ...
}
```

### Update Payload MQTT:

```cpp
void sendAbsensi(int pegawai_id, String nama, String keterangan) {
    String waktu = getCurrentTime();

    StaticJsonDocument<256> jsonDoc;
    jsonDoc["pegawai_id"] = pegawai_id;
    jsonDoc["waktu_absen"] = waktu;
    jsonDoc["keterangan"] = keterangan;
    jsonDoc["tipe_absen"] = modeAbsensi;  // ← BARU!

    String jsonString;
    serializeJson(jsonDoc, jsonString);

    mqttClient.publish(mqtt_topic, jsonString.c_str());

    Serial.println("Tipe: " + modeAbsensi);
}
```

### Update OLED Display:

```cpp
display.clearDisplay();
display.setCursor(0, 0);
display.setTextSize(1);

// Tampilkan mode di pojok kanan atas
display.setCursor(80, 0);
if (modeAbsensi == "Masuk") {
    display.print("[IN]");
} else {
    display.print("[OUT]");
}

display.setCursor(0, 10);
display.setTextSize(2);
display.println("Letakkan");
display.println("Jari Anda");
display.display();
```

---

## 🧪 Testing Flow Lengkap

### Skenario 1: Karyawan Tepat Waktu

1. **08:00** - Tap kartu (Mode: Masuk)
   ```
   ✅ Absen MASUK berhasil disimpan
   ```

2. **17:00** - Tap kartu (Mode: Keluar)
   ```
   ✅ Absen KELUAR berhasil diupdate
      Durasi Kerja: 09:00
      Durasi Lembur: 00:00
      Status Lembur: Tidak
   ```

### Skenario 2: Karyawan Lembur

1. **08:00** - Tap kartu (Mode: Masuk)
2. **19:00** - Tap kartu (Mode: Keluar)
   ```
   ✅ Absen KELUAR berhasil diupdate
      Durasi Kerja: 11:00
      Durasi Lembur: 03:00
      Status Lembur: Ya
   ```

### Skenario 3: Karyawan Datang Pagi Sekali

1. **06:00** - Tap kartu (Mode: Masuk)
2. **17:00** - Tap kartu (Mode: Keluar)
   ```
   ✅ Absen KELUAR berhasil diupdate
      Durasi Kerja: 11:00
      Durasi Lembur: 03:00
      Status Lembur: Ya
   ```

---

## ⚠️ Catatan Penting

### 1. **Validasi Database**
Pastikan kolom sudah ditambahkan sebelum restart server:
```sql
\d absensi
```

### 2. **Restart Server Wajib**
Setelah update server.js dan tambah attendance-logic.js, server harus direstart.

### 3. **Backward Compatible**
Jika `tipe_absen` tidak dikirim dari ESP32, default nya adalah "Masuk".

### 4. **Satu Kali Absen Masuk Per Hari**
Sistem akan cari absen masuk hari ini yang belum ada waktu_keluar.

### 5. **Error Handling**
Jika tap absen keluar tapi tidak ada absen masuk hari ini:
```
❌ Error absen keluar: Tidak ada record absen masuk hari ini
```

---

## 📊 Query Database Berguna

### Cek absensi dengan lembur:
```sql
SELECT
    p.nama_pegawai,
    a.waktu_absen AS masuk,
    a.waktu_keluar AS keluar,
    a.durasi_kerja,
    a.durasi_lembur,
    a.status_lembur,
    DATE(a.timestamp) AS tanggal
FROM absensi a
JOIN pegawai p ON a.pegawai_id = p.id_pegawai
WHERE a.status_lembur = 'Ya'
ORDER BY a.timestamp DESC;
```

### Total lembur per karyawan bulan ini:
```sql
SELECT
    p.nama_pegawai,
    SUM(a.durasi_lembur) AS total_lembur_menit,
    ROUND(SUM(a.durasi_lembur) / 60.0, 2) AS total_lembur_jam,
    COUNT(*) AS jumlah_hari_lembur
FROM absensi a
JOIN pegawai p ON a.pegawai_id = p.id_pegawai
WHERE a.status_lembur = 'Ya'
AND DATE_TRUNC('month', a.timestamp) = DATE_TRUNC('month', CURRENT_DATE)
GROUP BY p.nama_pegawai
ORDER BY total_lembur_menit DESC;
```

---

## ✅ Checklist Implementasi

- [ ] Update database dengan SQL script
- [ ] Restart server Node.js
- [ ] Test absen masuk via MQTT
- [ ] Test absen keluar via MQTT
- [ ] Verifikasi perhitungan durasi kerja
- [ ] Verifikasi perhitungan lembur
- [ ] Update halaman riwayat untuk tampilkan kolom baru
- [ ] Update ESP32 code untuk support mode Masuk/Keluar
- [ ] Upload ESP32 code baru
- [ ] Test end-to-end dengan hardware

---

## 🚀 Next Steps

### Priority 1 (Wajib):
1. Jalankan SQL script untuk update database
2. Restart server
3. Test dengan MQTT manual

### Priority 2 (Frontend):
4. Update halaman riwayat untuk menampilkan kolom baru
5. Tambah indikator visual untuk lembur

### Priority 3 (ESP32):
6. Update Absensi_MQTT.ino dengan mode toggle
7. Test dengan hardware

---

## 📞 Support

Jika ada error:

1. **Database Error**
   - Pastikan SQL script sudah dijalankan
   - Cek dengan `\d absensi`

2. **Server Error**
   - Pastikan attendance-logic.js ada di folder yang sama dengan server.js
   - Cek log error di console

3. **MQTT Error**
   - Cek format JSON payload
   - Cek tipe_absen: "Masuk" atau "Keluar"

---

**Status:** ✅ Backend Complete
**Next:** Frontend Update & ESP32 Update
**Last Updated:** 6 November 2025

---

Selamat mencoba! 🎉
