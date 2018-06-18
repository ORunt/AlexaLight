#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"

#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager WiFi Configuration Magic


/* ============== Settings ============== */
// define only ONE code compilation
#define LOUNGE_SWITCH
//#define BATHROOM_SWITCH
//#define CAMS_SWITCH

#define BUTTON_DELAY  400
/* ====================================== */


#define SERIAL_BAUDRATE   115200

#define RELAY_1   4
#define RELAY_2   5
#define RELAY_3   16
#define BTN_1     14
#define BTN_2     12
#define BTN_3     13
#define LED_CFG   2

#ifdef LOUNGE_SWITCH
#define AP_NAME "SharpTech0002"
#endif
#ifdef BATHROOM_SWITCH
#define AP_NAME "SharpTech0003"
#endif
#ifdef CAMS_SWITCH
#define AP_NAME "SharpTech0004"
#endif

/* --------- global variables --------- */
fauxmoESP fauxmo;
int led_cfg_brightness = 200;

void setup() {
  Serial.begin(SERIAL_BAUDRATE);
  pinSetup();
  wifiSetup();

  // fauxmo
  fauxmo.enable(true);
  addDevices();
  
  fauxmo.onSetState([](unsigned char device_id, const char * device_name, bool state) {
      foxSet(device_id, device_name, state);
  });

  fauxmo.onGetState([](unsigned char device_id, const char * device_name) {
      return foxGet(device_id, device_name);
  });
}

void loop() {
  fauxmo.handle();
  //delay(50);
}


void setupAutoWifiAp(){
    WiFiManager wifiManager;
    wifiManager.setConfigPortalTimeout(180);
    //wifiManager.setAPCallback(configModeCallback);
    wifiManager.autoConnect(AP_NAME);
    analogWrite(LED_CFG,200);
}

void wifiSetup() {
  Serial.printf("[WIFI] Trying to connect ");
  setupAutoWifiAp();
  
  // Wait
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(100);
  }
  
  // Connected!
  Serial.printf("\n[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

int getRelayNum(uint8_t device_id){
  switch (device_id) 
  {
    case 0: return RELAY_1;
    case 1: return RELAY_2;
    case 2: return RELAY_3;
    default: return 0;
  }
}

void foxSet(uint8_t device_id, const char * device_name, bool state){
  Serial.printf("[MAIN] Set: Device #%d (%s) state: %s\n", device_id, device_name, state ? "ON" : "OFF");
  digitalWrite(getRelayNum(device_id), state);
}

uint8_t foxGet(uint8_t device_id, const char * device_name){
  Serial.printf("[MAIN] Query: Device #%d (%s)\n", device_id, device_name);
  return digitalRead(getRelayNum(device_id));
}

void pinSetup(){
  pinMode(RELAY_1, OUTPUT);
  pinMode(RELAY_2, OUTPUT);
  pinMode(RELAY_3, OUTPUT);
  pinMode(LED_CFG, OUTPUT);
  pinMode(BTN_1, INPUT);
  pinMode(BTN_2, INPUT);
  pinMode(BTN_3, INPUT);
  
  digitalWrite(RELAY_1, LOW);
  digitalWrite(RELAY_2, LOW);
  digitalWrite(RELAY_3, LOW);
  analogWrite(LED_CFG,led_cfg_brightness);

  attachInterrupt(digitalPinToInterrupt(BTN_1), interrupt1, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_2), interrupt2, RISING);
  attachInterrupt(digitalPinToInterrupt(BTN_3), interrupt3, RISING);
}

void interrupt1() {interruptRoutine(RELAY_1);}
void interrupt2() {interruptRoutine(RELAY_2);}
void interrupt3() {interruptRoutine(RELAY_3);}

void interruptRoutine(int relay){
  int state = digitalRead(relay);
  if(state)
    digitalWrite(relay, LOW);
  else
    digitalWrite(relay, HIGH);

  Serial.printf("Relay %d went %s\n", relay, state ? "low" : "high");
  DelayMilli(BUTTON_DELAY);
}

void DelayMilli(int milliseconds){
  Serial.print("      Delay start... ");
  for (int i=0; i <= milliseconds; i++){
    delayMicroseconds(1000);
  }
  Serial.println("aaaand end");
}

#ifdef LOUNGE_SWITCH
void addDevices(){
  fauxmo.addDevice("lounge light");   // Device 0
  fauxmo.addDevice("outside light");  // Device 1
  fauxmo.addDevice("kendys light");   // Device 2
}
#endif

#ifdef BATHROOM_SWITCH
void addDevices(){
  fauxmo.addDevice("bathroom light"); // Device 0
  fauxmo.addDevice("kitchen light");  // Device 1
}
#endif

#ifdef CAMS_SWITCH
void addDevices(){
  fauxmo.addDevice("cams light");   // Device 0
}
#endif
