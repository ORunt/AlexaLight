#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "fauxmoESP.h"
#include "ESPAsyncWebServer.h"
#include <ESPAsyncTCP.h>
#include <Hash.h>

#define WIFI_SSID "CamsBay"
#define WIFI_PASS "randal5544"
#define SERIAL_BAUDRATE   115200

fauxmoESP fauxmo;
#define RELAY_PASSAGE   4
#define RELAY_KITCHEN   5
#define RELAY_LOUNGE    16
const int  btn_passage = 14;
const int  btn_kitchen = 12;
const int  btn_lounge = 13;

//int btn_state_passage = 0;
//int btn_state_kitchen = 0;
//int btn_state_lounge = 0;

//int lastButtonState = 0;     // previous state of the button

// -----------------------------------------------------------------------------
// Wifi
// -----------------------------------------------------------------------------

void wifiSetup() {

    // Set WIFI module to STA mode
    WiFi.mode(WIFI_STA);

    // Connect
    Serial.printf("[WIFI] Connecting to %s ", WIFI_SSID);
    WiFi.begin(WIFI_SSID, WIFI_PASS);

    // Wait
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(100);
    }
    Serial.println();

    // Connected!
    Serial.printf("[WIFI] STATION Mode, SSID: %s, IP address: %s\n", WiFi.SSID().c_str(), WiFi.localIP().toString().c_str());
}

void callback(uint8_t device_id, const char * device_name, bool state) {
  int relay = 0;
  Serial.print("Device "); Serial.print(device_id); Serial.print(device_name); 
  Serial.print(" state: ");
  
  switch (device_id) 
  {
    case 0:
      relay = RELAY_PASSAGE;
      break;
    case 1:
      relay = RELAY_KITCHEN;
      break;
    case 2:
      relay = RELAY_LOUNGE;
      break;
    default:
    return;
  }
  
  if (state) {
    Serial.println("ON");
    digitalWrite(relay, HIGH);
  } else {
    Serial.println("OFF");
    digitalWrite(relay, LOW);
  }
}

void setup() {
    pinMode(RELAY_PASSAGE, OUTPUT);
    pinMode(btn_passage, INPUT);
    pinMode(RELAY_KITCHEN, OUTPUT);
    pinMode(btn_kitchen, INPUT);
    pinMode(RELAY_LOUNGE, OUTPUT);
    pinMode(btn_lounge, INPUT);
    
    digitalWrite(RELAY_PASSAGE, LOW);
    digitalWrite(RELAY_KITCHEN, LOW);
    digitalWrite(RELAY_LOUNGE, LOW);
    
    // Init serial port and clean garbage
    Serial.begin(SERIAL_BAUDRATE);
    Serial.println("FauxMo demo sketch");
    Serial.println("After connection, ask Alexa/Echo to 'turn <devicename> on' or 'off'");

    // Wifi
    wifiSetup();

    fauxmo.enable(true);
    // Fauxmo
    fauxmo.addDevice("the kitchen light");  // Device 0
    fauxmo.addDevice("the lounge light");   // Device 1
    fauxmo.addDevice("the passage light");  // Device 2
    
    fauxmo.onMessage(callback);
}

void loop() {
  fauxmo.handle();
  
  if(digitalRead(btn_passage))
  {
    if(digitalRead(RELAY_PASSAGE))
    {
      digitalWrite(RELAY_PASSAGE, LOW);
      Serial.println("passage off");
    }
    else
    {
      digitalWrite(RELAY_PASSAGE, HIGH);
      Serial.println("passage on");
    }
    delay(1000);
  }
  
  if(digitalRead(btn_kitchen))
  {
    if(digitalRead(RELAY_KITCHEN))
    {
      digitalWrite(RELAY_KITCHEN, LOW);
      Serial.println("kitchen off");
    }
    else
    {
      digitalWrite(RELAY_KITCHEN, HIGH);
      Serial.println("kitchen on");
    }
    delay(1000);
  }
  
  if(digitalRead(btn_lounge))
  {
    if(digitalRead(RELAY_LOUNGE))
    {
      digitalWrite(RELAY_LOUNGE, LOW);
      Serial.println("lounge off");
    }
    else
    {
      digitalWrite(RELAY_LOUNGE, HIGH);
      Serial.println("lounge on");
    }
    delay(1000);
  }
  delay(50);
}
