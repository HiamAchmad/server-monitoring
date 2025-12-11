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
 * - Fingerprint TX  → ESP32 GPIO25 (RX2)
 * - Fingerprint RX  → ESP32 GPIO26 (TX2)
 * - LED Merah       → GPIO25 (tidak digunakan jika USE_LED=false)
 * - LED Hijau       → GPIO26 (tidak digunakan jika USE_LED=false)
 * - LED Biru        → GPIO27 (tidak digunakan jika USE_LED=false)
 *
 * Author: Claude Code
 * Date: 16 November 2025
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ===== WIFI CONFIGURATION =====
const char* ssid = "Server";              // ✅ Hotspot!
const char* password = "kucinglucu";     // ✅ Configured!

// ===== MQTT CONFIGURATION =====
const char* mqtt_server = "10.242.113.243";     // ✅ Hotspot IP!
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

// ===== OLED DISPLAY CONFIGURATION (SPI) =====
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// ✅ VERIFIED WIRING (from working Arduino code):
// OLED Pin → ESP32 GPIO
// D0 (CLK)  → GPIO 18
// D1 (MOSI) → GPIO 23
// RES (RST) → GPIO 4
// DC        → GPIO 16
// CS        → GPIO 5

#define OLED_MOSI 23   // D1 on OLED
#define OLED_CLK  18   // D0 on OLED
#define OLED_DC   16   // DC on OLED
#define OLED_CS   5    // CS on OLED
#define OLED_RES  4    // RES/RST on OLED

// Use HARDWARE SPI like Arduino code
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT,
  &SPI, OLED_DC, OLED_RES, OLED_CS);

// ===== FINGERPRINT SENSOR =====
// Hardware Serial 2 (RX=GPIO25, TX=GPIO26)
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

// OLED state
bool oledEnabled = false;

// Attendance state
unsigned long lastAttendanceScan = 0;
const unsigned long SCAN_COOLDOWN = 5000; // 5 detik cooldown antar scan
const unsigned long SAME_PERSON_COOLDOWN = 60000; // 60 detik cooldown untuk orang yang sama
int lastScannedID = -1;
unsigned long lastScannedTime = 0;

// ===== FUNCTION DECLARATIONS =====
void setupWiFi();
void setupMQTT();
void setupFingerprint();
void setupLEDs();
void setupOLED();
void displayStatus(String title, String message, bool success = true);
void displayWelcome();
void displayEnrollment(String nama, int step);
void displayAttendance(String nama, String status);
void displayError(String message);
void mqttCallback(char* topic, byte* payload, unsigned int length);
bool mqttReconnect();
void handleEnrollmentRequest(JsonDocument& doc);
void startEnrollment(int id, int pegawai_id, String nip, String nama);
void publishEnrollmentResponse(bool success, String message);
void publishEnrollmentProgress(int step, String status, String message);
void publishAttendance(int fingerprintID);
void setLED(int red, int green, int blue);
void blinkLED(int pin, int times, int delayMs);
int getFingerprintEnroll(int id);
int getFingerprintID();
void checkAttendance();

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

  // Setup OLED Display
  setupOLED();

  // Setup WiFi
  setupWiFi();

  // Setup Fingerprint Sensor
  setupFingerprint();

  // Setup MQTT
  setupMQTT();

  Serial.println("\n✅ System Ready!");
  Serial.println("Waiting for enrollment command...\n");

  // LED hijau = ready
  setLED(0, 1, 0);
  delay(1000);
  setLED(0, 0, 0);

  // Display welcome screen
  displayWelcome();
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

  // Check untuk attendance scanning
  checkAttendance();
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

  // Matikan semua LED
  setLED(0, 0, 0);

  Serial.println("✓ LEDs initialized");
}

void setupWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  // LED biru blink = connecting WiFi
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

  // Pin sensor: RX=GPIO25, TX=GPIO26
  mySerial.begin(57600, SERIAL_8N1, 25, 26);

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
    Serial.println("    Sensor TX → ESP32 GPIO25");
    Serial.println("    Sensor RX → ESP32 GPIO26");
    Serial.println("    Sensor VCC → ESP32 3.3V");
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
  Serial.print("\n[MQTT] Message received on topic: ");
  Serial.println(topic);

  // Convert payload to string
  String message = "";
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }

  Serial.print("Message: ");
  Serial.println(message);

  // Parse JSON
  StaticJsonDocument<512> doc;
  DeserializationError error = deserializeJson(doc, message);

  if (error) {
    Serial.print("JSON parse error: ");
    Serial.println(error.c_str());
    return;
  }

  // Handle enrollment request
  if (String(topic) == topic_enroll_request) {
    handleEnrollmentRequest(doc);
  }
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

  // Display enrollment screen
  displayEnrollment(currentNama, 1);

  // Mulai proses enrollment
  int result = getFingerprintEnroll(currentEnrollID);

  if (result == FINGERPRINT_OK) {
    Serial.println("\n✓ ENROLLMENT SUCCESS!");

    // Display success
    displayStatus("ENROLLMENT", "Sukses!\n" + currentNama, true);

    // Publish success response
    publishEnrollmentResponse(true, "Sidik jari berhasil didaftarkan");

    // LED hijau = success
    setLED(0, 1, 0);
    delay(3000);
    setLED(0, 0, 0);

    // Back to welcome screen
    displayWelcome();

  } else {
    Serial.println("\n✗ ENROLLMENT FAILED!");

    // Display error
    displayError("Enrollment Gagal!");

    // Publish error response
    String errorMsg = "Enrollment gagal (Error code: " + String(result) + ")";
    publishEnrollmentResponse(false, errorMsg);

    // LED merah = error
    setLED(1, 0, 0);
    delay(3000);
    setLED(0, 0, 0);

    // Back to welcome screen
    displayWelcome();
  }

  enrollmentInProgress = false;
}

int getFingerprintEnroll(int id) {
  int p = -1;

  Serial.println("Waiting for finger...");
  Serial.println(">>> Place your finger on the sensor");

  // Kirim progress ke dashboard: Step 1 - Tempelkan jari
  publishEnrollmentProgress(1, "waiting", "Silakan tempelkan sidik jari pada sensor");

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
      return FINGERPRINT_TIMEOUT;
    }

    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken!");
        setLED(0, 1, 0); // Green = OK
        break;
      case FINGERPRINT_NOFINGER:
        // No finger, keep waiting
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error!");
        return p;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error!");
        return p;
      default:
        Serial.println("Unknown error!");
        return p;
    }
  }

  // Kirim progress: sidik jari terdeteksi
  publishEnrollmentProgress(2, "captured", "Sidik jari pertama terdeteksi!");

  // Convert image 1
  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted!");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy!");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error!");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features!");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Invalid image!");
      return p;
    default:
      Serial.println("Unknown error!");
      return p;
  }

  Serial.println("\nRemove finger...");

  // Kirim progress: angkat jari
  publishEnrollmentProgress(3, "remove", "Angkat jari dari sensor...");

  delay(2000);
  setLED(0, 0, 0);

  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  Serial.println("Place SAME finger again...");

  // Kirim progress: tempelkan jari kedua kali
  publishEnrollmentProgress(4, "waiting", "Tempelkan jari yang SAMA sekali lagi");

  // Display step 2 on OLED
  displayEnrollment(currentNama, 2);

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
      return FINGERPRINT_TIMEOUT;
    }

    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken!");
        setLED(0, 1, 0);
        break;
      case FINGERPRINT_NOFINGER:
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error!");
        return p;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error!");
        return p;
      default:
        Serial.println("Unknown error!");
        return p;
    }
  }

  // Convert image 2
  // Kirim progress: sidik jari kedua terdeteksi
  publishEnrollmentProgress(5, "captured", "Sidik jari kedua terdeteksi!");

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted!");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy!");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error!");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features!");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Invalid image!");
      return p;
    default:
      Serial.println("Unknown error!");
      return p;
  }

  // Create model
  Serial.println("Creating model...");
  setLED(0, 0, 1); // Blue = processing

  // Kirim progress: memverifikasi
  publishEnrollmentProgress(6, "processing", "Memverifikasi sidik jari...");

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error!");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match!");
    return p;
  } else {
    Serial.println("Unknown error!");
    return p;
  }

  // Store model
  Serial.print("Storing model at ID #");
  Serial.println(id);

  // Kirim progress: menyimpan
  publishEnrollmentProgress(7, "saving", "Menyimpan sidik jari...");

  // Display saving status
  displayEnrollment(currentNama, 3);

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored successfully!");
    // Kirim progress: berhasil
    publishEnrollmentProgress(8, "success", "Sidik jari berhasil disimpan!");
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

void publishEnrollmentProgress(int step, String status, String message) {
  StaticJsonDocument<256> doc;

  doc["step"] = step;
  doc["status"] = status;
  doc["message"] = message;
  doc["pegawai_id"] = currentPegawaiID;
  doc["nama"] = currentNama;
  doc["fingerprint_id"] = currentEnrollID;
  doc["timestamp"] = millis();

  char buffer[256];
  serializeJson(doc, buffer);

  mqttClient.publish(topic_enroll_progress, buffer);
  Serial.print("[PROGRESS] Step ");
  Serial.print(step);
  Serial.print(": ");
  Serial.println(message);
}

// ============================================
//        ATTENDANCE CHECKING FUNCTIONS
// ============================================

void checkAttendance() {
  unsigned long currentTime = millis();

  // Cooldown antar scan
  if (currentTime - lastAttendanceScan < SCAN_COOLDOWN) {
    return;
  }

  lastAttendanceScan = currentTime;

  // Check apakah ada fingerprint di sensor
  int fingerprintID = getFingerprintID();

  if (fingerprintID > 0) {
    // Cek apakah ini orang yang sama dalam waktu singkat
    if (fingerprintID == lastScannedID &&
        (currentTime - lastScannedTime < SAME_PERSON_COOLDOWN)) {
      Serial.println("⚠️ Same person detected within cooldown period. Skipping...");
      return;
    }

    // Update last scanned
    lastScannedID = fingerprintID;
    lastScannedTime = currentTime;

    // Publish attendance
    publishAttendance(fingerprintID);
  }
}

int getFingerprintID() {
  uint8_t p = finger.getImage();

  if (p != FINGERPRINT_OK) {
    return -1;
  }

  // Convert image
  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) {
    return -1;
  }

  // Search for match
  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    Serial.print("✓ Fingerprint matched! ID #");
    Serial.println(finger.fingerID);
    Serial.print("  Confidence: ");
    Serial.println(finger.confidence);

    // LED hijau singkat
    setLED(0, 1, 0);
    delay(300);
    setLED(0, 0, 0);

    // Display attendance
    displayAttendance("ID: " + String(finger.fingerID), "Absensi OK!");
    delay(2000);
    displayWelcome();

    return finger.fingerID;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("✗ Fingerprint not found in database");

    // LED merah singkat
    setLED(1, 0, 0);
    delay(300);
    setLED(0, 0, 0);

    displayError("Fingerprint\nTidak Terdaftar!");
    delay(2000);
    displayWelcome();

    return -1;
  } else {
    Serial.println("✗ Error searching fingerprint");
    return -1;
  }
}

void publishAttendance(int fingerprintID) {
  StaticJsonDocument<256> doc;

  doc["fingerprint_id"] = fingerprintID;
  doc["timestamp"] = millis();
  doc["device_id"] = mqtt_client_id;

  char buffer[256];
  serializeJson(doc, buffer);

  Serial.println("\n📤 Publishing attendance data:");
  Serial.println(buffer);

  if (mqttClient.publish(topic_attendance, buffer)) {
    Serial.println("✓ Attendance published successfully!");
  } else {
    Serial.println("✗ Failed to publish attendance!");
  }
}

// ============================================
//          OLED DISPLAY FUNCTIONS
// ============================================

void setupOLED() {
  Serial.println("Initializing OLED display...");
  Serial.println("Using HARDWARE SPI mode (like Arduino code)");

  // Set pin modes explicitly
  pinMode(OLED_DC, OUTPUT);
  pinMode(OLED_CS, OUTPUT);
  pinMode(OLED_RES, OUTPUT);

  // Reset sequence
  digitalWrite(OLED_RES, HIGH);
  delay(10);
  digitalWrite(OLED_RES, LOW);
  delay(10);
  digitalWrite(OLED_RES, HIGH);
  delay(10);

  // Initialize Hardware SPI with custom pins
  SPI.begin(OLED_CLK, -1, OLED_MOSI, -1);
  SPI.setFrequency(8000000); // 8MHz - slower than default
  Serial.print("SPI initialized: CLK=");
  Serial.print(OLED_CLK);
  Serial.print(", MOSI=");
  Serial.print(OLED_MOSI);
  Serial.print(", DC=");
  Serial.print(OLED_DC);
  Serial.print(", CS=");
  Serial.print(OLED_CS);
  Serial.print(", RES=");
  Serial.println(OLED_RES);

  // Try multiple init attempts
  bool oledOK = false;
  for(int attempt = 0; attempt < 3; attempt++) {
    Serial.print("OLED init attempt ");
    Serial.println(attempt + 1);

    if(display.begin(SSD1306_SWITCHCAPVCC)) {
      oledOK = true;
      break;
    }
    delay(100);
  }

  if(!oledOK) {
    Serial.println("✗ SSD1306 allocation failed after 3 attempts!");
    Serial.println("⚠️ Continuing without OLED...");
    oledEnabled = false;
    return;
  }

  oledEnabled = true;
  Serial.println("✓ OLED display initialized!");

  // Aggressive display test
  for(int i = 0; i < 3; i++) {
    display.clearDisplay();
    display.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, SSD1306_WHITE);
    display.display();
    delay(200);

    display.clearDisplay();
    display.display();
    delay(200);
  }

  // Final display
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("ABSENSI");
  display.println("ESP32");
  display.setTextSize(1);
  display.println();
  display.println("System Ready!");
  display.display();

  Serial.println("✓ Welcome screen displayed");
  delay(2000);
}

void displayStatus(String title, String message, bool success) {
  if (!oledEnabled) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Title
  display.setCursor(0, 0);
  display.println(title);

  // Separator line
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

  // Status icon
  display.setCursor(0, 15);
  if (success) {
    display.println("[OK]");
  } else {
    display.println("[ERR]");
  }

  // Message (word wrap)
  display.setCursor(0, 30);
  display.setTextSize(1);
  display.println(message);

  display.display();
}

void displayWelcome() {
  if (!oledEnabled) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Title
  display.setCursor(10, 5);
  display.setTextSize(2);
  display.println("ABSENSI");

  display.setCursor(20, 25);
  display.setTextSize(1);
  display.println("FINGERPRINT");

  // Status
  display.setCursor(0, 45);
  display.println("WiFi:");
  display.setCursor(35, 45);
  if (WiFi.status() == WL_CONNECTED) {
    display.println("Connected");
  } else {
    display.println("Disconnected");
  }

  display.setCursor(0, 55);
  display.println("MQTT:");
  display.setCursor(35, 55);
  if (mqttClient.connected()) {
    display.println("Connected");
  } else {
    display.println("Disconnected");
  }

  display.display();
}

void displayEnrollment(String nama, int step) {
  if (!oledEnabled) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Title
  display.setCursor(0, 0);
  display.println("ENROLLMENT");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

  // Employee name (truncate if too long)
  display.setCursor(0, 15);
  display.setTextSize(1);
  String displayName = nama;
  if (displayName.length() > 21) {
    displayName = displayName.substring(0, 18) + "...";
  }
  display.println(displayName);

  // Step instructions
  display.setCursor(0, 30);
  display.setTextSize(1);
  if (step == 1) {
    display.println("Langkah 1/2");
    display.setCursor(0, 45);
    display.println("Tempelkan jari");
    display.setCursor(0, 55);
    display.println("pada sensor...");
  } else if (step == 2) {
    display.println("Langkah 2/2");
    display.setCursor(0, 45);
    display.println("Angkat jari,");
    display.setCursor(0, 55);
    display.println("tempel lagi...");
  } else if (step == 3) {
    display.println("Menyimpan...");
    display.setCursor(20, 45);
    display.setTextSize(2);
    display.println("WAIT");
  }

  display.display();
}

void displayAttendance(String nama, String status) {
  if (!oledEnabled) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Title
  display.setCursor(0, 0);
  display.println("ABSENSI");
  display.drawLine(0, 10, SCREEN_WIDTH, 10, SSD1306_WHITE);

  // Employee name (truncate if too long)
  display.setCursor(0, 15);
  display.setTextSize(1);
  String displayName = nama;
  if (displayName.length() > 21) {
    displayName = displayName.substring(0, 18) + "...";
  }
  display.println(displayName);

  // Status
  display.setCursor(0, 35);
  display.setTextSize(2);
  display.println(status);

  // Success checkmark
  display.setCursor(40, 55);
  display.setTextSize(1);
  display.println("[SUCCESS]");

  display.display();
}

void displayError(String message) {
  if (!oledEnabled) return;
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Error icon
  display.setCursor(50, 0);
  display.setTextSize(3);
  display.println("!");

  // Error message
  display.setCursor(0, 35);
  display.setTextSize(1);
  String displayMsg = message;
  if (displayMsg.length() > 21) {
    displayMsg = displayMsg.substring(0, 18) + "...";
  }
  display.println(displayMsg);

  display.setCursor(0, 55);
  display.println("Coba lagi...");

  display.display();
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
