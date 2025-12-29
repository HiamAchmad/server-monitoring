# Diagram Arsitektur Sistem

## 1. Arsitektur Keseluruhan

```mermaid
flowchart TB
    subgraph Hardware["Hardware Layer"]
        ESP32["ESP32 Microcontroller"]
        FP["Fingerprint Sensor<br/>R307/R503"]
        ESP32 --- FP
    end

    subgraph Communication["Communication Layer"]
        MQTT["MQTT Broker<br/>(Mosquitto)"]
        WS["WebSocket<br/>(Socket.IO)"]
    end

    subgraph Server["Application Layer"]
        NODE["Node.js Server<br/>(Express.js)"]
        API["REST API<br/>Endpoints"]
        NODE --- API
    end

    subgraph Database["Data Layer"]
        PG[("PostgreSQL<br/>Database")]
    end

    subgraph Client["Client Layer"]
        ADMIN["Admin Portal<br/>(Web Browser)"]
        USER["User Portal<br/>(Web Browser)"]
        PWA["PWA<br/>(Mobile/Desktop)"]
    end

    subgraph Cloud["Cloud Services"]
        OC["OwnCloud/WebDAV<br/>Backup Storage"]
    end

    ESP32 -->|Publish| MQTT
    MQTT -->|Subscribe| NODE
    NODE -->|Store/Query| PG
    NODE <-->|Real-time| WS
    WS <-->|Events| ADMIN
    WS <-->|Events| USER
    ADMIN <-->|HTTP| API
    USER <-->|HTTP| API
    PWA <-->|HTTP| API
    NODE -->|Backup| OC
```

## 2. Diagram Komponen

```mermaid
flowchart LR
    subgraph Frontend["Frontend Components"]
        direction TB
        L["Login Page"]
        AD["Admin Dashboard"]
        UD["User Dashboard"]
        MK["Manajemen Karyawan"]
        PY["Payroll"]
        LP["Laporan"]
        RW["Riwayat"]
        ST["Settings"]
        UA["User Absensi"]
        UG["User Gaji"]
        UC["User Cuti"]
    end

    subgraph Backend["Backend Components"]
        direction TB
        AUTH["Auth Module"]
        PEGAWAI["Pegawai Module"]
        ABSENSI["Absensi Module"]
        PAYROLL["Payroll Module"]
        CUTI["Cuti Module"]
        SHIFT["Shift Module"]
        NOTIF["Notification Module"]
        BACKUP["Backup Module"]
        MQTT_H["MQTT Handler"]
    end

    subgraph Database["Database Tables"]
        direction TB
        T_USER["users"]
        T_PEG["pegawai"]
        T_ABS["absensi"]
        T_PAY["payroll"]
        T_CUTI["pengajuan_cuti"]
        T_SHIFT["shift"]
        T_LIBUR["hari_libur"]
        T_NOTIF["notifikasi"]
    end

    L --> AUTH
    AD --> PEGAWAI
    AD --> ABSENSI
    MK --> PEGAWAI
    PY --> PAYROLL
    LP --> ABSENSI
    RW --> ABSENSI
    ST --> BACKUP
    UA --> ABSENSI
    UG --> PAYROLL
    UC --> CUTI

    AUTH --> T_USER
    PEGAWAI --> T_PEG
    ABSENSI --> T_ABS
    PAYROLL --> T_PAY
    CUTI --> T_CUTI
    SHIFT --> T_SHIFT
    SHIFT --> T_LIBUR
    NOTIF --> T_NOTIF
    MQTT_H --> ABSENSI
```

## 3. Diagram Deployment

```mermaid
flowchart TB
    subgraph Internet["Internet"]
        BROWSER["Web Browser"]
    end

    subgraph Server["Production Server"]
        direction TB
        NGINX["Nginx<br/>Reverse Proxy<br/>Port 80/443"]
        PM2["PM2<br/>Process Manager"]
        NODE["Node.js App<br/>Port 3000"]
        MOSQ["Mosquitto<br/>MQTT Broker<br/>Port 1883"]
        POSTGRES["PostgreSQL<br/>Database<br/>Port 5432"]

        NGINX --> NODE
        PM2 --> NODE
        NODE --> POSTGRES
        NODE --> MOSQ
    end

    subgraph IoT["IoT Device"]
        ESP["ESP32<br/>+ Fingerprint"]
    end

    BROWSER -->|HTTPS| NGINX
    ESP -->|MQTT| MOSQ
```

## 4. Diagram Network

```mermaid
flowchart LR
    subgraph LAN["Local Network"]
        ESP32["ESP32<br/>192.168.x.x"]
        MOSQ["MQTT Broker<br/>192.168.x.x:1883"]
    end

    subgraph WAN["Public Network"]
        SERVER["Web Server<br/>Public IP:80/443"]
        CLIENT["Client Browser"]
    end

    ESP32 <-->|MQTT Protocol| MOSQ
    MOSQ <-->|Internal| SERVER
    CLIENT <-->|HTTPS| SERVER
```

## 5. Aliran Data

```mermaid
sequenceDiagram
    participant FP as Fingerprint Sensor
    participant ESP as ESP32
    participant MQTT as MQTT Broker
    participant Server as Node.js Server
    participant DB as PostgreSQL
    participant Client as Web Browser

    FP->>ESP: Scan fingerprint
    ESP->>ESP: Process fingerprint
    ESP->>MQTT: Publish attendance data
    MQTT->>Server: Forward message
    Server->>DB: Store attendance record
    Server->>Client: Emit socket event
    Client->>Client: Update UI real-time
```

## 6. Layer Architecture

```
┌────────────────────────────────────────────────────────┐
│                   PRESENTATION LAYER                    │
│  ┌──────────────┐  ┌──────────────┐  ┌──────────────┐  │
│  │ Admin Portal │  │ User Portal  │  │  PWA Client  │  │
│  │  (HTML/CSS/  │  │  (HTML/CSS/  │  │  (Service    │  │
│  │   JS)        │  │   JS)        │  │   Worker)    │  │
│  └──────────────┘  └──────────────┘  └──────────────┘  │
└────────────────────────────────────────────────────────┘
                           │
                           ▼
┌────────────────────────────────────────────────────────┐
│                    APPLICATION LAYER                    │
│  ┌──────────────────────────────────────────────────┐  │
│  │              Node.js + Express.js                │  │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐ ┌────────┐  │  │
│  │  │  Auth   │ │ Pegawai │ │ Absensi │ │Payroll │  │  │
│  │  │ Module  │ │ Module  │ │ Module  │ │ Module │  │  │
│  │  └─────────┘ └─────────┘ └─────────┘ └────────┘  │  │
│  │  ┌─────────┐ ┌─────────┐ ┌─────────┐            │  │
│  │  │  Cuti   │ │  Shift  │ │  MQTT   │            │  │
│  │  │ Module  │ │ Module  │ │ Handler │            │  │
│  │  └─────────┘ └─────────┘ └─────────┘            │  │
│  └──────────────────────────────────────────────────┘  │
└────────────────────────────────────────────────────────┘
                           │
                           ▼
┌────────────────────────────────────────────────────────┐
│                      DATA LAYER                         │
│  ┌──────────────────────────────────────────────────┐  │
│  │                   PostgreSQL                      │  │
│  │  ┌────────┐ ┌────────┐ ┌────────┐ ┌────────────┐ │  │
│  │  │ users  │ │pegawai │ │absensi │ │  payroll   │ │  │
│  │  └────────┘ └────────┘ └────────┘ └────────────┘ │  │
│  │  ┌────────┐ ┌────────┐ ┌────────┐ ┌────────────┐ │  │
│  │  │  cuti  │ │ shift  │ │ libur  │ │ notifikasi │ │  │
│  │  └────────┘ └────────┘ └────────┘ └────────────┘ │  │
│  └──────────────────────────────────────────────────┘  │
└────────────────────────────────────────────────────────┘
                           │
                           ▼
┌────────────────────────────────────────────────────────┐
│                   HARDWARE LAYER                        │
│  ┌──────────────────────────────────────────────────┐  │
│  │         ESP32 + Fingerprint Sensor R307          │  │
│  │  ┌─────────────┐  ┌─────────────┐                │  │
│  │  │ WiFi Module │  │MQTT Client  │                │  │
│  │  └─────────────┘  └─────────────┘                │  │
│  └──────────────────────────────────────────────────┘  │
└────────────────────────────────────────────────────────┘
```

## 7. Technology Stack

| Layer | Technology | Purpose |
|-------|------------|---------|
| Frontend | HTML5, CSS3, JavaScript | User Interface |
| UI Framework | Glassmorphism CSS | Modern UI Design |
| PWA | Service Worker, Manifest | Offline Support |
| Backend | Node.js v16+ | Server Runtime |
| Framework | Express.js | Web Framework |
| Real-time | Socket.IO | WebSocket Communication |
| Database | PostgreSQL 13+ | Data Storage |
| IoT Protocol | MQTT | Device Communication |
| MQTT Broker | Mosquitto | Message Broker |
| Hardware | ESP32 | Microcontroller |
| Sensor | R307/R503 | Fingerprint Reader |
| Cloud | OwnCloud/WebDAV | Backup Storage |
