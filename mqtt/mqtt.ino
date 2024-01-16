#include <Wire.h>
#include <DHT.h>
#include <ctime>
#include <cstring>
#include "time.h"
#include <cmath>
#include <iostream> 
#include <iomanip>  
#include <sstream>  

#include "MQTTclient.h"
#include "wifiConnection.h"

//#include secrets.h to import SSID and SSIDPASSWORD
#include "mqtt_sensors\secrets.h"
#include "mqtt_sensors\ultrasonic.h"

const char* ssid = SSID;
const char* password = SSIDPASSWORD;
const char* mqtt_server = MQTT_HOST;

DHT dht(DHTPIN, DHTTYPE);


String sensorId = SENSORID;
String clientId = CLIENTID;


String state_topic = "haptic-mq/sensor/" + sensorId + "/state";
WiFiClient espClient;
MQTTclient mq(&espClient);

void setup() {
        
    Serial.begin(115200);
    while (!Serial);
    delay(100);


    initWifi(&espClient, ssid, password);

    mq.initMqtt(mqtt_server, clientId);
    String discovery_topic;
    String unique_id;

    JsonDocument ha_config;

    JsonObject device = ha_config["device"].to<JsonObject>();    
    JsonArray identifiers = device["identifiers"].to<JsonArray>();

    identifiers.add(clientId);
    device["name"] = sensorId;
    device["model"] = SENSORMODEL;
    device["manufacturer"] = SENSORBRAND;    

    
    mq.newSensor(ha_config, state_topic, "Temp (f)", "temp_f",  "temperature", SENSORID, SENSORMODEL, AREA, "°F", "{{ value_json.temp_f | round(2)  }}");
    Serial.printf("\n\n");
}



static std::time_t lastMsg = 0;

void loop() {  
    delay(5000);
    JsonDocument * _test;
    _test['foo']='bar';
    Serial.printf(mq.publishJson('test/cpp_mqtt',  *_test));
    
}
