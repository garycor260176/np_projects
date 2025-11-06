#include <mqtt_client.h>
#include <OneWire.h>
#include <Wire.h>
#include <MQTTds18b20.h>
#include <MQTTButtonClick.h>
#include <MQTTbh1750fvi.h>
#include <MQTTbmp280.h>
#include <MQTTwspeed.h>
#include <MQTTwdir.h>

#define PIN_RAIN            18
#define PIN_DS18B20         19
#define PIN_BH1750FVI_ADDR  16
#define ADS_CHANNEL_WDIR    0
#define ADS_CHANNEL_WSPEED  1
#define ADS_PIN_READY       5

#define DEF_TOPIC "meteo"

void ReadDevices(boolean refresh = false);

MQTTClient client( 
  DEF_TOPIC,
  "192.168.1.59",
  "METEO",
  "default",
  "nthfgtdn",
  "admin",
  "1",
  [](const String &payload) { //on topic command
    if (payload == "refresh") {
      ReadDevices(true);
    }
  },
  [](){}, //on connection
  [](boolean refresh){ //read devices
    ReadDevices(refresh);
  },
  1883,
  true, //send working
  180, //wdt
  false //console log
);

OneWire oneWire(PIN_DS18B20);
MQTTds18b20 ds18b20(new DallasTemperature(&oneWire), 
                    &client, 
                    "sensors/ds18b20");

MQTTButtonClick rain(PIN_RAIN, &client, "sensors/rain");

BH1750FVI::eDeviceAddress_t DEVICEADDRESS = BH1750FVI::k_DevAddress_H;
BH1750FVI::eDeviceMode_t DEVICEMODE = BH1750FVI::k_DevModeContHighRes2;
MQTTbh1750fvi bh1750fvi(new BH1750FVI(PIN_BH1750FVI_ADDR, DEVICEADDRESS, DEVICEMODE), 
                        &client, 
                        "sensors/bh1750fvi");

MQTTbmp280 bmp280(new GyverBME280(), 
                  &client, 
                  "sensors/bmp280");

Adafruit_ADS1115 ads;
MQTTwspeed wspeed(&ads, ADS_CHANNEL_WSPEED, &client, "sensors/wind/speed");
MQTTwdir wdir(&ads, ADS_CHANNEL_WDIR, &client, "sensors/wind/direction");

boolean adsOk = false;

void setup() {
  Serial.begin(115200);
  Serial.println("");  Serial.println("Start!");

  pinMode(PIN_RAIN, INPUT);

  adsOk = ads.begin();
  if (!adsOk)
  {
    Serial.println("Failed to initialize ADS.");
    
  }
  ds18b20.begin();
  bh1750fvi.begin();
  bmp280.begin(0x76);
  client.begin();

  sei();
  delay(200);
}

void loop() {
  client.loop();
}

void ReadDevices(boolean refresh){
  ds18b20.loop(refresh);
  rain.loop(refresh);
  bh1750fvi.loop();
  bmp280.loop();
  if(adsOk){
    wspeed.loop();
    wdir.loop();
  }
}