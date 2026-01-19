# OpenTux ESP32-PoE-ISO Firmware

## Description
Firmware for Olimex ESP32-PoE-ISO. Reads BME280 (temp, humidity, pressure), CCS811 (CO2)
and PIR sensor (AM312) and publishes to MQTT broker. Compatible with Home Assistant MQTT discovery.

## License
This firmware is licensed under **GNU General Public License v3 (GPLv3)**.

## Dependencies
- Arduino framework
- PubSubClient
- Adafruit_BME280
- Adafruit_CCS811
- AM312

## Usage
1. Configure MQTT broker settings in the firmware.
2. Upload to ESP32 using Arduino IDE / PlatformIO.
3. Ensure connected sensors are wired correctly.

## Notes
- MAC address is sanitized to remove colons for unique MQTT device ID.
- Motion sensor interval is 3 seconds.
