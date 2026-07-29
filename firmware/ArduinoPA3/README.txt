
# OpenTUX Pi-Home Arduino Mega Sketch

This repository contains an Arduino sketch for controlling relays using an Arduino Mega with an Ethernet Shield. The sketch is designed to be used with Arduino-compatible relays and sensors for various automation and control projects.

## Features

- Allows control of relays connected to the Arduino Mega via MQTT
- Allows collect data from sensors connected to the Arduino Mega via MQTT
- Utilizes Ethernet Shield for network connectivity.
- Enables MQTT communication.
- Supports customization of IP address for the Arduino Mega Ethernet Shield.
- Provides flexibility to set MQTT IP address for communication.

## Getting Started

To use this sketch, follow these steps:

1. Connect your Arduino Mega with the Ethernet Shield properly.
2. Install the required libraries specified in the Makefile.
3. Modify the `Arduino_Mega_Eth_Shield_IP` variable in the sketch to match your Arduino Mega's IP address.
4. Set the correct MQTT IP address in the sketch to ensure proper communication.
5. Compile and upload the sketch to your Arduino Mega 

## Usage

Once the sketch is uploaded and running on your Arduino Mega, you can control the relays using MQTT messages. Monitor the serial console for debugging and status messages.

## License

This project is licensed under the GNU General Public License v3.0 (GPL-3.0). You are free to modify and distribute this software under the terms of the GPL-3.0 license. See the LICENSE.txt file for details.

## Contributors

- Michal Novotny - OpenTUX (https://opentux.eu) - Creator and maintainer

Feel free to contribute by forking the repository and submitting pull requests.

## Acknowledgments

Special thanks to the Arduino community and the developers of the libraries used in this project.
