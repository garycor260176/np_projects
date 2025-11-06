#include <mqtt_client.h>

#define DEF_TOPIC "relays"
#define RELAYS_COUNT  17

int pins[] = {
   15,
   2,
   4,
   16,
   17,
   5,
   18,
   19,
   23,
   32,
   33,
   25,
   26,
   27,
   14,
   12,
   13 };

struct s_relay {
  String name;
  int state;
};

s_relay relays[RELAYS_COUNT];

void ReadStates(boolean refresh = false);
void setStateMessage(const String &topicStr, const String &message);

MQTTClient client( 
  DEF_TOPIC,
  "192.168.1.59",
  "RELAYS",
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
    client.subscribeWithPref("states/#", setStateMessage); 
  }, //on connection
  [](boolean refresh){ //read devices
    ReadStates(refresh);
  },
  1883,
  true, //send working
  180, //wdt
  false //console log
);

String getRelayName(int i) {
  String ret = String(i);
  if(ret.length() == 1) ret = "r0" + ret;
  else ret = "r" + ret;
  return ret;
}

void setup() {
  Serial.begin(115200);
  Serial.println("");  Serial.println("Start!");

  for(int i = 0; i < RELAYS_COUNT; i++) {
    pinMode(pins[i], OUTPUT);
    digitalWrite(pins[i], LOW);
    relays[i].name = getRelayName(i);
    relays[i].state = 0;
  }  
  client.begin();

  sei();
  delay(200);
}

void loop() {
  client.loop();
}

void setStateMessage(const String &topicStr, const String &message) {
  Serial.println("topic = " + topicStr + ", msg = " + message);
  String topic = DEF_TOPIC; topic = topic + "/states/";
  for(int i = 0; i < RELAYS_COUNT; i++) {
    if(topic + getRelayName(i) == topicStr) {
      relays[i].state = ( message.toInt() == 1 ? 1 : 0 );
    }
  }
}

void ReadStates(boolean refresh){
  for(int i = 0; i < RELAYS_COUNT; i++) {
    if (refresh || relays[i].state != digitalRead(pins[i]) ) {
      digitalWrite(pins[i], relays[i].state);
      client.Publish("states/" + getRelayName(i), String(relays[i].state));
    }
  }
}