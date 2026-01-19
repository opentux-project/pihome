# OpenTux

open-source firmware and application for local, privacy-respecting
embedded devices (ESP32, Arduino, Raspberry Pi)

## Motivation
Commercial home automation systems are centralized, opaque and vendor-locked.
Open solutions as Home Assistant and OpenHAB are perfect, but too dificult to build complex home automation from scratch. OpenTux aims to bring ready-to-use, pre-configurated
system including open source software and develop multisenzor solution based on open hardware to easily connect it to this systems even for non-IT users 

## Supported platforms
- ESP32 (tested) for multisenzor
- Arduino Mega (tested) as PLC for relay automation

## Current state
⚠️ ESP32 - Beta – basic firmware is ready for ESP32 multisenzor. Works as expected. Currently developing option for automatic UDP preset without need to edit FW manually. 

⚠️ Arduino Mega - v.1.05 -  firmware already tested in production. Users should edit FW manually - MQTT credentials/server, topics as needed. 




