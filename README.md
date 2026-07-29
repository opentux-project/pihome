# PiHome by OpenTUX

is open-source smart home automation based on Raspberry Pi & Arduino hardware and open project Casa OS with OpenHAB or Home Assistant setups.

🌐 Website: https://opentux.eu 
📦 Ready-to-flash images for Raspberry Pi 5: available on the website (links below) 
💬 Contact / community: info@opentux.eu

## Motivation
Commercial home automation systems are centralized, opaque and vendor-locked.
Open solutions as Home Assistant and OpenHAB are perfect, but too difficult to build complex home automation from scratch. OpenTUX aims to bring ready-to-use, pre-configurated system including open source software and hardware.

## Screenshots
<img width="300"  alt="image" src="https://github.com/user-attachments/assets/953a6a98-fe44-48a3-a9dc-a7d225f491f6" />
<img width="300"  alt="image" src="https://github.com/user-attachments/assets/96f1251f-4592-45ae-8c9d-51d491d0966b" />
<img width="300"  alt="image" src="https://github.com/user-attachments/assets/e5035e4e-c9ca-4a76-bfa6-169126440526" />


## Getting started
Download the image for your project from https://opentux.eu/downloads
Flash it to an SD card / SSD using Raspberry Pi Imager
Boot your Raspberry Pi 5 and follow the first-run wizard
See each repo's README for detailed setup and firmware flashing instructions

## Supported platforms
- Raspberry Pi 5 (4GB and more)
- Arduino Mega with Ethernet shield (tested) as PLC for relay automation
- ESP32 (tested) for multisenzor
- Arduino sensors

## Current state

📦 Arduino Mega - v.1.05 -  firmware already tested in production. Users should edit FW manually - MQTT credentials/server, topics as needed. 

⚠️ ESP32 - Beta – basic firmware is ready for ESP32 multisenzor. Works as expected. Currently developing option for automatic UDP preset without need to edit FW manually. 




