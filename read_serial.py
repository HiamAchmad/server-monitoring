#!/usr/bin/env python3
import serial
import time

try:
    # Reset connection
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)

    # Tunggu sebentar
    time.sleep(0.5)

    # Reset ESP32 with DTR
    ser.setDTR(False)
    time.sleep(0.1)
    ser.setDTR(True)

    print("ESP32 direset, membaca output selama 10 detik...")
    print("=" * 70)

    start_time = time.time()
    while time.time() - start_time < 10:
        if ser.in_waiting:
            try:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(line)
            except:
                pass

    print("=" * 70)
    ser.close()

except Exception as e:
    print(f"Error: {e}")
