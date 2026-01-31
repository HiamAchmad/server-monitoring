/*
 * ============================================
 * ESP32 FINGERPRINT - FINAL FIX (COOLDOWN ADDED)
 * ============================================
 */

#include <Arduino.h> 
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>

// ===== WIFI & MQTT CONFIGURATION =====
const char* ssid = "a";
const char* password = "1234567891011121314151617";
const char* mqtt_server = "47.84.67.102"; 
const int mqtt_port = 1883;
const char* mqtt_client_id = "ESP32_Fingerprint";

// MQTT Topics
const char* topic_enroll_request = "fingerprint/enroll/request";
const char* topic_enroll_response = "fingerprint/enroll/response";
const char* topic_enroll_progress = "fingerprint/enroll/progress";
const char* topic_attendance = "fingerprint/attendance";

// ===== LED CONFIGURATION =====
#define USE_LED true 

// ===== LED PINS (FIXED) =====
#define LED_RED    12   // Merah (Gagal)
#define LED_GREEN  13   // Hijau (Sukses)

// ===== FINGERPRINT SENSOR =====
#define FP_RX_PIN 26  
#define FP_TX_PIN 25  
HardwareSerial mySerial(1);  
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
void blinkLED(int pin, int times, int delayMs);
void standbyLED(); 
int checkFingerprint();
void publishAttendance(int fingerprintID);
bool mqttReconnect();
void mqttCallback(char* topic, byte* payload, unsigned int length);
void handleEnrollmentRequest(JsonDocument& doc);
void startEnrollment(int id, int pegawai_id, String nip, String nama);
int getFingerprintEnroll(int id);
void publishEnrollmentResponse(bool success, String message);
void publishProgress(String message, int step, int totalSteps);

// ============================================
//               SETUP
// ============================================

void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("\n\n=================================");
  Serial.println("   FINGERPRINT SYSTEM (FULL)");
  Serial.println("=================================\n");

  setupLEDs();

  blinkLED(LED_RED, 2, 100);
  blinkLED(LED_GREEN, 2, 100);

  setupWiFi();       
  setupFingerprint();
  setupMQTT();

  Serial.println("\n✅ System Ready!");
  standbyLED();
}

// ============================================
//               MAIN LOOP
// ============================================

void loop() {
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

  if (enrollmentInProgress) {
    return;
  }

  // Attendance scanning
  int fingerprintID = checkFingerprint();
  if (fingerprintID > 0) {
    Serial.print("\n✓ Fingerprint matched! ID: ");
    Serial.println(fingerprintID);
    publishAttendance(fingerprintID);
    
    // Setelah sukses, delay sebentar lalu pastikan mati
    delay(2000); 
    standbyLED(); 
  }
}

// ============================================
//          HELPER FUNCTIONS
// ============================================

void setupLEDs() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
}

void standbyLED() {
  if (!USE_LED) return;
  digitalWrite(LED_RED, LOW);   // MATI
  digitalWrite(LED_GREEN, LOW); // MATI
}

void blinkLED(int pin, int times, int delayMs) {
  if (!USE_LED) return;
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);

  for (int i = 0; i < times; i++) {
    digitalWrite(pin, HIGH);
    delay(delayMs);
    digitalWrite(pin, LOW);
    delay(delayMs);
  }
}

void setupWiFi() {
  Serial.print("Connecting to WiFi: ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 30) {
    digitalWrite(LED_GREEN, !digitalRead(LED_GREEN)); 
    delay(250);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected!");
    blinkLED(LED_GREEN, 3, 200);
    standbyLED(); 
  } else {
    Serial.println("\n✗ WiFi connection failed!");
    digitalWrite(LED_RED, HIGH); 
    while(1) { delay(1000); }
  }
}

void setupFingerprint() {
  mySerial.begin(57600, SERIAL_8N1, 26, 25);
  delay(100);
  if (finger.verifyPassword()) {
    Serial.println("✓ Sensor Found");
  } else {
    Serial.println("✗ Sensor Not Found");
    while(1) { blinkLED(LED_RED, 5, 100); }
  }
}

void setupMQTT() {
  mqttClient.setServer(mqtt_server, mqtt_port);
  mqttClient.setBufferSize(512);
  mqttClient.setCallback(mqttCallback);
  if (mqttReconnect()) {
    Serial.println("✓ MQTT Ready");
  }
}

bool mqttReconnect() {
  if (mqttClient.connect(mqtt_client_id)) {
    mqttClient.subscribe(topic_enroll_request);
    blinkLED(LED_GREEN, 1, 200);
    standbyLED();
    return true;
  } else {
    return false;
  }
}

void mqttCallback(char* topic, byte* payload, unsigned int length) {
  char message[512];
  if (length >= 512) length = 511;
  memcpy(message, payload, length);
  message[length] = '\0';
  
  JsonDocument doc;
  deserializeJson(doc, message);
  
  if (String(topic) == topic_enroll_request) {
    handleEnrollmentRequest(doc);
    standbyLED(); 
  }
}

// ============================================
//       LOGIKA ABSENSI
// ============================================

int checkFingerprint() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return 0;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return 0;

  p = finger.fingerSearch();
  if (p == FINGERPRINT_OK) {
    return finger.fingerID;
  } else {
    Serial.println("✗ Sidik jari tidak dikenal");
    blinkLED(LED_RED, 3, 200); 
    standbyLED(); 
    return 0;
  }
}

void publishAttendance(int fingerprintID) {
  JsonDocument doc;
  doc["fingerprint_id"] = fingerprintID;
  doc["confidence"] = finger.confidence;
  doc["timestamp"] = millis();

  char buffer[256];
  serializeJson(doc, buffer);

  if (mqttClient.publish(topic_attendance, buffer)) {
    Serial.println("✓ Attendance Sent");
    blinkLED(LED_GREEN, 3, 200); 
  } else {
    blinkLED(LED_RED, 3, 200);
  }
  standbyLED();
}

// ============================================
//       ENROLLMENT (REVISI DISINI)
// ============================================

void handleEnrollmentRequest(JsonDocument& doc) {
    String command = doc["command"] | "";
    if (command == "ENROLL") {
        int fid = doc["fingerprint_id"];
        int pid = doc["pegawai_id"];
        String nip = doc["nip"];
        String nama = doc["nama"];
        startEnrollment(fid, pid, nip, nama);
    }
}

// ↓↓↓ FUNGSI INI YANG DIREVISI ↓↓↓
void startEnrollment(int id, int pegawai_id, String nip, String nama) {
    enrollmentInProgress = true;
    currentEnrollID = id;
    currentPegawaiID = pegawai_id;
    currentNIP = nip;
    currentNama = nama;

    standbyLED(); 
    
    int result = getFingerprintEnroll(id);
    
    if (result == FINGERPRINT_OK) {
        publishEnrollmentResponse(true, "Success");
        blinkLED(LED_GREEN, 3, 500); 

        // ==========================================================
        // [REVISI DIMULAI DISINI] 
        // Mencegah absensi otomatis setelah enroll
        // ==========================================================
        Serial.println(">>> ENROLL SUKSES. Menunggu jari diangkat...");
        
        // 1. Jeda sebentar 2 detik
        delay(2000); 

        // 2. Loop penahan: Program akan STUCK disini sampai jari LEPAS
        while (finger.getImage() != FINGERPRINT_NOFINGER) {
            delay(100); // Cek setiap 100ms
        }
        
        Serial.println(">>> Jari diangkat. Sensor bersih. Kembali ke loop.");
        // ==========================================================
        // [REVISI SELESAI]
        // ==========================================================

    } else {
        publishEnrollmentResponse(false, "Failed");
        blinkLED(LED_RED, 3, 500); 
    }
    enrollmentInProgress = false;
    standbyLED(); 
}
// ↑↑↑ BATAS REVISI ↑↑↑

int getFingerprintEnroll(int id) {
  int p = -1;
  Serial.println("Waiting for finger...");
  publishProgress("📍 Langkah 1/4: Tempelkan jari...", 1, 4);

  unsigned long startTime = millis();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if ((millis() - startTime) % 1000 < 500) {
       digitalWrite(LED_GREEN, HIGH);
    } else {
       digitalWrite(LED_GREEN, LOW);
    }
    if (millis() - startTime > 30000) { 
      publishProgress("⏱️ Timeout!", 0, 4);
      return FINGERPRINT_TIMEOUT;
    }
    if (p == FINGERPRINT_OK) {
        digitalWrite(LED_GREEN, HIGH); 
        publishProgress("✅ Gambar 1 OK", 1, 4);
    }
  }

  p = finger.image2Tz(1);
  if (p != FINGERPRINT_OK) return p;

  Serial.println("Remove finger...");
  publishProgress("📍 Langkah 2/4: Angkat jari...", 2, 4);
  digitalWrite(LED_GREEN, LOW); 
  delay(2000);
  
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }

  Serial.println("Place SAME finger again...");
  publishProgress("📍 Langkah 3/4: Tempelkan jari lagi...", 3, 4);

  p = -1;
  startTime = millis();
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    if ((millis() - startTime) % 1000 < 500) {
       digitalWrite(LED_GREEN, HIGH);
    } else {
       digitalWrite(LED_GREEN, LOW);
    }
    if (millis() - startTime > 30000) return FINGERPRINT_TIMEOUT;
    if (p == FINGERPRINT_OK) {
        digitalWrite(LED_GREEN, HIGH);
        publishProgress("✅ Gambar 2 OK", 3, 4);
    }
  }

  p = finger.image2Tz(2);
  if (p != FINGERPRINT_OK) return p;

  publishProgress("📍 Langkah 4/4: Menyimpan...", 4, 4);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    publishProgress("❌ Sidik jari tidak cocok!", 0, 4);
    return p;
  } else {
    return p;
  }

  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    publishProgress("✅ Berhasil disimpan!", 4, 4);
    return FINGERPRINT_OK;
  } else {
    publishProgress("❌ Gagal simpan flash!", 0, 4);
    return p;
  }
}

void publishEnrollmentResponse(bool success, String message) {
    JsonDocument doc;
    doc["success"] = success;
    doc["message"] = message;
    doc["fingerprint_id"] = currentEnrollID;
    doc["pegawai_id"] = currentPegawaiID;
    char buffer[256];
    serializeJson(doc, buffer);
    mqttClient.publish(topic_enroll_response, buffer);
}

void publishProgress(String message, int step, int totalSteps) {
  JsonDocument doc;
  doc["pegawai_id"] = currentPegawaiID;
  doc["fingerprint_id"] = currentEnrollID;
  doc["message"] = message;
  doc["step"] = step;
  doc["total_steps"] = totalSteps;
  char buffer[256];
  serializeJson(doc, buffer);
  mqttClient.publish(topic_enroll_progress, buffer);
}