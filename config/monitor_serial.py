#!/usr/bin/env python3
import serial
import time

try:
    ser = serial.Serial('/dev/ttyUSB0', 115200, timeout=1)
    print("=" * 70)
    print("ESP32 Serial Monitor - Press Ctrl+C to exit")
    print("=" * 70)

    while True:
        if ser.in_waiting:
            try:
                line = ser.readline().decode('utf-8', errors='ignore').strip()
                if line:
                    print(line)
            except:
                pass

except KeyboardInterrupt:
    print("\n\nMonitor stopped by user")
    ser.close()
except Exception as e:
    print(f"Error: {e}")
