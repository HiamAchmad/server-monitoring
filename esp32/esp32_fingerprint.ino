/*
 * ============================================
 *   ESP32 FINGERPRINT ENROLLMENT SYSTEM
 *   Sistem Absensi IoT dengan Sidik Jari
 * ============================================
 *
 * Hardware:
 * - ESP32 DevKit
 * - Fingerprint Sensor R307/AS608
 * - LED RGB (Merah, Hijau, Biru)
 *
 * Koneksi:
 * - Fingerprint TX  → ESP32 GPIO16 (RX2)
 * - Fingerprint RX  → ESP32 GPIO17 (TX2)
 * - LED Merah       → ESP32 GPIO25
 * - LED Hijau       → ESP32 GPIO26
 * - LED Biru        → ESP32 GPIO27
 *
 * Author: Claude Code
 * Date: 16 November 2025
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>
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

// ===== LED CONFIGURATION =====
#define USE_LED false  // ← Set 'true' jika pakai LED, 'false' jika tidak pakai LED

// ===== LED PINS =====
#define LED_RED    25  // Error / Gagal
#define LED_GREEN  26  // Success / Berhasil
#define LED_BLUE   27  // Processing / Tunggu

// ===== FINGERPRINT SENSOR =====
// Hardware Serial 2 (RX=GPIO16, TX=GPIO17)
HardwareSerial mySerial(2);
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

// ===== FUNCTION DECLARATIONS =====
void setupWiFi();
void setupMQTT();
void setupFingerprint();
void setupLEDs();
void mqttCallback(char* topic, byte* payload, unsigned int length);
bool mqttReconnect();
void handleEnrollmentRequest(JsonDocument& doc);
void startEnrollment(int id, int pegawai_id, String nip, String nama);
void publishEnrollmentResponse(bool success, String message);
void publishProgress(String message, int step, int totalSteps);
void setLED(int red, int green, int blue);
void blinkLED(int pin, int times, int delayMs);
int getFingerprintEnroll(int id);

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

  // Blink all LEDs untuk test
  blinkLED(LED_RED, 2, 200);
  blinkLED(LED_GREEN, 2, 200);
  blinkLED(LED_BLUE, 2, 200);

  // Setup WiFi
  setupWiFi();

  // Setup Fingerprint Sensor
  setupFingerprint();

  // Setup MQTT
  setupMQTT();

  Serial.println("\n✅ System Ready!");
  Serial.println("Mode: Absensi aktif - Silakan tempelkan jari untuk absen\n");

  // LED hijau = ready
  setLED(0, 1, 0);
  delay(1000);
  setLED(0, 0, 0);
}

// Variable untuk debug
unsigned long lastDebugTime = 0;

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

// Check fingerprint dan return ID jika match, 0 jika tidak ada/tidak match
int checkFingerprint() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) {
    return 0;  // Tidak ada jari atau error
  }

  Serial.println("🔍 Jari terdeteksi, memproses...");

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    Serial.println("❌ Gagal convert image");
    return 0;
  }

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("✅ Match! ID: ");
    Serial.print(finger.fingerID);
    Serial.print(" Confidence: ");
    Serial.println(finger.confidence);
    return finger.fingerID;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("❌ Sidik jari tidak ditemukan di database sensor");
  } else {
    Serial.print("❌ Error search: ");
    Serial.println(p);
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

  Serial.print("📤 Publishing attendance: ");
  Serial.println(buffer);

  if (mqttClient.publish(topic_attendance, buffer)) {
    Serial.println("✓ Attendance published!");
    // LED hijau = success
    setLED(0, 1, 0);
    delay(500);
    setLED(0, 0, 0);
  } else {
    Serial.println("✗ Failed to publish attendance!");
    // LED merah = error
    setLED(1, 0, 0);
    delay(500);
    setLED(0, 0, 0);
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
  pinMode(LED_BLUE, OUTPUT);

 
  setLED(0, 0, 0);

  Serial.println("✓ LEDs initialized");
}

void setupWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);


  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    setLED(0, 0, 1);
    delay(250);
    setLED(0, 0, 0);
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
    setLED(1, 0, 0);
    while(1) { delay(1000); } // Stop here
  }
}

void setupFingerprint() {
  Serial.println("Initializing fingerprint sensor...");

  // Start Serial untuk sensor (RX=26, TX=25, Baud=57600)
  // GPIO26 ← Sensor TX (ESP32 receives)
  // GPIO25 → Sensor RX (ESP32 sends)
  mySerial.begin(57600, SERIAL_8N1, 26, 25);

  delay(1000);

  if (finger.verifyPassword()) {
    Serial.println("✓ Fingerprint sensor detected!");

    // Get sensor info
    Serial.print("  Sensor capacity: ");
    Serial.print(finger.capacity);
    Serial.println(" fingerprints");

    Serial.print("  Template count: ");
    Serial.println(finger.templateCount);

  } else {
    Serial.println("✗ Did not find fingerprint sensor!");
    Serial.println("  Check wiring:");
    Serial.println("    Sensor TX → ESP32 GPIO26");
    Serial.println("    Sensor RX → ESP32 GPIO25");
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

    // LED biru blink = trying to reconnect
    blinkLED(LED_BLUE, 1, 100);

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
      setLED(1, 0, 0); // Red LED
      delay(2000);
      setLED(0, 0, 0);
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

  // LED biru = proses enrollment
  setLED(0, 0, 1);

  // Mulai proses enrollment
  int result = getFingerprintEnroll(currentEnrollID);

  if (result == FINGERPRINT_OK) {
    Serial.println("\n✓ ENROLLMENT SUCCESS!");

    // Publish success response
    publishEnrollmentResponse(true, "Sidik jari berhasil didaftarkan");

    // LED hijau = success
    setLED(0, 1, 0);
    delay(3000);
    setLED(0, 0, 0);

  } else {
    Serial.println("\n✗ ENROLLMENT FAILED!");

    // Publish error response
    String errorMsg = "Enrollment gagal (Error code: " + String(result) + ")";
    publishEnrollmentResponse(false, errorMsg);

    // LED merah = error
    setLED(1, 0, 0);
    delay(3000);
    setLED(0, 0, 0);
  }

  enrollmentInProgress = false;
}

int getFingerprintEnroll(int id) {
  int p = -1;

  Serial.println("Waiting for finger...");
  Serial.println(">>> Place your finger on the sensor");

  // Step 1: Menunggu jari pertama
  publishProgress("📍 Langkah 1/4: Tempelkan jari pada sensor...", 1, 4);

  // LED biru blink cepat = waiting for finger
  unsigned long startTime = millis();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();

    // Blink LED biru
    if ((millis() - startTime) % 500 < 250) {
      setLED(0, 0, 1);
    } else {
      setLED(0, 0, 0);
    }

    // Timeout after 30 seconds
    if (millis() - startTime > 30000) {
      Serial.println("Timeout! No finger detected.");
      publishProgress("⏱️ Timeout! Tidak ada jari terdeteksi", 0, 4);
      return FINGERPRINT_TIMEOUT;
    }

    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken!");
        setLED(0, 1, 0); // Green = OK
        publishProgress("✅ Gambar jari pertama berhasil diambil!", 1, 4);
        break;
      case FINGERPRINT_NOFINGER:
        // No finger, keep waiting
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
  setLED(0, 0, 0);

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

    // Blink LED biru
    if ((millis() - startTime) % 500 < 250) {
      setLED(0, 0, 1);
    } else {
      setLED(0, 0, 0);
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
        setLED(0, 1, 0);
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
  setLED(0, 0, 1); // Blue = processing

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
    return FINGERPRINT_OK;
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error!");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Invalid storage location!");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Flash storage error!");
    return p;
  } else {
    Serial.println("Unknown error!");
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
//          LED CONTROL FUNCTIONS
// ============================================

void setLED(int red, int green, int blue) {
  if (!USE_LED) return;  // Skip jika LED tidak digunakan

  digitalWrite(LED_RED, red ? HIGH : LOW);
  digitalWrite(LED_GREEN, green ? HIGH : LOW);
  digitalWrite(LED_BLUE, blue ? HIGH : LOW);
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
//                  END
// ============================================
