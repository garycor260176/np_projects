#include "mqtt_client.h"
#include "MQTTHLK_LD2450.h"

// Definition of pins to which the sensor is connected
#define HLK_RX 21
#define HLK_TX 22

#define DEF_TOPIC "HLK2450"

void ReadStates(boolean refresh = false);

MQTTClient client( 
  DEF_TOPIC,
  "192.168.1.59",
  "HLK2450-01",
  "default",
  "nthfgtdn",
  "admin",
  "1",
  [](const String &payload) { //on topic command
    if (payload == "refresh") {
      ReadStates(true);
    }
  },
  [](){
  }, //on connection
  [](boolean refresh){ //read devices
    ReadStates(refresh);
  },
  1883,
  true, //send working
  180, //wdt
  false //console log
);

MQTTHLK_LD2450 hlk(new HLK_LD2450(), &client, "01", HLK_TX, HLK_RX);

void setup() {
  Serial.begin(115200);
  Serial.println("");  Serial.println("Start!");
  
  hlk.begin();
  client.begin();

  sei();
  delay(200);
}

void loop() {
  client.loop();
}

void ReadStates(boolean refresh){
  hlk.loop(); 
}