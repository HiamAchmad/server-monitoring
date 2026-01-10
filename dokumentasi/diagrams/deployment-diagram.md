# Deployment Diagram

## 1. Deployment Diagram Keseluruhan

```mermaid
flowchart TB
    subgraph Internet["Internet/Cloud"]
        OwnCloud[("OwnCloud Server<br/>WebDAV Backup")]
    end

    subgraph LAN["Local Area Network"]
        subgraph ServerNode["Server Node"]
            subgraph Docker["Docker Container (Optional)"]
                NodeJS["Node.js v18+<br/>Express Server<br/>Port: 3000"]
                Aedes["Aedes MQTT Broker<br/>Port: 1883"]
            end
            PostgreSQL[("PostgreSQL 14+<br/>Port: 5432")]
            FileStorage[("File Storage<br/>/uploads")]
        end

        subgraph ClientDevices["Client Devices"]
            AdminPC["Admin PC<br/>Chrome/Firefox"]
            UserPC["User PC/Laptop<br/>Chrome/Firefox"]
            Mobile["Mobile Device<br/>PWA Support"]
        end

        subgraph IoTDevices["IoT Devices"]
            ESP32_1["ESP32 DevKit V1<br/>Fingerprint Station 1"]
            ESP32_2["ESP32 DevKit V1<br/>Fingerprint Station 2"]
        end

        Router["WiFi Router<br/>2.4GHz / 5GHz"]
    end

    %% Connections
    AdminPC -->|HTTP/WS| NodeJS
    UserPC -->|HTTP/WS| NodeJS
    Mobile -->|HTTP/WS| NodeJS

    ESP32_1 -->|MQTT| Aedes
    ESP32_2 -->|MQTT| Aedes

    NodeJS -->|TCP| PostgreSQL
    NodeJS -->|File I/O| FileStorage
    NodeJS -->|HTTPS| OwnCloud

    AdminPC --> Router
    UserPC --> Router
    Mobile --> Router
    ESP32_1 --> Router
    ESP32_2 --> Router
    Router --> ServerNode
```

---

## 2. Deployment Diagram - Physical View

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                          DEPLOYMENT ARCHITECTURE                              │
│                    Sistem Absensi IoT Berbasis Fingerprint                   │
└─────────────────────────────────────────────────────────────────────────────┘

                                   ┌─────────────────┐
                                   │   INTERNET      │
                                   │                 │
                                   │  ┌───────────┐  │
                                   │  │ OwnCloud  │  │
                                   │  │  Server   │  │
                                   │  │ (Backup)  │  │
                                   │  └───────────┘  │
                                   └────────┬────────┘
                                            │ HTTPS
                                            │
┌───────────────────────────────────────────┼─────────────────────────────────┐
│                              LOCAL NETWORK │                                 │
│                                           │                                 │
│   ┌─────────────────────────────────────────────────────────────────────┐   │
│   │                        SERVER NODE                                   │   │
│   │   ┌──────────────────────────────────────────────────────────────┐  │   │
│   │   │                    APPLICATION SERVER                         │  │   │
│   │   │                                                               │  │   │
│   │   │   ┌─────────────────┐     ┌─────────────────┐                │  │   │
│   │   │   │   Node.js       │     │  Aedes MQTT     │                │  │   │
│   │   │   │   Express.js    │     │    Broker       │                │  │   │
│   │   │   │   Socket.IO     │     │                 │                │  │   │
│   │   │   │   Port: 3000    │     │   Port: 1883    │                │  │   │
│   │   │   └────────┬────────┘     └────────┬────────┘                │  │   │
│   │   │            │                       │                          │  │   │
│   │   └────────────┼───────────────────────┼──────────────────────────┘  │   │
│   │                │                       │                              │   │
│   │   ┌────────────┴───────────┐   ┌───────┴───────────┐                 │   │
│   │   │    PostgreSQL DB       │   │   File Storage    │                 │   │
│   │   │    Port: 5432          │   │   /uploads        │                 │   │
│   │   │    - pegawai           │   │   - photos        │                 │   │
│   │   │    - absensi           │   │   - documents     │                 │   │
│   │   │    - payroll           │   │   - backups       │                 │   │
│   │   │    - dll               │   │                   │                 │   │
│   │   └────────────────────────┘   └───────────────────┘                 │   │
│   └──────────────────────────────────────────────────────────────────────┘   │
│                                    │                                         │
│                           ┌────────┴────────┐                               │
│                           │   WiFi Router   │                               │
│                           │   192.168.x.x   │                               │
│                           └────────┬────────┘                               │
│                                    │                                         │
│        ┌──────────────────┬────────┴────────┬──────────────────┐           │
│        │                  │                  │                  │           │
│   ┌────┴────┐       ┌────┴────┐       ┌────┴────┐       ┌────┴────┐       │
│   │Admin PC │       │ User PC │       │ Mobile  │       │  ESP32  │       │
│   │ Browser │       │ Browser │       │   PWA   │       │ Station │       │
│   │         │       │         │       │         │       │         │       │
│   │ Chrome/ │       │ Chrome/ │       │ Android/│       │  WiFi   │       │
│   │ Firefox │       │ Firefox │       │   iOS   │       │  MQTT   │       │
│   └─────────┘       └─────────┘       └─────────┘       └─────────┘       │
│                                                                             │
└─────────────────────────────────────────────────────────────────────────────┘
```

---

## 3. Deployment Diagram - Server Detail

```mermaid
flowchart TB
    subgraph ServerMachine["Server Machine"]
        subgraph OS["Ubuntu Server 22.04 LTS"]
            subgraph Runtime["Runtime Environment"]
                NodeRuntime["Node.js v18 LTS"]
                NPM["NPM v9+"]
            end

            subgraph AppProcess["Application Processes"]
                PM2["PM2 Process Manager"]
                ExpressApp["Express Application<br/>server.js"]
                AedesBroker["Aedes MQTT<br/>Built-in"]
                SocketIO["Socket.IO<br/>Built-in"]
            end

            subgraph Database["Database Service"]
                PGService["PostgreSQL Service"]
                PGData[("Data Directory<br/>/var/lib/postgresql")]
            end

            subgraph Storage["Storage"]
                AppDir["/var/www/absensi"]
                UploadDir["/var/www/absensi/uploads"]
                LogDir["/var/log/absensi"]
            end

            subgraph SystemServices["System Services"]
                Systemd["Systemd"]
                Nginx["Nginx (Reverse Proxy)"]
                UFW["UFW Firewall"]
            end
        end
    end

    %% Relationships
    PM2 --> ExpressApp
    NodeRuntime --> PM2
    ExpressApp --> AedesBroker
    ExpressApp --> SocketIO
    ExpressApp --> PGService
    ExpressApp --> UploadDir
    ExpressApp --> LogDir
    Nginx --> ExpressApp
    Systemd --> PM2
    Systemd --> PGService
    Systemd --> Nginx
```

---

## 4. Deployment Diagram - ESP32 Device

```mermaid
flowchart TB
    subgraph ESP32["ESP32 DevKit V1"]
        subgraph MCU["Microcontroller"]
            CPU["Xtensa LX6<br/>Dual Core 240MHz"]
            Flash["4MB Flash"]
            SRAM["520KB SRAM"]
            WiFiModule["WiFi 802.11 b/g/n"]
        end

        subgraph Firmware["Firmware"]
            Arduino["Arduino Framework"]
            PubSub["PubSubClient<br/>(MQTT)"]
            Adafruit["Adafruit Fingerprint<br/>Library"]
            SSD1306["SSD1306 OLED<br/>Library"]
        end

        subgraph GPIO["GPIO Pins"]
            TX_RX["TX2/RX2 (Serial)"]
            I2C["SDA/SCL (I2C)"]
            Digital["Digital Pins"]
        end
    end

    subgraph Peripherals["Connected Peripherals"]
        AS608["AS608 Fingerprint<br/>Sensor"]
        OLED["OLED Display<br/>128x64 px"]
        LED["RGB LED"]
        Buzzer["Passive Buzzer"]
    end

    subgraph Power["Power Supply"]
        USB["USB 5V"]
        Regulator["3.3V Regulator"]
    end

    %% Connections
    TX_RX -->|Serial 57600| AS608
    I2C -->|I2C| OLED
    Digital -->|GPIO| LED
    Digital -->|GPIO| Buzzer
    USB --> Regulator
    Regulator --> MCU
```

---

## 5. Network Topology

```mermaid
flowchart TB
    subgraph Internet["Internet Zone"]
        Cloud["Cloud Services<br/>(OwnCloud)"]
    end

    subgraph DMZ["DMZ (Optional)"]
        ReverseProxy["Nginx<br/>Reverse Proxy"]
    end

    subgraph InternalNetwork["Internal Network<br/>192.168.1.0/24"]
        subgraph ServerVLAN["Server VLAN"]
            AppServer["Application Server<br/>192.168.1.10"]
            DBServer["Database Server<br/>192.168.1.11"]
        end

        subgraph ClientVLAN["Client VLAN"]
            AdminWS["Admin Workstation<br/>192.168.1.100"]
            UserWS["User Workstations<br/>192.168.1.101-150"]
        end

        subgraph IoTVLAN["IoT VLAN"]
            ESP_1["ESP32 Station 1<br/>192.168.1.201"]
            ESP_2["ESP32 Station 2<br/>192.168.1.202"]
        end

        Switch["Managed Switch"]
        Router["Router/Firewall"]
    end

    %% Connections
    Cloud -->|HTTPS| ReverseProxy
    ReverseProxy -->|HTTP| AppServer

    AdminWS --> Switch
    UserWS --> Switch
    ESP_1 --> Switch
    ESP_2 --> Switch
    AppServer --> Switch
    DBServer --> Switch

    Switch --> Router
    Router --> Internet
```

---

## 6. Deployment Configuration

### 6.1 Server Requirements

| Komponen | Minimum | Recommended |
|----------|---------|-------------|
| CPU | 2 Core | 4 Core |
| RAM | 4 GB | 8 GB |
| Storage | 50 GB SSD | 100 GB SSD |
| OS | Ubuntu 20.04+ | Ubuntu 22.04 LTS |
| Network | 100 Mbps | 1 Gbps |

### 6.2 Software Stack

| Layer | Software | Version |
|-------|----------|---------|
| OS | Ubuntu Server | 22.04 LTS |
| Runtime | Node.js | 18.x LTS |
| Database | PostgreSQL | 14+ |
| Web Server | Nginx | 1.18+ |
| Process Manager | PM2 | 5.x |
| SSL | Let's Encrypt | - |

### 6.3 Port Configuration

| Service | Port | Protocol | Access |
|---------|------|----------|--------|
| HTTP | 80 | TCP | Public |
| HTTPS | 443 | TCP | Public |
| Node.js App | 3000 | TCP | Internal |
| PostgreSQL | 5432 | TCP | Internal |
| MQTT | 1883 | TCP | Internal |
| MQTT SSL | 8883 | TCP | Internal |
| Socket.IO | 3000 | TCP/WS | Internal |

### 6.4 ESP32 Requirements

| Komponen | Spesifikasi |
|----------|-------------|
| Board | ESP32 DevKit V1 |
| Flash | 4 MB minimum |
| WiFi | 2.4 GHz b/g/n |
| Power | 5V USB / 3.3V |
| Fingerprint | AS608 / R307 |
| Display | OLED SSD1306 128x64 |

---

## 7. Deployment Process

```mermaid
flowchart TB
    subgraph Development["Development"]
        Code["Source Code"]
        Test["Unit Tests"]
        Build["Build Process"]
    end

    subgraph Staging["Staging Environment"]
        StagingServer["Staging Server"]
        StagingDB["Staging Database"]
        IntegrationTest["Integration Tests"]
    end

    subgraph Production["Production Environment"]
        LoadBalancer["Load Balancer<br/>(Optional)"]
        ProdServer["Production Server"]
        ProdDB["Production Database"]
        Backup["Backup System"]
    end

    Code --> Test
    Test --> Build
    Build --> StagingServer
    StagingServer --> StagingDB
    StagingServer --> IntegrationTest

    IntegrationTest -->|Pass| LoadBalancer
    LoadBalancer --> ProdServer
    ProdServer --> ProdDB
    ProdDB --> Backup
```

---

## 8. High Availability Setup (Optional)

```mermaid
flowchart TB
    subgraph Clients["Clients"]
        Browser["Web Browsers"]
        ESP["ESP32 Devices"]
    end

    subgraph LoadBalancer["Load Balancer"]
        HAProxy["HAProxy / Nginx"]
    end

    subgraph AppServers["Application Servers"]
        App1["Node.js Server 1"]
        App2["Node.js Server 2"]
    end

    subgraph DatabaseCluster["Database Cluster"]
        Primary["PostgreSQL Primary"]
        Replica["PostgreSQL Replica"]
    end

    subgraph MessageBroker["Message Broker"]
        Redis["Redis<br/>(Session Store)"]
        MQTT["MQTT Cluster"]
    end

    Browser --> HAProxy
    ESP --> MQTT

    HAProxy --> App1
    HAProxy --> App2

    App1 --> Redis
    App2 --> Redis
    App1 --> Primary
    App2 --> Primary

    Primary --> Replica
    MQTT --> App1
    MQTT --> App2
```

---

## 9. Monitoring & Logging

```mermaid
flowchart TB
    subgraph Application["Application"]
        NodeApp["Node.js App"]
        PG["PostgreSQL"]
        MQTT["MQTT Broker"]
    end

    subgraph Monitoring["Monitoring Stack"]
        Prometheus["Prometheus"]
        Grafana["Grafana"]
        AlertManager["Alert Manager"]
    end

    subgraph Logging["Logging Stack"]
        Winston["Winston Logger"]
        LogFiles["Log Files"]
        Logrotate["Logrotate"]
    end

    NodeApp --> Winston
    NodeApp --> Prometheus
    PG --> Prometheus
    MQTT --> Prometheus

    Winston --> LogFiles
    LogFiles --> Logrotate

    Prometheus --> Grafana
    Prometheus --> AlertManager
```

---

## 10. Security Configuration

### 10.1 Firewall Rules (UFW)

```bash
# Default policies
ufw default deny incoming
ufw default allow outgoing

# Allow SSH
ufw allow 22/tcp

# Allow HTTP/HTTPS
ufw allow 80/tcp
ufw allow 443/tcp

# Allow MQTT (internal only)
ufw allow from 192.168.1.0/24 to any port 1883

# Allow PostgreSQL (internal only)
ufw allow from 192.168.1.10 to any port 5432
```

### 10.2 SSL/TLS Configuration

| Service | Certificate | Protocol |
|---------|-------------|----------|
| HTTPS | Let's Encrypt | TLS 1.3 |
| MQTT SSL | Self-signed | TLS 1.2 |
| PostgreSQL | Self-signed | TLS 1.2 |

---

*Dokumentasi Deployment Diagram - Sistem Absensi IoT Berbasis Fingerprint*
