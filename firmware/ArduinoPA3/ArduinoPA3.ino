/*
 Arduino 3 (PA03) - DEMO
 
  - connects to an MQTT server
  - publishes "hello world" to the topic "pihome"
  - subscribes to the specific topic pihome/xxx
  - sends a specific "on"/"off" to the specific topic when a specific button is pressed
  - multiple arduino's with same generic sketch can run parallel to each other
  - multiple arduino's need each to have a unique ip-addres, unique mac address and unique MQTT client-ID
  - tested on arduino-mega with W5500 ethernet shield
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <avr/wdt.h>

// ============================================================================
// WATCHDOG TIMER - 4 sekundy timeout
// ============================================================================
void setupWatchdog() {
  cli();
  wdt_reset();
  WDTCSR = (1 << WDCE) | (1 << WDE);
  WDTCSR = (1 << WDE) | (1 << WDP3);  // 4 sekundy timeout
  sei();
}

// ============================================================================
// PIN DEFINITIONS - Přiřazení pinů
// ============================================================================
// pin 0 RX
// pin 1 TX
// pin 4 (SS for SD Card)
//B111 has underscore because B111 variable already exist in some of the library :D and PIN 5 is not usable as well
int B111_ = 6; 
int B112  = 7;
int B121  = 8;
int B122  = 9;
// pin 10 (SS for Ethernet)
int B131  = 11;
int B132  = 12;
int B133  = 13;
int B134  = 2;
int B141 = 3;
int B142 = 14;
int B151 = 15;
int B152 = 16;
int B153 = 17;
int B154 = 18;
int B155 = 19;
// pin 20 SDA
// pin 21 SCL
int B156 = 23;
int B157 = 25;
int B158 = 27;
int B159 = 29;
int B160 = 31;
int B161 = 33;
int B162 = 35;
int B201 = 37;
int B202 = 39;
int B211 = 41;
int B212 = 43;
int B213 = 45;
int B214 = 47;
int B221 = 49;
int B222 = 22;
int B223 = 24;
int B224 = 26;		   
int B231 = 28;
int B232 = 30;
int B233 = 32;
int B234 = 34;
int B241 = 36;
int B242 = 38;
int B243 = 40;
int B244 = 42;
// pin 50 (SS for Ethernet)
// pin 51 (MOSI)
// pin 52 (MISO)
// pin 53 (SCK)

// ============================================================================
// MQTT & ETHERNET
// ============================================================================
byte mac[] = { 0xCC, 0xFB, 0x0B, 0xCB, 0x19, 0x11 };
IPAddress ip(10, 0, 0, 123);
byte server[] = { 10, 0, 0, 112 };

EthernetClient arduino3;
PubSubClient client(server, 1883, callback, arduino3);

// ============================================================================
// STATE MANAGEMENT
// ============================================================================
const int debounceDelay = 50;
unsigned long lastDebounceTime[42] = {0};  
int lastButtonState[42] = {HIGH};          
int currentButtonState[42] = {HIGH}; 

// MQTT reconnect
long lastReconnectAttempt = 0;
const long RECONNECT_INTERVAL = 5000;

// Watchdog ping
unsigned long lastWdtReset = 0;
const long WDT_RESET_INTERVAL = 2500;  // Reset watchdog každých 2.5s (máme 4s timeout)

// ============================================================================
// MQTT CALLBACK
// ============================================================================
void callback(char* topic, byte* payload, unsigned int length) {
  (void)topic;
  (void)payload;
  (void)length;
}

// ============================================================================
// MQTT RECONNECT
// ============================================================================
boolean reconnect() {
  if (client.connect("arduino3", "opentux", "opentux")) {
    client.publish("pihome", "Arduino PA3 reconnected");
    return true;
  }
  return false;
}

// ============================================================================
// SETUP
// ============================================================================
void setup() {
  // Watchdog
  setupWatchdog();
  
  Ethernet.begin(mac, ip);
  delay(500);
  
  // MQTT connect
  if (client.connect("arduino3", "opentux", "opentux")) {
    client.publish("pihome", "Hello world - here Arduino PA03 with IP 10.1.1.213");
  }
  
  // Initialize button pins as input with pull-up resistors
  int buttons[] = {B111_, B112, B121, B122, B131, B132, B133, B134, B141, B142,
                   B151, B152, B153, B154, B155, B156, B157, B158, B159, B160,
                   B161, B162, B201, B202, B211, B212, B213, B214, B221, B222,
                   B223, B224, B231, B232, B233, B234, B241, B242, B243, B244};
  
  for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
    pinMode(buttons[i], INPUT_PULLUP);
  }
  
  lastReconnectAttempt = millis();
  lastWdtReset = millis();
}

// ============================================================================
// LOOP
// ============================================================================
void loop() {
  unsigned long now = millis();  // Jeden čas pro celou iteraci!
  
  // *** WATCHDOG RESET - PRIORITA 1 ***
  if (now - lastWdtReset > WDT_RESET_INTERVAL) {
    wdt_reset();
    lastWdtReset = now;
  }
  
  // *** MQTT CONNECT/RECONNECT ***
  if (!client.connected()) {
    if (now - lastReconnectAttempt > RECONNECT_INTERVAL) {
      lastReconnectAttempt = now;
      reconnect();
    }
  } else {
    client.loop();
  }
  
  // *** BUTTON SCANNING ***
  int buttons[] = {B111_, B112, B121, B122, B131, B132, B133, B134, B141, B142,
                   B151, B152, B153, B154, B155, B156, B157, B158, B159, B160,
                   B161, B162, B201, B202, B211, B212, B213, B214, B221, B222,
                   B223, B224, B231, B232, B233, B234, B241, B242, B243, B244};
  
  const char* buttonNames[] = {"B111_", "B112", "B121", "B122", "B131", "B132", "B133", "B134", "B141", "B142",
                               "B151", "B152", "B153", "B154", "B155", "B156", "B157", "B158", "B159", "B160",
                               "B161", "B162", "B201", "B202", "B211", "B212", "B213", "B214", "B221", "B222",
                               "B223", "B224", "B231", "B232", "B233", "B234", "B241", "B242", "B243", "B244"};
  
  for (int i = 0; i < sizeof(buttons)/sizeof(buttons[0]); i++) {
    int reading = digitalRead(buttons[i]);
    
    // Debounce logika
    if (reading != lastButtonState[i]) {
      lastDebounceTime[i] = now;  // Používáme již vypočítaný `now`!
    }

    if ((now - lastDebounceTime[i]) > debounceDelay) {
      if (reading != currentButtonState[i]) {
        currentButtonState[i] = reading;
        
        // Publikuj jen když je MQTT connected a tlačítko je stisknuto
        if (currentButtonState[i] == LOW && client.connected()) {
          char message[30];
          snprintf(message, 30, "%s_push", buttonNames[i]);
          client.publish("pihome/buttons", message);
        }
      }
    }

    lastButtonState[i] = reading;
  }
 
  delay(30);  //
}
