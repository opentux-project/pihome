/*
 Arduino 1 (PA01) - DEMO
 
  - connects to an MQTT server
  - publishes "hello world" to the topic "relay"
  - subscribes to the topic "relay"
  - controls as many relays as defined
  - turns on/off a specific led when it receives a specific "on"/"off" from the "relay" topic
  - multiple arduino's with same generic sketch can run parallel to each other
  - multiple arduino's need each to have a unique ip-addres, unique mac address and unique MQTT client-ID
  - tested on arduino-mega with W5100 ethernet shield
  - watchdog timer + non-blocking MQTT reconnect

*/

//------------------------------------------------------------------------------

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <avr/wdt.h>

// ============================================================================
// WATCHDOG TIMER - 4 second timeout
// ============================================================================
void setupWatchdog() {
  cli();
  wdt_reset();
  WDTCSR = (1 << WDCE) | (1 << WDE);
  WDTCSR = (1 << WDE) | (1 << WDP3);  // 4 second timeout
  sei();
}

//PR01
// pin 0 RX
// pin 1 TX
// pin 4 (SS for SD Card)
int L011 = 5; 
int L012 = 6;
int L013 = 7;
int L014 = 8;
int L021 = 9;
// pin 10 (SS for Ethernet)
int L022 = 11;
int L031 = 12;
// pin 13 (Communication)

//PR02
int L041 = 2;
int L042 = 3;
int L043 = 14;
int L111 = 15;
int L112 = 16;
int L121 = 17;
int L122 = 18;
int L131 = 19;
// pin 20 SDA
// pin 21 SCL

//PR03
int L141 = 23;
int L151 = 25;
int L152 = 27;
int L153 = 29;
int L154 = 31;
int L155 = 33;
int L156 = 35;
int L161 = 37;

//PR04     
int L201 = 22;
int L211 = 24;
int L212 = 26;
int L221 = 28;
int L231 = 30;
int L241 = 32;
//int rez = 34;
//int rez = 36;

//PR05
int H111 = 39;	     
int H121 = 41;
int H131 = 43;
int H151 = 45;
int H152 = 47;
int H201 = 49;
int H211 = 38;
int H221 = 40;
// pin 50 (SS for Ethernet)
// pin 51 (MOSI)
// pin 52 (MISO)
// pin 53 (SCK)

//PR06
//A0 (D54)
int H231 = 54;
int H241 = 55;
// int Rez = 56;
// int Rez = 57;
// int Rez = 58;
// int Rez = 59;
// pin 60 inaccesibble
// int Rez = 61;
// int Rez = 62;
// int Rez = 63;
// int Rez = 64;
// int Rez = 65;
// int Rez = 66;
// int Rez = 67;
// int Rez = 68;
int resetPIN = 69;
//A15 (D69)

// Set variables to act as virtual switches
// Set variable values initially to LOW (and not HIGH)

int ValL011 = LOW;
int ValL012 = LOW;
int ValL013 = LOW;
int ValL014 = LOW;
int ValL021 = LOW;
int ValL022 = LOW;
int ValL031 = LOW;
int ValL041 = LOW;
int ValL042 = LOW;
int ValL043 = LOW;

int ValL111 = LOW;
int ValL112 = LOW;
int ValL121 = LOW;
int ValL122 = LOW;
int ValL131 = LOW;
int ValL141 = LOW;
int ValL151 = LOW;
int ValL152 = LOW;
int ValL153 = LOW;
int ValL154 = LOW;
int ValL155 = LOW;
int ValL156 = LOW;
int ValL161 = LOW;

int ValL201 = LOW;
int ValL211 = LOW;
int ValL212 = LOW;
int ValL221 = LOW;
int ValL231 = LOW;
int ValL241 = LOW;

int ValH111 = LOW;
int ValH121 = LOW;
int ValH131 = LOW;
int ValH151 = LOW;
int ValH152 = LOW;

int ValH201 = LOW;
int ValH211 = LOW;
int ValH221 = LOW;
int ValH231 = LOW;
int ValH241 = LOW;

//---------------------------------------------------------------------------

// Arduino MAC address must be unique for every node in same network
// To make a new unique address change last letter
// Arduino 1
byte mac[]    = { 0xCC, 0xFB, 0x0F, 0xCA, 0x19, 0x00 };  

// Unique static IP address of this Arduino 0
IPAddress ip(10,1,1,211);


// IP Address of your MQTT broker (OpenHAB server)
byte server[] = { 10, 1, 1, 200 };

// Handle and convert incoming MQTT messages ----------------------------------------

void callback(char* topic, byte* payload, unsigned int length) {
  // handle message arrived
  String content="";
  char character;
  for (int num=0;num<length;num++) {
      character = payload[num];
      content.concat(character);
  }   
  

// Set specific virtual switches on basis of specific incoming messages ----------------------------
  if (content == "L011on") {    ValL011 = HIGH; }
  if (content == "L011off"){    ValL011 = LOW;  }
  if (content == "L012on") {    ValL012 = HIGH; }
  if (content == "L012off"){    ValL012 = LOW;  }
  if (content == "L013on") {    ValL013 = HIGH; }
  if (content == "L013off"){    ValL013 = LOW;  }
  if (content == "L014on") {    ValL014 = HIGH; }
  if (content == "L014off"){    ValL014 = LOW;  }
  if (content == "L021on") {    ValL021 = HIGH; }
  if (content == "L021off"){    ValL021 = LOW;  }
  if (content == "L022on") {    ValL022 = HIGH; }
  if (content == "L022off"){    ValL022 = LOW;  }
  if (content == "L031on") {    ValL031 = HIGH; }
  if (content == "L031off"){    ValL031 = LOW;  }
  if (content == "L041on") {    ValL041 = HIGH; }
  if (content == "L041off"){    ValL041 = LOW;  }
  if (content == "L042on") {    ValL042 = HIGH; }
  if (content == "L042off"){    ValL042 = LOW;  }
  if (content == "L043on") {    ValL043 = HIGH; }
  if (content == "L043off"){    ValL043 = LOW;  }

  if (content == "L111on") {    ValL111 = HIGH; }
  if (content == "L111off"){    ValL111 = LOW;  }
  if (content == "L112on") {    ValL112 = HIGH; }
  if (content == "L112off"){    ValL112 = LOW;  }
  if (content == "L121on") {    ValL121 = HIGH; }
  if (content == "L121off"){    ValL121 = LOW;  }
  if (content == "L122on") {    ValL122 = HIGH; }
  if (content == "L122off"){    ValL122 = LOW;  }
  if (content == "L131on") {    ValL131 = HIGH; }
  if (content == "L131off"){    ValL131 = LOW;  }
  if (content == "L141on") {    ValL141 = HIGH; }
  if (content == "L141off"){    ValL141 = LOW;  }
  if (content == "L151on") {    ValL151 = HIGH; }
  if (content == "L151off"){    ValL151 = LOW;  }
  if (content == "L152on") {    ValL152 = HIGH; }
  if (content == "L152off"){    ValL152 = LOW;  }
  if (content == "L153on") {    ValL153 = HIGH; }
  if (content == "L153off"){    ValL153 = LOW;  }
  if (content == "L154on") {    ValL154 = HIGH; }
  if (content == "L154off"){    ValL154 = LOW;  }
  if (content == "L155on") {    ValL155 = HIGH; }
  if (content == "L155off"){    ValL155 = LOW;  }
  if (content == "L156on") {    ValL156 = HIGH; }
  if (content == "L156off"){    ValL156 = LOW;  }
  if (content == "L161on") {    ValL161 = HIGH; }
  if (content == "L161off"){    ValL161 = LOW;  }

  if (content == "L201on") {    ValL201 = HIGH; }
  if (content == "L201off"){    ValL201 = LOW;  }
  if (content == "L211on") {    ValL211 = HIGH; }
  if (content == "L211off"){    ValL211 = LOW;  }
  if (content == "L212on") {    ValL212 = HIGH; }
  if (content == "L212off"){    ValL212 = LOW;  }
  if (content == "L221on") {    ValL221 = HIGH; }
  if (content == "L221off"){    ValL221 = LOW;  }
  if (content == "L231on") {    ValL231 = HIGH; }
  if (content == "L231off"){    ValL231 = LOW;  }
  if (content == "L241on") {    ValL241 = HIGH; }
  if (content == "L241off"){    ValL241 = LOW;  }

  if (content == "H111on") {    ValH111 = HIGH; }
  if (content == "H111off"){    ValH111 = LOW;  }
  if (content == "H121on") {    ValH121 = HIGH; }
  if (content == "H121off"){    ValH121 = LOW;  }
  if (content == "H131on") {    ValH131 = HIGH; }
  if (content == "H131off"){    ValH131 = LOW;  }
  if (content == "H151on") {    ValH151 = HIGH; }
  if (content == "H151off"){    ValH151 = LOW;  }
  if (content == "H152on") {    ValH152 = HIGH; }
  if (content == "H152off"){    ValH152 = LOW;  }

  if (content == "H201on") {    ValH201 = HIGH; }
  if (content == "H201off"){    ValH201 = LOW;  }
  if (content == "H211on") {    ValH211 = HIGH; }
  if (content == "H211off"){    ValH211 = LOW;  }
  if (content == "H221on") {    ValH221 = HIGH; }
  if (content == "H221off"){    ValH221 = LOW;  }
  if (content == "H231on") {    ValH231 = HIGH; }
  if (content == "H231off"){    ValH231 = LOW;  }
  if (content == "H241on") {    ValH241 = HIGH; }
  if (content == "H241off"){    ValH241 = LOW;  }

  // Set digital pin states according to virtual switch settings
  digitalWrite(L011, ValL011);  
  digitalWrite(L012, ValL012);
  digitalWrite(L013, ValL013);
  digitalWrite(L014, ValL014);
  digitalWrite(L021, ValL021);
  digitalWrite(L022, ValL022);
  digitalWrite(L031, ValL031);
  digitalWrite(L041, ValL041);
  digitalWrite(L042, ValL042);
  digitalWrite(L043, ValL043);

  digitalWrite(L111, ValL111);  
  digitalWrite(L112, ValL112);
  digitalWrite(L121, ValL121);
  digitalWrite(L122, ValL122);
  digitalWrite(L131, ValL131);
  digitalWrite(L141, ValL141);
  digitalWrite(L151, ValL151);
  digitalWrite(L152, ValL152);
  digitalWrite(L153, ValL153);
  digitalWrite(L154, ValL154);
  digitalWrite(L155, ValL155);
  digitalWrite(L156, ValL156);
  digitalWrite(L161, ValL161);

  digitalWrite(L201, ValL201);  
  digitalWrite(L211, ValL211);
  digitalWrite(L212, ValL212);
  digitalWrite(L221, ValL221);
  digitalWrite(L231, ValL231);
  digitalWrite(L241, ValL241);

  digitalWrite(H111, ValH111);  
  digitalWrite(H121, ValH121);
  digitalWrite(H131, ValH131);
  digitalWrite(H151, ValH151);
  digitalWrite(H152, ValH152);

  digitalWrite(H201, ValH201);  
  digitalWrite(H211, ValH211);
  digitalWrite(H221, ValH221);
  digitalWrite(H231, ValH231);
  digitalWrite(H241, ValH241);
}

// Initiate instances -----------------------------------
EthernetClient arduino1;
PubSubClient client(server, 1883, callback, arduino1);
//-------------------------------------------------------

// ============================================================================
// MQTT RECONNECT (non-blocking)
// ============================================================================
const long RECONNECT_INTERVAL = 5000;
long lastReconnectAttempt = 0;

boolean reconnect() {
  if (client.connect("arduino1", "opentux", "opentux")) {
    // Once connected, publish an announcement...
    client.publish("pihome","Arduino PA01 - reconnected");
    client.subscribe("pihome/lights");
    client.subscribe("pihome/sockets");
    return true;
  }
  return false;
}

// Watchdog ping
unsigned long lastWdtReset = 0;
const long WDT_RESET_INTERVAL = 2500;  // Reset 2.5s ( 4s timeout)

void setup()
{
  // Watchdog
  setupWatchdog();

  digitalWrite(L011, LOW);  
  digitalWrite(L012, LOW);
  digitalWrite(L013, LOW);
  digitalWrite(L014, LOW);
  digitalWrite(L021, LOW);
  digitalWrite(L022, LOW);
  digitalWrite(L031, LOW);
  digitalWrite(L041, LOW);
  digitalWrite(L042, LOW);
  digitalWrite(L043, LOW);
  digitalWrite(L111, LOW);  
  digitalWrite(L112, LOW);
  digitalWrite(L121, LOW);
  digitalWrite(L122, LOW);
  digitalWrite(L131, LOW);
  digitalWrite(L141, LOW);
  digitalWrite(L151, LOW);
  digitalWrite(L152, LOW);
  digitalWrite(L153, LOW);
  digitalWrite(L154, LOW);
  digitalWrite(L155, LOW);
  digitalWrite(L156, LOW);
  digitalWrite(L161, LOW);
  digitalWrite(L201, LOW);  
  digitalWrite(L211, LOW);
  digitalWrite(L212, LOW);
  digitalWrite(L221, LOW);
  digitalWrite(L231, LOW);
  digitalWrite(L241, LOW);
  digitalWrite(H111, LOW);  
  digitalWrite(H121, LOW);
  digitalWrite(H131, LOW);
  digitalWrite(H151, LOW);
  digitalWrite(H152, LOW);
  digitalWrite(H201, LOW);  
  digitalWrite(H211, LOW);
  digitalWrite(H221, LOW);
  digitalWrite(H231, LOW);
  digitalWrite(H241, LOW);
  
  pinMode(L011, OUTPUT);  
  pinMode(L012, OUTPUT);
  pinMode(L013, OUTPUT);
  pinMode(L014, OUTPUT);
  pinMode(L021, OUTPUT);
  pinMode(L022, OUTPUT);
  pinMode(L031, OUTPUT);
  pinMode(L041, OUTPUT);
  pinMode(L042, OUTPUT);
  pinMode(L043, OUTPUT);
  pinMode(L111, OUTPUT);  
  pinMode(L112, OUTPUT);
  pinMode(L121, OUTPUT);
  pinMode(L122, OUTPUT);
  pinMode(L131, OUTPUT);
  pinMode(L141, OUTPUT);
  pinMode(L151, OUTPUT);
  pinMode(L152, OUTPUT);
  pinMode(L153, OUTPUT);
  pinMode(L154, OUTPUT);
  pinMode(L155, OUTPUT);
  pinMode(L156, OUTPUT);
  pinMode(L161, OUTPUT);
  pinMode(L201, OUTPUT);  
  pinMode(L211, OUTPUT);
  pinMode(L212, OUTPUT);
  pinMode(L221, OUTPUT);
  pinMode(L231, OUTPUT);
  pinMode(L241, OUTPUT);
  pinMode(H111, OUTPUT);  
  pinMode(H121, OUTPUT);
  pinMode(H131, OUTPUT);
  pinMode(H151, OUTPUT);
  pinMode(H152, OUTPUT);
  pinMode(H201, OUTPUT);  
  pinMode(H211, OUTPUT);
  pinMode(H221, OUTPUT);
  pinMode(H231, OUTPUT);
  pinMode(H241, OUTPUT);
 
	  
  // Setup ethernet connection to MQTT broker
  Ethernet.begin(mac, ip);
  delay(500);

  if (client.connect("arduino1", "opentux", "opentux")) {  							
    client.publish("pihome","Hello world - here Arduino PA01 with IP 10.1.1.211");
    client.subscribe("pihome/lights");
    client.subscribe("pihome/sockets");										                       
  }

  lastReconnectAttempt = millis();
  lastWdtReset = millis();
}

// ============================================================================
// LOOP - non-blocking, watchdog priority
// ============================================================================
void loop()
{
  unsigned long now = millis();  

  // *** WATCHDOG RESET - PRIORITY 1 ***
  if (now - lastWdtReset > WDT_RESET_INTERVAL) {
    wdt_reset();
    lastWdtReset = now;
  }

  // *** MQTT CONNECT/RECONNECT (non-blocking) ***
  if (!client.connected()) {
    if (now - lastReconnectAttempt > RECONNECT_INTERVAL) {
      lastReconnectAttempt = now;
      reconnect();
    }
  } else {
    client.loop();
  }
}

// End of sketch ---------------------------------
