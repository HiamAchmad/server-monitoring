/*
 * ============================================
 *  ESP32 FINGERPRINT ATTENDANCE (OLED ONLY)
 * ============================================
 */

#include <WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Fingerprint.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

/* ========== OLED CONFIG ========== */
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C   // ganti ke 0x3D jika OLED tidak muncul

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

/* ========== WIFI & MQTT CONFIG ========== */
const char* ssid = "ZTE_2.4G_R2Ush5";
const char* password = "XPUfSYAZ";

const char* mqtt_server = "47.84.67.102";
const int   mqtt_port = 1883;
const char* mqtt_client_id = "ESP32_Fingerprint";
const char* topic_attendance = "fingerprint/attendance";

/* ========== FINGERPRINT CONFIG ========== */
HardwareSerial mySerial(2); // UART2 ESP32
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

/* ========== GLOBAL OBJECT ========== */
WiFiClient espClient;
PubSubClient mqttClient(espClient);

/* ========== OLED HELPER FUNCTION ========== */
void oledPrint(String line1, String line2 = "") {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(0, 10);
  display.println(line1);

  if (line2 != "") {
    display.setCursor(0, 30);
    display.println(line2);
  }

  display.display();
}

/* ========== WIFI FUNCTION ========== */
void setupWiFi() {
  oledPrint("Connecting WiFi", ssid);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  oledPrint("WiFi Connected", WiFi.localIP().toString());
  delay(1500);
}

/* ========== MQTT FUNCTION ========== */
bool connectMQTT() {
  oledPrint("Connecting MQTT");
  if (mqttClient.connect(mqtt_client_id)) {
    oledPrint("MQTT Connected");
    delay(1000);
    return true;
  } else {
    oledPrint("MQTT Failed", "Retrying...");
    delay(2000);
    return false;
  }
}

/* ========== FINGERPRINT SETUP ========== */
void setupFingerprint() {
  oledPrint("Init Fingerprint");
  finger.begin(57600);

  if (finger.verifyPassword()) {
    oledPrint("Fingerprint Ready");
  } else {
    oledPrint("Fingerprint Error");
    while (true) delay(10);
  }

  delay(1000);
}

/* ========== FINGERPRINT SCAN ========== */
int getFingerprintID() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerSearch();
  if (p != FINGERPRINT_OK) return -1;

  return finger.fingerID;
}

/* ========== SETUP ========== */
void setup() {
  Serial.begin(115200);

  // I2C OLED
  Wire.begin(4, 15); // SDA, SCL

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not detected");
    while (true);
  }

  oledPrint("SISTEM ABSENSI", "Initializing...");
  delay(1500);

  setupWiFi();

  mqttClient.setServer(mqtt_server, mqtt_port);
  while (!connectMQTT());

  mySerial.begin(57600, SERIAL_8N1, 25, 26); // RX, TX Fingerprint
  setupFingerprint();

  oledPrint("Tempelkan Jari");
}

/* ========== LOOP ========== */
void loop() {
  if (!mqttClient.connected()) {
    connectMQTT();
  }
  mqttClient.loop();

  int fingerID = getFingerprintID();
  if (fingerID > 0) {
    oledPrint("Absensi Berhasil", "ID: " + String(fingerID));

    StaticJsonDocument<128> doc;
    doc["finger_id"] = fingerID;

    char payload[128];
    serializeJson(doc, payload);
    mqttClient.publish(topic_attendance, payload);

    delay(3000);
    oledPrint("Tempelkan Jari");
  }
}
