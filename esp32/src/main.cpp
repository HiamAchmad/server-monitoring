/*
 * ============================================
 * ESP32 FINGERPRINT - STANDBY OFF MODE
 * ============================================
 */

#include <Arduino.h> // Wajib untuk PlatformIO
#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>

// ===== WIFI & MQTT CONFIGURATION =====
const char* ssid = "Kari ayam";
const char* password = "023311129";
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

// ===== LED PINS =====
// Pastikan posisi kabel sesuai:
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
void standbyLED(); // Fungsi untuk mematikan semua LED
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
  Serial.println("   FINGERPRINT SYSTEM (SILENT)");
  Serial.println("=================================\n");

  setupLEDs();

  // Test Blink Awal (Cek nyawa LED)
  blinkLED(LED_RED, 1, 200);
  blinkLED(LED_GREEN, 1, 200);

  setupWiFi();       
  setupFingerprint();
  setupMQTT();

  Serial.println("\n✅ System Ready!");
  
  // Pastikan posisi terakhir adalah MATI (Standby)
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
    delay(2000); // Delay agar tidak spam
    standbyLED(); // Kembali mati
  }
}

// ============================================
//          HELPER FUNCTIONS
// ============================================

void setupLEDs() {
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  // Default mati
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
}

// FUNGSI INI YANG DIUBAH:
// Standby = Semua Mati
void standbyLED() {
  if (!USE_LED) return;
  digitalWrite(LED_RED, LOW);   // Merah Mati
  digitalWrite(LED_GREEN, LOW); // Hijau Mati (Standby Silent)
}

void blinkLED(int pin, int times, int delayMs) {
  if (!USE_LED) return;
  
  // Matikan dulu biar blink terlihat jelas
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
    // Blink Hijau pelan saat connecting
    digitalWrite(LED_GREEN, !digitalRead(LED_GREEN)); 
    delay(250);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✓ WiFi connected!");
    Serial.println(WiFi.localIP());
    
    // Blink Hijau 3x cepat tanda sukses connect
    blinkLED(LED_GREEN, 3, 200);
    
    // Lalu MATI (Standby)
    standbyLED(); 
  } else {
    Serial.println("\n✗ WiFi connection failed!");
    digitalWrite(LED_GREEN, LOW);
    digitalWrite(LED_RED, HIGH); // Merah nyala terus jika gagal WiFi
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
    // Blink Hijau sekali tanda Reconnected
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
    
    // GAGAL: Blink Merah
    blinkLED(LED_RED, 3, 200); 
    
    // Kembali Mati
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
    // SUKSES: Blink Hijau 2x
    blinkLED(LED_GREEN, 2, 300);
  } else {
    // Gagal kirim MQTT -> Blink Merah
    blinkLED(LED_RED, 3, 200);
  }
  // Kembali Mati
  standbyLED();
}

// ============================================
//       ENROLLMENT
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

void startEnrollment(int id, int pegawai_id, String nip, String nama) {
    enrollmentInProgress = true;
    currentEnrollID = id;
    
    // Mulai process, pastikan mati dulu
    standbyLED();
    
    int result = getFingerprintEnroll(id);
    
    if (result == FINGERPRINT_OK) {
        publishEnrollmentResponse(true, "Success");
        blinkLED(LED_GREEN, 3, 500); // Sukses Enroll
    } else {
        publishEnrollmentResponse(false, "Failed");
        blinkLED(LED_RED, 3, 500); // Gagal Enroll
    }
    enrollmentInProgress = false;
    standbyLED(); // KEMBALI MATI
}

int getFingerprintEnroll(int id) {
    // Logika Enrollment Sederhana
    int p = -1;
    unsigned long start = millis();
    
    // Step 1
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        if (p == FINGERPRINT_OK) break;
        // Blink Hijau sangat cepat tanda sedang scan mode
        digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
        delay(100);
        if (millis() - start > 20000) return FINGERPRINT_TIMEOUT;
    }
    
    finger.image2Tz(1);
    blinkLED(LED_GREEN, 1, 500); // Tanda berhasil ambil gambar 1
    delay(2000);
    
    // Step 2
    p = -1;
    start = millis();
    while (p != FINGERPRINT_OK) {
        p = finger.getImage();
        if (p == FINGERPRINT_OK) break;
        digitalWrite(LED_GREEN, !digitalRead(LED_GREEN));
        delay(100);
        if (millis() - start > 20000) return FINGERPRINT_TIMEOUT;
    }
    
    finger.image2Tz(2);
    p = finger.createModel();
    if (p == FINGERPRINT_OK) {
        p = finger.storeModel(id);
        if (p == FINGERPRINT_OK) return FINGERPRINT_OK;
    }
    return -1;
}

void publishEnrollmentResponse(bool success, String message) {
    JsonDocument doc;
    doc["success"] = success;
    doc["message"] = message;
    char buffer[256];
    serializeJson(doc, buffer);
    mqttClient.publish(topic_enroll_response, buffer);
}

void publishProgress(String message, int step, int totalSteps) {
   // Sama seperti sebelumnya
}