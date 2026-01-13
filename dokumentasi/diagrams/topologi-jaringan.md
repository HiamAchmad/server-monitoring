# Topologi Jaringan Sistem Absensi IoT

## 1. Topologi Jaringan Fisik

```
                                    ┌─────────────────────────────────────────────────────────────┐
                                    │                      CLOUD LAYER                            │
                                    │  ┌─────────────────────────────────────────────────────┐   │
                                    │  │              OwnCloud Server (Docker)                │   │
                                    │  │                   Port: 8080                         │   │
                                    │  │              Protocol: HTTP/WebDAV                   │   │
                                    │  └──────────────────────┬──────────────────────────────┘   │
                                    └──────────────────────────┼──────────────────────────────────┘
                                                               │ WebDAV (HTTP PUT/GET)
                                                               │ Basic Authentication
                                    ┌──────────────────────────┼──────────────────────────────────┐
                                    │                          │                                  │
                                    │     ┌────────────────────▼────────────────────┐             │
                                    │     │           APPLICATION SERVER            │             │
                                    │     │  ┌────────────────────────────────────┐ │             │
                                    │     │  │     Node.js + Express.js           │ │             │
                                    │     │  │     ┌──────────┐  ┌─────────────┐  │ │             │
                                    │     │  │     │HTTP Server│  │ MQTT Broker │  │ │             │
                                    │     │  │     │Port: 3000 │  │ Port: 1883  │  │ │             │
                                    │     │  │     │  (REST)   │  │  (Aedes)    │  │ │             │
                                    │     │  │     └──────────┘  └─────────────┘  │ │             │
                                    │     │  │           │              │         │ │             │
                                    │     │  │     ┌─────▼──────────────▼─────┐   │ │             │
                                    │     │  │     │      Socket.IO           │   │ │             │
                                    │     │  │     │  (WebSocket Real-time)   │   │ │             │
                                    │     │  │     └──────────────────────────┘   │ │             │
                                    │     │  └────────────────────────────────────┘ │             │
                                    │     └────────────────────┬────────────────────┘             │
                                    │                          │ TCP Port: 5432                   │
                                    │     ┌────────────────────▼────────────────────┐             │
                                    │     │         PostgreSQL Database             │             │
                                    │     │            (Data Storage)               │             │
                                    │     └─────────────────────────────────────────┘             │
                                    │                     SERVER LAYER                            │
                                    └──────────────────────────┬──────────────────────────────────┘
                                                               │
                            ┌──────────────────────────────────┼──────────────────────────────────┐
                            │                                  │                                  │
                            │             ┌────────────────────▼────────────────────┐             │
                            │             │          WIRELESS ACCESS POINT          │             │
                            │             │           (WiFi Router 2.4GHz)           │             │
                            │             │          SSID: Office_Network            │             │
                            │             └───────┬─────────────────────────┬───────┘             │
                            │                     │                         │                     │
                            │         ┌───────────┴───────────┐   ┌────────┴─────────┐           │
                            │         │    WiFi Connection    │   │  WiFi Connection │           │
                            │         │      (TCP/MQTT)       │   │   (HTTP/WS)      │           │
                            │         │                       │   │                  │           │
                            │   ┌─────▼─────────────────┐   ┌─▼───▼────────────────┐             │
                            │   │    IoT DEVICE         │   │   CLIENT DEVICES     │             │
                            │   │  ┌───────────────┐    │   │  ┌────────────────┐  │             │
                            │   │  │    ESP32      │    │   │  │ Admin Browser  │  │             │
                            │   │  │  DevKit V1   │    │   │  │  (Dashboard)   │  │             │
                            │   │  └───────┬───────┘    │   │  └────────────────┘  │             │
                            │   │          │            │   │  ┌────────────────┐  │             │
                            │   │  ┌───────▼───────┐    │   │  │ User Browser   │  │             │
                            │   │  │  Fingerprint  │    │   │  │  (Portal)      │  │             │
                            │   │  │ Sensor AS608  │    │   │  └────────────────┘  │             │
                            │   │  └───────────────┘    │   │  ┌────────────────┐  │             │
                            │   │  ┌───────────────┐    │   │  │ Mobile Device  │  │             │
                            │   │  │  OLED Display │    │   │  │    (PWA)       │  │             │
                            │   │  │  SPI 0.96"    │    │   │  └────────────────┘  │             │
                            │   │  └───────────────┘    │   └──────────────────────┘             │
                            │   └───────────────────────┘                                        │
                            │                          DEVICE LAYER                              │
                            └─────────────────────────────────────────────────────────────────────┘
```

---

## 2. Topologi Jaringan Logis

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                    LOGICAL NETWORK TOPOLOGY                                      │
├─────────────────────────────────────────────────────────────────────────────────────────────────┤
│                                                                                                 │
│   ┌─────────────┐          MQTT Protocol           ┌──────────────────────────────────────┐    │
│   │   ESP32     │◄────────────────────────────────►│         APPLICATION SERVER           │    │
│   │  + Sensor   │      Port: 1883 (TCP)            │                                      │    │
│   │  + OLED     │      QoS Level: 1                │  ┌──────────────┐  ┌──────────────┐  │    │
│   └─────────────┘      Topics:                     │  │  HTTP Server │  │ MQTT Broker  │  │    │
│         │              - fingerprint/attendance    │  │  (Express)   │  │   (Aedes)    │  │    │
│         │              - fingerprint/enroll/*      │  │  Port: 3000  │  │  Port: 1883  │  │    │
│         │              - fingerprint/command       │  └──────┬───────┘  └──────┬───────┘  │    │
│         │                                          │         │                 │          │    │
│         │                                          │         │    Socket.IO    │          │    │
│         │                                          │         │◄───────────────►│          │    │
│         │                                          │         │   (Internal)    │          │    │
│   ┌─────▼─────┐                                    │  ┌──────▼─────────────────▼───────┐  │    │
│   │  WiFi AP  │                                    │  │      Business Logic Layer      │  │    │
│   │  Router   │                                    │  │   - Authentication Service      │  │    │
│   └───────────┘                                    │  │   - Attendance Processing       │  │    │
│                                                    │  │   - Payroll Calculation         │  │    │
│                                                    │  └──────────────┬─────────────────┘  │    │
│                                                    │                 │                    │    │
│                                                    │         ┌───────▼───────┐            │    │
│   ┌─────────────┐      HTTP/WebSocket              │         │  PostgreSQL   │            │    │
│   │   Browser   │◄────────────────────────────────►│         │  Port: 5432   │            │    │
│   │  (Client)   │      Port: 3000                  │         └───────────────┘            │    │
│   └─────────────┘      Protocol: WS/HTTP           └──────────────────┬───────────────────┘    │
│                                                                       │                        │
│                                                                       │ HTTP/WebDAV            │
│                                                              ┌────────▼────────┐               │
│                                                              │    OwnCloud     │               │
│                                                              │   Port: 8080    │               │
│                                                              │   (Docker)      │               │
│                                                              └─────────────────┘               │
│                                                                                                 │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘
```

---

## 3. Diagram Alur Komunikasi Data

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                              DATA COMMUNICATION FLOW                                             │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

    SCAN FINGERPRINT                                                              UPDATE DASHBOARD
          │                                                                              ▲
          ▼                                                                              │
┌──────────────────┐    ┌──────────────┐    ┌──────────────┐    ┌──────────────┐    ┌────┴─────┐
│                  │    │              │    │              │    │              │    │          │
│   FINGERPRINT    │───►│    ESP32     │───►│  WiFi + MQTT │───►│   SERVER     │───►│ BROWSER  │
│   SENSOR AS608   │    │   DevKit     │    │  Port: 1883  │    │  Port: 3000  │    │ (Client) │
│                  │    │              │    │              │    │              │    │          │
└──────────────────┘    └──────────────┘    └──────────────┘    └──────┬───────┘    └──────────┘
                                                                       │
                        ┌──────────────────────────────────────────────┼──────────────────────────┐
                        │                                              │                          │
                        ▼                                              ▼                          ▼
                ┌──────────────┐                              ┌──────────────┐            ┌──────────────┐
                │     OLED     │                              │  PostgreSQL  │            │   OwnCloud   │
                │   DISPLAY    │                              │   Database   │            │   (Backup)   │
                │   (Feedback) │                              │  Port: 5432  │            │  Port: 8080  │
                └──────────────┘                              └──────────────┘            └──────────────┘


    ═══════════════════════════════════════════════════════════════════════════════════════════════
    PROTOKOL YANG DIGUNAKAN:
    ═══════════════════════════════════════════════════════════════════════════════════════════════

    ┌────────────────────┬────────────────────┬────────────────────┬────────────────────┐
    │   LAYER            │   PROTOKOL         │   PORT             │   FUNGSI           │
    ├────────────────────┼────────────────────┼────────────────────┼────────────────────┤
    │   Device → Server  │   MQTT             │   1883 (TCP)       │   IoT Data         │
    │   Server → Client  │   WebSocket        │   3000 (WS)        │   Real-time        │
    │   Client → Server  │   HTTP/REST        │   3000 (TCP)       │   API Request      │
    │   Server → Cloud   │   WebDAV (HTTP)    │   8080 (TCP)       │   Backup Data      │
    │   Server → DB      │   PostgreSQL       │   5432 (TCP)       │   Data Storage     │
    └────────────────────┴────────────────────┴────────────────────┴────────────────────┘
```

---

## 4. Arsitektur Three-Tier Network

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                   THREE-TIER ARCHITECTURE                                        │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

╔═════════════════════════════════════════════════════════════════════════════════════════════════╗
║                                        TIER 3: CLOUD LAYER                                      ║
║  ┌───────────────────────────────────────────────────────────────────────────────────────────┐  ║
║  │                                    OwnCloud Server                                        │  ║
║  │                          ┌─────────────────────────────────┐                              │  ║
║  │                          │  WebDAV Endpoint                │                              │  ║
║  │                          │  /remote.php/dav/files/admin/   │                              │  ║
║  │                          │  Protocol: HTTP (Port 8080)     │                              │  ║
║  │                          │  Auth: Basic Authentication     │                              │  ║
║  │                          └─────────────────────────────────┘                              │  ║
║  └───────────────────────────────────────────────────────────────────────────────────────────┘  ║
╚══════════════════════════════════════════════╦══════════════════════════════════════════════════╝
                                               ║ WebDAV Protocol
                                               ║ HTTP PUT/GET
╔══════════════════════════════════════════════╩══════════════════════════════════════════════════╗
║                                   TIER 2: APPLICATION LAYER                                     ║
║  ┌───────────────────────────────────────────────────────────────────────────────────────────┐  ║
║  │                               Application Server (Node.js)                                │  ║
║  │  ┌─────────────────┐    ┌─────────────────┐    ┌─────────────────┐    ┌───────────────┐   │  ║
║  │  │   HTTP Server   │    │   MQTT Broker   │    │   Socket.IO     │    │   PostgreSQL  │   │  ║
║  │  │   (Express.js)  │    │    (Aedes)      │    │   (Real-time)   │    │   (Database)  │   │  ║
║  │  │   Port: 3000    │    │   Port: 1883    │    │   Port: 3000    │    │   Port: 5432  │   │  ║
║  │  └────────┬────────┘    └────────┬────────┘    └────────┬────────┘    └───────┬───────┘   │  ║
║  │           │                      │                      │                     │           │  ║
║  │           └──────────────────────┴──────────────────────┴─────────────────────┘           │  ║
║  │                                    Internal Communication                                 │  ║
║  └───────────────────────────────────────────────────────────────────────────────────────────┘  ║
╚══════════════════════════════════════════════╦══════════════════════════════════════════════════╝
                                               ║ WiFi 2.4GHz (IEEE 802.11 b/g/n)
                                               ║ MQTT + HTTP/WebSocket
╔══════════════════════════════════════════════╩══════════════════════════════════════════════════╗
║                                     TIER 1: DEVICE LAYER                                        ║
║  ┌─────────────────────────────────┐              ┌─────────────────────────────────────────┐   ║
║  │       IoT Device (ESP32)        │              │           Client Devices                │   ║
║  │  ┌───────────────────────────┐  │              │  ┌─────────────────────────────────────┐│   ║
║  │  │  Fingerprint Sensor AS608 │  │              │  │  Desktop Browser (Admin Dashboard) ││   ║
║  │  │  - Capture fingerprint    │  │              │  │  - HTTP/REST API                   ││   ║
║  │  │  - Template matching      │  │              │  │  - WebSocket Real-time             ││   ║
║  │  └───────────────────────────┘  │              │  └─────────────────────────────────────┘│   ║
║  │  ┌───────────────────────────┐  │              │  ┌─────────────────────────────────────┐│   ║
║  │  │  OLED Display SPI 0.96"   │  │              │  │  Mobile Browser (PWA)              ││   ║
║  │  │  - Visual feedback        │  │              │  │  - Responsive Web App              ││   ║
║  │  │  - Status display         │  │              │  │  - Push Notification               ││   ║
║  │  └───────────────────────────┘  │              │  └─────────────────────────────────────┘│   ║
║  │  ┌───────────────────────────┐  │              └─────────────────────────────────────────┘   ║
║  │  │  WiFi Module (Built-in)   │  │                                                            ║
║  │  │  - 2.4GHz 802.11 b/g/n    │  │                                                            ║
║  │  │  - MQTT Client            │  │                                                            ║
║  │  └───────────────────────────┘  │                                                            ║
║  └─────────────────────────────────┘                                                            ║
╚═════════════════════════════════════════════════════════════════════════════════════════════════╝
```

---

## 5. Detail Port dan Protokol

| Layer | Komponen | Port | Protokol | Transport | Fungsi |
|-------|----------|------|----------|-----------|--------|
| Device | ESP32 → Server | 1883 | MQTT | TCP | Publish data absensi |
| Device | Server → ESP32 | 1883 | MQTT | TCP | Subscribe response |
| Application | Client → Server | 3000 | HTTP/REST | TCP | API request |
| Application | Server → Client | 3000 | WebSocket | TCP | Real-time update |
| Application | Server → Database | 5432 | PostgreSQL | TCP | Query database |
| Cloud | Server → OwnCloud | 8080 | WebDAV | HTTP | Backup file |

---

## 5.1 Detail MQTT Topics

| Topic | Direction | QoS | Payload | Fungsi |
|-------|-----------|-----|---------|--------|
| `fingerprint/attendance` | ESP32 → Server | 1 | `{fingerprint_id, timestamp}` | Data scan absensi |
| `fingerprint/enroll/request` | Server → ESP32 | 1 | `{pegawai_id, fingerprint_id}` | Perintah mulai enrollment |
| `fingerprint/enroll/response` | ESP32 → Server | 1 | `{success, fingerprint_id, message}` | Hasil enrollment |
| `fingerprint/enroll/progress` | ESP32 → Server | 0 | `{step, message}` | Progress enrollment real-time |
| `fingerprint/command` | Server → ESP32 | 1 | `{command, params}` | Perintah umum (reset, dll) |

**MQTT Communication Flow:**

```
┌──────────────────────────────────────────────────────────────────────────────────────────┐
│                           MQTT PUBLISH/SUBSCRIBE FLOW                                    │
└──────────────────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────┐                                              ┌─────────────────┐
    │   ESP32     │                                              │  NODE.JS SERVER │
    │  (Client)   │                                              │  (MQTT Broker)  │
    └──────┬──────┘                                              └────────┬────────┘
           │                                                              │
           │  ──────── PUBLISH: fingerprint/attendance ────────────────►  │
           │           {fingerprint_id: 5, timestamp: "..."}              │
           │                                                              │
           │  ◄─────── PUBLISH: fingerprint/enroll/request ─────────────  │
           │           {pegawai_id: 10, fingerprint_id: 6}                │
           │                                                              │
           │  ──────── PUBLISH: fingerprint/enroll/progress ───────────►  │
           │           {step: 1, message: "Place finger..."}              │
           │                                                              │
           │  ──────── PUBLISH: fingerprint/enroll/response ───────────►  │
           │           {success: true, fingerprint_id: 6}                 │
           │                                                              │
           │  ◄─────── PUBLISH: fingerprint/command ────────────────────  │
           │           {command: "reset", params: {}}                     │
           │                                                              │
    ┌──────▼──────┐                                              ┌────────▼────────┐
    │   ESP32     │                                              │  NODE.JS SERVER │
    └─────────────┘                                              └─────────────────┘
```

---

## 6. Network Security Architecture

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                   NETWORK SECURITY LAYERS                                        │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

                              ┌─────────────────────────────┐
                              │      INTERNET/EXTERNAL      │
                              └──────────────┬──────────────┘
                                             │
                              ┌──────────────▼──────────────┐
                              │         FIREWALL            │
                              │   - Port Filtering          │
                              │   - Rate Limiting           │
                              │   - DDoS Protection         │
                              └──────────────┬──────────────┘
                                             │
          ┌──────────────────────────────────┼──────────────────────────────────┐
          │                                  │                                  │
          │                   ┌──────────────▼──────────────┐                   │
          │                   │       REVERSE PROXY         │                   │
          │                   │   (Nginx - Optional)        │                   │
          │                   │   - SSL Termination         │                   │
          │                   │   - Load Balancing          │                   │
          │                   └──────────────┬──────────────┘                   │
          │                                  │                                  │
          │    ┌─────────────────────────────┼─────────────────────────────┐    │
          │    │                             │                             │    │
          │    │  ┌──────────────────────────▼──────────────────────────┐  │    │
          │    │  │                  APPLICATION                        │  │    │
          │    │  │  ┌────────────────┐  ┌────────────────────────────┐ │  │    │
          │    │  │  │  HTTP Server   │  │   Security Middleware      │ │  │    │
          │    │  │  │  + MQTT Broker │  │   - CSRF Protection        │ │  │    │
          │    │  │  │                │  │   - Session Management     │ │  │    │
          │    │  │  │                │  │   - Input Validation       │ │  │    │
          │    │  │  │                │  │   - bcrypt Password Hash   │ │  │    │
          │    │  │  └────────────────┘  └────────────────────────────┘ │  │    │
          │    │  └─────────────────────────────────────────────────────┘  │    │
          │    │                     INTERNAL NETWORK                      │    │
          │    └───────────────────────────────────────────────────────────┘    │
          │                          DMZ (Optional)                             │
          └─────────────────────────────────────────────────────────────────────┘
```

---

## 7. Spesifikasi Jaringan

### Hardware Requirements

| Komponen | Spesifikasi | Keterangan |
|----------|-------------|------------|
| Server | RAM 4GB, CPU 2 Core | Minimum requirement |
| WiFi Router | 2.4GHz 802.11 b/g/n | Untuk koneksi ESP32 |
| ESP32 | DevKit V1, WiFi built-in | IoT device |
| Switch | Gigabit Ethernet | Optional untuk LAN |

### Network Requirements

| Parameter | Nilai | Keterangan |
|-----------|-------|------------|
| Bandwidth | Min 1 Mbps | Untuk MQTT + WebSocket |
| Latency | < 100ms | Jaringan lokal |
| WiFi Coverage | Radius 30m | Area fingerprint scanner |
| IP Address | Static/DHCP | Server: Static recommended |

### Protocol Stack

```
┌─────────────────────────────────┐
│        APPLICATION LAYER        │
│   MQTT │ HTTP │ WebSocket │ WD  │
├─────────────────────────────────┤
│        TRANSPORT LAYER          │
│              TCP                │
├─────────────────────────────────┤
│         NETWORK LAYER           │
│            IPv4                 │
├─────────────────────────────────┤
│        DATA LINK LAYER          │
│     Ethernet │ WiFi 802.11      │
├─────────────────────────────────┤
│        PHYSICAL LAYER           │
│    CAT5e/CAT6 │ Radio 2.4GHz    │
└─────────────────────────────────┘
```

---

## 8. Detail Protokol Komunikasi

### 8.1 MQTT Protocol (Publish/Subscribe)

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                              MQTT PUBLISH/SUBSCRIBE ARCHITECTURE                                 │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

                                    ┌─────────────────────────┐
                                    │      MQTT BROKER        │
                                    │        (Aedes)          │
                                    │      Port: 1883         │
                                    └───────────┬─────────────┘
                                                │
                    ┌───────────────────────────┼───────────────────────────┐
                    │                           │                           │
            ┌───────▼───────┐           ┌───────▼───────┐           ┌───────▼───────┐
            │   PUBLISHER   │           │   PUBLISHER   │           │  SUBSCRIBER   │
            │    ESP32      │           │    Server     │           │    Server     │
            └───────┬───────┘           └───────┬───────┘           └───────┬───────┘
                    │                           │                           │
                    │ PUBLISH                   │ PUBLISH                   │ SUBSCRIBE
                    │                           │                           │
    ┌───────────────▼───────────────┐   ┌───────▼───────────────┐   ┌───────▼───────────────────┐
    │ fingerprint/attendance        │   │ fingerprint/enroll/   │   │ fingerprint/attendance    │
    │ fingerprint/enroll/response   │   │ request               │   │ fingerprint/enroll/+      │
    │ fingerprint/enroll/progress   │   │ fingerprint/command   │   │ fingerprint/command       │
    └───────────────────────────────┘   └───────────────────────┘   └───────────────────────────┘
```

**Karakteristik MQTT:**
| Parameter | Nilai | Keterangan |
|-----------|-------|------------|
| Broker | Aedes (Node.js) | Lightweight MQTT broker |
| Port | 1883 | Standard MQTT port |
| QoS Level | 0, 1 | At most once, At least once |
| Keep Alive | 60 detik | Interval ping client |
| Clean Session | true | Fresh session setiap koneksi |
| Payload Format | JSON | Human-readable, easy parsing |

### 8.2 WebSocket Protocol (Socket.IO)

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                              WEBSOCKET BIDIRECTIONAL COMMUNICATION                               │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────┐                                          ┌─────────────────┐
    │   WEB BROWSER   │                                          │     SERVER      │
    │    (Client)     │                                          │   (Socket.IO)   │
    └────────┬────────┘                                          └────────┬────────┘
             │                                                            │
             │  ═══════ HTTP Upgrade Request (Handshake) ═══════════════► │
             │                                                            │
             │  ◄═══════ HTTP 101 Switching Protocols ══════════════════  │
             │                                                            │
             │  ◄═══════════════ WebSocket Connection ══════════════════► │
             │                     (Full Duplex)                          │
             │                                                            │
             │  ──────── emit('attendance_update', data) ──────────────►  │
             │                                                            │
             │  ◄─────── emit('new_attendance', data) ─────────────────   │
             │                                                            │
             │  ◄─────── emit('enrollment_progress', data) ────────────   │
             │                                                            │
             │  ◄─────── emit('enrollment_success', data) ─────────────   │
             │                                                            │
    └────────▼────────┘                                          └────────▼────────┘
```

**Socket.IO Events:**
| Event | Direction | Data | Fungsi |
|-------|-----------|------|--------|
| `connection` | Client → Server | - | Koneksi baru |
| `initial_data` | Server → Client | `[attendance]` | Data awal |
| `new_attendance` | Server → Client | `{pegawai, waktu, status}` | Absensi baru real-time |
| `enrollment_progress` | Server → Client | `{step, message}` | Progress enrollment |
| `enrollment_success` | Server → Client | `{pegawai_id, fp_id}` | Enrollment berhasil |
| `enrollment_error` | Server → Client | `{message}` | Enrollment gagal |

### 8.3 HTTP/REST Protocol

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                    REST API ARCHITECTURE                                         │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────┐         HTTP Request          ┌─────────────────────────────────────┐
    │   CLIENT    │ ────────────────────────────► │            EXPRESS.JS               │
    │  (Browser)  │                               │           REST API                  │
    └─────────────┘                               │                                     │
                                                  │  ┌─────────────────────────────────┐│
         Request:                                 │  │         ENDPOINTS               ││
         GET /api/pegawai                         │  │                                 ││
         POST /api/absensi                        │  │  GET    /api/pegawai            ││
         PUT /api/pegawai/:id                     │  │  POST   /api/pegawai            ││
         DELETE /api/pegawai/:id                  │  │  PUT    /api/pegawai/:id        ││
                                                  │  │  DELETE /api/pegawai/:id        ││
    ┌─────────────┐         HTTP Response         │  │  GET    /api/absensi            ││
    │   CLIENT    │ ◄──────────────────────────── │  │  POST   /api/login              ││
    │  (Browser)  │                               │  │  POST   /api/backup-owncloud    ││
    └─────────────┘                               │  │  GET    /api/mqtt-status        ││
                                                  │  └─────────────────────────────────┘│
         Response:                                └─────────────────────────────────────┘
         {success: true, data: [...]}
         Content-Type: application/json
```

### 8.4 WebDAV Protocol (OwnCloud)

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                    WEBDAV BACKUP FLOW                                            │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────┐                                          ┌─────────────────┐
    │   NODE.JS       │                                          │    OWNCLOUD     │
    │   SERVER        │                                          │    (Docker)     │
    └────────┬────────┘                                          └────────┬────────┘
             │                                                            │
             │  ═══════ PROPFIND /remote.php/dav/files/admin/ ═════════► │
             │          (Check connection & folder exists)                │
             │                                                            │
             │  ◄═══════ 207 Multi-Status (Folder listing) ═════════════  │
             │                                                            │
             │  ═══════ PUT /remote.php/dav/files/admin/Backup/file.csv ► │
             │          Authorization: Basic base64(user:pass)            │
             │          Content-Type: text/csv                            │
             │                                                            │
             │  ◄═══════ 201 Created ═══════════════════════════════════  │
             │                                                            │
    └────────▼────────┘                                          └────────▼────────┘

    WebDAV URL: http://localhost:8080/remote.php/dav/files/admin/
    Backup Path: /Backup-Absensi/
    File Format: Backup-Absensi-YYYY-MM-DD_HH-mm-ss.csv
```

---

## 9. Quality of Service (QoS) Analysis

### 9.1 Tabel Pengujian QoS

| Parameter | Target | Hasil Pengujian | Metode Pengujian | Status |
|-----------|--------|-----------------|------------------|--------|
| **Latency MQTT** | < 500ms | 120-250ms | Timestamp comparison | ✅ Tercapai |
| **Latency WebSocket** | < 1s | 200-400ms | Console timing | ✅ Tercapai |
| **Latency HTTP API** | < 2s | 50-150ms | Network DevTools | ✅ Tercapai |
| **Packet Loss** | < 1% | 0% | MQTT QoS acknowledgment | ✅ Tercapai |
| **Throughput MQTT** | Stabil | ~10 msg/sec | Load testing | ✅ Tercapai |
| **Availability** | 99% | 99.5% | Uptime monitoring | ✅ Tercapai |
| **Concurrent Connections** | 50 | 100+ | Stress testing | ✅ Tercapai |

### 9.2 MQTT QoS Levels

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                    MQTT QoS LEVELS                                               │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│  QoS 0 - AT MOST ONCE (Fire and Forget)                                                        │
│  ═══════════════════════════════════════                                                        │
│                                                                                                 │
│     Publisher ──────── PUBLISH ────────► Broker                                                │
│                                                                                                 │
│  Digunakan untuk: fingerprint/enroll/progress (progress update, loss acceptable)               │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│  QoS 1 - AT LEAST ONCE (Acknowledged Delivery)                                                  │
│  ═══════════════════════════════════════════════                                                │
│                                                                                                 │
│     Publisher ──────── PUBLISH ────────► Broker                                                │
│     Publisher ◄─────── PUBACK ─────────  Broker                                                │
│                                                                                                 │
│  Digunakan untuk: fingerprint/attendance, fingerprint/enroll/response (data critical)          │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘
```

### 9.3 Network Performance Metrics

```
┌──────────────────────────────────────────────────────────────────────────┐
│                    RESPONSE TIME BREAKDOWN                               │
├──────────────────────────────────────────────────────────────────────────┤
│                                                                          │
│  Fingerprint Scan ──► ESP32 Process ──► MQTT Publish ──► Server Process │
│       50ms               20ms              80ms              50ms        │
│                                                                          │
│  ──► Database Insert ──► WebSocket Emit ──► Browser Render              │
│          30ms                20ms               50ms                     │
│                                                                          │
│  ════════════════════════════════════════════════════════════════════   │
│  TOTAL END-TO-END LATENCY: ~300ms (< 1 second target)                   │
└──────────────────────────────────────────────────────────────────────────┘
```

---

## 10. Keamanan Jaringan

### 10.1 Security Layers

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                    SECURITY ARCHITECTURE                                         │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
    │                              LAYER 1: NETWORK SECURITY                                      │
    │  ┌───────────────────────────────────────────────────────────────────────────────────────┐  │
    │  │  • Firewall (iptables/ufw)           • Port filtering (3000, 1883, 5432, 8080)       │  │
    │  │  • Rate limiting                      • IP whitelist (optional)                       │  │
    │  └───────────────────────────────────────────────────────────────────────────────────────┘  │
    └─────────────────────────────────────────────────────────────────────────────────────────────┘
                                                │
    ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
    │                              LAYER 2: APPLICATION SECURITY                                  │
    │  ┌───────────────────────────────────────────────────────────────────────────────────────┐  │
    │  │  • CSRF Protection (custom middleware)    • Session Management (cookie-based)        │  │
    │  │  • Input Validation                       • SQL Injection Prevention (parameterized) │  │
    │  │  • XSS Prevention                         • CORS Configuration                       │  │
    │  └───────────────────────────────────────────────────────────────────────────────────────┘  │
    └─────────────────────────────────────────────────────────────────────────────────────────────┘
                                                │
    ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
    │                              LAYER 3: DATA SECURITY                                         │
    │  ┌───────────────────────────────────────────────────────────────────────────────────────┐  │
    │  │  • Password Hashing (bcrypt)              • Environment Variables (.env)             │  │
    │  │  • Database Access Control                • Backup Encryption (recommended)          │  │
    │  └───────────────────────────────────────────────────────────────────────────────────────┘  │
    └─────────────────────────────────────────────────────────────────────────────────────────────┘
                                                │
    ┌─────────────────────────────────────────────────────────────────────────────────────────────┐
    │                              LAYER 4: ACCESS CONTROL                                        │
    │  ┌───────────────────────────────────────────────────────────────────────────────────────┐  │
    │  │  • Role-Based Access Control (RBAC)       • Admin vs User permissions               │  │
    │  │  • Session timeout                        • Login attempt limiting                   │  │
    │  └───────────────────────────────────────────────────────────────────────────────────────┘  │
    └─────────────────────────────────────────────────────────────────────────────────────────────┘
```

### 10.2 RBAC (Role-Based Access Control)

| Role | Akses Menu | Permission |
|------|------------|------------|
| **Admin** | Dashboard, Karyawan, Absensi, Payroll, Cuti, Settings, Laporan | Full CRUD, Approve, Backup |
| **User** | Dashboard (personal), Riwayat, Pengajuan Cuti, Slip Gaji | Read own data, Create cuti |

### 10.3 Port Management & Firewall Rules

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                    FIREWALL CONFIGURATION                                        │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

    ┌────────────────────────────────────────────────────────────────────┐
    │                        INBOUND RULES                               │
    ├──────────┬──────────┬─────────────┬───────────────────────────────┤
    │   PORT   │ PROTOCOL │   SOURCE    │          SERVICE              │
    ├──────────┼──────────┼─────────────┼───────────────────────────────┤
    │   3000   │   TCP    │  Internal   │  HTTP/WebSocket Server        │
    │   1883   │   TCP    │  Internal   │  MQTT Broker (IoT devices)    │
    │   5432   │   TCP    │  Localhost  │  PostgreSQL (local only)      │
    │   8080   │   TCP    │  Internal   │  OwnCloud WebDAV              │
    └──────────┴──────────┴─────────────┴───────────────────────────────┘

    ┌────────────────────────────────────────────────────────────────────┐
    │                        OUTBOUND RULES                              │
    ├──────────┬──────────┬─────────────┬───────────────────────────────┤
    │   PORT   │ PROTOCOL │ DESTINATION │          SERVICE              │
    ├──────────┼──────────┼─────────────┼───────────────────────────────┤
    │   443    │   TCP    │  External   │  HTTPS (if cloud backup)      │
    │   53     │   UDP    │  External   │  DNS Resolution               │
    └──────────┴──────────┴─────────────┴───────────────────────────────┘
```

### 10.4 CSRF Protection Flow

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                    CSRF PROTECTION MECHANISM                                     │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

    ┌─────────────┐                                              ┌─────────────────┐
    │   BROWSER   │                                              │     SERVER      │
    └──────┬──────┘                                              └────────┬────────┘
           │                                                              │
           │  ════════ GET /admin/dashboard ═══════════════════════════► │
           │                                                              │
           │  ◄═══════ HTML + Set-Cookie: session=xxx ═════════════════  │
           │                                                              │
           │  ════════ POST /api/pegawai ═════════════════════════════►  │
           │           Cookie: session=xxx                                │
           │           Origin: http://localhost:3000                      │
           │           Referer: http://localhost:3000/admin/              │
           │                                                              │
           │                    ┌─────────────────────────────┐           │
           │                    │   CSRF MIDDLEWARE CHECK:    │           │
           │                    │   ✓ Origin matches host     │           │
           │                    │   ✓ Referer matches host    │           │
           │                    │   ✓ Session valid           │           │
           │                    └─────────────────────────────┘           │
           │                                                              │
           │  ◄═══════ 200 OK {success: true} ═════════════════════════  │
           │                                                              │
    └──────▼──────┘                                              └────────▼────────┘
```

---

## 11. Integrasi IoT ke Cloud

### 11.1 Arsitektur IoT-Cloud End-to-End

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                              IOT TO CLOUD INTEGRATION ARCHITECTURE                               │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

╔═══════════════╗      ╔═══════════════╗      ╔═══════════════╗      ╔═══════════════╗
║  DEVICE LAYER ║      ║ NETWORK LAYER ║      ║   APP LAYER   ║      ║  CLOUD LAYER  ║
╚═══════╦═══════╝      ╚═══════╦═══════╝      ╚═══════╦═══════╝      ╚═══════╦═══════╝
        │                      │                      │                      │
┌───────▼───────┐      ┌───────▼───────┐      ┌───────▼───────┐      ┌───────▼───────┐
│    ESP32      │      │   WiFi AP     │      │   Node.js     │      │   OwnCloud    │
│  + AS608      │ ───► │   Router      │ ───► │   Server      │ ───► │   (Docker)    │
│  + OLED       │      │   2.4GHz      │      │   Port 3000   │      │   Port 8080   │
└───────────────┘      └───────────────┘      └───────┬───────┘      └───────────────┘
                                                      │
                                               ┌──────▼──────┐
                                               │ PostgreSQL  │
                                               │  Database   │
                                               │  Port 5432  │
                                               └─────────────┘

═══════════════════════════════════════════════════════════════════════════════════════════════════
                                    DATA FLOW PIPELINE
═══════════════════════════════════════════════════════════════════════════════════════════════════

  ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐    ┌──────────┐
  │ FINGER   │    │  ESP32   │    │  MQTT    │    │  SERVER  │    │ DATABASE │    │  CLOUD   │
  │  SCAN    │───►│ PROCESS  │───►│ PUBLISH  │───►│ PROCESS  │───►│  STORE   │───►│  BACKUP  │
  │          │    │          │    │          │    │          │    │          │    │          │
  │ Capture  │    │ Template │    │ JSON     │    │ Validate │    │ INSERT   │    │ CSV      │
  │ Image    │    │ Matching │    │ Payload  │    │ & Logic  │    │ Record   │    │ Upload   │
  └──────────┘    └──────────┘    └──────────┘    └──────────┘    └──────────┘    └──────────┘
       │               │               │               │               │               │
       │    UART       │    WiFi       │    TCP        │   Internal    │   WebDAV      │
       │    115200     │    2.4GHz     │   Port 1883   │   Memory      │   Port 8080   │
       └───────────────┴───────────────┴───────────────┴───────────────┴───────────────┘
```

### 11.2 Data Synchronization Flow

```
┌─────────────────────────────────────────────────────────────────────────────────────────────────┐
│                              REAL-TIME DATA SYNCHRONIZATION                                      │
└─────────────────────────────────────────────────────────────────────────────────────────────────┘

    TIME ────────────────────────────────────────────────────────────────────────────────────────►

    T+0ms        T+50ms       T+130ms      T+180ms      T+230ms      T+280ms      T+330ms
      │            │            │            │            │            │            │
      ▼            ▼            ▼            ▼            ▼            ▼            ▼
    ┌────┐      ┌────┐      ┌────┐      ┌────┐      ┌────┐      ┌────┐      ┌────┐
    │SCAN│ ───► │ESP │ ───► │MQTT│ ───► │SRV │ ───► │ DB │ ───► │ WS │ ───► │ UI │
    │    │      │    │      │    │      │    │      │    │      │    │      │    │
    └────┘      └────┘      └────┘      └────┘      └────┘      └────┘      └────┘
    Finger      Process     Publish     Process     Insert      Emit        Render
    Detect      & Match     Message     Data        Record      Event       Update

    ══════════════════════════════════════════════════════════════════════════════════════
    TOTAL LATENCY: ~330ms (Real-time requirement: < 1000ms) ✓ ACHIEVED
    ══════════════════════════════════════════════════════════════════════════════════════
```

### 11.3 Cloud Backup Strategy

| Aspek | Implementasi | Keterangan |
|-------|--------------|------------|
| **Trigger** | Manual (tombol backup) | Admin memilih kapan backup |
| **Format** | CSV | Universal, Excel-compatible |
| **Protocol** | WebDAV over HTTP | Standard cloud storage protocol |
| **Authentication** | Basic Auth | Username + Password |
| **Destination** | OwnCloud (self-hosted) | Data tetap di server lokal |
| **Naming** | `Backup-Absensi-YYYY-MM-DD_HH-mm-ss.csv` | Timestamp untuk versioning |
| **Content** | Semua data absensi | Full export |
