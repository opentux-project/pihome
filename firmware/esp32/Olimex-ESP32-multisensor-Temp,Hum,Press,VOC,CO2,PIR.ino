/*
 * OpenTux ESP32-PoE-ISO Firmware
 * Copyright (C) 2026 Michal Novotny / OpenTUX
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 * Description:
 *   Firmware for Olimex ESP32-PoE-ISO, connects sensors MOD-ENV and PIR AM312 to MQTT broker. Automatic detection by Home Assistant based on Ethernet MAC as key
 */


#include <ETH.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <Adafruit_BME280.h>
#include <Adafruit_CCS811.h>

// -------- MQTT Broker settings --------
const char* mqtt_server = "<YOUR MQTT SERVER IP"; //CHANGE TO YOUR MQTT SERVER
const int mqtt_port = 1883;
const char* mqtt_user = "opentux"; //MQTT SERVER USERNAME
const char* mqtt_pass = "opentux"; //MQTT SERVER PASS

#define PIR_PIN 4 //AM312 DATA PIN
#define MOTION_INTERVAL 3000  // 3 second, can be adjusted

WiFiClient ethClient;
PubSubClient client(ethClient);

static bool eth_connected = false;
unsigned long lastMotionPublish = 0;

// -------- Sensors --------
Adafruit_BME280 bme;       // BME280 at 0x77
Adafruit_CCS811 ccs;       // CCS811 at 0x5A

String macID;        // without :
String macRaw;       // raw MAC

// -------- Ethernet Event Handler --------
void WiFiEvent(WiFiEvent_t event) {
  switch (event) {
    case ARDUINO_EVENT_ETH_START:
      Serial.println("ETH Started");
      ETH.setHostname("esp32-ethernet");
      break;
    case ARDUINO_EVENT_ETH_CONNECTED:
      Serial.println("ETH Connected");
      break;
    case ARDUINO_EVENT_ETH_GOT_IP:
      Serial.print("ETH IP: ");
      Serial.println(ETH.localIP());
      eth_connected = true;
      break;
    case ARDUINO_EVENT_ETH_DISCONNECTED:
      Serial.println("ETH Disconnected");
      eth_connected = false;
      break;
    case ARDUINO_EVENT_ETH_STOP:
      Serial.println("ETH Stopped");
      eth_connected = false;
      break;
    default:
      break;
  }
}

// -------- MQTT Discovery --------
void mqttDiscovery() {
  String base = "homeassistant";
  String id = macID;
  String payload;

  payload =
    "{\"name\":\"Temperature\",\"state_topic\":\"pihome/temp\","
    "\"unit_of_measurement\":\"°C\",\"device_class\":\"temperature\","
    "\"unique_id\":\"" + id + "_temp\","
    "\"device\":{\"identifiers\":[\"" + id + "\"],\"name\":\"ESP32 Multisensor\"}}";
  client.publish((base + "/sensor/" + id + "_temp/config").c_str(), payload.c_str(), true);

  payload =
    "{\"name\":\"Humidity\",\"state_topic\":\"pihome/humidity\","
    "\"unit_of_measurement\":\"%\",\"device_class\":\"humidity\","
    "\"unique_id\":\"" + id + "_hum\","
    "\"device\":{\"identifiers\":[\"" + id + "\"],\"name\":\"ESP32 Multisensor\"}}";
  client.publish((base + "/sensor/" + id + "_hum/config").c_str(), payload.c_str(), true);

  payload =
    "{\"name\":\"Pressure\",\"state_topic\":\"pihome/pressure\","
    "\"unit_of_measurement\":\"hPa\",\"device_class\":\"pressure\","
    "\"unique_id\":\"" + id + "_press\","
    "\"device\":{\"identifiers\":[\"" + id + "\"],\"name\":\"ESP32 Multisensor\"}}";
  client.publish((base + "/sensor/" + id + "_press/config").c_str(), payload.c_str(), true);

  payload =
    "{\"name\":\"CO2\",\"state_topic\":\"pihome/co2\","
    "\"unit_of_measurement\":\"ppm\",\"device_class\":\"carbon_dioxide\","
    "\"unique_id\":\"" + id + "_co2\","
    "\"device\":{\"identifiers\":[\"" + id + "\"],\"name\":\"ESP32 Multisensor\"}}";
  client.publish((base + "/sensor/" + id + "_co2/config").c_str(), payload.c_str(), true);

  payload =
    "{\"name\":\"Motion\",\"state_topic\":\"pihome/motion\","
    "\"device_class\":\"motion\",\"payload_on\":\"1\",\"payload_off\":\"0\","
    "\"unique_id\":\"" + id + "_motion\","
    "\"device\":{\"identifiers\":[\"" + id + "\"],\"name\":\"ESP32 Multisenzor\"}}";
  client.publish((base + "/binary_sensor/" + id + "_motion/config").c_str(), payload.c_str(), true);

  Serial.println("MQTT discovery published");
}

// -------- MQTT reconnect --------
void reconnectMQTT() {
  while (!client.connected() && eth_connected) {
    Serial.print("Connecting to MQTT...");
    if (client.connect(macID.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println(" connected");
      mqttDiscovery();
    } else {
      Serial.print(" failed, rc=");
      Serial.println(client.state());
      delay(2000);
    }
  }
}

// -------- Setup --------
void setup() {
  Serial.begin(115200);
  delay(500);
  pinMode(PIR_PIN, INPUT);

  Serial.println("Starting ETH...");
  WiFi.onEvent(WiFiEvent);
  ETH.begin();

  client.setServer(mqtt_server, mqtt_port);
  Wire.begin();

  if (!bme.begin(0x77)) Serial.println("BME280 not found!");
  if (!ccs.begin()) Serial.println("CCS811 not found!");
  while (!ccs.available()) delay(100);

  macRaw = ETH.macAddress();
  macID = macRaw;
  macID.replace(":", "");   // don't use ":" in topic

  Serial.print("MAC raw: ");
  Serial.println(macRaw);
  Serial.print("MAC sanitized: ");
  Serial.println(macID);
}

// -------- Loop --------
void loop() {
  if (eth_connected) {
    if (!client.connected()) reconnectMQTT();
    client.loop();

    char temp_msg[16], hum_msg[16], press_msg[16];
    snprintf(temp_msg, sizeof(temp_msg), "%.2f", bme.readTemperature());
    snprintf(hum_msg, sizeof(hum_msg), "%.2f", bme.readHumidity());
    snprintf(press_msg, sizeof(press_msg), "%.2f", bme.readPressure() / 100.0F);

    client.publish("pihome/temp", temp_msg, true);
    client.publish("pihome/humidity", hum_msg, true);
    client.publish("pihome/pressure", press_msg, true);

    if (ccs.available() && !ccs.readData()) {
      char co2_msg[16];
      snprintf(co2_msg, sizeof(co2_msg), "%d", ccs.geteCO2());
      client.publish("pihome/co2", co2_msg, true);
    }

    unsigned long now = millis();
    if (now - lastMotionPublish >= MOTION_INTERVAL) {
      lastMotionPublish = now;
      client.publish("pihome/motion", digitalRead(PIR_PIN) ? "1" : "0", true);
    }
  }
  delay(1000);
}
