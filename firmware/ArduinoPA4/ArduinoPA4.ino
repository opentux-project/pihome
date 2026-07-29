/*
 Arduino 4 (PA04) - demo/12-zone verze

  - connects to an MQTT server
  - publishes "hello world" to the topic "pihome"
  - subscribes to the specific topic pihome/xxx
  - collect and sends a values from PIR, DHT22 and DS18B20 sensors
  - multiple arduino's with same generic sketch can run parallel to each other
  - multiple arduino's need each to have a unique ip-addres, unique mac address and unique MQTT client-ID
  - tested on arduino-mega with W5100 ethernet shield

  Updated: watchdog timer + non-blocking reconnect logic (dle aktualniho PROD vzoru)
*/

//------------------------------------------------------------------------------

//MQTT
#include <PubSubClient.h>
#include <Ethernet2.h>
#include <avr/wdt.h>

//DHT+DS18B20
#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>

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

//Time variables
unsigned long time_now = 0;
unsigned long time_now1 = 0;

//Strings for MQTT
char buff_ST111[10];
char buff_ST121[10];
char buff_ST131[10];
char buff_ST141[10];
char buff_ST151[10];
char buff_ST152[10];
char buff_ST161[10];
char buff_ST201[10];
char buff_ST211[10];
char buff_ST221[10];
char buff_ST231[10];
char buff_ST241[10];

char buff_SH111[10];
char buff_SH121[10];
char buff_SH131[10];
char buff_SH141[10];
char buff_SH151[10];
char buff_SH152[10];
char buff_SH161[10];
char buff_SH201[10];
char buff_SH211[10];
char buff_SH221[10];
char buff_SH231[10];
char buff_SH241[10];



//Variables for floor temperatures
float SF111 =0;
float SF121 =0;
float SF131 =0;
float SF151 =0;
float SF152 =0;
float SF201 =0;
float SF211 =0;
float SF221 =0;
float SF231 =0;
float SF241 =0;

char buff_SF111[10];
char buff_SF121[10];
char buff_SF131[10];
char buff_SF151[10];
char buff_SF152[10];
char buff_SF201[10];
char buff_SF211[10];
char buff_SF221[10];
char buff_SF231[10];
char buff_SF241[10];


// No motion detected at start
int SP111 = 0;
int SP121 = 0;
int SP131 = 0;  
int SP141 = 0;     
int SP151 = 0;
int SP152 = 0;
int SP161 = 0;
int SP201 = 0;
int SP211 = 0;
int SP221 = 0;
int SP231 = 0;
int SP241 = 0;

//Last values for PIR
int lastSP111 = LOW;
int lastSP121 = LOW;
int lastSP131 = LOW;  
int lastSP141 = LOW;     
int lastSP151 = LOW;
int lastSP152 = LOW;
int lastSP161 = LOW;
int lastSP201 = LOW;
int lastSP211 = LOW;
int lastSP221 = LOW;
int lastSP231 = LOW;
int lastSP241 = LOW;

//OneWire (DS1820)
#define ONE_WIRE_BUS 28 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
//**********Change this Dallas sensosrs MAC to your devices**********// 
DeviceAddress mac_SF111   = { 0x28, 0xFF, 0x8D, 0xCB, 0x55, 0x16, 0x3, 0x28 };
DeviceAddress mac_SF121   = { 0x28, 0xFF, 0x8D, 0xCB, 0x55, 0x16, 0x3, 0x28 };
DeviceAddress mac_SF131   = { 0x28, 0xFF, 0x8D, 0xCB, 0x55, 0x16, 0x3, 0x28 };
DeviceAddress mac_SF151   = { 0x28, 0xFF, 0x8D, 0xCB, 0x55, 0x16, 0x3, 0x28 };
DeviceAddress mac_SF152   = { 0x28, 0xFF, 0x8D, 0xCB, 0x55, 0x16, 0x3, 0x28 };
DeviceAddress mac_SF201   = { 0x28, 0xFF, 0x8D, 0xCB, 0x55, 0x16, 0x3, 0x28 };
DeviceAddress mac_SF211   = { 0x28, 0xFF, 0x8D, 0xCB, 0x55, 0x16, 0x3, 0x28 };
DeviceAddress mac_SF221   = { 0x28, 0xFF, 0x8D, 0xCB, 0x55, 0x16, 0x3, 0x28 };
DeviceAddress mac_SF231   = { 0x28, 0xFF, 0x8D, 0xCB, 0x55, 0x16, 0x3, 0x28 };
DeviceAddress mac_SF241   = { 0x28, 0xFF, 0x8D, 0xCB, 0x55, 0x16, 0x3, 0x28 };

//DHT22
#define DHTP111 6 
#define DHTP121 7   
#define DHTP131 8     
#define DHTP141 9 
#define DHTP151 11 
#define DHTP152 12   
#define DHTP161 13     
#define DHTP201 2 
#define DHTP211 3 
#define DHTP221 14   
#define DHTP231 15     
#define DHTP241 16 

#define DHTTYPE DHT22    
     
DHT DHT111(DHTP111, DHTTYPE);
DHT DHT121(DHTP121, DHTTYPE);
DHT DHT131(DHTP131, DHTTYPE);
DHT DHT141(DHTP141, DHTTYPE);
DHT DHT151(DHTP151, DHTTYPE);
DHT DHT152(DHTP152, DHTTYPE);
DHT DHT161(DHTP161, DHTTYPE);
DHT DHT201(DHTP201, DHTTYPE);
DHT DHT211(DHTP211, DHTTYPE);
DHT DHT221(DHTP221, DHTTYPE);
DHT DHT231(DHTP231, DHTTYPE);
DHT DHT241(DHTP241, DHTTYPE);


//PIR's pins
int SP111P = 33;    
int SP121P = 35;    
int SP131P = 37;    
int SP141P = 39;    
int SP151P = 41;    
int SP152P = 43;    
int SP161P = 45;
int SP201P = 47;
int SP211P = 49;
int SP221P = 22; 
int SP231P = 24;
int SP241P = 26;

// Arduino MAC address must be unique for every node in same network
// To make a new unique address change last letter
// Arduino 4
byte mac[]    = { 0xCC, 0xFA, 0x0B, 0xC4, 0x19, 0x01 };  

// Unique static IP address
IPAddress ip(10 ,1 ,1 ,214); 

// IP Address of your MQTT broker (OpenHAB server)
byte server[] = { 10, 1, 1, 200 };

// Handle and convert incoming MQTT messages ----------------------------------------
void callback(char* topic, byte* payload, unsigned int length) {
  }   
	
// Initiate instances -----------------------------------
EthernetClient arduino4;
PubSubClient client(server, 1883, callback, arduino4);

// ============================================================================
// MQTT RECONNECT (non-blocking)
// ============================================================================
const long RECONNECT_INTERVAL = 5000;
long lastReconnectAttempt = 0;

boolean reconnect() {
  if (client.connect("arduino4", "opentux", "opentux")) {
    // Once connected, publish an announcement...
    client.publish("pihome","Arduino 4 - reconnected");
    return true;
  }
  return false;
}

// Watchdog ping
unsigned long lastWdtReset = 0;
const long WDT_RESET_INTERVAL = 2500;  // Reset watchdog 2.5s (4s timeout)


void setup(){
  // Watchdog
  setupWatchdog();

  //Only for diagnostics when you need to read DS18B20 mac addresses 
  //Serial.begin(115200);

  sensors.begin();      
  DHT111.begin();
  DHT121.begin();
  DHT131.begin();
  DHT141.begin();	
  DHT151.begin();
  DHT152.begin();
  DHT161.begin();
  DHT201.begin();	
  DHT211.begin();
  DHT221.begin();
  DHT231.begin();
  DHT241.begin();	

  // Setup ethernet connection to MQTT broker
  Ethernet.begin(mac, ip);
  delay(500);

  if (client.connect("arduino4", "opentux", "opentux")) {
    client.publish("pihome", "Hello world - here Arduino PA04 with IP 10.1.1.214");
  }

  pinMode(SP111P, INPUT);
  pinMode(SP121P, INPUT);
  pinMode(SP131P, INPUT);
  pinMode(SP141P, INPUT);
  pinMode(SP151P, INPUT);
  pinMode(SP152P, INPUT);
  pinMode(SP161P, INPUT);
  pinMode(SP201P, INPUT);
  pinMode(SP211P, INPUT);
  pinMode(SP221P, INPUT);
  pinMode(SP231P, INPUT);
  pinMode(SP241P, INPUT);

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
    // Client connected
    client.loop();
  }

  //Only for diagnostics when you need to read DS18B20 mac addresses 
  //if(millis() >= time_now1 + 5000){
  //   time_now1 += 5000;
  //     //Print all physical addresses for oneWire to Serial port
  //     byte i;
  //     byte addr[8];
  //     int id=0;	 
  //     
  //     Serial.print("test");
  //     
  //     while(oneWire.search(addr))
  //     {
  //         Serial.print(id);
  //         Serial.print(": "); 
  //       for( i = 0; i < 8; i++) {      
  //         Serial.print(addr[i], HEX);
  //         Serial.print(" ");
  //         }    
  //         Serial.println("");      
  //       id++;   
  //     }
  //     oneWire.reset_search();
  //     
  //}  

  // Collect sensor data every 30 sec
  if (now - time_now >= 30000) {
    time_now = now;

    //============DHT Sensors - Temperature + Humidity============

    //DHT sensor - read temp, convert and send via MQTT

    //Temperature
    float ST111 = DHT111.readTemperature();
    if ((ST111 >=-20) && (ST111<= 99)){
       dtostrf(ST111,3, 1, buff_ST111);
       client.publish("pihome/sensor/temp/st111", buff_ST111);
       }
    float ST121 = DHT121.readTemperature();
    if ((ST121 >=-20) && (ST121<= 99)){
       dtostrf(ST121,3, 1, buff_ST121);
       client.publish("pihome/sensor/temp/st121", buff_ST121);
       }
    float ST131 = DHT131.readTemperature();
    if ((ST131 >=-20) && (ST131<= 99)){
       dtostrf(ST131,3, 1, buff_ST131);
       client.publish("pihome/sensor/temp/st131", buff_ST131);
       }
    float ST141 = DHT141.readTemperature();
    if ((ST141 >=-20) && (ST141<= 99)){
       dtostrf(ST141,3, 1, buff_ST141);
       client.publish("pihome/sensor/temp/st141", buff_ST141);
       }
    float ST151 = DHT151.readTemperature();
    if ((ST151 >=-20) && (ST151<= 99)){
       dtostrf(ST151,3, 1, buff_ST151);
       client.publish("pihome/sensor/temp/st151", buff_ST151);
       }
    float ST161 = DHT161.readTemperature();
    if ((ST161 >=-20) && (ST161<= 99)){
       dtostrf(ST161,3, 1, buff_ST161);
       client.publish("pihome/sensor/temp/st161", buff_ST161);
       }
    float ST201 = DHT201.readTemperature();
    if ((ST201 >=-20) && (ST201<= 99)){
       dtostrf(ST201,3, 1, buff_ST201);
       client.publish("pihome/sensor/temp/st201", buff_ST201);
       }
    float ST211 = DHT211.readTemperature();
    if ((ST211 >=-20) && (ST211<= 99)){
       dtostrf(ST211,3, 1, buff_ST211);
       client.publish("pihome/sensor/temp/st211", buff_ST211);
       }
    float ST221 = DHT221.readTemperature();
    if ((ST221 >=-20) && (ST221<= 99)){
       dtostrf(ST221,3, 1, buff_ST221);
       client.publish("pihome/sensor/temp/st221", buff_ST221);
       }
    float ST231 = DHT231.readTemperature();
    if ((ST231 >=-20) && (ST231<= 99)){
       dtostrf(ST231,3, 1, buff_ST231);
       client.publish("pihome/sensor/temp/st231", buff_ST231);
       }
    float ST241 = DHT241.readTemperature();
    if ((ST241 >=-20) && (ST241<= 99)){
       dtostrf(ST241,3, 1, buff_ST241);
       client.publish("pihome/sensor/temp/st241", buff_ST241);
       }

    //Humidity
    float SH111 = DHT111.readHumidity();
    if ((SH111 >=5) && (SH111<= 99)){
       dtostrf(SH111,3, 1, buff_SH111);
       client.publish("pihome/sensor/humidity/sh111", buff_SH111);
       }
    float SH121 = DHT121.readHumidity();
    if ((SH121 >=5) && (SH121<= 99)){
       dtostrf(SH121,3, 1, buff_SH121);
       client.publish("pihome/sensor/humidity/sh121", buff_SH121);
       }
    float SH131 = DHT131.readHumidity();
    if ((SH131 >=5) && (SH131<= 99)){
       dtostrf(SH131,3, 1, buff_SH131);
       client.publish("pihome/sensor/humidity/sh131", buff_SH131);
       }
    float SH141 = DHT141.readHumidity();
    if ((SH141 >=5) && (SH141<= 99)){
       dtostrf(SH141,3, 1, buff_SH141);
       client.publish("pihome/sensor/humidity/sh141", buff_SH141);
       }
    float SH151 = DHT151.readHumidity();
    if ((SH151 >=5) && (SH151<= 99)){
       dtostrf(SH151,3, 1, buff_SH151);
       client.publish("pihome/sensor/humidity/sh151", buff_SH151);
       }
    float SH161 = DHT161.readHumidity();
    if ((SH161 >=5) && (SH161<= 99)){
       dtostrf(SH161,3, 1, buff_SH161);
       client.publish("pihome/sensor/humidity/sh161", buff_SH161);
       }
    float SH201 = DHT201.readHumidity();
    if ((SH201 >=5) && (SH201<= 99)){
       dtostrf(SH201,3, 1, buff_SH201);
       client.publish("pihome/sensor/humidity/sh201", buff_SH201);
       }
    float SH211 = DHT211.readHumidity();
    if ((SH211 >=5) && (SH211<= 99)){
       dtostrf(SH211,3, 1, buff_SH211);
       client.publish("pihome/sensor/humidity/sh211", buff_SH211);
       }
    float SH221 = DHT221.readHumidity();
    if ((SH221 >=5) && (SH221<= 99)){
       dtostrf(SH221,3, 1, buff_SH221);
       client.publish("pihome/sensor/humidity/sh221", buff_SH221);
       }
    float SH231 = DHT231.readHumidity();
    if ((SH231 >=5) && (SH231<= 99)){
       dtostrf(SH231,3, 1, buff_SH231);
       client.publish("pihome/sensor/humidity/sh231", buff_SH231);
       }
    float SH241 = DHT241.readHumidity();
    if ((SH241 >=5) && (SH241<= 99)){
       dtostrf(SH241,3, 1, buff_SH241);
       client.publish("pihome/sensor/humidity/sh241", buff_SH241);
       }

    //=============DS18B20 sensors temp waterproof==================
    sensors.requestTemperatures();
    SF111=sensors.getTempC(mac_SF111);
    SF121=sensors.getTempC(mac_SF121);
    SF131=sensors.getTempC(mac_SF131);
    SF151=sensors.getTempC(mac_SF151);
    SF152=sensors.getTempC(mac_SF152);
    SF201=sensors.getTempC(mac_SF201);
    SF211=sensors.getTempC(mac_SF211);
    SF221=sensors.getTempC(mac_SF221);
    SF231=sensors.getTempC(mac_SF231);
    SF241=sensors.getTempC(mac_SF241);

    if ((SF111 >=-10) && (SF111<= 150)){
    dtostrf(SF111,3, 1, buff_SF111);
    client.publish("pihome/sensor/floortemp/sf111", buff_SF111);
    }
    if ((SF121 >=-10) && (SF121<= 150)){
    dtostrf(SF121,3, 1, buff_SF121);
    client.publish("pihome/sensor/floortemp/sf121", buff_SF121);
    }
    if ((SF131 >=-10) && (SF131<= 150)){
    dtostrf(SF131,3, 1, buff_SF131);
    client.publish("pihome/sensor/floortemp/sf131", buff_SF131);
    }
    if ((SF151 >=-10) && (SF151<= 150)){
    dtostrf(SF151,3, 1, buff_SF151);
    client.publish("pihome/sensor/floortemp/sf151", buff_SF151);
    }
    if ((SF152 >=-10) && (SF152<= 150)){
    dtostrf(SF152,3, 1, buff_SF152);
    client.publish("pihome/sensor/floortemp/sf152", buff_SF152);
    }
    if ((SF201 >=-10) && (SF201<= 150)){
    dtostrf(SF201,3, 1, buff_SF201);
    client.publish("pihome/sensor/floortemp/sf201", buff_SF201);
    }
    if ((SF211 >=-10) && (SF211<= 150)){
    dtostrf(SF211,3, 1, buff_SF211);
    client.publish("pihome/sensor/floortemp/sf211", buff_SF211);
    }
    if ((SF221 >=-10) && (SF221<= 150)){
    dtostrf(SF221,3, 1, buff_SF221);
    client.publish("pihome/sensor/floortemp/sf221", buff_SF221);
    }
    if ((SF231 >=-10) && (SF231<= 150)){
    dtostrf(SF231,3, 1, buff_SF231);
    client.publish("pihome/sensor/floortemp/sf231", buff_SF231);
    }
    if ((SF241 >=-10) && (SF241<= 150)){
    dtostrf(SF241,3, 1, buff_SF241);
    client.publish("pihome/sensor/floortemp/sf241", buff_SF241);
    }
  }

  //===========================PIR SENSORS====================================
  SP111 = digitalRead(SP111P);
  if (SP111 != lastSP111) {
   if (SP111 == HIGH) {
	client.publish("pihome/sensor/pir","SP111_on");
   }
  }
  lastSP111 = SP111;

  SP121 = digitalRead(SP121P);
  if (SP121 != lastSP121) {
   if (SP121 == HIGH) {
	client.publish("pihome/sensor/pir","SP121_on");
   }
  }
  lastSP121 = SP121;

  SP131 = digitalRead(SP131P);
  if (SP131 != lastSP131) {
   if (SP131 == HIGH) {
	client.publish("pihome/sensor/pir","SP131_on");
   }
  }
  lastSP131 = SP131;

  SP141 = digitalRead(SP141P);
  if (SP141 != lastSP141) {
   if (SP141 == HIGH) {
	client.publish("pihome/sensor/pir","SP141_on");
   }
  }
  lastSP141 = SP141;

  SP151 = digitalRead(SP151P);
  if (SP151 != lastSP151) {
   if (SP151 == HIGH) {
	client.publish("pihome/sensor/pir","SP151_on");
   }
  }
  lastSP151 = SP151;

  SP152 = digitalRead(SP152P);
  if (SP152 != lastSP152) {
   if (SP152 == HIGH) {
	client.publish("pihome/sensor/pir","SP152_on");
   }
  }
  lastSP152 = SP152;

  SP201 = digitalRead(SP201P);
  if (SP201 != lastSP201) {
   if (SP201 == HIGH) {
	client.publish("pihome/sensor/pir","SP201_on");
   }
  }
  lastSP201 = SP201;

  SP211 = digitalRead(SP211P);
  if (SP211 != lastSP211) {
   if (SP211 == HIGH) {
	client.publish("pihome/sensor/pir","SP211_on");
   }
  }
  lastSP211 = SP211;

  SP221 = digitalRead(SP221P);
  if (SP221 != lastSP221) {
   if (SP221 == HIGH) {
	client.publish("pihome/sensor/pir","SP221_on");
   }
  }
  lastSP221 = SP221;

  SP231 = digitalRead(SP231P);
  if (SP231 != lastSP231) {
   if (SP231 == HIGH) {
	client.publish("pihome/sensor/pir","SP231_on");
   }
  }
  lastSP231 = SP231;

  SP241 = digitalRead(SP241P);
  if (SP241 != lastSP241) {
   if (SP241 == HIGH) {
	client.publish("pihome/sensor/pir","SP241_on");
   }
  }
  lastSP241 = SP241;

  delay(500);

}

// End of sketch ---------------------------------
