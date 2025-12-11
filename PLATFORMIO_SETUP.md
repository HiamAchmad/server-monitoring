# 🚀 Setup ESP32 dengan PlatformIO

**PlatformIO adalah IDE modern untuk embedded development yang lebih powerful daripada Arduino IDE.**

---

## 📦 Instalasi PlatformIO

### Opsi 1: VS Code Extension (Recommended)

1. Install **VS Code**: https://code.visualstudio.com/
2. Buka VS Code
3. Klik **Extensions** (Ctrl+Shift+X)
4. Cari **"PlatformIO IDE"**
5. Klik **Install**
6. Restart VS Code

### Opsi 2: PlatformIO Core (CLI)

```bash
# Install via pip
pip install -U platformio

# Install via curl
curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py -o get-platformio.py
python3 get-platformio.py
```

---

## 📁 Struktur Project

```
server-monitoring/
├── platformio.ini       # Configuration file
├── src/
│   └── main.cpp        # Main source code (esp32_fingerprint.ino)
├── lib/                # Custom libraries
├── include/            # Header files
├── public/             # Web files
└── node_modules/       # Node.js dependencies
```

---

## ⚙️ Konfigurasi

File `platformio.ini` sudah dikonfigurasi dengan:
- ✅ Board: ESP32 Dev Module
- ✅ Libraries: Adafruit Fingerprint, PubSubClient, ArduinoJson
- ✅ Serial Monitor: 115200 baud
- ✅ Upload speed: 921600

### Edit Konfigurasi WiFi & MQTT

Buka file: **src/main.cpp**

```cpp
// Baris 29-30: WiFi Configuration
const char* ssid = "NAMA_WIFI_ANDA";          // ← Ganti!
const char* password = "PASSWORD_WIFI_ANDA";  // ← Ganti!

// Baris 33: MQTT Server
const char* mqtt_server = "192.168.1.13";     // ← IP Laptop Anda
```

---

## 🔌 Wiring (Tanpa LED)

```
FINGERPRINT SENSOR → ESP32
━━━━━━━━━━━━━━━━━━━━━━━━━
VCC (Merah)   →  3.3V
GND (Hitam)   →  GND
TX  (Kuning)  →  GPIO16 (RX2)  ⚠️ SILANG!
RX  (Hijau)   →  GPIO17 (TX2)  ⚠️ SILANG!
```

---

## 🚀 Cara Build & Upload

### Via VS Code (GUI)

1. Buka folder `server-monitoring` di VS Code
2. PlatformIO akan auto-detect project
3. Di PlatformIO Toolbar (bottom):
   - ✅ Klik **Build** (✓) untuk compile
   - 📤 Klik **Upload** (→) untuk upload ke ESP32
   - 📊 Klik **Serial Monitor** untuk lihat output

### Via Terminal (CLI)

```bash
# Masuk ke direktori project
cd /home/hasan/Documents/Perancangan/Perancangan/server-monitoring

# Build project
pio run

# Upload ke ESP32
pio run --target upload

# Open Serial Monitor
pio device monitor

# Build + Upload + Monitor (all in one)
pio run --target upload && pio device monitor
```

---

## 🔍 Troubleshooting

### Error: Port tidak ditemukan

**Linux:**
```bash
# Cek port yang tersedia
ls /dev/ttyUSB*

# Jika /dev/ttyUSB0, edit platformio.ini:
upload_port = /dev/ttyUSB0

# Berikan permission
sudo chmod 666 /dev/ttyUSB0
```

**Windows:**
```bash
# Cek di Device Manager
# Edit platformio.ini:
upload_port = COM3  # Sesuaikan dengan port Anda
```

### Error: Permission denied

```bash
# Tambahkan user ke dialout group (Linux)
sudo usermod -a -G dialout $USER

# Logout dan login lagi
```

### Library tidak terinstall

```bash
# Install manual
pio lib install "Adafruit Fingerprint Sensor Library"
pio lib install "PubSubClient"
pio lib install "ArduinoJson"

# Atau otomatis saat build
pio run
```

---

## 📊 Serial Monitor Output

Setelah upload, buka Serial Monitor. Anda akan melihat:

```
=================================
  FINGERPRINT ENROLLMENT SYSTEM
=================================

⊘ LEDs disabled (USE_LED = false)
✓ WiFi connected!
✓ IP Address: 192.168.1.xxx
✓ Fingerprint sensor detected!
✓ MQTT Connecting to 192.168.1.13...
✓ MQTT Connected!

✅ System Ready!
Waiting for enrollment command...
```

---

## 🎯 Advanced Features

### Clean Build

```bash
pio run --target clean
pio run
```

### OTA Update (Over-The-Air)

Edit `platformio.ini`:
```ini
upload_protocol = espota
upload_port = 192.168.1.xxx  ; IP ESP32 Anda
```

### Debug Mode

Edit `platformio.ini`:
```ini
build_flags =
    -DCORE_DEBUG_LEVEL=5  ; 0=None, 5=Verbose
```

---

## 🆚 PlatformIO vs Arduino IDE

| Feature | PlatformIO | Arduino IDE |
|---------|-----------|-------------|
| IDE | VS Code | Standalone |
| Library Management | Auto-install | Manual |
| IntelliSense | ✅ Excellent | ❌ Basic |
| Build Speed | ✅ Fast | ⚠️ Slower |
| Multi-board | ✅ Yes | ⚠️ Limited |
| Debugging | ✅ Yes | ❌ No |
| Git Integration | ✅ Built-in | ❌ External |

---

## 📚 Dokumentasi

- **PlatformIO Docs**: https://docs.platformio.org/
- **ESP32 Platform**: https://docs.platformio.org/en/latest/platforms/espressif32.html
- **Library Registry**: https://registry.platformio.org/

---

## ✅ Next Steps

1. ✅ Install PlatformIO
2. ✅ Edit `src/main.cpp` (WiFi & MQTT config)
3. ✅ Connect ESP32 via USB
4. ✅ Build & Upload
5. ✅ Open Serial Monitor
6. ✅ Test dari web interface!

---

**Happy Coding!** 🎉

Jika ada error, cek Serial Monitor output dan sesuaikan dengan troubleshooting di atas.
