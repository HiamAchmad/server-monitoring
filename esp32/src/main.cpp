/*
 * ============================================
 *   ESP32 FINGERPRINT ENROLLMENT SYSTEM
 *   Sistem Absensi IoT dengan Sidik Jari
 * ============================================
 *
 * Hardware:
 * - ESP32 DevKit
 * - Fingerprint Sensor AS608
 * - LED Indikator (Merah, Hijau)
 *
 * Koneksi:
 * - Fingerprint TX  → ESP32 GPIO26
 * - Fingerprint RX  → ESP32 GPIO25
 * - LED Merah (+)   → Resistor 220Ω → GPIO12, (-) → GND
 * - LED Hijau (+)   → Resistor 220Ω → GPIO13, (-) → GND
 *
 * LED Indicator:
 * - Hijau menyala   = Success / Ready
 * - Merah men
 * yala   = Error / Gagal
 * - Hijau blink     = Processing / Waiting
 *
 * Author: Claude Code
 * Date: 14 Januari 2026
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SoftwareSerial.h>  // EspSoftwareSerial untuk ESP32

// ===== WIFI & MQTT CONFIGURATION =====
const char* ssid = "ZTE_2.4G_R2Ush5";
const char* password = "XPUfSYAZ";
const char* mqtt_server = "47.84.67.102";  // VPS IP
const int mqtt_port = 1883;
const char* mqtt_client_id = "ESP32_Fingerprint";
// MQTT Topics
const char* topic_enroll_request = "fingerprint/enroll/request";
const char* topic_enroll_response = "fingerprint/enroll/response";
const char* topic_enroll_progress = "fingerprint/enroll/progress";
const char* topic_attendance = "fingerprint/attendance";

// ===== OLED I2C CONFIGURATION =====
#define USE_OLED true   // OLED I2C aktif
#define USE_LED false   // LED dinonaktifkan (gunakan OLED saja)

// OLED I2C Configuration (4 pin)
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1       // Reset pin (-1 jika tidak digunakan)
#define OLED_ADDR 0x3D      // Alamat I2C OLED (0x3C atau 0x3D)

// I2C Pins (menggunakan default ESP32)
// SDA = GPIO 21 (default)
// SCL = GPIO 22 (default)

// ===== LED PINS (Optional, jika USE_LED = true) =====
#define LED_RED    12   // Error / Gagal
#define LED_GREEN  13   // Success / Berhasil

// ===== OLED OBJECT =====
// Inisialisasi objek OLED I2C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ===== FINGERPRINT SENSOR =====
// Pin mapping (SoftwareSerial):
// Sensor TX → ESP32 RX (GPIO26)
// Sensor RX → ESP32 TX (GPIO25)
#define FP_RX_PIN 26  // RX pin ESP32
#define FP_TX_PIN 25  // TX pin ESP32
SoftwareSerial mySerial;  // Menggunakan EspSoftwareSerial
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// ===== GLOBAL VARIABLES =====
WiFiClient espClient;
PubSubClient mqttClient(espClient);

// Enrollment state
bool enrollmentInProgress = false;
int currentEnrollID = 0;
int currentPegawaiID = 0;
String currentNIP = "";
String currentNama = "";

unsigned long lastReconnectAttempt = 0;
unsigned long lastDebugTime = 0;

// ===== FUNCTION DECLARATIONS =====
void setupWiFi();
void setupMQTT();
void setupFingerprint();
void setupLEDs();
void setupOLED();
void displayMessage(String line1, String line2 = "", String line3 = "");
void displayStatus(String status);
void mqttCallback(char* topic, byte* payload, unsigned int length);
bool mqttReconnect();
void handleEnrollmentRequest(JsonDocument& doc);
void startEnrollment(int id, int pegawai_id, String nip, String nama);
void publishEnrollmentResponse(bool success, String message);
void setLED(int red, int green);
void blinkLED(int pin, int times, int delayMs);
int getFingerprintEnroll(int id);
int checkFingerprint();
void publishAttendance(int fingerprintID);
void publishProgress(String message, int step, int totalSteps);

// ============================================
//               SETUP
// ============================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=================================");
  Serial.println("   FINGERPRINT ENROLLMENT SYSTEM");
  Serial.println("=================================\n");

  // Setup LED pins
  setupLEDs();

  // Setup OLED Display
  setupOLED();
  displayMessage("SISTEM ABSENSI", "Initializing...");

  // Blink LEDs untuk test
  blinkLED(LED_RED, 2, 200);
  blinkLED(LED_GREEN, 2, 200);
  
  // Setup WiFi
  displayMessage("SISTEM ABSENSI", "Connecting WiFi...");
  setupWiFi();

  // Setup Fingerprint Sensor
  displayMessage("SISTEM ABSENSI", "Init Fingerprint...");
  setupFingerprint();

  // Setup MQTT
  displayMessage("SISTEM ABSENSI", "Connecting MQTT...");
  setupMQTT();

  Serial.println("\n✅ System Ready!");
  Serial.println("Mode: Absensi aktif - Silakan tempelkan jari untuk absen\n");

  // LED hijau = ready
  setLED(0, 1);
  delay(1000);
  setLED(0, 0);

  // Display ready message
  displayMessage("SISTEM ABSENSI", "Silakan Absen", "Tempelkan Jari...");
}

// ============================================
//               MAIN LOOP
// ============================================

void loop() {
  // Maintain MQTT connection
  if (!mqttClient.connected()) {
    unsigned long now = millis();
    if (now - lastReconnectAttempt > 5000) {
      lastReconnectAttempt = now;
      if (mqttReconnect()) {
        lastReconnectAttempt = 0;
      }
    }
  } else {
    mqttClient.loop();
  }

  // Jika ada enrollment in progress, tidak perlu cek absensi
  if (enrollmentInProgress) {
    return;
  }

  // Debug: print setiap 10 detik untuk konfirmasi loop berjalan
  if (millis() - lastDebugTime > 10000) {
    lastDebugTime = millis();
    Serial.println("⏳ Menunggu sidik jari untuk absensi...");
  }

  // Attendance scanning - cek fingerprint
  int fingerprintID = checkFingerprint();
  if (fingerprintID > 0) {
    Serial.print("\n✓ Fingerprint matched! ID: ");
    Serial.println(fingerprintID);

    // Publish attendance ke server
    publishAttendance(fingerprintID);

    // Delay untuk mencegah multiple scan
    delay(3000);
  }
}

// ============================================
//          SETUP FUNCTIONS
// ============================================

void setupLEDs() {
  if (!USE_LED) {
    Serial.println("⊘ LEDs disabled (USE_LED = false)");
    return;
  }

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);

  Serial.println("✓ LEDs initialized (GPIO12=Red, GPIO13=Green)");
}

// Setup OLED I2C Display
void setupOLED() {
  if (!USE_OLED) {
    Serial.println("⊘ OLED disabled");
    return;
  }

  Serial.println("Initializing OLED I2C...");

  // Inisialisasi I2C dengan pin default ESP32 (SDA=21, SCL=22)
  Wire.begin();

  // Inisialisasi OLED dengan alamat I2C
  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("✗ OLED I2C allocation failed!");
    Serial.println("  Periksa wiring:");
    Serial.println("    VCC → 3.3V");
    Serial.println("    GND → GND");
    Serial.println("    SDA → GPIO21");
    Serial.println("    SCL → GPIO22");
    Serial.print("  Alamat I2C: 0x");
    Serial.println(OLED_ADDR, HEX);

    // Blink red LED jika error (jika LED enabled)
    if (USE_LED) {
      while(1) {
        blinkLED(LED_RED, 3, 200);
        delay(1000);
      }
    } else {
      while(1) { delay(1000); }
    }
  }

  Serial.println("✓ OLED I2C initialized!");

  // Clear dan test display
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("OLED Ready!");
  display.display();
  delay(1000);
}

void displayMessage(String line1, String line2, String line3) {
  // Print ke Serial Monitor
  Serial.println("📺 " + line1 + " | " + line2 + " | " + line3);

  // Tampilkan di OLED jika enabled
  if (!USE_OLED) return;

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Line 1 (header/title) - ukuran lebih besar
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println(line1);

  // Line 2 (content)
  if (line2.length() > 0) {
    display.setTextSize(1);
    display.setCursor(0, 20);
    display.println(line2);
  }

  // Line 3 (additional info)
  if (line3.length() > 0) {
    display.setTextSize(1);
    display.setCursor(0, 40);
    display.println(line3);
  }

  display.display();
}

void displayStatus(String status) {
  Serial.println("📺 Status: " + status);

  if (!USE_OLED) return;

  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 20);
  display.println(status);
  display.display();
}

void setupWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);


  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    setLED(0, 1);  // Green blink saat connecting
    delay(250);
    setLED(0, 0);
    delay(250);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected!");
    Serial.print("  IP Address: ");
    Serial.println(WiFi.localIP());

    // Blink hijau = WiFi OK
    blinkLED(LED_GREEN, 3, 150);
  } else {
    Serial.println("\n✗ WiFi connection failed!");
    Serial.println("  Please check SSID and password!");

    // LED merah tetap nyala = error
    setLED(1, 0);
    while(1) { delay(1000); } // Stop here
  }
}

void setupFingerprint() {
  Serial.println("Initializing fingerprint sensor...");

  // Inisialisasi SoftwareSerial
  // Format: begin(baud, mode, RX_pin, TX_pin, invert)
  mySerial.begin(57600, SWSERIAL_8N1, FP_RX_PIN, FP_TX_PIN, false);

  delay(1000);

  if (finger.verifyPassword()) {
    Serial.println("✓ Fingerprint sensor detected!");


    Serial.print("  Sensor capacity: ");
    Serial.print(finger.capacity);
    Serial.println(" fingerprints");

    Serial.print("  Template count: ");
    Serial.println(finger.templateCount);

  } else {
    Serial.println("✗ Did not find fingerprint sensor!");
    Serial.println("  Check wiring:");
    Serial.println("    Sensor TX → ESP32 RX (GPIO26)");
    Serial.println("    Sensor RX → ESP32 TX (GPIO25)");
    Serial.println("    Sensor VCC → ESP32 5V");
    Serial.println("    Sensor GND → ESP32 GND");

    // LED merah blink = sensor error
    while(1) {
      blinkLED(LED_RED, 3, 200);
      delay(1000);
    }
  }
}

void setupMQTT() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);  // Increase buffer for JSON messages
  mqttClient.setCallback(mqttCallback);
  mqttClient.setKeepAlive(60);

  Serial.println("Connecting to MQTT...");

  if (mqttReconnect()) {
    Serial.println("✓ MQTT Connected!");
  } else {
    Serial.println("⚠ MQTT connection failed, will retry...");
  }
}

// ============================================
//          MQTT FUNCTIONS
// ============================================

bool mqttReconnect() {
  if (mqttClient.connect(mqtt_client_id)) {
    Serial.println("MQTT Connected!");

    // Subscribe to enrollment request topic
    mqttClient.subscribe(topic_enroll_request);
    Serial.print("Subscribed to: ");
    Serial.println(topic_enroll_request);

    // LED hijau blink = MQTT OK
    blinkLED(LED_GREEN, 2, 200);

    return true;
  } else {
    Serial.print("MQTT connection failed, rc=");
    Serial.println(mqttClient.state());

    // LED merah blink = trying to reconnect
    blinkLED(LED_RED, 1, 100);

    return false;
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  Serial.println("\n[MQTT] >>> Callback triggered!");
  Serial.print("[MQTT] Topic: ");
  Serial.println(topic);
  Serial.print("[MQTT] Length: ");
  Serial.println(length);

  // Convert payload to string safely
  char message[512];
  if (length >= 512) length = 511;
  memcpy(message, payload, length);
  message[length] = '\0';

  Serial.print("[MQTT] Message: ");
  Serial.println(message);

  Serial.println("[MQTT] Parsing JSON...");

  JsonDocument doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("[MQTT] JSON error: ");
    Serial.println(error.c_str());
    return;
  }

  Serial.println("[MQTT] JSON parsed OK!");

  if (String(topic) == topic_enroll_request) {
    Serial.println("[MQTT] Calling handleEnrollmentRequest...");
    handleEnrollmentRequest(doc);
  }

  Serial.println("[MQTT] Callback done!");
}

void handleEnrollmentRequest(JsonDocument& doc) {
  String command = doc["command"] | "";

  if (command == "ENROLL") {
    int fingerprint_id = doc["fingerprint_id"] | 0;
    int pegawai_id = doc["pegawai_id"] | 0;
    String nip = doc["nip"] | "";
    String nama = doc["nama"] | "";

    Serial.println("\n[ENROLLMENT] Request received:");
    Serial.print("  Fingerprint ID: ");
    Serial.println(fingerprint_id);
    Serial.print("  Pegawai ID: ");
    Serial.println(pegawai_id);
    Serial.print("  NIP: ");
    Serial.println(nip);
    Serial.print("  Nama: ");
    Serial.println(nama);

    if (fingerprint_id > 0 && fingerprint_id <= 127 && pegawai_id > 0) {
      startEnrollment(fingerprint_id, pegawai_id, nip, nama);
    } else {
      Serial.println("[ERROR] Invalid fingerprint_id or pegawai_id!");
      publishEnrollmentResponse(false, "Invalid ID");
      setLED(1, 0); // Red LED
      delay(2000);
      setLED(0, 0);
    }
  }
}

// ============================================
//       FINGERPRINT ENROLLMENT
// ============================================

void startEnrollment(int id, int pegawai_id, String nip, String nama) {
  enrollmentInProgress = true;
  currentEnrollID = id;
  currentPegawaiID = pegawai_id;
  currentNIP = nip;
  currentNama = nama;

  Serial.println("\n========================================");
  Serial.println("  STARTING FINGERPRINT ENROLLMENT");
  Serial.println("========================================");
  Serial.print("Employee: ");
  Serial.print(currentNama);
  Serial.print(" (");
  Serial.print(currentNIP);
  Serial.println(")");
  Serial.print("Fingerprint Slot: ");
  Serial.println(currentEnrollID);
  Serial.println("========================================\n");

  // LED hijau = proses enrollment
  setLED(0, 1);

  // Mulai proses enrollment
  int result = getFingerprintEnroll(currentEnrollID);

  if (result == FINGERPRINT_OK) {
    Serial.println("\n✓ ENROLLMENT SUCCESS!");

    // Publish success response
    publishEnrollmentResponse(true, "Sidik jari berhasil didaftarkan");

    // LED hijau = success
    setLED(0, 1);
    delay(3000);
    setLED(0, 0);

  } else {
    Serial.println("\n✗ ENROLLMENT FAILED!");

    // Publish error response
    String errorMsg = "Enrollment gagal (Error code: " + String(result) + ")";
    publishEnrollmentResponse(false, errorMsg);

    // LED merah = error
    setLED(1, 0);
    delay(3000);
    setLED(0, 0);
  }

  enrollmentInProgress = false;
}

int getFingerprintEnroll(int id) {
  int p = -1;

  Serial.println("Waiting for finger...");
  Serial.println(">>> Place your finger on the sensor");

  // Step 1: Tempelkan jari pertama
  publishProgress("Langkah 1/4: Tempelkan jari pada sensor...", 1, 4);

  // LED hijau blink = waiting for finger
  unsigned long startTime = millis();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();

    // Blink LED hijau
    if ((millis() - startTime) % 500 < 250) {
      setLED(0, 1);
    } else {
      setLED(0, 0);
    }

    // Timeout after 30 seconds
    if (millis() - startTime > 30000) {
      Serial.println("Timeout! No finger detected.");
      publishProgress("⏱ Timeout! Tidak ada jari terdeteksi", 0, 4);
      return FINGERPRINT_TIMEOUT;
    }

    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken!");
        setLED(0, 1); // Green = OK
        publishProgress(" Gambar jari pertama berhasil diambil!", 1, 4);
        break;
      case FINGERPRINT_NOFINGER:
        // No finger, keep waiting
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error!");
        publishProgress("Error komunikasi sensor!", 0, 4);
        return p;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error!");
        publishProgress("Gagal mengambil gambar!", 0, 4);
        return p;
      default:
        Serial.println("Unknown error!");
        publishProgress("Error tidak dikenal!", 0, 4);
        return p;
    }
  }

  // Convert image 1
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted!");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy!");
      publishProgress("❌ Gambar terlalu kotor, coba lagi!", 0, 4);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error!");
      publishProgress("❌ Error komunikasi sensor!", 0, 4);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features!");
      publishProgress("❌ Sidik jari tidak terdeteksi dengan jelas!", 0, 4);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Invalid image!");
      publishProgress("❌ Gambar tidak valid!", 0, 4);
      return p;
    default:
      Serial.println("Unknown error!");
      publishProgress("❌ Error tidak dikenal!", 0, 4);
      return p;
  }

  // Step 2: Angkat jari
  Serial.println("\nRemove finger...");
  publishProgress("📍 Langkah 2/4: Angkat jari dari sensor...", 2, 4);
  delay(2000);
  setLED(0, 0);

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  // Step 3: Tempelkan jari lagi
  Serial.println("Place SAME finger again...");
  publishProgress("📍 Langkah 3/4: Tempelkan jari yang SAMA lagi...", 3, 4);

  // Get second image
  p = -1;
  startTime = millis();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();

    // Blink LED hijau
    if ((millis() - startTime) % 500 < 250) {
      setLED(0, 1);
    } else {
      setLED(0, 0);
    }

    // Timeout
    if (millis() - startTime > 30000) {
      Serial.println("Timeout! No finger detected.");
      publishProgress("⏱️ Timeout! Tidak ada jari terdeteksi", 0, 4);
      return FINGERPRINT_TIMEOUT;
    }

    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken!");
        setLED(0, 1);
        publishProgress("✅ Gambar jari kedua berhasil diambil!", 3, 4);
        break;
      case FINGERPRINT_NOFINGER:
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error!");
        publishProgress("❌ Error komunikasi sensor!", 0, 4);
        return p;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error!");
        publishProgress("❌ Gagal mengambil gambar!", 0, 4);
        return p;
      default:
        Serial.println("Unknown error!");
        publishProgress("❌ Error tidak dikenal!", 0, 4);
        return p;
    }
  }

  // Convert image 2
  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted!");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy!");
      publishProgress("❌ Gambar terlalu kotor, coba lagi!", 0, 4);
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error!");
      publishProgress("❌ Error komunikasi sensor!", 0, 4);
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features!");
      publishProgress("❌ Sidik jari tidak terdeteksi dengan jelas!", 0, 4);
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Invalid image!");
      publishProgress("❌ Gambar tidak valid!", 0, 4);
      return p;
    default:
      Serial.println("Unknown error!");
      publishProgress("❌ Error tidak dikenal!", 0, 4);
      return p;
  }

  // Step 4: Create model and store
  Serial.println("Creating model...");
  publishProgress("📍 Langkah 4/4: Menyimpan sidik jari...", 4, 4);
  setLED(0, 1); // Green = processing

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error!");
    publishProgress("❌ Error komunikasi sensor!", 0, 4);
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match!");
    publishProgress("❌ Kedua sidik jari tidak cocok! Ulangi dari awal.", 0, 4);
    return p;
  } else {
    Serial.println("Unknown error!");
    publishProgress("❌ Error tidak dikenal!", 0, 4);
    return p;
  }

  // Store model
  Serial.print("Storing model at ID #");
  Serial.println(id);

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored successfully!");
    publishProgress("✅ Sidik jari berhasil disimpan!", 4, 4);
    return FINGERPRINT_OK;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error!");
    publishProgress("❌ Error komunikasi sensor!", 0, 4);
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Invalid storage location!");
    publishProgress("❌ Lokasi penyimpanan tidak valid!", 0, 4);
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Flash storage error!");
    publishProgress("❌ Error penyimpanan flash!", 0, 4);
    return p;
  } else {
    Serial.println("Unknown error!");
    publishProgress("❌ Error tidak dikenal!", 0, 4);
    return p;
  }
}

void publishEnrollmentResponse(bool success, String message) {
  StaticJsonDocument<256> doc;

  doc["success"] = success;
  doc["fingerprint_id"] = currentEnrollID;
  doc["pegawai_id"] = currentPegawaiID;
  doc["nip"] = currentNIP;
  doc["nama"] = currentNama;
  doc["message"] = message;
  doc["timestamp"] = millis();

  char buffer[256];
  serializeJson(doc, buffer);

  if (mqttClient.publish(topic_enroll_response, buffer)) {
    Serial.println("\n[MQTT] Response published:");
    Serial.println(buffer);
  } else {
    Serial.println("\n[MQTT] Publish failed!");
  }
}

// ============================================
//          LED CONTROL FUNCTIONS
// ============================================

void setLED(int red, int green) {
  if (!USE_LED) return;  // Skip jika LED tidak digunakan

  digitalWrite(LED_RED, red ? HIGH : LOW);
  digitalWrite(LED_GREEN, green ? HIGH : LOW);

}

void blinkLED(int pin, int times, int delayMs) {
  if (!USE_LED) return;  // Skip jika LED tidak digunakan

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(delayMs);
    digitalWrite(pin, LOW);
    delay(delayMs);
  }
}

// ============================================
//       ATTENDANCE FUNCTIONS
// ============================================

int checkFingerprint() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    return 0;  // Tidak ada jari atau error
  }

  Serial.println("Jari terdeteksi, memproses...");

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("Gagal convert image");
    return 0;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("Match! ID: ");
    Serial.print(finger.fingerID);
    Serial.print(" Confidence: ");
    Serial.println(finger.confidence);
    return finger.fingerID;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Sidik jari tidak ditemukan di database sensor");
    Serial.println(">>> LED MERAH BLINK...");
    // LED merah blink = tidak ditemukan
    blinkLED(LED_RED, 3, 300);
    Serial.println(">>> LED MERAH SELESAI");
  } else {
    Serial.print("Error search: ");
    Serial.println(p);
    Serial.println(">>> LED MERAH BLINK...");
    // LED merah blink = error
    blinkLED(LED_RED, 3, 300);
    Serial.println(">>> LED MERAH SELESAI");
  }

  return 0;
}

// Publish attendance data ke MQTT
void publishAttendance(int fingerprintID) {
  JsonDocument doc;
  doc["fingerprint_id"] = fingerprintID;
  doc["confidence"] = finger.confidence;
  doc["timestamp"] = millis();

  char buffer[256];
  serializeJson(doc, buffer);

  Serial.print("Publishing attendance: ");
  Serial.println(buffer);

  if (mqttClient.publish(topic_attendance, buffer)) {
    Serial.println("Attendance published!");
    // LED hijau = success
    setLED(0, 1);
    delay(500);
    setLED(0, 0);
  } else {
    Serial.println("Failed to publish attendance!");
    // LED merah = error
    setLED(1, 0);
    delay(500);
    setLED(0, 0);
  }
}

// Publish progress update ke frontend
void publishProgress(String message, int step, int totalSteps) {
  StaticJsonDocument<256> doc;

  doc["pegawai_id"] = currentPegawaiID;
  doc["fingerprint_id"] = currentEnrollID;
  doc["message"] = message;
  doc["step"] = step;
  doc["total_steps"] = totalSteps;
  doc["timestamp"] = millis();

  char buffer[256];
  serializeJson(doc, buffer);

  mqttClient.publish(topic_enroll_progress, buffer);
  Serial.print("[PROGRESS] ");
  Serial.println(message);
}

// ============================================
//                  END
// ============================================
