/*
 * ========================================
 * ESP32 ABSENSI SYSTEM dengan RFID RC522
 * ========================================
 *
 * Fitur:
 * - WiFi Connection
 * - MQTT Client
 * - RFID RC522 Reader
 * - Auto-reconnect WiFi & MQTT
 * - LED & Buzzer Indicator
 * - Serial Monitor Logging
 *
 * Hardware Requirements:
 * - ESP32 DevKit
 * - MFRC522 RFID Reader
 * - LED (Green & Red)
 * - Buzzer
 *
 * Pin Configuration:
 * RFID RC522:
 *   - SDA  -> GPIO 5
 *   - SCK  -> GPIO 18
 *   - MOSI -> GPIO 23
 *   - MISO -> GPIO 19
 *   - RST  -> GPIO 22
 *   - GND  -> GND
 *   - 3.3V -> 3.3V
 *
 * LED & Buzzer:
 *   - LED Green -> GPIO 2
 *   - LED Red   -> GPIO 4
 *   - Buzzer    -> GPIO 15
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <SPI.h>
#include <MFRC522.h>
#include <time.h>

// ============================================
// KONFIGURASI WIFI
// ============================================
const char* ssid = "NAMA_WIFI_ANDA";           // Ganti dengan nama WiFi Anda
const char* password = "PASSWORD_WIFI_ANDA";   // Ganti dengan password WiFi Anda

// ============================================
// KONFIGURASI MQTT SERVER
// ============================================
const char* mqtt_server = "192.168.1.100";     // Ganti dengan IP server Anda
const int mqtt_port = 1883;
const char* mqtt_topic = "absensi/data";
const char* mqtt_client_id = "ESP32_Absensi_001";

// ============================================
// KONFIGURASI RFID RC522
// ============================================
#define SS_PIN 5
#define RST_PIN 22
MFRC522 rfid(SS_PIN, RST_PIN);

// ============================================
// KONFIGURASI PIN LED & BUZZER
// ============================================
#define LED_GREEN 2
#define LED_RED 4
#define BUZZER 15

// ============================================
// KONFIGURASI NTP (untuk waktu)
// ============================================
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 25200;  // GMT+7 (Indonesia WIB)
const int daylightOffset_sec = 0;

// ============================================
// DATABASE KARTU RFID
// ============================================
// Format: {"UID_KARTU", pegawai_id, "Nama Karyawan"}
struct RFIDCard {
    String uid;
    int pegawai_id;
    String nama;
};

RFIDCard registeredCards[] = {
    {"A1 B2 C3 D4", 1, "John Doe"},
    {"E5 F6 G7 H8", 2, "Jane Smith"},
    {"11 22 33 44", 3, "Ahmad Zainudin"},
    // Tambahkan kartu lainnya di sini
};

const int totalCards = sizeof(registeredCards) / sizeof(RFIDCard);

// ============================================
// OBJEK MQTT & WIFI
// ============================================
WiFiClient espClient;
PubSubClient client(espClient);

// ============================================
// VARIABEL GLOBAL
// ============================================
unsigned long lastReconnectAttempt = 0;
String lastScannedUID = "";
unsigned long lastScanTime = 0;
const unsigned long scanCooldown = 3000; // 3 detik cooldown

// ============================================
// FUNGSI SETUP WiFi
// ============================================
void setup_wifi() {
    delay(10);
    Serial.println();
    Serial.print("Menghubungkan ke WiFi: ");
    Serial.println(ssid);

    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);

    // LED merah berkedip saat connecting
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 30) {
        digitalWrite(LED_RED, HIGH);
        delay(250);
        digitalWrite(LED_RED, LOW);
        delay(250);
        Serial.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        Serial.println();
        Serial.println("WiFi terhubung!");
        Serial.print("IP Address: ");
        Serial.println(WiFi.localIP());

        // LED hijau ON saat WiFi terhubung
        digitalWrite(LED_GREEN, HIGH);
        beep(2, 100); // Beep 2x
    } else {
        Serial.println();
        Serial.println("Gagal terhubung ke WiFi!");
        digitalWrite(LED_RED, HIGH);
    }
}

// ============================================
// FUNGSI KONEKSI MQTT
// ============================================
boolean reconnect() {
    if (client.connect(mqtt_client_id)) {
        Serial.println("MQTT terhubung!");
        beep(1, 200);
        return true;
    }
    return false;
}

// ============================================
// FUNGSI BUZZER BEEP
// ============================================
void beep(int times, int duration) {
    for (int i = 0; i < times; i++) {
        digitalWrite(BUZZER, HIGH);
        delay(duration);
        digitalWrite(BUZZER, LOW);
        if (i < times - 1) delay(duration);
    }
}

// ============================================
// FUNGSI BACA KARTU RFID
// ============================================
String readRFID() {
    String uid = "";
    for (byte i = 0; i < rfid.uid.size; i++) {
        if (rfid.uid.uidByte[i] < 0x10) uid += "0";
        uid += String(rfid.uid.uidByte[i], HEX);
        if (i < rfid.uid.size - 1) uid += " ";
    }
    uid.toUpperCase();
    return uid;
}

// ============================================
// FUNGSI CARI KARTU DI DATABASE
// ============================================
int findCard(String uid) {
    for (int i = 0; i < totalCards; i++) {
        if (registeredCards[i].uid == uid) {
            return i;
        }
    }
    return -1; // Kartu tidak terdaftar
}

// ============================================
// FUNGSI DAPATKAN WAKTU SAAT INI
// ============================================
String getCurrentTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Gagal mendapatkan waktu");
        return "00:00:00";
    }

    char timeStr[9];
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", &timeinfo);
    return String(timeStr);
}

// ============================================
// FUNGSI CEK KETERLAMBATAN
// ============================================
String checkAttendanceStatus() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "Hadir";
    }

    int hour = timeinfo.tm_hour;
    int minute = timeinfo.tm_min;

    // Jam masuk: 08:00
    // Jika lebih dari 08:00 = Terlambat
    if (hour > 8 || (hour == 8 && minute > 0)) {
        return "Terlambat";
    }

    return "Hadir";
}

// ============================================
// FUNGSI KIRIM DATA ABSENSI VIA MQTT
// ============================================
void sendAbsensi(int pegawai_id, String nama, String keterangan) {
    String waktu = getCurrentTime();

    // Format JSON
    String payload = "{";
    payload += "\"pegawai_id\":" + String(pegawai_id) + ",";
    payload += "\"waktu_absen\":\"" + waktu + "\",";
    payload += "\"keterangan\":\"" + keterangan + "\"";
    payload += "}";

    Serial.println("\n=== MENGIRIM DATA ABSENSI ===");
    Serial.println("Nama: " + nama);
    Serial.println("ID Pegawai: " + String(pegawai_id));
    Serial.println("Waktu: " + waktu);
    Serial.println("Status: " + keterangan);
    Serial.println("Payload: " + payload);

    // Publish ke MQTT
    if (client.publish(mqtt_topic, payload.c_str())) {
        Serial.println("✓ Data berhasil dikirim!");

        // Indikator sukses
        digitalWrite(LED_GREEN, HIGH);
        if (keterangan == "Hadir") {
            beep(2, 100); // Beep 2x pendek (Hadir)
        } else if (keterangan == "Terlambat") {
            beep(3, 150); // Beep 3x agak lama (Terlambat)
        }
        delay(500);
        digitalWrite(LED_GREEN, LOW);

    } else {
        Serial.println("✗ Gagal mengirim data!");

        // Indikator error
        digitalWrite(LED_RED, HIGH);
        beep(1, 500); // Beep panjang (error)
        delay(500);
        digitalWrite(LED_RED, LOW);
    }

    Serial.println("=============================\n");
}

// ============================================
// SETUP
// ============================================
void setup() {
    // Serial Monitor
    Serial.begin(115200);
    Serial.println("\n\n=================================");
    Serial.println("ESP32 SISTEM ABSENSI RFID");
    Serial.println("=================================\n");

    // Setup Pin
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, LOW);
    digitalWrite(BUZZER, LOW);

    // Startup beep
    beep(3, 100);

    // Init SPI & RFID
    SPI.begin();
    rfid.PCD_Init();
    Serial.println("RFID RC522 Ready!");

    // Connect WiFi
    setup_wifi();

    // Setup NTP Time
    if (WiFi.status() == WL_CONNECTED) {
        configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
        Serial.println("NTP Time configured");

        // Tunggu waktu ter-sync
        delay(2000);
        Serial.print("Waktu sekarang: ");
        Serial.println(getCurrentTime());
    }

    // Setup MQTT
    client.setServer(mqtt_server, mqtt_port);

    Serial.println("\nSistem siap!");
    Serial.println("Tempelkan kartu RFID...\n");
}

// ============================================
// LOOP UTAMA
// ============================================
void loop() {
    // Auto-reconnect WiFi
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi terputus! Reconnecting...");
        digitalWrite(LED_GREEN, LOW);
        digitalWrite(LED_RED, HIGH);
        setup_wifi();
    }

    // Auto-reconnect MQTT
    if (!client.connected()) {
        unsigned long now = millis();
        if (now - lastReconnectAttempt > 5000) {
            lastReconnectAttempt = now;
            Serial.println("MQTT terputus! Reconnecting...");
            if (reconnect()) {
                lastReconnectAttempt = 0;
            }
        }
    } else {
        client.loop();
    }

    // Cek kartu RFID
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {

        // Baca UID kartu
        String uid = readRFID();

        // Cooldown untuk prevent double scan
        unsigned long currentTime = millis();
        if (uid == lastScannedUID && (currentTime - lastScanTime) < scanCooldown) {
            rfid.PICC_HaltA();
            rfid.PCD_StopCrypto1();
            return;
        }

        lastScannedUID = uid;
        lastScanTime = currentTime;

        Serial.println("\n--- KARTU TERDETEKSI ---");
        Serial.print("UID: ");
        Serial.println(uid);

        // Cari kartu di database
        int cardIndex = findCard(uid);

        if (cardIndex >= 0) {
            // Kartu terdaftar
            RFIDCard card = registeredCards[cardIndex];
            String status = checkAttendanceStatus();

            Serial.println("Status: TERDAFTAR");
            Serial.println("Nama: " + card.nama);
            Serial.println("ID: " + String(card.pegawai_id));

            // Kirim data absensi
            sendAbsensi(card.pegawai_id, card.nama, status);

        } else {
            // Kartu tidak terdaftar
            Serial.println("Status: TIDAK TERDAFTAR");
            Serial.println("Kartu tidak dikenal!");

            digitalWrite(LED_RED, HIGH);
            beep(4, 100); // Beep 4x cepat (error)
            delay(1000);
            digitalWrite(LED_RED, LOW);
        }

        Serial.println("------------------------\n");

        // Halt kartu
        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
    }

    delay(100);
}
