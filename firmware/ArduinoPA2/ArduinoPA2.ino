/*
 Arduino 2 (PA02) - DEMO
 
  - connects to an MQTT server
  - publishes "hello world" to the topic "relay"
  - subscribes to the topic "relay"
  - controls as many relays as defined
  - turns on/off a specific led when it receives a specific "on"/"off" from the "relay" topic
  - multiple arduino's with same generic sketch can run parallel to each other
  - multiple arduino's need each to have a unique ip-addres, unique mac address and unique MQTT client-ID
  - tested on arduino-mega with W5100 ethernet shield

*/

//------------------------------------------------------------------------------

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <avr/wdt.h>

// ============================================================================
// WATCHDOG TIMER - 4 sec timeout
// ============================================================================
void setupWatchdog() {
  cli();
  wdt_reset();
  WDTCSR = (1 << WDCE) | (1 << WDE);
  WDTCSR = (1 << WDE) | (1 << WDP3);  // 4 sec timeout
  sei();
}

// Set relay variables to Arduino digital pins

//PR07
// pin 0 RX
// pin 1 TX
// pin 4 (SS for SD Card)
int S011 = 5;
int S012 = 6;
int S013 = 7;	     
int S014 = 8;
int S015 = 9;
// pin 10 (SS for Ethernet)
int S016 = 11;
int S111 = 12;
int R241a = 64; 

//PR08
int S121 = 2;
int S131 = 3;
int S141 = 14;
int S142 = 15;
int S151 = 16;
int S152 = 17;
int S153 = 18;
int S154 = 19;
// pin 20 SDA
// pin 21 SCL

//PR9
int S155  = 23;
int S161  = 25;
int S201  = 27;
int S211  = 29;
int S221  = 31;
int S231  = 33;
int S241  = 35;
int R241b = 37;

//PR10     
int R111a = 22;
int R111b = 24;
int R131a = 26;
int R131b = 28;
int R132a = 30;
int R132b = 32;
int R141a = 34;
int R141b = 36;		   

//PR11     
int R151a = 39;
int R151b = 41;
int R152a = 43;
int R152b = 45;
int R153a = 47;
int R153b = 49;
int R154a = 38;
int R154b = 40; 
// pin 50 (SS for Ethernet)
// pin 51 (MOSI)
// pin 52 (MISO)
// pin 53 (SCK)

//PR12
//A0 (D54)
int R155a = 54;
int R155b = 55;
int R211a = 56;
int R211b = 57;
int R221a = 58;
int R221b = 59; 
// pin 60 inaccesibble 
int R231a = 62; 
int R231b = 63;
//int rez = 65;
//int rez = 66;
//int rez = 67;
//int rez = 68;
int resetPIN = 69;
//A15 (D69)


int ValS011 = LOW;
int ValS012 = LOW;
int ValS013 = LOW;
int ValS014 = LOW;
int ValS015 = LOW;
int ValS016 = LOW;

int ValS111 = LOW;
int ValS121 = LOW;
int ValS131 = LOW;
int ValS141 = LOW;
int ValS142 = LOW;
int ValS151 = LOW;
int ValS152 = LOW;
int ValS153 = LOW;
int ValS154 = LOW;
int ValS155 = LOW;
int ValS161 = LOW;

int ValS201 = LOW;
int ValS211 = LOW;
int ValS221 = LOW;
int ValS231 = LOW;
int ValS241 = LOW;

int ValR111a = LOW;
int ValR111b = LOW;
int ValR131a = LOW;
int ValR131b = LOW;
int ValR132a = LOW;
int ValR132b = LOW;
int ValR141a = LOW;
int ValR141b = LOW;
int ValR151a = LOW;
int ValR151b = LOW;
int ValR152a = LOW;
int ValR152b = LOW;
int ValR153a = LOW;
int ValR153b = LOW;
int ValR154a = LOW;
int ValR154b = LOW;
int ValR155a = LOW;
int ValR155b = LOW;
int ValR211a = LOW;
int ValR211b = LOW;
int ValR221a = LOW;
int ValR221b = LOW;
int ValR231a = LOW;
int ValR231b = LOW;

//---------------------------------------------------------------------------

// Arduino MAC address must be unique for every node in same network
// To make a new unique address change last letter
// Arduino 1
byte mac[]    = { 0xCC, 0xFC, 0x09, 0xCB, 0x19, 0x01 };  

// Unique static IP address of this Arduino 1
IPAddress ip(10,1,1,212);


// IP Address of your MQTT broker (OpenHAB server)
byte server[] = { 10, 1, 1, 200 };

// ============================================================================
// SHUTTER CONTROL - 300ms dead-time protection
// ============================================================================
void shutterSet(int pinUp, int pinDown, int &valUp, int &valDown, bool up, bool down) {
  bool wasActive = (valUp == HIGH) || (valDown == HIGH);

  // turn off both
  valUp = LOW;
  valDown = LOW;
  digitalWrite(pinUp, LOW);
  digitalWrite(pinDown, LOW);

  if (up || down) {
    if (wasActive) {
      // if up/down active -> wait 300 ms before change direction
      unsigned long t = millis();
      while (millis() - t < 300) {
        wdt_reset();   // watchdog nesmi vyprset
      }
    }
    if (up)   { valUp = HIGH;   digitalWrite(pinUp, HIGH); }
    if (down) { valDown = HIGH; digitalWrite(pinDown, HIGH); }
  }
}

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
  
  if (content == "S011on") {    ValS011 = HIGH; }
  if (content == "S011off"){    ValS011 = LOW;  }
  if (content == "S012on") {    ValS012 = HIGH; }
  if (content == "S012off"){    ValS012 = LOW;  }
  if (content == "S013on") {    ValS013 = HIGH; }
  if (content == "S013off"){    ValS013 = LOW;  }
  if (content == "S014on") {    ValS014 = HIGH; }
  if (content == "S014off"){    ValS014 = LOW;  }
  if (content == "S015on") {    ValS015 = HIGH; }
  if (content == "S015off"){    ValS015 = LOW;  }
  if (content == "S016on") {    ValS016 = HIGH; }
  if (content == "S016off"){    ValS016 = LOW;  }

  if (content == "S111on") {    ValS111 = HIGH; }
  if (content == "S111off"){    ValS111 = LOW;  }
  if (content == "S121on") {    ValS121 = HIGH; }
  if (content == "S121off"){    ValS121 = LOW;  }
  if (content == "S131on") {    ValS131 = HIGH; }
  if (content == "S131off"){    ValS131 = LOW;  }
  if (content == "S141on") {    ValS141 = HIGH; }
  if (content == "S141off"){    ValS141 = LOW;  }
  if (content == "S142on") {    ValS142 = HIGH; }
  if (content == "S142off"){    ValS142 = LOW;  }
  if (content == "S151on") {    ValS151 = HIGH; }
  if (content == "S151off"){    ValS151 = LOW;  }
  if (content == "S152on") {    ValS152 = HIGH; }
  if (content == "S152off"){    ValS152 = LOW;  }
  if (content == "S153on") {    ValS153 = HIGH; }
  if (content == "S153off"){    ValS153 = LOW;  }
  if (content == "S154on") {    ValS154 = HIGH; }
  if (content == "S154off"){    ValS154 = LOW;  }
  if (content == "S155on") {    ValS155 = HIGH; }
  if (content == "S155off"){    ValS155 = LOW;  }
  if (content == "S161on") {    ValS161 = HIGH; }
  if (content == "S161off"){    ValS161 = LOW;  }

  if (content == "S201on") {    ValS201 = HIGH; }
  if (content == "S201off"){    ValS201 = LOW;  }
  if (content == "S211on") {    ValS211 = HIGH; }
  if (content == "S211off"){    ValS211 = LOW;  }
  if (content == "S221on") {    ValS221 = HIGH; }
  if (content == "S221off"){    ValS221 = LOW;  }
  if (content == "S231on") {    ValS231 = HIGH; }
  if (content == "S231off"){    ValS231 = LOW;  }
  if (content == "S241on") {    ValS241 = HIGH; }
  if (content == "S241off"){    ValS241 = LOW;  }

  // Shutters (pair a=up, b=down, 300ms dead-time)
  if (content == "R111on")     shutterSet(R111a, R111b, ValR111a, ValR111b, true, false);
  if (content == "R111down")   shutterSet(R111a, R111b, ValR111a, ValR111b, false, true);
  if (content == "R111off")    shutterSet(R111a, R111b, ValR111a, ValR111b, false, false);

  if (content == "R131on")     shutterSet(R131a, R131b, ValR131a, ValR131b, true, false);
  if (content == "R131down")   shutterSet(R131a, R131b, ValR131a, ValR131b, false, true);
  if (content == "R131off")    shutterSet(R131a, R131b, ValR131a, ValR131b, false, false);

  if (content == "R132on")     shutterSet(R132a, R132b, ValR132a, ValR132b, true, false);
  if (content == "R132down")   shutterSet(R132a, R132b, ValR132a, ValR132b, false, true);
  if (content == "R132off")    shutterSet(R132a, R132b, ValR132a, ValR132b, false, false);

  if (content == "R141on")     shutterSet(R141a, R141b, ValR141a, ValR141b, true, false);
  if (content == "R141down")   shutterSet(R141a, R141b, ValR141a, ValR141b, false, true);
  if (content == "R141off")    shutterSet(R141a, R141b, ValR141a, ValR141b, false, false);

  if (content == "R151on")     shutterSet(R151a, R151b, ValR151a, ValR151b, true, false);
  if (content == "R151down")   shutterSet(R151a, R151b, ValR151a, ValR151b, false, true);
  if (content == "R151off")    shutterSet(R151a, R151b, ValR151a, ValR151b, false, false);

  if (content == "R152on")     shutterSet(R152a, R152b, ValR152a, ValR152b, true, false);
  if (content == "R152down")   shutterSet(R152a, R152b, ValR152a, ValR152b, false, true);
  if (content == "R152off")    shutterSet(R152a, R152b, ValR152a, ValR152b, false, false);

  if (content == "R153on")     shutterSet(R153a, R153b, ValR153a, ValR153b, true, false);
  if (content == "R153down")   shutterSet(R153a, R153b, ValR153a, ValR153b, false, true);
  if (content == "R153off")    shutterSet(R153a, R153b, ValR153a, ValR153b, false, false);

  if (content == "R154on")     shutterSet(R154a, R154b, ValR154a, ValR154b, true, false);
  if (content == "R154down")   shutterSet(R154a, R154b, ValR154a, ValR154b, false, true);
  if (content == "R154off")    shutterSet(R154a, R154b, ValR154a, ValR154b, false, false);

  if (content == "R155on")     shutterSet(R155a, R155b, ValR155a, ValR155b, true, false);
  if (content == "R155down")   shutterSet(R155a, R155b, ValR155a, ValR155b, false, true);
  if (content == "R155off")    shutterSet(R155a, R155b, ValR155a, ValR155b, false, false);

  if (content == "R211on")     shutterSet(R211a, R211b, ValR211a, ValR211b, true, false);
  if (content == "R211down")   shutterSet(R211a, R211b, ValR211a, ValR211b, false, true);
  if (content == "R211off")    shutterSet(R211a, R211b, ValR211a, ValR211b, false, false);

  if (content == "R221on")     shutterSet(R221a, R221b, ValR221a, ValR221b, true, false);
  if (content == "R221down")   shutterSet(R221a, R221b, ValR221a, ValR221b, false, true);
  if (content == "R221off")    shutterSet(R221a, R221b, ValR221a, ValR221b, false, false);

  if (content == "R231on")     shutterSet(R231a, R231b, ValR231a, ValR231b, true, false);
  if (content == "R231down")   shutterSet(R231a, R231b, ValR231a, ValR231b, false, true);
  if (content == "R231off")    shutterSet(R231a, R231b, ValR231a, ValR231b, false, false);

  // Set digital pin states according to virtual switch settings
  digitalWrite(S011, ValS011);  
  digitalWrite(S012, ValS012);
  digitalWrite(S013, ValS013);
  digitalWrite(S014, ValS014);
  digitalWrite(S015, ValS015);
  digitalWrite(S016, ValS016);

  digitalWrite(S111, ValS111);  
  digitalWrite(S121, ValS121);
  digitalWrite(S131, ValS131);
  digitalWrite(S141, ValS141);
  digitalWrite(S142, ValS142);
  digitalWrite(S151, ValS151);
  digitalWrite(S152, ValS152);
  digitalWrite(S153, ValS153);
  digitalWrite(S154, ValS154);
  digitalWrite(S155, ValS155);
  digitalWrite(S161, ValS161);

  digitalWrite(S201, ValS201);  
  digitalWrite(S211, ValS211);
  digitalWrite(S221, ValS221);
  digitalWrite(S231, ValS231);
  digitalWrite(S241, ValS241);
}

// Initiate instances -----------------------------------

EthernetClient arduino2;
PubSubClient client(server, 1883, callback, arduino2);

// ============================================================================
// MQTT RECONNECT (non-blocking)
// ============================================================================
const long RECONNECT_INTERVAL = 5000;
long lastReconnectAttempt = 0;

boolean reconnect() {
  if (client.connect("arduino2", "opentux", "opentux")) {
    // Once connected, publish an announcement...
    client.publish("pihome","Arduino PA02 - reconnected");
    client.subscribe("pihome/sockets");
    client.subscribe("pihome/heating");	
    client.subscribe("pihome/shutters");
    return true;
  }
  return false;
}

// Watchdog ping
unsigned long lastWdtReset = 0;
const long WDT_RESET_INTERVAL = 2500;  // Reset watchdog 2.5s (4s timeout)


void setup()
{
  // Watchdog
  setupWatchdog();

  digitalWrite(S011, LOW);  
  digitalWrite(S012, LOW);
  digitalWrite(S013, LOW);
  digitalWrite(S014, LOW);
  digitalWrite(S015, LOW);
  digitalWrite(S016, LOW);
  digitalWrite(S111, LOW);  
  digitalWrite(S121, LOW);
  digitalWrite(S131, LOW);
  digitalWrite(S141, LOW);
  digitalWrite(S142, LOW);
  digitalWrite(S151, LOW);
  digitalWrite(S152, LOW);
  digitalWrite(S153, LOW);
  digitalWrite(S154, LOW);
  digitalWrite(S155, LOW);
  digitalWrite(S161, LOW);
  digitalWrite(S201, LOW);  
  digitalWrite(S211, LOW);
  digitalWrite(S221, LOW);
  digitalWrite(S231, LOW);
  digitalWrite(S241, LOW);

  digitalWrite(R111a, LOW);
  digitalWrite(R111b, LOW);
  digitalWrite(R131a, LOW);
  digitalWrite(R131b, LOW);
  digitalWrite(R132a, LOW);
  digitalWrite(R132b, LOW);
  digitalWrite(R141a, LOW);
  digitalWrite(R141b, LOW);
  digitalWrite(R151a, LOW);
  digitalWrite(R151b, LOW);
  digitalWrite(R152a, LOW);
  digitalWrite(R152b, LOW);
  digitalWrite(R153a, LOW);
  digitalWrite(R153b, LOW);
  digitalWrite(R154a, LOW);
  digitalWrite(R154b, LOW);
  digitalWrite(R155a, LOW);
  digitalWrite(R155b, LOW);
  digitalWrite(R211a, LOW);
  digitalWrite(R211b, LOW);
  digitalWrite(R221a, LOW);
  digitalWrite(R221b, LOW);
  digitalWrite(R231a, LOW);
  digitalWrite(R231b, LOW);

  pinMode(S011, OUTPUT);  
  pinMode(S012, OUTPUT);
  pinMode(S013, OUTPUT);
  pinMode(S014, OUTPUT);
  pinMode(S015, OUTPUT);
  pinMode(S016, OUTPUT);
  pinMode(S111, OUTPUT);  
  pinMode(S121, OUTPUT);
  pinMode(S131, OUTPUT);
  pinMode(S141, OUTPUT);
  pinMode(S142, OUTPUT);
  pinMode(S151, OUTPUT);
  pinMode(S152, OUTPUT);
  pinMode(S153, OUTPUT);
  pinMode(S154, OUTPUT);
  pinMode(S155, OUTPUT);
  pinMode(S161, OUTPUT);
  pinMode(S201, OUTPUT);  
  pinMode(S211, OUTPUT);
  pinMode(S221, OUTPUT);
  pinMode(S231, OUTPUT);
  pinMode(S241, OUTPUT);

  pinMode(R111a, OUTPUT);
  pinMode(R111b, OUTPUT);
  pinMode(R131a, OUTPUT);
  pinMode(R131b, OUTPUT);
  pinMode(R132a, OUTPUT);
  pinMode(R132b, OUTPUT);
  pinMode(R141a, OUTPUT);
  pinMode(R141b, OUTPUT);
  pinMode(R151a, OUTPUT);
  pinMode(R151b, OUTPUT);
  pinMode(R152a, OUTPUT);
  pinMode(R152b, OUTPUT);
  pinMode(R153a, OUTPUT);
  pinMode(R153b, OUTPUT);
  pinMode(R154a, OUTPUT);
  pinMode(R154b, OUTPUT);
  pinMode(R155a, OUTPUT);
  pinMode(R155b, OUTPUT);
  pinMode(R211a, OUTPUT);
  pinMode(R211b, OUTPUT);
  pinMode(R221a, OUTPUT);
  pinMode(R221b, OUTPUT);
  pinMode(R231a, OUTPUT);
  pinMode(R231b, OUTPUT);
       
  // Setup ethernet connection to MQTT broker
  Ethernet.begin(mac, ip);
  delay(500);

  if (client.connect("arduino2", "opentux", "opentux")) {  							
    client.publish("pihome","Hello world - here Arduino PA02 with IP 10.1.1.212");
    client.subscribe("pihome/sockets");
    client.subscribe("pihome/heating");	
    client.subscribe("pihome/shutters");											                       
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
