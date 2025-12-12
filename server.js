const express = require('express');
const path = require('path');
const http = require('http');
const { Server } = require("socket.io");
const cron = require('node-cron');
const fs = require('fs');
const moment = require('moment-timezone');
const { Pool } = require('pg');
const os = require('os');
const { createClient } = require("webdav");
const multer = require('multer');
const aedes = require('aedes');
const net = require('net');
const mqtt = require('mqtt'); // MQTT Client
const attendanceLogic = require('./attendance-logic');

const app = express();
const port = 3000;
const mqttPort = 1883;
const httpServer = http.createServer(app);
const io = new Server(httpServer);

// --- KONFIGURASI MQTT BROKER ---
const mqttBroker = aedes();
const mqttServer = net.createServer(mqttBroker.handle);

// Track connected MQTT clients
const connectedMqttClients = new Set();

// Event: Client connected
mqttBroker.on('client', (client) => {
    console.log('📡 MQTT Client terhubung:', client.id);
    connectedMqttClients.add(client.id);
});

// Event: Client disconnected
mqttBroker.on('clientDisconnect', (client) => {
    console.log('🔌 MQTT Client terputus:', client.id);
    connectedMqttClients.delete(client.id);
});

// --- KONFIGURASI MQTT CLIENT (untuk publish & subscribe) ---
const mqttClient = mqtt.connect('mqtt://localhost:1883');

mqttClient.on('connect', () => {
    console.log('✅ MQTT Client connected to broker');

    // Subscribe to enrollment response topic
    mqttClient.subscribe('fingerprint/enroll/response', (err) => {
        if (err) {
            console.error('❌ Error subscribe to enrollment response:', err);
        } else {
            console.log('📥 Subscribed to: fingerprint/enroll/response');
        }
    });

    // Subscribe to attendance topic
    mqttClient.subscribe('fingerprint/attendance', (err) => {
        if (err) {
            console.error('❌ Error subscribe to attendance:', err);
        } else {
            console.log('📥 Subscribed to: fingerprint/attendance');
        }
    });

    // Subscribe to enrollment progress topic
    mqttClient.subscribe('fingerprint/enroll/progress', (err) => {
        if (err) {
            console.error('❌ Error subscribe to enrollment progress:', err);
        } else {
            console.log('📥 Subscribed to: fingerprint/enroll/progress');
        }
    });
});

mqttClient.on('error', (err) => {
    console.error('❌ MQTT Client error:', err);
});

// Listener untuk enrollment response, progress, dan attendance dari ESP32
mqttClient.on('message', async (topic, message) => {
    // Handle enrollment progress
    if (topic === 'fingerprint/enroll/progress') {
        try {
            const data = JSON.parse(message.toString());
            console.log('📊 Enrollment progress:', data.step, '-', data.message);

            // Emit progress ke semua client
            io.emit('enrollment_progress', data);
        } catch (error) {
            console.error('❌ Error parsing enrollment progress:', error);
        }
        return;
    }

    if (topic === 'fingerprint/enroll/response') {
        try {
            const data = JSON.parse(message.toString());
            console.log('📩 Enrollment response received:', data);

            // ALWAYS emit response to web clients (for test page)
            io.emit('enrollment_response', data);

            // Update database dengan fingerprint_id (only if success)
            if (data.success && data.fingerprint_id && data.pegawai_id) {
                const updateQuery = 'UPDATE pegawai SET fingerprint_id = $1 WHERE id_pegawai = $2 RETURNING *';
                const result = await db.query(updateQuery, [data.fingerprint_id, data.pegawai_id]);

                if (result.rows.length > 0) {
                    console.log('✅ Fingerprint ID berhasil disimpan:', result.rows[0]);

                    // Broadcast ke semua client via Socket.IO
                    io.emit('enrollment_success', {
                        success: true,
                        pegawai_id: data.pegawai_id,
                        fingerprint_id: data.fingerprint_id,
                        message: data.message || 'Sidik jari berhasil didaftarkan'
                    });
                } else {
                    console.error('❌ Pegawai tidak ditemukan untuk update (ID:', data.pegawai_id, ')');
                    // Still emit success for test page, but note database update failed
                    io.emit('enrollment_success', {
                        success: true,
                        pegawai_id: data.pegawai_id,
                        fingerprint_id: data.fingerprint_id,
                        message: data.message + ' (Note: Pegawai not in database)',
                        db_updated: false
                    });
                }
            } else {
                console.error('❌ Enrollment gagal:', data.message);

                // Broadcast error
                io.emit('enrollment_error', {
                    success: false,
                    pegawai_id: data.pegawai_id,
                    message: data.message || 'Enrollment gagal'
                });
            }
        } catch (error) {
            console.error('❌ Error parsing enrollment response:', error);
        }
    } else if (topic === 'fingerprint/attendance') {
        try {
            const data = JSON.parse(message.toString());
            console.log('📩 Attendance data received:', data);

            const fingerprintID = data.fingerprint_id;

            // Cari pegawai berdasarkan fingerprint_id
            const pegawaiQuery = 'SELECT * FROM pegawai WHERE fingerprint_id = $1';
            const pegawaiResult = await db.query(pegawaiQuery, [fingerprintID]);

            if (pegawaiResult.rows.length === 0) {
                console.error('❌ Pegawai dengan fingerprint ID', fingerprintID, 'tidak ditemukan');
                io.emit('attendance_error', {
                    success: false,
                    fingerprint_id: fingerprintID,
                    message: 'Pegawai tidak ditemukan'
                });
                return;
            }

            const pegawai = pegawaiResult.rows[0];
            const pegawaiId = pegawai.id_pegawai;
            const waktuAbsen = new Date();

            console.log('👤 Pegawai detected:', pegawai.nama_pegawai, '(NIP:', pegawai.nip, ')');

            // Simpan ke database menggunakan logic yang sudah ada
            const absensiData = {
                pegawai_id: pegawaiId,
                waktu_absen: waktuAbsen.toISOString(),
                keterangan: 'Hadir'
            };

            handleAbsensiData(absensiData);

            // Broadcast ke web clients
            io.emit('attendance_success', {
                success: true,
                fingerprint_id: fingerprintID,
                pegawai_id: pegawaiId,
                nama: pegawai.nama_pegawai,
                nip: pegawai.nip,
                waktu: waktuAbsen,
                message: 'Absensi berhasil dicatat'
            });

            console.log('✅ Attendance recorded for', pegawai.nama_pegawai);

        } catch (error) {
            console.error('❌ Error handling attendance:', error);
            io.emit('attendance_error', {
                success: false,
                message: 'Error processing attendance: ' + error.message
            });
        }
    }
});

// --- KONFIGURASI MULTER UNTUK UPLOAD FILE ---
const storage = multer.diskStorage({
    destination: function (req, file, cb) {
        const uploadDir = path.join(__dirname, 'uploads');
        // Buat folder uploads jika belum ada
        if (!fs.existsSync(uploadDir)) {
            fs.mkdirSync(uploadDir, { recursive: true });
        }
        cb(null, uploadDir);
    },
    filename: function (req, file, cb) {
        // Generate unique filename: timestamp-originalname
        const uniqueName = Date.now() + '-' + file.originalname;
        cb(null, uniqueName);
    }
});

const upload = multer({
    storage: storage,
    limits: {
        fileSize: 10 * 1024 * 1024 // 10MB max
    },
    fileFilter: function (req, file, cb) {
        // Allowed extensions
        const allowedExts = /\.(pdf|doc|docx|xls|xlsx|ppt|pptx|txt|zip|rar)$/i;
        const extname = allowedExts.test(file.originalname.toLowerCase());

        // Allowed MIME types
        const allowedMimeTypes = [
            'application/pdf',
            'application/msword',
            'application/vnd.openxmlformats-officedocument.wordprocessingml.document',
            'application/vnd.ms-excel',
            'application/vnd.openxmlformats-officedocument.spreadsheetml.sheet',
            'application/vnd.ms-powerpoint',
            'application/vnd.openxmlformats-officedocument.presentationml.presentation',
            'text/plain',
            'application/zip',
            'application/x-zip-compressed',
            'application/x-rar-compressed',
            'application/octet-stream'
        ];

        const mimetypeValid = allowedMimeTypes.includes(file.mimetype);

        if (extname && mimetypeValid) {
            return cb(null, true);
        } else {
            cb(new Error('Format file tidak diizinkan! Hanya PDF, DOC, XLS, PPT, TXT, ZIP, RAR'));
        }
    }
});

// --- KONFIGURASI OWNCLOUD ---
const ownCloudConfig = {
    url: 'http://localhost:8080/remote.php/dav/files/admin/',
    username: 'admin',
    password: 'admin'
};

app.use(express.json());

// Disable cache for HTML files
app.use(express.static(path.join(__dirname, 'public'), {
    etag: false,
    setHeaders: (res, path) => {
        if (path.endsWith('.html')) {
            res.setHeader('Cache-Control', 'no-cache, no-store, must-revalidate');
            res.setHeader('Pragma', 'no-cache');
            res.setHeader('Expires', '0');
        }
    }
}));

// Serve uploaded files
app.use('/uploads', express.static(path.join(__dirname, 'uploads')));

// Konfigurasi Koneksi PostgreSQL
const db = new Pool({
    host: 'localhost',
    port: 5432,
    user: 'absensi_user',
    password: 'absensi_password',
    database: 'db_absensi',
    max: 20,
    idleTimeoutMillis: 30000,
    connectionTimeoutMillis: 2000,
});

// Test Koneksi PostgreSQL
db.query('SELECT NOW()', async (err, result) => {
    if (err) {
        console.error('🔴 Error menghubungkan ke database PostgreSQL:', err.stack);
        return;
    }
    console.log('✅ Terhubung ke database PostgreSQL.');
    console.log(`   Server time: ${result.rows[0].now}`);

    // Auto-create tabel notifikasi jika belum ada
    try {
        await db.query(`
            CREATE TABLE IF NOT EXISTS notifikasi (
                id_notifikasi SERIAL PRIMARY KEY,
                pegawai_id INT REFERENCES pegawai(id_pegawai) ON DELETE CASCADE,
                judul VARCHAR(100) NOT NULL,
                pesan TEXT NOT NULL,
                tipe VARCHAR(50) DEFAULT 'info',
                kategori VARCHAR(50) DEFAULT 'sistem',
                is_read BOOLEAN DEFAULT false,
                is_admin BOOLEAN DEFAULT false,
                link VARCHAR(255),
                created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
            )
        `);
        console.log('✅ Tabel notifikasi siap.');
    } catch (e) {
        console.log('ℹ️ Tabel notifikasi sudah ada atau error:', e.message);
    }
});

// Helper function untuk membuat notifikasi
async function createNotification(data) {
    const { pegawai_id, judul, pesan, tipe = 'info', kategori = 'sistem', is_admin = false, link = null } = data;
    try {
        const result = await db.query(
            `INSERT INTO notifikasi (pegawai_id, judul, pesan, tipe, kategori, is_admin, link)
             VALUES ($1, $2, $3, $4, $5, $6, $7) RETURNING *`,
            [pegawai_id, judul, pesan, tipe, kategori, is_admin, link]
        );
        const notif = result.rows[0];

        // Emit ke Socket.IO
        if (is_admin) {
            io.emit('admin-notification', notif);
        } else if (pegawai_id) {
            io.emit(`user-notification-${pegawai_id}`, notif);
        }
        io.emit('new-notification', notif);

        return notif;
    } catch (error) {
        console.error('Error creating notification:', error);
        return null;
    }
}

// --- MQTT BROKER EVENT HANDLERS --- (already defined above with connectedMqttClients tracking)

mqttBroker.on('publish', (packet, client) => {
    if (!client) return; // Ignore broker messages

    const topic = packet.topic;
    const message = packet.payload.toString();

    console.log(`📥 MQTT Message diterima dari ${client.id}`);
    console.log(`   Topic: ${topic}`);
    console.log(`   Message: ${message}`);

    // Handle data absensi dari ESP32
    if (topic === 'absensi/data') {
        try {
            const data = JSON.parse(message);
            handleAbsensiData(data);
        } catch (error) {
            console.error('❌ Error parsing MQTT message:', error.message);
        }
    }
});

// Fungsi untuk handle data absensi (digunakan oleh HTTP dan MQTT)
function handleAbsensiData(data) {
    const { pegawai_id, waktu_absen, keterangan, tipe_absen } = data;

    console.log('📥 Data absensi diterima:');
    console.log(`   ID Pegawai: ${pegawai_id}`);
    console.log(`   Waktu: ${waktu_absen}`);
    console.log(`   Keterangan: ${keterangan}`);
    console.log(`   Tipe: ${tipe_absen || 'Masuk (default)'}`);

    // Validasi data
    if (!pegawai_id || !waktu_absen) {
        console.error('❌ Data tidak lengkap');
        return;
    }

    // Tentukan tipe absen (default: Masuk)
    const tipe = tipe_absen || 'Masuk';

    if (tipe === 'Masuk') {
        // Handle absen masuk
        attendanceLogic.handleAbsenMasuk(db, { pegawai_id, waktu_absen, keterangan }, (err, result) => {
            if (err) {
                console.error('❌ Error menyimpan absen masuk:', err.message);
                return;
            }

            console.log('✅ Absen MASUK berhasil disimpan');

            // Ambil data lengkap untuk broadcast
            const insertedId = result.rows[0].id_absensi;
            broadcastAbsensiUpdate(insertedId);
        });

    } else if (tipe === 'Keluar') {
        // Handle absen keluar
        attendanceLogic.handleAbsenKeluar(db, { pegawai_id, waktu_keluar: waktu_absen }, (err, result) => {
            if (err) {
                console.error('❌ Error absen keluar:', err.message);
                return;
            }

            console.log('✅ Absen KELUAR berhasil diupdate');

            const absensi = result.rows[0];
            console.log(`   Durasi Kerja: ${attendanceLogic.formatDurasi(absensi.durasi_kerja)}`);
            console.log(`   Durasi Lembur: ${attendanceLogic.formatDurasi(absensi.durasi_lembur)}`);
            console.log(`   Status Lembur: ${absensi.status_lembur}`);

            // Broadcast update
            broadcastAbsensiUpdate(absensi.id_absensi);
        });
    }
}

// Fungsi helper untuk broadcast update via Socket.IO
function broadcastAbsensiUpdate(id_absensi) {
    const selectQuery = `
        SELECT a.*, p.nama_pegawai, p.nip
        FROM absensi a
        JOIN pegawai p ON a.pegawai_id = p.id_pegawai
        WHERE a.id_absensi = $1
    `;

    db.query(selectQuery, [id_absensi], (err, result) => {
        if (err) {
            console.error('Error mengambil data pegawai:', err.message);
        } else if (result.rows.length > 0) {
            const data = result.rows[0];

            // Kirim update real-time ke semua client
            io.emit('data_update', {
                nama: data.nama_pegawai,
                nip: data.nip,
                waktu_absen: data.waktu_absen,
                waktu_keluar: data.waktu_keluar,
                keterangan: data.keterangan,
                tipe_absen: data.tipe_absen,
                durasi_kerja: data.durasi_kerja,
                durasi_lembur: data.durasi_lembur,
                status_lembur: data.status_lembur
            });

            // Buat notifikasi untuk admin
            const tipeAbsen = data.waktu_keluar ? 'Keluar' : 'Masuk';
            const statusText = data.keterangan === 'Terlambat' ? ' (Terlambat)' : '';
            createNotification({
                pegawai_id: null,
                judul: `Absensi ${tipeAbsen}`,
                pesan: `${data.nama_pegawai} absen ${tipeAbsen.toLowerCase()} pukul ${data.waktu_absen || data.waktu_keluar}${statusText}`,
                tipe: data.keterangan === 'Terlambat' ? 'warning' : 'success',
                kategori: 'absensi',
                is_admin: true,
                link: '/riwayat-glass.html'
            });

            console.log('🔄 Data dikirim ke semua client via Socket.IO');
        }
    });
}

// Start MQTT Server
mqttServer.listen(mqttPort, () => {
    console.log(`🚀 MQTT Broker berjalan pada port ${mqttPort}`);
});

io.on('connection', (socket) => {
    console.log('🔌 Client website terhubung.');
    const initialQuery = `SELECT a.*, p.nama_pegawai
                          FROM absensi a
                          JOIN pegawai p ON a.pegawai_id = p.id_pegawai
                          ORDER BY a.timestamp DESC LIMIT 10`;
    db.query(initialQuery, (err, result) => {
        if (err) {
            console.error("Error mengambil data awal:", err.message);
        } else {
            socket.emit('initial_data', result.rows);
        }
    });
});

// Endpoint untuk menerima data absensi dari ESP32 (HTTP - Backward Compatibility)
app.post('/data', (req, res) => {
    const { pegawai_id, waktu_absen, keterangan } = req.body;

    console.log('📥 Data absensi diterima via HTTP dari ESP32');

    // Validasi data
    if (!pegawai_id || !waktu_absen) {
        return res.status(400).json({
            success: false,
            message: 'Data tidak lengkap'
        });
    }

    // Gunakan fungsi handleAbsensiData yang sama dengan MQTT
    handleAbsensiData(req.body);

    res.json({
        success: true,
        message: 'Data absensi berhasil disimpan (via HTTP)'
    });
});

// Endpoint untuk mendapatkan data pegawai (untuk ESP32 atau web)
app.get('/pegawai', (req, res) => {
    const query = 'SELECT * FROM pegawai ORDER BY nama_pegawai';

    db.query(query, (err, result) => {
        if (err) {
            console.error('Error mengambil data pegawai:', err.message);
            return res.status(500).json({ success: false, message: 'Gagal mengambil data' });
        }
        res.json({ success: true, data: result.rows });
    });
});

// Endpoint untuk tambah pegawai baru
app.post('/pegawai/add', async (req, res) => {
    const { nip, nama_pegawai, posisi, email, no_telepon, tanggal_bergabung } = req.body;

    console.log('📝 Request tambah pegawai:', req.body);

    // Validasi
    if (!nip || !nama_pegawai) {
        return res.status(400).json({
            success: false,
            message: 'NIP dan Nama Pegawai wajib diisi'
        });
    }

    try {
        // Cek apakah NIP sudah ada
        const checkQuery = 'SELECT id_pegawai FROM pegawai WHERE nip = $1';
        const checkResult = await db.query(checkQuery, [nip]);

        if (checkResult.rows.length > 0) {
            return res.status(400).json({
                success: false,
                message: 'NIP sudah terdaftar!'
            });
        }

        // Insert pegawai baru
        const insertQuery = `
            INSERT INTO pegawai (nip, nama_pegawai, posisi, email, no_telepon, tanggal_bergabung, status)
            VALUES ($1, $2, $3, $4, $5, $6, 'Aktif')
            RETURNING *
        `;

        const values = [
            nip,
            nama_pegawai,
            posisi || 'Karyawan',
            email || null,
            no_telepon || null,
            tanggal_bergabung || new Date()
        ];

        const result = await db.query(insertQuery, values);
        const newPegawai = result.rows[0];

        console.log('✅ Pegawai baru berhasil ditambahkan:', newPegawai);

        res.json({
            success: true,
            message: 'Pegawai berhasil ditambahkan',
            data: newPegawai
        });

    } catch (error) {
        console.error('❌ Error tambah pegawai:', error);
        res.status(500).json({
            success: false,
            message: 'Gagal menambahkan pegawai: ' + error.message
        });
    }
});

// Endpoint untuk trigger enrollment fingerprint via MQTT
app.post('/fingerprint/enroll', async (req, res) => {
    const { pegawai_id } = req.body;

    console.log('🔐 Request enrollment fingerprint untuk pegawai_id:', pegawai_id);

    if (!pegawai_id) {
        return res.status(400).json({
            success: false,
            message: 'Pegawai ID wajib diisi'
        });
    }

    try {
        // Ambil data pegawai
        const query = 'SELECT * FROM pegawai WHERE id_pegawai = $1';
        const result = await db.query(query, [pegawai_id]);

        if (result.rows.length === 0) {
            return res.status(404).json({
                success: false,
                message: 'Pegawai tidak ditemukan'
            });
        }

        const pegawai = result.rows[0];

        // Cari fingerprint_id yang tersedia (1-127)
        const usedIdsQuery = 'SELECT fingerprint_id FROM pegawai WHERE fingerprint_id IS NOT NULL';
        const usedIdsResult = await db.query(usedIdsQuery);
        const usedIds = usedIdsResult.rows.map(row => row.fingerprint_id);

        // Cari ID yang belum dipakai (1-127)
        let availableId = null;
        for (let i = 1; i <= 127; i++) {
            if (!usedIds.includes(i)) {
                availableId = i;
                break;
            }
        }

        if (!availableId) {
            return res.status(400).json({
                success: false,
                message: 'Tidak ada slot fingerprint tersedia (max 127)'
            });
        }

        // Kirim command ke ESP32 via MQTT
        const enrollPayload = {
            command: 'ENROLL',
            fingerprint_id: availableId,
            pegawai_id: pegawai.id_pegawai,
            nip: pegawai.nip,
            nama: pegawai.nama_pegawai
        };

        mqttClient.publish('fingerprint/enroll/request', JSON.stringify(enrollPayload));

        console.log('📤 MQTT command sent:', enrollPayload);

        res.json({
            success: true,
            message: 'Perintah enrollment dikirim ke ESP32',
            data: {
                fingerprint_id: availableId,
                pegawai_id: pegawai.id_pegawai,
                nama: pegawai.nama_pegawai
            }
        });

    } catch (error) {
        console.error('❌ Error enrollment:', error);
        res.status(500).json({
            success: false,
            message: 'Gagal memulai enrollment: ' + error.message
        });
    }
});

// ==== FINGERPRINT ENROLLMENT API ====

// Endpoint untuk memulai proses enrollment fingerprint
app.post('/api/enroll-fingerprint', async (req, res) => {
    const { pegawai_id, nama, nip } = req.body;

    if (!pegawai_id || !nama || !nip) {
        return res.status(400).json({
            success: false,
            message: 'Data pegawai tidak lengkap'
        });
    }

    try {
        // Cari fingerprint ID berikutnya yang available
        const queryUsed = 'SELECT fingerprint_id FROM pegawai WHERE fingerprint_id IS NOT NULL ORDER BY fingerprint_id';
        const usedIdsResult = await db.query(queryUsed);
        const usedIds = usedIdsResult.rows.map(row => row.fingerprint_id);

        // Cari ID berikutnya yang tersedia (mulai dari 1)
        let nextId = 1;
        while (usedIds.includes(nextId)) {
            nextId++;
        }

        console.log('🔐 Starting fingerprint enrollment:');
        console.log('   Pegawai ID:', pegawai_id);
        console.log('   Nama:', nama);
        console.log('   NIP:', nip);
        console.log('   Fingerprint ID akan di-assign:', nextId);

        // Publish MQTT message ke ESP32 untuk memulai enrollment
        const enrollmentData = {
            command: 'ENROLL',
            fingerprint_id: nextId,
            pegawai_id: parseInt(pegawai_id),
            nama: nama,
            nip: nip
        };

        mqttClient.publish('fingerprint/enroll/request', JSON.stringify(enrollmentData), (err) => {
            if (err) {
                console.error('❌ Error publishing enrollment request:', err);
                return res.status(500).json({
                    success: false,
                    message: 'Gagal mengirim request enrollment ke ESP32'
                });
            }

            console.log('✅ Enrollment request sent to ESP32');
            res.json({
                success: true,
                message: 'Proses enrollment dimulai',
                fingerprint_id: nextId,
                pegawai_id: pegawai_id
            });
        });

    } catch (error) {
        console.error('❌ Error starting enrollment:', error);
        res.status(500).json({
            success: false,
            message: error.message
        });
    }
});

// ==== TEST ENDPOINTS FOR DEBUGGING ====

// Endpoint untuk test publish MQTT (debugging)
app.post('/api/test-mqtt-publish', (req, res) => {
    const { topic, payload } = req.body;

    if (!topic || !payload) {
        return res.status(400).json({
            success: false,
            message: 'Topic and payload required'
        });
    }

    console.log('🧪 [TEST] Publishing MQTT message:');
    console.log('   Topic:', topic);
    console.log('   Payload:', payload);

    try {
        mqttClient.publish(topic, JSON.stringify(payload), (err) => {
            if (err) {
                console.error('❌ [TEST] MQTT publish error:', err);
                return res.status(500).json({
                    success: false,
                    message: 'Failed to publish: ' + err.message
                });
            }

            console.log('✅ [TEST] MQTT message published successfully');
            res.json({
                success: true,
                message: 'MQTT message published',
                data: { topic, payload }
            });
        });
    } catch (error) {
        console.error('❌ [TEST] Error:', error);
        res.status(500).json({
            success: false,
            message: error.message
        });
    }
});

// Endpoint untuk cek status MQTT clients
app.get('/api/mqtt-status', (req, res) => {
    const clients = Array.from(connectedMqttClients);

    // Check if ESP32 is connected (client ID biasanya mengandung "ESP")
    const esp32Connected = clients.some(id => id.toLowerCase().includes('esp') || id.toLowerCase().includes('fingerprint'));

    res.json({
        success: true,
        broker: 'active',
        esp32: esp32Connected ? 'online' : 'offline',
        clients: clients,
        count: clients.length
    });
});

// ============================================
// SETTINGS API ENDPOINTS
// ============================================

// Endpoint untuk export data absensi (CSV/JSON)
app.get('/api/export', async (req, res) => {
    const format = req.query.format || 'csv';
    const startDate = req.query.startDate;
    const endDate = req.query.endDate;

    try {
        let query = `
            SELECT
                a.id_absensi,
                p.nip,
                p.nama_pegawai,
                p.posisi,
                a.waktu_absen,
                a.waktu_keluar,
                a.keterangan,
                a.durasi_kerja,
                a.durasi_lembur,
                a.status_lembur,
                a.timestamp
            FROM absensi a
            JOIN pegawai p ON a.pegawai_id = p.id_pegawai
        `;

        const params = [];
        if (startDate && endDate) {
            query += ' WHERE DATE(a.timestamp) BETWEEN $1 AND $2';
            params.push(startDate, endDate);
        }

        query += ' ORDER BY a.timestamp DESC';

        const result = await db.query(query, params);

        if (format === 'json') {
            res.setHeader('Content-Type', 'application/json');
            res.setHeader('Content-Disposition', `attachment; filename=absensi_${new Date().toISOString().split('T')[0]}.json`);
            res.json(result.rows);
        } else {
            // CSV format
            let csv = 'ID,NIP,Nama,Posisi,Waktu Masuk,Waktu Keluar,Keterangan,Durasi Kerja,Durasi Lembur,Status Lembur,Timestamp\n';

            result.rows.forEach(row => {
                csv += `${row.id_absensi},${row.nip},"${row.nama_pegawai}",${row.posisi || ''},${row.waktu_absen || ''},${row.waktu_keluar || ''},${row.keterangan || ''},${row.durasi_kerja || ''},${row.durasi_lembur || ''},${row.status_lembur || ''},${row.timestamp}\n`;
            });

            res.setHeader('Content-Type', 'text/csv');
            res.setHeader('Content-Disposition', `attachment; filename=absensi_${new Date().toISOString().split('T')[0]}.csv`);
            res.send(csv);
        }

        console.log(`📊 Data exported as ${format.toUpperCase()} (${result.rows.length} records)`);

    } catch (error) {
        console.error('❌ Error exporting data:', error);
        res.status(500).json({ success: false, message: 'Gagal export data: ' + error.message });
    }
});

// Endpoint untuk reset semua fingerprint
app.post('/api/reset-fingerprints', async (req, res) => {
    try {
        // 1. Reset fingerprint_id di database
        const updateQuery = 'UPDATE pegawai SET fingerprint_id = NULL WHERE fingerprint_id IS NOT NULL';
        const result = await db.query(updateQuery);

        console.log(`🗑️ Reset ${result.rowCount} fingerprint IDs in database`);

        // 2. Kirim command ke ESP32 via MQTT untuk menghapus semua fingerprint
        const resetPayload = {
            command: 'RESET_ALL',
            timestamp: new Date().toISOString()
        };

        mqttClient.publish('fingerprint/command', JSON.stringify(resetPayload), (err) => {
            if (err) {
                console.error('⚠️ Warning: Failed to send reset command to ESP32:', err);
            } else {
                console.log('📤 Reset command sent to ESP32');
            }
        });

        res.json({
            success: true,
            message: `${result.rowCount} fingerprint berhasil direset`,
            count: result.rowCount
        });

    } catch (error) {
        console.error('❌ Error resetting fingerprints:', error);
        res.status(500).json({ success: false, message: 'Gagal reset fingerprint: ' + error.message });
    }
});

// Endpoint untuk ubah password admin
app.post('/api/change-password', async (req, res) => {
    const { oldPassword, newPassword } = req.body;

    if (!oldPassword || !newPassword) {
        return res.status(400).json({ success: false, message: 'Password lama dan baru harus diisi' });
    }

    if (newPassword.length < 6) {
        return res.status(400).json({ success: false, message: 'Password minimal 6 karakter' });
    }

    try {
        // Baca file settings atau gunakan default
        const settingsPath = path.join(__dirname, 'admin-settings.json');
        let settings = { username: 'admin', password: 'admin123' };

        if (fs.existsSync(settingsPath)) {
            settings = JSON.parse(fs.readFileSync(settingsPath, 'utf8'));
        }

        // Verifikasi password lama
        if (oldPassword !== settings.password) {
            return res.status(401).json({ success: false, message: 'Password lama salah' });
        }

        // Simpan password baru
        settings.password = newPassword;
        settings.lastChanged = new Date().toISOString();

        fs.writeFileSync(settingsPath, JSON.stringify(settings, null, 2));

        console.log('🔐 Admin password changed successfully');

        res.json({ success: true, message: 'Password berhasil diubah' });

    } catch (error) {
        console.error('❌ Error changing password:', error);
        res.status(500).json({ success: false, message: 'Gagal mengubah password: ' + error.message });
    }
});

// Endpoint untuk backup manual ke OwnCloud
app.post('/api/backup-owncloud', async (req, res) => {
    try {
        const client = createClient(
            ownCloudConfig.url,
            {
                username: ownCloudConfig.username,
                password: ownCloudConfig.password
            }
        );

        // Test koneksi ke OwnCloud
        try {
            await client.getDirectoryContents('/');
        } catch (connErr) {
            return res.status(503).json({
                success: false,
                message: 'Tidak dapat terhubung ke OwnCloud. Pastikan OwnCloud berjalan di ' + ownCloudConfig.url
            });
        }

        // Ambil data absensi
        const query = `
            SELECT a.*, p.nama_pegawai, p.nip
            FROM absensi a
            JOIN pegawai p ON a.pegawai_id = p.id_pegawai
            ORDER BY a.timestamp DESC
        `;
        const result = await db.query(query);

        // Buat file CSV
        const timestamp = moment().format('YYYY-MM-DD_HH-mm-ss');
        const fileName = `Backup-Absensi-${timestamp}.csv`;

        let csvContent = 'ID,NIP,Nama,Waktu Masuk,Waktu Keluar,Keterangan,Durasi Kerja,Durasi Lembur,Status Lembur,Timestamp\n';
        result.rows.forEach(row => {
            csvContent += `${row.id_absensi},${row.nip},"${row.nama_pegawai}",${row.waktu_absen || ''},${row.waktu_keluar || ''},${row.keterangan || ''},${row.durasi_kerja || ''},${row.durasi_lembur || ''},${row.status_lembur || ''},${row.timestamp}\n`;
        });

        // Buat direktori backup jika belum ada
        try {
            await client.createDirectory('/Backup-Absensi/');
        } catch (e) {
            // Directory mungkin sudah ada
        }

        // Upload ke OwnCloud
        const remotePath = `/Backup-Absensi/${fileName}`;
        await client.putFileContents(remotePath, csvContent);

        console.log(`☁️ Backup uploaded to OwnCloud: ${remotePath}`);

        res.json({
            success: true,
            message: 'Backup berhasil diupload ke OwnCloud',
            path: remotePath,
            records: result.rows.length
        });

    } catch (error) {
        console.error('❌ Error backing up to OwnCloud:', error);
        res.status(500).json({ success: false, message: 'Gagal backup ke OwnCloud: ' + error.message });
    }
});

// Endpoint untuk cek status OwnCloud
app.get('/api/owncloud-status', async (req, res) => {
    try {
        const client = createClient(
            ownCloudConfig.url,
            {
                username: ownCloudConfig.username,
                password: ownCloudConfig.password
            }
        );

        // Test koneksi
        await client.getDirectoryContents('/');

        res.json({
            success: true,
            status: 'online',
            url: ownCloudConfig.url
        });

    } catch (error) {
        res.json({
            success: true,
            status: 'offline',
            url: ownCloudConfig.url,
            error: error.message
        });
    }
});

// Endpoint untuk simpan settings ke server
app.post('/api/settings', async (req, res) => {
    try {
        const settings = req.body;
        const settingsPath = path.join(__dirname, 'app-settings.json');

        fs.writeFileSync(settingsPath, JSON.stringify(settings, null, 2));

        console.log('⚙️ Settings saved to server');

        res.json({ success: true, message: 'Settings berhasil disimpan' });

    } catch (error) {
        console.error('❌ Error saving settings:', error);
        res.status(500).json({ success: false, message: 'Gagal menyimpan settings: ' + error.message });
    }
});

// Endpoint untuk load settings dari server
app.get('/api/settings', async (req, res) => {
    try {
        const settingsPath = path.join(__dirname, 'app-settings.json');

        if (fs.existsSync(settingsPath)) {
            const settings = JSON.parse(fs.readFileSync(settingsPath, 'utf8'));
            res.json({ success: true, data: settings });
        } else {
            // Return default settings
            res.json({
                success: true,
                data: {
                    companyName: 'PT. Contoh Indonesia',
                    timezone: 'Asia/Jakarta',
                    dateFormat: 'DD/MM/YYYY',
                    workStart: '08:00',
                    workEnd: '17:00',
                    lateTolerance: 15,
                    overtimeStart: '18:00',
                    notifyRealtime: true,
                    notifyLate: true,
                    notifyOvertime: true,
                    notifySound: true,
                    autoBackup: true,
                    archivePeriod: '90'
                }
            });
        }

    } catch (error) {
        console.error('❌ Error loading settings:', error);
        res.status(500).json({ success: false, message: 'Gagal memuat settings: ' + error.message });
    }
});

// ============================================
// ENDPOINTS UNTUK EMPLOYEE PORTAL
// ============================================

// Endpoint untuk login karyawan (autentikasi dengan NIP)
app.post('/api/employee/login', async (req, res) => {
    const { nip } = req.body;

    if (!nip) {
        return res.status(400).json({
            success: false,
            message: 'NIP wajib diisi'
        });
    }

    try {
        const query = 'SELECT * FROM pegawai WHERE nip = $1';
        const result = await db.query(query, [nip]);

        if (result.rows.length === 0) {
            return res.status(404).json({
                success: false,
                message: 'NIP tidak ditemukan'
            });
        }

        const pegawai = result.rows[0];

        // Check if employee is active
        if (pegawai.status && pegawai.status.toLowerCase() !== 'aktif') {
            return res.status(403).json({
                success: false,
                message: 'Akun tidak aktif. Hubungi administrator.'
            });
        }

        res.json({
            success: true,
            message: 'Login berhasil',
            data: {
                id_pegawai: pegawai.id_pegawai,
                nip: pegawai.nip,
                nama_pegawai: pegawai.nama_pegawai,
                posisi: pegawai.posisi,
                email: pegawai.email,
                fingerprint_id: pegawai.fingerprint_id,
                tanggal_bergabung: pegawai.tanggal_bergabung
            }
        });

    } catch (error) {
        console.error('❌ Error login karyawan:', error);
        res.status(500).json({
            success: false,
            message: 'Gagal login: ' + error.message
        });
    }
});

// Endpoint untuk mendapatkan data karyawan berdasarkan ID
app.get('/api/employee/:id', async (req, res) => {
    const { id } = req.params;

    try {
        const query = 'SELECT * FROM pegawai WHERE id_pegawai = $1';
        const result = await db.query(query, [id]);

        if (result.rows.length === 0) {
            return res.status(404).json({
                success: false,
                message: 'Karyawan tidak ditemukan'
            });
        }

        const pegawai = result.rows[0];

        res.json({
            success: true,
            data: {
                id_pegawai: pegawai.id_pegawai,
                nip: pegawai.nip,
                nama_pegawai: pegawai.nama_pegawai,
                posisi: pegawai.posisi,
                email: pegawai.email,
                no_telepon: pegawai.no_telepon,
                fingerprint_id: pegawai.fingerprint_id,
                tanggal_bergabung: pegawai.tanggal_bergabung,
                status: pegawai.status
            }
        });

    } catch (error) {
        console.error('❌ Error mendapatkan data karyawan:', error);
        res.status(500).json({
            success: false,
            message: 'Gagal mendapatkan data: ' + error.message
        });
    }
});

// Endpoint untuk mendapatkan riwayat absensi karyawan
app.get('/api/employee/:id/attendance', async (req, res) => {
    const { id } = req.params;
    const { limit = 30, offset = 0 } = req.query;

    try {
        // Get attendance records
        const query = `
            SELECT
                id_absensi,
                pegawai_id,
                waktu_absen,
                waktu_keluar,
                keterangan,
                tipe_absen,
                durasi_kerja,
                durasi_lembur,
                status_lembur,
                timestamp,
                DATE(timestamp) as tanggal
            FROM absensi
            WHERE pegawai_id = $1
            ORDER BY timestamp DESC
            LIMIT $2 OFFSET $3
        `;

        const result = await db.query(query, [id, parseInt(limit), parseInt(offset)]);

        // Get total count
        const countQuery = 'SELECT COUNT(*) as total FROM absensi WHERE pegawai_id = $1';
        const countResult = await db.query(countQuery, [id]);
        const total = countResult.rows[0].total;

        res.json({
            success: true,
            data: result.rows,
            pagination: {
                total: parseInt(total),
                limit: parseInt(limit),
                offset: parseInt(offset)
            }
        });

    } catch (error) {
        console.error('❌ Error mendapatkan riwayat absensi:', error);
        res.status(500).json({
            success: false,
            message: 'Gagal mendapatkan riwayat absensi: ' + error.message
        });
    }
});

// Endpoint untuk statistik absensi karyawan (bulan ini)
app.get('/api/employee/:id/stats', async (req, res) => {
    const { id } = req.params;

    try {
        const query = `
            SELECT
                COUNT(*) as total_hadir,
                COUNT(CASE WHEN waktu_keluar IS NOT NULL THEN 1 END) as dengan_absen_keluar,
                COUNT(CASE WHEN keterangan = 'Terlambat' THEN 1 END) as terlambat,
                SUM(CASE WHEN status_lembur = 'Ya' THEN 1 ELSE 0 END) as total_lembur
            FROM absensi
            WHERE pegawai_id = $1
            AND EXTRACT(MONTH FROM timestamp) = EXTRACT(MONTH FROM CURRENT_DATE)
            AND EXTRACT(YEAR FROM timestamp) = EXTRACT(YEAR FROM CURRENT_DATE)
        `;

        const result = await db.query(query, [id]);

        res.json({
            success: true,
            data: result.rows[0]
        });

    } catch (error) {
        console.error('❌ Error mendapatkan statistik:', error);
        res.status(500).json({
            success: false,
            message: 'Gagal mendapatkan statistik: ' + error.message
        });
    }
});

// Endpoint untuk mendapatkan riwayat absensi
app.get('/absensi', (req, res) => {
    const limit = req.query.limit || 50;
    const query = `
        SELECT a.*, p.nama_pegawai
        FROM absensi a
        JOIN pegawai p ON a.pegawai_id = p.id_pegawai
        ORDER BY a.timestamp DESC
        LIMIT $1
    `;

    db.query(query, [parseInt(limit)], (err, result) => {
        if (err) {
            console.error('Error mengambil riwayat absensi:', err.message);
            return res.status(500).json({ success: false, message: 'Gagal mengambil data' });
        }
        res.json({ success: true, data: result.rows });
    });
});

// Endpoint untuk absensi manual (Masuk/Keluar)
app.post('/absensi/manual', (req, res) => {
    const { pegawai_id, waktu_absen, keterangan, tipe_absen } = req.body;

    if (!pegawai_id || !waktu_absen || !tipe_absen) {
        return res.status(400).json({
            success: false,
            message: 'Data tidak lengkap. Mohon isi pegawai_id, waktu_absen, dan tipe_absen'
        });
    }

    const tipe = tipe_absen || 'Masuk';

    if (tipe === 'Masuk') {
        // Handle Absen Masuk
        attendanceLogic.handleAbsenMasuk(db, {
            pegawai_id,
            waktu_absen,
            keterangan: keterangan || 'Hadir'
        }, (err, result) => {
            if (err) {
                console.error('❌ Error absen masuk manual:', err.message);
                return res.status(500).json({
                    success: false,
                    message: 'Gagal menyimpan absen masuk: ' + err.message
                });
            }

            console.log(`✅ Absen MASUK manual berhasil - Pegawai ID: ${pegawai_id}`);

            // Broadcast via Socket.IO
            const id_absensi = result.rows[0].id_absensi;
            broadcastAbsensiUpdate(id_absensi);

            res.json({
                success: true,
                message: 'Absen masuk berhasil dicatat',
                data: result.rows[0]
            });
        });

    } else if (tipe === 'Keluar') {
        // Handle Absen Keluar
        attendanceLogic.handleAbsenKeluar(db, {
            pegawai_id,
            waktu_keluar: waktu_absen
        }, (err, result) => {
            if (err) {
                console.error('❌ Error absen keluar manual:', err.message);
                return res.status(500).json({
                    success: false,
                    message: err.message || 'Gagal menyimpan absen keluar'
                });
            }

            const absensi = result.rows[0];
            console.log(`✅ Absen KELUAR manual berhasil - Pegawai ID: ${pegawai_id}`);
            console.log(`   Durasi Kerja: ${attendanceLogic.formatDurasi(absensi.durasi_kerja)}`);
            console.log(`   Durasi Lembur: ${attendanceLogic.formatDurasi(absensi.durasi_lembur)}`);
            console.log(`   Status Lembur: ${absensi.status_lembur}`);

            // Broadcast via Socket.IO
            broadcastAbsensiUpdate(absensi.id_absensi);

            res.json({
                success: true,
                message: 'Absen keluar berhasil dicatat',
                data: absensi,
                overtime: absensi.status_lembur === 'Ya' ? attendanceLogic.formatDurasi(absensi.durasi_lembur) : null
            });
        });

    } else {
        return res.status(400).json({
            success: false,
            message: 'tipe_absen harus "Masuk" atau "Keluar"'
        });
    }
});

// Endpoint untuk statistik absensi hari ini
app.get('/stats/today', (req, res) => {
    const query = `
        SELECT
            COUNT(DISTINCT pegawai_id) as jumlah_hadir,
            (SELECT COUNT(*) FROM pegawai) as total_pegawai
        FROM absensi
        WHERE DATE(timestamp) = CURRENT_DATE
    `;

    db.query(query, (err, result) => {
        if (err) {
            console.error('Error mengambil statistik:', err.message);
            return res.status(500).json({ success: false, message: 'Gagal mengambil statistik' });
        }
        res.json({
            success: true,
            data: {
                hadir: result.rows[0].jumlah_hadir,
                total: result.rows[0].total_pegawai
            }
        });
    });
});

// Endpoint untuk mendapatkan total absensi per karyawan
app.get('/stats/employees', (req, res) => {
    const query = `
        SELECT
            p.id_pegawai,
            p.nama_pegawai,
            p.nip,
            p.posisi,
            p.fingerprint_id,
            COUNT(a.id_absensi) as total_absensi,
            MAX(a.timestamp) as absensi_terakhir
        FROM pegawai p
        LEFT JOIN absensi a ON p.id_pegawai = a.pegawai_id
        GROUP BY p.id_pegawai, p.nama_pegawai, p.nip, p.posisi, p.fingerprint_id
        ORDER BY total_absensi DESC, p.nama_pegawai ASC
    `;

    db.query(query, (err, result) => {
        if (err) {
            console.error('Error mengambil statistik karyawan:', err.message);
            return res.status(500).json({ success: false, message: 'Gagal mengambil statistik karyawan' });
        }
        res.json({
            success: true,
            data: result.rows
        });
    });
});


// Fungsi untuk membuat dan mengarsipkan data
async function createAndArchiveData() {
    console.log('🗂️  Memulai proses arsip...');

    try {
        // Ambil data absensi bulan lalu
        const lastMonth = moment().subtract(1, 'month');
        const tahun = lastMonth.year();
        const bulan = lastMonth.format('MM');
        const namaBulan = lastMonth.format('MMMM-YYYY');

        const query = `
            SELECT a.*, p.nama_pegawai, p.nip
            FROM absensi a
            JOIN pegawai p ON a.pegawai_id = p.id_pegawai
            WHERE EXTRACT(YEAR FROM a.timestamp)::INTEGER = $1::INTEGER AND EXTRACT(MONTH FROM a.timestamp)::INTEGER = $2::INTEGER
            ORDER BY a.timestamp DESC
        `;

        db.query(query, [parseInt(tahun), parseInt(bulan)], async (err, result) => {
            if (err) {
                console.error('❌ Error mengambil data untuk arsip:', err.message);
                return;
            }

            if (result.rows.length === 0) {
                console.log('ℹ️  Tidak ada data untuk diarsipkan');
                return;
            }

            // Buat CSV
            let csvContent = 'Nama Pegawai,NIP,Waktu Absensi,Keterangan,Timestamp\n';
            result.rows.forEach(row => {
                csvContent += `${row.nama_pegawai},${row.nip},${row.waktu_absen},${row.keterangan},${row.timestamp}\n`;
            });

            const namaFile = `Absensi-${namaBulan}.csv`;
            const pathFile = path.join(__dirname, namaFile);

            // Simpan file CSV sementara
            fs.writeFileSync(pathFile, csvContent);
            console.log(`✅ File CSV dibuat: ${namaFile}`);

            // Upload ke OwnCloud
            try {
                const client = createClient(
                    ownCloudConfig.url,
                    {
                        username: ownCloudConfig.username,
                        password: ownCloudConfig.password
                    }
                );

                const fileBuffer = fs.readFileSync(pathFile);
                const remotePath = `/Arsip-Absensi/${namaFile}`;

                // Buat direktori jika belum ada
                try {
                    await client.createDirectory('/Arsip-Absensi/');
                } catch (e) {
                    // Direktori mungkin sudah ada, abaikan error
                }

                await client.putFileContents(remotePath, fileBuffer);
                console.log(`☁️  File berhasil diupload ke OwnCloud: ${remotePath}`);

                // Hapus file lokal setelah upload
                fs.unlinkSync(pathFile);
                console.log('🗑️  File lokal dihapus');

            } catch (uploadErr) {
                console.error('❌ Error upload ke OwnCloud:', uploadErr.message);
            }
        });

    } catch (error) {
        console.error('❌ Error dalam proses arsip:', error.message);
    }
}

cron.schedule('5 0 * * *', createAndArchiveData, { timezone: "Asia/Jakarta" });
console.log('Penjadwalan arsip telah diatur.');

app.get('/test-archive', (req, res) => {
    console.log("Memicu fungsi arsip secara manual...");
    createAndArchiveData();
    res.send("Proses pengarsipan manual telah dipicu. Cek konsol server dan OwnCloud Anda.");
});

// ============================================
// API ENDPOINTS - PENGAJUAN CUTI/IZIN
// ============================================

// Get semua pengajuan cuti
app.get('/api/cuti', async (req, res) => {
    try {
        const query = `
            SELECT c.*, p.nama_pegawai, p.nip, p.posisi,
                   ap.nama_pegawai as approver_nama
            FROM pengajuan_cuti c
            JOIN pegawai p ON c.pegawai_id = p.id_pegawai
            LEFT JOIN pegawai ap ON c.disetujui_oleh = ap.id_pegawai
            ORDER BY c.created_at DESC
        `;
        const result = await db.query(query);
        res.json({ success: true, data: result.rows });
    } catch (error) {
        console.error('Error get cuti:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Get pengajuan cuti terbaru (untuk notifikasi)
app.get('/api/cuti/recent', async (req, res) => {
    try {
        const query = `
            SELECT c.*, p.nama_pegawai, p.nip
            FROM pengajuan_cuti c
            JOIN pegawai p ON c.pegawai_id = p.id_pegawai
            WHERE c.status = 'Menunggu'
            ORDER BY c.created_at DESC
            LIMIT 10
        `;
        const result = await db.query(query);
        res.json({ success: true, data: result.rows });
    } catch (error) {
        console.error('Error get recent cuti:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Tambah pengajuan cuti
app.post('/api/cuti', async (req, res) => {
    const { pegawai_id, jenis, tanggal_mulai, tanggal_selesai, alasan } = req.body;

    if (!pegawai_id || !jenis || !tanggal_mulai || !tanggal_selesai) {
        return res.status(400).json({ success: false, message: 'Data tidak lengkap' });
    }

    try {
        const query = `
            INSERT INTO pengajuan_cuti (pegawai_id, jenis, tanggal_mulai, tanggal_selesai, alasan)
            VALUES ($1, $2, $3, $4, $5)
            RETURNING *
        `;
        const result = await db.query(query, [pegawai_id, jenis, tanggal_mulai, tanggal_selesai, alasan]);

        // Get nama pegawai
        const pegawaiResult = await db.query('SELECT nama_pegawai FROM pegawai WHERE id_pegawai = $1', [pegawai_id]);
        const namaPegawai = pegawaiResult.rows[0]?.nama_pegawai || 'Karyawan';

        // Emit notifikasi via socket
        io.emit('new_cuti_request', result.rows[0]);

        // Buat notifikasi untuk admin
        await createNotification({
            pegawai_id: null,
            judul: 'Pengajuan Cuti Baru',
            pesan: `${namaPegawai} mengajukan ${jenis} (${tanggal_mulai} s/d ${tanggal_selesai})`,
            tipe: 'info',
            kategori: 'cuti',
            is_admin: true,
            link: '/cuti-glass.html'
        });

        res.json({ success: true, data: result.rows[0], message: 'Pengajuan berhasil dikirim' });
    } catch (error) {
        console.error('Error add cuti:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Approve/Reject cuti
app.put('/api/cuti/:id/status', async (req, res) => {
    const { id } = req.params;
    const { status, disetujui_oleh, komentar } = req.body;

    if (!['Disetujui', 'Ditolak'].includes(status)) {
        return res.status(400).json({ success: false, message: 'Status tidak valid' });
    }

    try {
        const query = `
            UPDATE pengajuan_cuti
            SET status = $1, disetujui_oleh = $2, tanggal_disetujui = NOW(), komentar_approval = $3
            WHERE id_pengajuan = $4
            RETURNING *
        `;
        const result = await db.query(query, [status, disetujui_oleh, komentar, id]);

        if (result.rows.length === 0) {
            return res.status(404).json({ success: false, message: 'Pengajuan tidak ditemukan' });
        }

        const cutiData = result.rows[0];

        // Emit update
        io.emit('cuti_updated', cutiData);

        // Buat notifikasi untuk user yang mengajukan
        const statusEmoji = status === 'Disetujui' ? '✅' : '❌';
        const tipeNotif = status === 'Disetujui' ? 'success' : 'error';
        await createNotification({
            pegawai_id: cutiData.pegawai_id,
            judul: `Cuti ${status}`,
            pesan: `${statusEmoji} Pengajuan ${cutiData.jenis} Anda telah ${status.toLowerCase()}${komentar ? ': ' + komentar : ''}`,
            tipe: tipeNotif,
            kategori: 'cuti',
            is_admin: false,
            link: '/user-izin.html'
        });

        res.json({ success: true, data: cutiData, message: `Pengajuan ${status.toLowerCase()}` });
    } catch (error) {
        console.error('Error update cuti:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Delete cuti
app.delete('/api/cuti/:id', async (req, res) => {
    const { id } = req.params;

    try {
        const query = 'DELETE FROM pengajuan_cuti WHERE id_pengajuan = $1 RETURNING *';
        const result = await db.query(query, [id]);

        if (result.rows.length === 0) {
            return res.status(404).json({ success: false, message: 'Data cuti tidak ditemukan' });
        }

        io.emit('cuti_updated', { deleted: true, id });
        res.json({ success: true, message: 'Data cuti berhasil dihapus' });
    } catch (error) {
        console.error('Error delete cuti:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// ============================================
// API ENDPOINTS - MANAJEMEN KARYAWAN (Enhanced)
// ============================================

// Upload foto karyawan
const fotoStorage = multer.diskStorage({
    destination: (req, file, cb) => {
        const uploadDir = path.join(__dirname, 'uploads/foto');
        if (!fs.existsSync(uploadDir)) {
            fs.mkdirSync(uploadDir, { recursive: true });
        }
        cb(null, uploadDir);
    },
    filename: (req, file, cb) => {
        const ext = path.extname(file.originalname);
        cb(null, `foto-${Date.now()}${ext}`);
    }
});
const uploadFoto = multer({
    storage: fotoStorage,
    limits: { fileSize: 5 * 1024 * 1024 },
    fileFilter: (req, file, cb) => {
        const allowed = /\.(jpg|jpeg|png|gif)$/i;
        if (allowed.test(file.originalname)) {
            cb(null, true);
        } else {
            cb(new Error('Format file tidak diizinkan! Hanya JPG, PNG, GIF'));
        }
    }
});

app.post('/api/pegawai/:id/foto', uploadFoto.single('foto'), async (req, res) => {
    const { id } = req.params;

    if (!req.file) {
        return res.status(400).json({ success: false, message: 'File foto tidak ditemukan' });
    }

    try {
        const fotoPath = `/uploads/foto/${req.file.filename}`;
        const query = 'UPDATE pegawai SET foto = $1 WHERE id_pegawai = $2 RETURNING *';
        const result = await db.query(query, [fotoPath, id]);

        if (result.rows.length === 0) {
            fs.unlinkSync(req.file.path);
            return res.status(404).json({ success: false, message: 'Pegawai tidak ditemukan' });
        }

        res.json({ success: true, data: result.rows[0], message: 'Foto berhasil diupload' });
    } catch (error) {
        fs.unlinkSync(req.file.path);
        console.error('Error upload foto:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Update data pegawai
app.put('/api/pegawai/:id', async (req, res) => {
    const { id } = req.params;
    const { nama_pegawai, posisi, email, no_telepon, divisi, gaji_pokok, status } = req.body;

    try {
        const query = `
            UPDATE pegawai
            SET nama_pegawai = COALESCE($1, nama_pegawai),
                posisi = COALESCE($2, posisi),
                email = COALESCE($3, email),
                no_telepon = COALESCE($4, no_telepon),
                divisi = COALESCE($5, divisi),
                gaji_pokok = COALESCE($6, gaji_pokok),
                status = COALESCE($7, status)
            WHERE id_pegawai = $8
            RETURNING *
        `;
        const result = await db.query(query, [nama_pegawai, posisi, email, no_telepon, divisi, gaji_pokok, status, id]);

        if (result.rows.length === 0) {
            return res.status(404).json({ success: false, message: 'Pegawai tidak ditemukan' });
        }

        res.json({ success: true, data: result.rows[0], message: 'Data pegawai berhasil diupdate' });
    } catch (error) {
        console.error('Error update pegawai:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Update status pegawai
app.put('/api/pegawai/:id/status', async (req, res) => {
    const { id } = req.params;
    const { status } = req.body;

    if (!['Aktif', 'Nonaktif'].includes(status)) {
        return res.status(400).json({ success: false, message: 'Status tidak valid' });
    }

    try {
        const query = 'UPDATE pegawai SET status = $1 WHERE id_pegawai = $2 RETURNING *';
        const result = await db.query(query, [status, id]);

        if (result.rows.length === 0) {
            return res.status(404).json({ success: false, message: 'Pegawai tidak ditemukan' });
        }

        res.json({ success: true, data: result.rows[0], message: `Status pegawai diubah ke ${status}` });
    } catch (error) {
        console.error('Error update status:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Reset password pegawai
app.post('/api/pegawai/:id/reset-password', async (req, res) => {
    const { id } = req.params;
    const defaultPassword = 'password123';

    try {
        const query = 'UPDATE pegawai SET password = $1 WHERE id_pegawai = $2 RETURNING id_pegawai, nip, nama_pegawai';
        const result = await db.query(query, [defaultPassword, id]);

        if (result.rows.length === 0) {
            return res.status(404).json({ success: false, message: 'Pegawai tidak ditemukan' });
        }

        res.json({ success: true, message: 'Password berhasil direset ke default', default_password: defaultPassword });
    } catch (error) {
        console.error('Error reset password:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// User change password (for user profile page)
app.post('/api/user/change-password', async (req, res) => {
    const { pegawai_id, oldPassword, newPassword } = req.body;

    if (!pegawai_id || !oldPassword || !newPassword) {
        return res.status(400).json({ success: false, message: 'Data tidak lengkap' });
    }

    if (newPassword.length < 6) {
        return res.status(400).json({ success: false, message: 'Password minimal 6 karakter' });
    }

    try {
        // Verify old password
        const checkQuery = 'SELECT password FROM pegawai WHERE id_pegawai = $1';
        const checkResult = await db.query(checkQuery, [pegawai_id]);

        if (checkResult.rows.length === 0) {
            return res.status(404).json({ success: false, message: 'Pegawai tidak ditemukan' });
        }

        if (checkResult.rows[0].password !== oldPassword) {
            return res.status(401).json({ success: false, message: 'Password lama salah' });
        }

        // Update password
        const updateQuery = 'UPDATE pegawai SET password = $1 WHERE id_pegawai = $2';
        await db.query(updateQuery, [newPassword, pegawai_id]);

        console.log(`🔐 User ${pegawai_id} password changed successfully`);
        res.json({ success: true, message: 'Password berhasil diubah' });
    } catch (error) {
        console.error('Error changing user password:', error);
        res.status(500).json({ success: false, message: 'Gagal mengubah password: ' + error.message });
    }
});

// Delete pegawai
app.delete('/pegawai/delete/:id', async (req, res) => {
    const { id } = req.params;

    try {
        const query = 'DELETE FROM pegawai WHERE id_pegawai = $1 RETURNING *';
        const result = await db.query(query, [id]);

        if (result.rows.length === 0) {
            return res.status(404).json({ success: false, message: 'Pegawai tidak ditemukan' });
        }

        res.json({ success: true, message: 'Pegawai berhasil dihapus' });
    } catch (error) {
        console.error('Error delete pegawai:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Import pegawai dari CSV
app.post('/api/pegawai/import', uploadFoto.single('file'), async (req, res) => {
    if (!req.file) {
        return res.status(400).json({ success: false, message: 'File tidak ditemukan' });
    }

    try {
        const fileContent = fs.readFileSync(req.file.path, 'utf8');
        const lines = fileContent.split('\n').filter(line => line.trim());

        if (lines.length < 2) {
            fs.unlinkSync(req.file.path);
            return res.status(400).json({ success: false, message: 'File CSV kosong atau tidak valid' });
        }

        const header = lines[0].toLowerCase();
        let imported = 0;
        let failed = 0;

        for (let i = 1; i < lines.length; i++) {
            const cols = lines[i].split(',').map(c => c.trim().replace(/"/g, ''));
            if (cols.length >= 2) {
                try {
                    const [nip, nama, posisi, email, no_telepon] = cols;
                    await db.query(
                        `INSERT INTO pegawai (nip, nama_pegawai, posisi, email, no_telepon, tanggal_bergabung, status)
                         VALUES ($1, $2, $3, $4, $5, CURRENT_DATE, 'Aktif')
                         ON CONFLICT (nip) DO NOTHING`,
                        [nip, nama, posisi || 'Karyawan', email || null, no_telepon || null]
                    );
                    imported++;
                } catch (e) {
                    failed++;
                }
            }
        }

        fs.unlinkSync(req.file.path);
        res.json({ success: true, message: `Import selesai: ${imported} berhasil, ${failed} gagal` });
    } catch (error) {
        console.error('Error import:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Get divisi list
app.get('/api/divisi', async (req, res) => {
    try {
        const result = await db.query('SELECT * FROM divisi ORDER BY nama_divisi');
        res.json({ success: true, data: result.rows });
    } catch (error) {
        res.json({ success: true, data: [] }); // Return empty if table doesn't exist
    }
});

// ============================================
// API ENDPOINTS - SHIFT & JADWAL
// ============================================

// Get semua shift
app.get('/api/shift', async (req, res) => {
    try {
        const result = await db.query('SELECT * FROM shift WHERE is_active = true ORDER BY jam_masuk');
        res.json({ success: true, data: result.rows });
    } catch (error) {
        res.json({ success: true, data: [] });
    }
});

// Tambah shift baru
app.post('/api/shift', async (req, res) => {
    const { nama_shift, jam_masuk, jam_keluar, toleransi_terlambat, jam_lembur_mulai } = req.body;

    try {
        const query = `
            INSERT INTO shift (nama_shift, jam_masuk, jam_keluar, toleransi_terlambat, jam_lembur_mulai)
            VALUES ($1, $2, $3, $4, $5)
            RETURNING *
        `;
        const result = await db.query(query, [nama_shift, jam_masuk, jam_keluar, toleransi_terlambat || 15, jam_lembur_mulai]);
        res.json({ success: true, data: result.rows[0] });
    } catch (error) {
        console.error('Error add shift:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Update shift
app.put('/api/shift/:id', async (req, res) => {
    const { id } = req.params;
    const { nama_shift, jam_masuk, jam_keluar, toleransi_terlambat, jam_lembur_mulai } = req.body;

    try {
        const query = `
            UPDATE shift
            SET nama_shift = $1, jam_masuk = $2, jam_keluar = $3,
                toleransi_terlambat = $4, jam_lembur_mulai = $5
            WHERE id_shift = $6
            RETURNING *
        `;
        const result = await db.query(query, [nama_shift, jam_masuk, jam_keluar, toleransi_terlambat, jam_lembur_mulai, id]);
        res.json({ success: true, data: result.rows[0] });
    } catch (error) {
        console.error('Error update shift:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Get hari libur
app.get('/api/hari-libur', async (req, res) => {
    const { tahun } = req.query;
    try {
        let query = 'SELECT * FROM hari_libur';
        const params = [];

        if (tahun) {
            query += ' WHERE EXTRACT(YEAR FROM tanggal) = $1';
            params.push(tahun);
        }

        query += ' ORDER BY tanggal';
        const result = await db.query(query, params);
        res.json({ success: true, data: result.rows });
    } catch (error) {
        res.json({ success: true, data: [] });
    }
});

// Tambah hari libur
app.post('/api/hari-libur', async (req, res) => {
    const { tanggal, nama_libur, jenis } = req.body;

    try {
        const query = `
            INSERT INTO hari_libur (tanggal, nama_libur, jenis)
            VALUES ($1, $2, $3)
            ON CONFLICT (tanggal) DO UPDATE SET nama_libur = $2, jenis = $3
            RETURNING *
        `;
        const result = await db.query(query, [tanggal, nama_libur, jenis || 'Nasional']);
        res.json({ success: true, data: result.rows[0] });
    } catch (error) {
        console.error('Error add holiday:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Delete hari libur
app.delete('/api/hari-libur/:id', async (req, res) => {
    try {
        await db.query('DELETE FROM hari_libur WHERE id_libur = $1', [req.params.id]);
        res.json({ success: true, message: 'Hari libur dihapus' });
    } catch (error) {
        res.status(500).json({ success: false, message: error.message });
    }
});

// ============================================
// API ENDPOINTS - PAYROLL
// ============================================

// Get payroll list
app.get('/api/payroll', async (req, res) => {
    const { bulan, tahun } = req.query;

    try {
        let query = `
            SELECT pay.*, p.nama_pegawai, p.nip, p.posisi, p.divisi
            FROM payroll pay
            JOIN pegawai p ON pay.pegawai_id = p.id_pegawai
        `;
        const params = [];

        if (bulan && tahun) {
            query += ' WHERE pay.bulan = $1 AND pay.tahun = $2';
            params.push(bulan, tahun);
        }

        query += ' ORDER BY p.nama_pegawai';

        const result = await db.query(query, params);
        res.json({ success: true, data: result.rows });
    } catch (error) {
        res.json({ success: true, data: [] });
    }
});

// Generate payroll untuk bulan tertentu
// Rumus: Gaji = (Hadir × Gaji/Hari) + (Jam Lembur × Tarif Lembur) - Potongan

app.post('/api/payroll/generate', async (req, res) => {
    const { bulan, tahun } = req.body;

    if (!bulan || !tahun) {
        return res.status(400).json({ success: false, message: 'Bulan dan tahun wajib diisi' });
    }

    try {
        // Load settings from database
        const settingsResult = await db.query('SELECT nama_setting, nilai FROM setting_payroll');
        const settings = {};
        settingsResult.rows.forEach(s => {
            settings[s.nama_setting] = parseFloat(s.nilai) || 0;
        });

        // Default values if not set
        const GAJI_PER_HARI = settings.gaji_per_hari || 150000;
        const TARIF_LEMBUR = settings.tarif_lembur || 25000;
        const POTONGAN_ALPHA = settings.potongan_alpha || 50000;
        const POTONGAN_TERLAMBAT = settings.potongan_terlambat || 10000;

        // Get semua pegawai aktif
        const pegawaiResult = await db.query("SELECT * FROM pegawai WHERE status = 'Aktif'");

        let generated = 0;

        for (const pegawai of pegawaiResult.rows) {
            // Hitung absensi bulan ini
            const absensiQuery = `
                SELECT
                    COUNT(*) as total_hadir,
                    COUNT(CASE WHEN keterangan = 'Terlambat' THEN 1 END) as terlambat,
                    COUNT(CASE WHEN keterangan = 'Izin' THEN 1 END) as izin,
                    COUNT(CASE WHEN keterangan = 'Sakit' THEN 1 END) as sakit,
                    COALESCE(SUM(CASE WHEN status_lembur = 'Ya' AND durasi_lembur IS NOT NULL THEN durasi_lembur / 60.0 ELSE 0 END), 0) as jam_lembur
                FROM absensi
                WHERE pegawai_id = $1
                AND EXTRACT(MONTH FROM "timestamp") = $2
                AND EXTRACT(YEAR FROM "timestamp") = $3
            `;
            const absensi = (await db.query(absensiQuery, [pegawai.id_pegawai, parseInt(bulan), parseInt(tahun)])).rows[0];

            // Hitung gaji
            const totalHadir = parseInt(absensi.total_hadir) || 0;
            const totalTerlambat = parseInt(absensi.terlambat) || 0;
            const jamLembur = parseFloat(absensi.jam_lembur) || 0;

            // Gaji Pokok = Hari Hadir × Gaji Per Hari
            const gajiPokok = totalHadir * GAJI_PER_HARI;

            // Uang Lembur = Jam Lembur × Tarif Lembur
            const uangLembur = jamLembur * TARIF_LEMBUR;

            // Potongan = (Terlambat × Potongan Terlambat)
            const potonganAbsen = totalTerlambat * POTONGAN_TERLAMBAT;

            // Total Gaji
            const tunjangan = 0;
            const totalGaji = gajiPokok + uangLembur + tunjangan - potonganAbsen;

            // Insert atau update payroll
            const insertQuery = `
                INSERT INTO payroll (pegawai_id, bulan, tahun, total_hadir, total_terlambat, total_izin, total_sakit,
                    total_jam_lembur, gaji_pokok, uang_lembur, tunjangan, potongan_absen, total_gaji)
                VALUES ($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13)
                ON CONFLICT (pegawai_id, bulan, tahun)
                DO UPDATE SET total_hadir = $4, total_terlambat = $5, total_izin = $6, total_sakit = $7,
                    total_jam_lembur = $8, gaji_pokok = $9, uang_lembur = $10, tunjangan = $11,
                    potongan_absen = $12, total_gaji = $13, updated_at = NOW()
            `;

            await db.query(insertQuery, [
                pegawai.id_pegawai, bulan, tahun,
                totalHadir, totalTerlambat, absensi.izin || 0, absensi.sakit || 0,
                jamLembur, gajiPokok, uangLembur, tunjangan, potonganAbsen, totalGaji
            ]);

            generated++;
        }

        res.json({ success: true, message: `Payroll berhasil digenerate untuk ${generated} pegawai` });
    } catch (error) {
        console.error('Error generate payroll:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Get payroll detail (slip gaji)
app.get('/api/payroll/:id', async (req, res) => {
    try {
        const query = `
            SELECT pay.*, p.nama_pegawai, p.nip, p.posisi, p.divisi, p.email
            FROM payroll pay
            JOIN pegawai p ON pay.pegawai_id = p.id_pegawai
            WHERE pay.id_payroll = $1
        `;
        const result = await db.query(query, [req.params.id]);

        if (result.rows.length === 0) {
            return res.status(404).json({ success: false, message: 'Data tidak ditemukan' });
        }

        res.json({ success: true, data: result.rows[0] });
    } catch (error) {
        res.status(500).json({ success: false, message: error.message });
    }
});

// Get setting payroll
app.get('/api/setting-payroll', async (req, res) => {
    try {
        const result = await db.query('SELECT * FROM setting_payroll ORDER BY nama_setting');
        res.json({ success: true, data: result.rows });
    } catch (error) {
        res.json({ success: true, data: [] });
    }
});

// Update setting payroll
app.put('/api/setting-payroll/:nama', async (req, res) => {
    const { nama } = req.params;
    const { nilai } = req.body;

    try {
        const query = `
            UPDATE setting_payroll SET nilai = $1, updated_at = NOW()
            WHERE nama_setting = $2
            RETURNING *
        `;
        const result = await db.query(query, [nilai, nama]);
        res.json({ success: true, data: result.rows[0] });
    } catch (error) {
        res.status(500).json({ success: false, message: error.message });
    }
});

// Save multiple settings at once
app.post('/api/setting-payroll', async (req, res) => {
    const settings = req.body;

    try {
        for (const [nama, nilai] of Object.entries(settings)) {
            await db.query(`
                INSERT INTO setting_payroll (nama_setting, nilai)
                VALUES ($1, $2)
                ON CONFLICT (nama_setting) DO UPDATE SET nilai = $2, updated_at = NOW()
            `, [nama, nilai]);
        }
        res.json({ success: true, message: 'Setting berhasil disimpan' });
    } catch (error) {
        res.status(500).json({ success: false, message: error.message });
    }
});

// ============================================
// API ENDPOINTS - LAPORAN
// ============================================

// Rekap absensi bulanan
app.get('/api/laporan/absensi-bulanan', async (req, res) => {
    const { bulan, tahun } = req.query;

    try {
        const query = `
            SELECT
                p.id_pegawai,
                p.nip,
                p.nama_pegawai,
                p.posisi,
                p.divisi,
                COUNT(a.id_absensi) as total_hadir,
                COUNT(CASE WHEN a.keterangan = 'Terlambat' THEN 1 END) as terlambat,
                COUNT(CASE WHEN a.keterangan = 'Izin' THEN 1 END) as izin,
                COUNT(CASE WHEN a.keterangan = 'Sakit' THEN 1 END) as sakit,
                SUM(CASE WHEN a.status_lembur = 'Ya' THEN 1 ELSE 0 END) as lembur
            FROM pegawai p
            LEFT JOIN absensi a ON p.id_pegawai = a.pegawai_id
                AND EXTRACT(MONTH FROM a.timestamp)::INTEGER = $1::INTEGER
                AND EXTRACT(YEAR FROM a.timestamp)::INTEGER = $2::INTEGER
            WHERE p.status = 'Aktif'
            GROUP BY p.id_pegawai, p.nip, p.nama_pegawai, p.posisi, p.divisi
            ORDER BY p.nama_pegawai
        `;

        const result = await db.query(query, [parseInt(bulan) || new Date().getMonth() + 1, parseInt(tahun) || new Date().getFullYear()]);
        res.json({ success: true, data: result.rows });
    } catch (error) {
        console.error('Error laporan:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Rekap keterlambatan
app.get('/api/laporan/terlambat', async (req, res) => {
    const { bulan, tahun } = req.query;

    try {
        const query = `
            SELECT
                p.id_pegawai,
                p.nip,
                p.nama_pegawai,
                p.posisi,
                COUNT(*) as total_terlambat,
                array_agg(DATE(a.timestamp) ORDER BY a.timestamp) as tanggal_terlambat
            FROM pegawai p
            JOIN absensi a ON p.id_pegawai = a.pegawai_id
            WHERE a.keterangan = 'Terlambat'
                AND EXTRACT(MONTH FROM a.timestamp)::INTEGER = $1::INTEGER
                AND EXTRACT(YEAR FROM a.timestamp)::INTEGER = $2::INTEGER
            GROUP BY p.id_pegawai, p.nip, p.nama_pegawai, p.posisi
            ORDER BY total_terlambat DESC
        `;

        const result = await db.query(query, [parseInt(bulan) || new Date().getMonth() + 1, parseInt(tahun) || new Date().getFullYear()]);
        res.json({ success: true, data: result.rows });
    } catch (error) {
        res.status(500).json({ success: false, message: error.message });
    }
});

// Rekap cuti/izin
app.get('/api/laporan/cuti', async (req, res) => {
    const { bulan, tahun } = req.query;

    try {
        const query = `
            SELECT
                p.id_pegawai,
                p.nip,
                p.nama_pegawai,
                p.posisi,
                c.jenis,
                COUNT(*) as jumlah,
                SUM(c.jumlah_hari) as total_hari
            FROM pegawai p
            JOIN pengajuan_cuti c ON p.id_pegawai = c.pegawai_id
            WHERE c.status = 'Disetujui'
                AND EXTRACT(MONTH FROM c.tanggal_mulai) = $1
                AND EXTRACT(YEAR FROM c.tanggal_mulai) = $2
            GROUP BY p.id_pegawai, p.nip, p.nama_pegawai, p.posisi, c.jenis
            ORDER BY p.nama_pegawai
        `;

        const result = await db.query(query, [bulan || new Date().getMonth() + 1, tahun || new Date().getFullYear()]);
        res.json({ success: true, data: result.rows });
    } catch (error) {
        res.json({ success: true, data: [] });
    }
});

// Rekap lembur
app.get('/api/laporan/lembur', async (req, res) => {
    const { bulan, tahun } = req.query;

    try {
        const query = `
            SELECT
                p.id_pegawai,
                p.nip,
                p.nama_pegawai,
                p.posisi,
                COUNT(*) as jumlah_hari_lembur,
                SUM(EXTRACT(EPOCH FROM a.durasi_lembur)/3600) as total_jam_lembur
            FROM pegawai p
            JOIN absensi a ON p.id_pegawai = a.pegawai_id
            WHERE a.status_lembur = 'Ya'
                AND EXTRACT(MONTH FROM a.timestamp) = $1
                AND EXTRACT(YEAR FROM a.timestamp) = $2
            GROUP BY p.id_pegawai, p.nip, p.nama_pegawai, p.posisi
            ORDER BY total_jam_lembur DESC
        `;

        const result = await db.query(query, [bulan || new Date().getMonth() + 1, tahun || new Date().getFullYear()]);
        res.json({ success: true, data: result.rows });
    } catch (error) {
        res.json({ success: true, data: [] });
    }
});

// Export laporan ke Excel/CSV
app.get('/api/laporan/export', async (req, res) => {
    const { jenis, bulan, tahun, format } = req.query;

    try {
        let data = [];
        let filename = '';
        let headers = [];

        switch (jenis) {
            case 'absensi':
                const absensiResult = await db.query(`
                    SELECT p.nip, p.nama_pegawai, p.posisi, p.divisi,
                        COUNT(a.id_absensi) as hadir,
                        COUNT(CASE WHEN a.keterangan = 'Terlambat' THEN 1 END) as terlambat,
                        COUNT(CASE WHEN a.keterangan = 'Izin' THEN 1 END) as izin,
                        COUNT(CASE WHEN a.keterangan = 'Sakit' THEN 1 END) as sakit
                    FROM pegawai p
                    LEFT JOIN absensi a ON p.id_pegawai = a.pegawai_id
                        AND EXTRACT(MONTH FROM a.timestamp) = $1 AND EXTRACT(YEAR FROM a.timestamp) = $2
                    GROUP BY p.nip, p.nama_pegawai, p.posisi, p.divisi
                `, [bulan, tahun]);
                data = absensiResult.rows;
                headers = ['NIP', 'Nama', 'Posisi', 'Divisi', 'Hadir', 'Terlambat', 'Izin', 'Sakit'];
                filename = `Laporan-Absensi-${bulan}-${tahun}`;
                break;

            case 'lembur':
                const lemburResult = await db.query(`
                    SELECT p.nip, p.nama_pegawai, p.posisi,
                        COUNT(*) as hari_lembur,
                        SUM(EXTRACT(EPOCH FROM a.durasi_lembur)/3600) as jam_lembur
                    FROM pegawai p
                    JOIN absensi a ON p.id_pegawai = a.pegawai_id
                    WHERE a.status_lembur = 'Ya'
                        AND EXTRACT(MONTH FROM a.timestamp) = $1 AND EXTRACT(YEAR FROM a.timestamp) = $2
                    GROUP BY p.nip, p.nama_pegawai, p.posisi
                `, [bulan, tahun]);
                data = lemburResult.rows;
                headers = ['NIP', 'Nama', 'Posisi', 'Hari Lembur', 'Total Jam'];
                filename = `Laporan-Lembur-${bulan}-${tahun}`;
                break;

            default:
                return res.status(400).json({ success: false, message: 'Jenis laporan tidak valid' });
        }

        // Generate CSV
        let csv = headers.join(',') + '\n';
        data.forEach(row => {
            csv += Object.values(row).map(v => `"${v || ''}"`).join(',') + '\n';
        });

        res.setHeader('Content-Type', 'text/csv');
        res.setHeader('Content-Disposition', `attachment; filename=${filename}.csv`);
        res.send(csv);

    } catch (error) {
        console.error('Error export:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Serve uploaded foto
app.use('/uploads/foto', express.static(path.join(__dirname, 'uploads/foto')));

// ================================================
// API NOTIFIKASI
// ================================================

// Get notifikasi untuk admin
app.get('/api/notifikasi/admin', async (req, res) => {
    try {
        const result = await db.query(`
            SELECT * FROM notifikasi
            WHERE is_admin = true
            ORDER BY created_at DESC
            LIMIT 50
        `);
        const unreadCount = await db.query(`
            SELECT COUNT(*) FROM notifikasi WHERE is_admin = true AND is_read = false
        `);
        res.json({
            success: true,
            data: result.rows,
            unread_count: parseInt(unreadCount.rows[0].count)
        });
    } catch (error) {
        console.error('Error get admin notifications:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Get notifikasi untuk user tertentu
app.get('/api/notifikasi/user/:pegawai_id', async (req, res) => {
    const { pegawai_id } = req.params;
    try {
        const result = await db.query(`
            SELECT * FROM notifikasi
            WHERE (pegawai_id = $1 OR (pegawai_id IS NULL AND is_admin = false))
            ORDER BY created_at DESC
            LIMIT 50
        `, [pegawai_id]);
        const unreadCount = await db.query(`
            SELECT COUNT(*) FROM notifikasi
            WHERE (pegawai_id = $1 OR (pegawai_id IS NULL AND is_admin = false)) AND is_read = false
        `, [pegawai_id]);
        res.json({
            success: true,
            data: result.rows,
            unread_count: parseInt(unreadCount.rows[0].count)
        });
    } catch (error) {
        console.error('Error get user notifications:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Tandai notifikasi sebagai sudah dibaca
app.put('/api/notifikasi/:id/read', async (req, res) => {
    const { id } = req.params;
    try {
        await db.query('UPDATE notifikasi SET is_read = true WHERE id_notifikasi = $1', [id]);
        res.json({ success: true, message: 'Notifikasi ditandai sudah dibaca' });
    } catch (error) {
        console.error('Error mark notification read:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Tandai semua notifikasi sebagai sudah dibaca (admin)
app.put('/api/notifikasi/admin/read-all', async (req, res) => {
    try {
        await db.query('UPDATE notifikasi SET is_read = true WHERE is_admin = true');
        res.json({ success: true, message: 'Semua notifikasi admin ditandai sudah dibaca' });
    } catch (error) {
        console.error('Error mark all admin notifications read:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Tandai semua notifikasi sebagai sudah dibaca (user)
app.put('/api/notifikasi/user/:pegawai_id/read-all', async (req, res) => {
    const { pegawai_id } = req.params;
    try {
        await db.query(`
            UPDATE notifikasi SET is_read = true
            WHERE pegawai_id = $1 OR (pegawai_id IS NULL AND is_admin = false)
        `, [pegawai_id]);
        res.json({ success: true, message: 'Semua notifikasi ditandai sudah dibaca' });
    } catch (error) {
        console.error('Error mark all user notifications read:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Hapus notifikasi
app.delete('/api/notifikasi/:id', async (req, res) => {
    const { id } = req.params;
    try {
        await db.query('DELETE FROM notifikasi WHERE id_notifikasi = $1', [id]);
        res.json({ success: true, message: 'Notifikasi dihapus' });
    } catch (error) {
        console.error('Error delete notification:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

// Hapus semua notifikasi yang sudah dibaca (admin)
app.delete('/api/notifikasi/admin/clear-read', async (req, res) => {
    try {
        await db.query('DELETE FROM notifikasi WHERE is_admin = true AND is_read = true');
        res.json({ success: true, message: 'Notifikasi yang sudah dibaca dihapus' });
    } catch (error) {
        console.error('Error clear read notifications:', error);
        res.status(500).json({ success: false, message: error.message });
    }
});

httpServer.listen(port, '0.0.0.0', () => {
    console.log(`🚀 Server absensi berjalan pada port ${port}`);
});
