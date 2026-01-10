# Component Diagram

## 1. Component Diagram Keseluruhan Sistem

```mermaid
flowchart TB
    subgraph Client["Client Layer"]
        subgraph AdminUI["Admin Interface"]
            AD[Dashboard Admin]
            AM[Manajemen Pegawai]
            AP[Payroll]
            AC[Manajemen Cuti]
            AR[Laporan]
        end

        subgraph UserUI["User Interface"]
            UD[Dashboard User]
            UA[Riwayat Absensi]
            US[Slip Gaji]
            UC[Pengajuan Cuti]
        end
    end

    subgraph Server["Server Layer"]
        subgraph API["REST API"]
            AuthAPI[Auth API]
            PegawaiAPI[Pegawai API]
            AbsensiAPI[Absensi API]
            PayrollAPI[Payroll API]
            CutiAPI[Cuti API]
            SettingAPI[Setting API]
        end

        subgraph Realtime["Real-time Services"]
            WS[WebSocket Server]
            MQTT[MQTT Handler]
        end

        subgraph Core["Core Services"]
            AuthService[Auth Service]
            PayrollService[Payroll Service]
            NotifService[Notifikasi Service]
        end
    end

    subgraph Data["Data Layer"]
        DB[(PostgreSQL)]
        FS[File Storage]
    end

    subgraph IoT["IoT Layer"]
        ESP[ESP32]
        FP[Fingerprint Sensor]
        LED[LED Indicator]
        BUZ[Buzzer]
    end

    subgraph External["External Services"]
        MQTTBroker[MQTT Broker<br/>Aedes]
        OwnCloud[OwnCloud<br/>WebDAV]
    end

    %% Client to API connections
    AdminUI --> API
    UserUI --> API

    %% API to Services
    API --> Core
    API --> DB
    API --> FS

    %% Real-time connections
    WS -.-> AdminUI
    WS -.-> UserUI
    MQTT --> MQTTBroker

    %% IoT connections
    ESP --> FP
    ESP --> LED
    ESP --> BUZ
    ESP --> MQTTBroker

    %% External services
    Core --> OwnCloud
    MQTTBroker --> MQTT
```

---

## 2. Component Diagram Detail - Server Components

```mermaid
flowchart TB
    subgraph ExpressServer["Express.js Server"]
        subgraph Middleware["Middleware Layer"]
            CORS[CORS]
            BodyParser[Body Parser]
            Auth[Auth Middleware]
            CSRF[CSRF Protection]
            RateLimit[Rate Limiter]
        end

        subgraph Routes["Route Layer"]
            AuthRoute[/api/auth/*]
            PegawaiRoute[/api/pegawai/*]
            AbsensiRoute[/api/absensi/*]
            PayrollRoute[/api/payroll/*]
            CutiRoute[/api/cuti/*]
            ShiftRoute[/api/shift/*]
            NotifRoute[/api/notifikasi/*]
            SettingRoute[/api/setting/*]
            BackupRoute[/api/backup/*]
        end

        subgraph Controllers["Controller Layer"]
            AuthCtrl[AuthController]
            PegawaiCtrl[PegawaiController]
            AbsensiCtrl[AbsensiController]
            PayrollCtrl[PayrollController]
            CutiCtrl[CutiController]
        end

        subgraph Services["Service Layer"]
            AuthSvc[AuthService]
            PayrollSvc[PayrollService]
            NotifSvc[NotifikasiService]
            BackupSvc[BackupService]
        end
    end

    subgraph Database["Database Layer"]
        Pool[Connection Pool]
        PG[(PostgreSQL)]
    end

    subgraph RealTime["Real-time Layer"]
        SocketIO[Socket.IO]
        Aedes[Aedes MQTT]
    end

    %% Flow
    Middleware --> Routes
    Routes --> Controllers
    Controllers --> Services
    Services --> Pool
    Pool --> PG

    Controllers --> SocketIO
    Aedes --> Controllers
```

---

## 3. Component Diagram - Frontend Components

```mermaid
flowchart TB
    subgraph PublicFolder["public/"]
        subgraph AdminPages["admin/"]
            DashGlass[dashboard-glass.html]
            ManajemenPegawai[manajemen-pegawai.html]
            ManajemenAbsensi[manajemen-absensi.html]
            ManajemenPayroll[manajemen-payroll.html]
            ManajemenCuti[manajemen-cuti.html]
            ManajemenShift[manajemen-shift.html]
            ManajemenLibur[hari-libur.html]
            Laporan[laporan.html]
            Settings[settings.html]
        end

        subgraph UserPages["user/"]
            UserDash[user-dashboard.html]
            UserAbsensi[user-absensi.html]
            UserCuti[user-cuti.html]
            UserSlip[user-slip.html]
            UserProfil[user-profil.html]
        end

        subgraph SharedAssets["assets/"]
            CSS[css/]
            JS[js/]
            Images[images/]
        end

        LoginPage[login.html]
        IndexPage[index.html]
        ServiceWorker[service-worker.js]
        Manifest[manifest.json]
    end

    subgraph JSModules["JavaScript Modules"]
        AuthModule[auth.js]
        APIModule[api.js]
        ChartModule[charts.js]
        TableModule[tables.js]
        SocketModule[socket-client.js]
    end

    subgraph Libraries["External Libraries"]
        Bootstrap[Bootstrap 5]
        ChartJS[Chart.js]
        SocketIOClient[Socket.IO Client]
        FontAwesome[Font Awesome]
    end

    %% Connections
    AdminPages --> JSModules
    UserPages --> JSModules
    JSModules --> Libraries
    ServiceWorker --> Manifest
```

---

## 4. Component Diagram - IoT Components

```mermaid
flowchart TB
    subgraph ESP32Module["ESP32 DevKit"]
        subgraph Firmware["Firmware Components"]
            MainLoop[Main Loop]
            WiFiMgr[WiFi Manager]
            MQTTClient[MQTT Client]
            FPHandler[Fingerprint Handler]
            LEDCtrl[LED Controller]
            BuzzerCtrl[Buzzer Controller]
            DisplayCtrl[Display Controller]
        end

        subgraph Memory["Memory"]
            SPIFFS[SPIFFS Storage]
            EEPROM[EEPROM]
        end
    end

    subgraph Peripherals["Peripheral Devices"]
        AS608[AS608 Fingerprint<br/>Sensor]
        OLED[OLED Display<br/>128x64]
        LEDRGB[RGB LED]
        Buzzer[Passive Buzzer]
    end

    subgraph Network["Network"]
        WiFi[WiFi 2.4GHz]
        MQTTBroker[MQTT Broker]
    end

    %% Internal connections
    MainLoop --> WiFiMgr
    MainLoop --> MQTTClient
    MainLoop --> FPHandler
    MainLoop --> LEDCtrl
    MainLoop --> BuzzerCtrl
    MainLoop --> DisplayCtrl

    FPHandler --> SPIFFS
    WiFiMgr --> EEPROM

    %% Peripheral connections
    FPHandler --> AS608
    DisplayCtrl --> OLED
    LEDCtrl --> LEDRGB
    BuzzerCtrl --> Buzzer

    %% Network connections
    WiFiMgr --> WiFi
    MQTTClient --> WiFi
    WiFi --> MQTTBroker
```

---

## 5. Component Diagram - Database Components

```mermaid
flowchart TB
    subgraph PostgreSQL["PostgreSQL Database"]
        subgraph Tables["Tables"]
            T1[pegawai]
            T2[absensi]
            T3[users]
            T4[shift]
            T5[pengajuan_cuti]
            T6[hari_libur]
            T7[payroll]
            T8[setting_payroll]
            T9[notifikasi]
            T10[tugas]
        end

        subgraph Views["Views"]
            V1[view_absensi_lengkap]
            V2[view_statistik_pegawai]
        end

        subgraph Indexes["Indexes"]
            I1[idx_timestamp]
            I2[idx_pegawai_id]
            I3[idx_pegawai_nip]
            I4[idx_cuti_status]
            I5[idx_payroll_periode]
        end

        subgraph Functions["Functions/Triggers"]
            F1[update_timestamp_trigger]
            F2[calculate_duration_trigger]
        end

        subgraph Types["Custom Types"]
            E1[status_pegawai ENUM]
            E2[keterangan_absensi ENUM]
            E3[role_user ENUM]
        end
    end

    subgraph NodeApp["Node.js Application"]
        Pool[pg Pool]
        Query[Query Builder]
    end

    %% Relationships
    V1 --> T1
    V1 --> T2
    V2 --> T1
    V2 --> T2

    Pool --> Tables
    Pool --> Views
    Query --> Pool
```

---

## 6. Component Diagram - API Endpoints

```mermaid
flowchart LR
    subgraph AuthEndpoints["Authentication"]
        POST_Login[POST /api/login]
        POST_Logout[POST /api/logout]
        GET_Me[GET /api/me]
    end

    subgraph PegawaiEndpoints["Pegawai"]
        GET_Pegawai[GET /api/pegawai]
        POST_Pegawai[POST /api/pegawai]
        PUT_Pegawai[PUT /api/pegawai/:id]
        DEL_Pegawai[DELETE /api/pegawai/:id]
    end

    subgraph AbsensiEndpoints["Absensi"]
        GET_Absensi[GET /api/absensi]
        GET_Statistik[GET /api/absensi/statistik]
        GET_Rekap[GET /api/absensi/rekap]
    end

    subgraph PayrollEndpoints["Payroll"]
        GET_Payroll[GET /api/payroll]
        POST_Generate[POST /api/payroll/generate]
        GET_Slip[GET /api/payroll/slip/:id]
    end

    subgraph CutiEndpoints["Cuti"]
        GET_Cuti[GET /api/cuti]
        POST_Cuti[POST /api/cuti]
        PUT_Approve[PUT /api/cuti/:id/approve]
        PUT_Reject[PUT /api/cuti/:id/reject]
    end

    subgraph SettingEndpoints["Settings"]
        GET_Setting[GET /api/setting]
        PUT_Setting[PUT /api/setting]
        POST_Backup[POST /api/backup/owncloud]
    end

    subgraph Database["Database"]
        DB[(PostgreSQL)]
    end

    AuthEndpoints --> DB
    PegawaiEndpoints --> DB
    AbsensiEndpoints --> DB
    PayrollEndpoints --> DB
    CutiEndpoints --> DB
    SettingEndpoints --> DB
```

---

## 7. Component Diagram - Communication Flow

```mermaid
flowchart TB
    subgraph Browser["Web Browser"]
        AdminUI[Admin Dashboard]
        UserUI[User Dashboard]
        WSClient[WebSocket Client]
    end

    subgraph Server["Node.js Server"]
        Express[Express.js]
        SocketServer[Socket.IO Server]
        MQTTHandler[MQTT Handler]
    end

    subgraph MQTTLayer["MQTT Layer"]
        Aedes[Aedes Broker]
    end

    subgraph IoTDevice["ESP32 Device"]
        ESP[ESP32]
        MQTTLib[PubSubClient]
    end

    subgraph Database["Database"]
        PG[(PostgreSQL)]
    end

    %% HTTP Connections
    AdminUI -->|HTTP/REST| Express
    UserUI -->|HTTP/REST| Express

    %% WebSocket Connections
    AdminUI <-->|WebSocket| WSClient
    UserUI <-->|WebSocket| WSClient
    WSClient <-->|Socket.IO| SocketServer

    %% MQTT Connections
    ESP -->|MQTT Publish| MQTTLib
    MQTTLib <-->|TCP/1883| Aedes
    Aedes <-->|Internal| MQTTHandler

    %% Server to DB
    Express -->|pg| PG
    MQTTHandler -->|pg| PG

    %% Real-time updates
    MQTTHandler -.->|emit| SocketServer
    Express -.->|emit| SocketServer
```

---

## 8. Tabel Deskripsi Komponen

### 8.1 Client Components

| Komponen | Teknologi | Deskripsi | Port |
|----------|-----------|-----------|------|
| Admin Dashboard | HTML/CSS/JS | Interface untuk admin | 80/443 |
| User Dashboard | HTML/CSS/JS | Interface untuk karyawan | 80/443 |
| PWA Service Worker | JavaScript | Offline capability | - |
| Socket.IO Client | JavaScript | Real-time updates | 3000 |

### 8.2 Server Components

| Komponen | Teknologi | Deskripsi | Port |
|----------|-----------|-----------|------|
| Express.js | Node.js | Web server & REST API | 3000 |
| Socket.IO Server | Node.js | WebSocket server | 3000 |
| Aedes | Node.js | MQTT broker | 1883 |
| pg Pool | Node.js | Database connection pool | - |

### 8.3 Data Components

| Komponen | Teknologi | Deskripsi | Port |
|----------|-----------|-----------|------|
| PostgreSQL | PostgreSQL 14+ | Relational database | 5432 |
| File Storage | File System | Upload storage | - |
| OwnCloud | WebDAV | Backup storage | 443 |

### 8.4 IoT Components

| Komponen | Teknologi | Deskripsi | Koneksi |
|----------|-----------|-----------|---------|
| ESP32 | Arduino/C++ | Microcontroller | WiFi |
| AS608 | Hardware | Fingerprint sensor | Serial |
| OLED Display | Hardware | Status display | I2C |
| LED/Buzzer | Hardware | Indicator | GPIO |

---

## 9. Interface Description

### 9.1 REST API Interface

```
┌─────────────────────────────────────────────────────────────┐
│                    REST API Interface                        │
├─────────────────────────────────────────────────────────────┤
│ Base URL: http://localhost:3000/api                         │
│ Content-Type: application/json                              │
│ Authentication: Bearer Token (JWT)                          │
├─────────────────────────────────────────────────────────────┤
│ Headers:                                                    │
│   Authorization: Bearer <token>                             │
│   Content-Type: application/json                            │
│   X-CSRF-Token: <csrf_token>                               │
├─────────────────────────────────────────────────────────────┤
│ Response Format:                                            │
│   Success: { success: true, data: {...} }                   │
│   Error: { success: false, error: "message" }               │
└─────────────────────────────────────────────────────────────┘
```

### 9.2 WebSocket Interface

```
┌─────────────────────────────────────────────────────────────┐
│                 WebSocket Interface                          │
├─────────────────────────────────────────────────────────────┤
│ URL: ws://localhost:3000                                    │
│ Protocol: Socket.IO                                         │
├─────────────────────────────────────────────────────────────┤
│ Events (Server → Client):                                   │
│   - attendance-new: New attendance record                   │
│   - attendance-update: Updated attendance                   │
│   - notification: New notification                          │
│   - cuti-status: Leave status change                        │
├─────────────────────────────────────────────────────────────┤
│ Events (Client → Server):                                   │
│   - join-room: Join specific room                           │
│   - leave-room: Leave room                                  │
└─────────────────────────────────────────────────────────────┘
```

### 9.3 MQTT Interface

```
┌─────────────────────────────────────────────────────────────┐
│                    MQTT Interface                            │
├─────────────────────────────────────────────────────────────┤
│ Broker: mqtt://localhost:1883                               │
│ Protocol: MQTT 3.1.1                                        │
├─────────────────────────────────────────────────────────────┤
│ Topics:                                                     │
│   fingerprint/attendance    - Attendance data (ESP→Server)  │
│   fingerprint/enroll/request - Start enrollment (Server→ESP)│
│   fingerprint/enroll/response - Enrollment result (ESP→Srv) │
│   fingerprint/enroll/progress - Enrollment progress         │
│   fingerprint/status        - Device status                 │
├─────────────────────────────────────────────────────────────┤
│ Payload Format: JSON                                        │
│   { fingerprint_id: int, timestamp: ISO8601, type: string } │
└─────────────────────────────────────────────────────────────┘
```

---

*Dokumentasi Component Diagram - Sistem Absensi IoT Berbasis Fingerprint*
