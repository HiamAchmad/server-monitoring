#!/usr/bin/env python3
import serial
import time

try:
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    print("Reading ESP32 serial output for 5 seconds...")
    print("=" * 60)

    start_time = time.time()
    while time.time() - start_time < 5:
        if ser.in_waiting:
            line = ser.readline().decode('utf-8', errors='ignore').strip()
            if line:
                print(line)

    print("=" * 60)
    ser.close()

except Exception as e:
    print(f"Error: {e}")
