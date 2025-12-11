#!/usr/bin/env node

/**
 * MQTT Test Client
 * Script untuk testing MQTT broker dengan simulasi data dari ESP32
 */

const mqtt = require('mqtt');

// Konfigurasi MQTT
const MQTT_BROKER = 'mqtt://localhost:1883';
const MQTT_TOPIC = 'absensi/data';
const CLIENT_ID = 'ESP32_Test_Client';

console.log('🧪 MQTT Test Client');
console.log('===================\n');

// Connect ke MQTT broker
const client = mqtt.connect(MQTT_BROKER, {
    clientId: CLIENT_ID,
    clean: true,
    connectTimeout: 4000,
    reconnectPeriod: 1000
});

client.on('connect', () => {
    console.log('✅ Connected to MQTT Broker');
    console.log(`   Broker: ${MQTT_BROKER}`);
    console.log(`   Client ID: ${CLIENT_ID}\n`);

    // Subscribe untuk monitoring
    client.subscribe('absensi/#', (err) => {
        if (!err) {
            console.log('📡 Subscribed to topic: absensi/#\n');
        }
    });

    // Simulasi data absensi dari ESP32
    console.log('📤 Sending test data...\n');

    const testData = {
        pegawai_id: 1,
        waktu_absen: new Date().toLocaleTimeString('id-ID', { hour12: false }),
        keterangan: 'Hadir (Test)'
    };

    const payload = JSON.stringify(testData);
    console.log('📋 Test Payload:');
    console.log(JSON.stringify(testData, null, 2));
    console.log('');

    // Publish test message
    client.publish(MQTT_TOPIC, payload, { qos: 0, retain: false }, (err) => {
        if (err) {
            console.error('❌ Publish failed:', err.message);
        } else {
            console.log('✅ Message published successfully!');
            console.log(`   Topic: ${MQTT_TOPIC}`);
            console.log(`   Payload: ${payload}\n`);
        }

        // Tunggu sebentar untuk menerima message, lalu disconnect
        setTimeout(() => {
            console.log('🔌 Disconnecting...');
            client.end();
            console.log('✅ Test completed!\n');
            console.log('💡 Tips:');
            console.log('   - Check server.log untuk melihat message diterima');
            console.log('   - Check database MySQL untuk verify data tersimpan');
            console.log('   - Check dashboard web untuk real-time update\n');
            process.exit(0);
        }, 2000);
    });
});

client.on('message', (topic, message) => {
    console.log(`📥 Message received:`);
    console.log(`   Topic: ${topic}`);
    console.log(`   Message: ${message.toString()}\n`);
});

client.on('error', (err) => {
    console.error('❌ Connection error:', err.message);
    process.exit(1);
});

client.on('offline', () => {
    console.log('⚠️  Client offline');
});

client.on('reconnect', () => {
    console.log('🔄 Reconnecting...');
});
