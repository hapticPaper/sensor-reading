#ifndef MQTTclient_h
#define MQTTclient_h
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>


#include <PubSubClient.h>
#include <WiFi.h>

WiFiClient appClient;
PubSubClient client;

class MQTTclient{    
    public: 
        MQTTclient();
        PubSubClient client;
        MQTTclient(WiFiClient& wifi) : client(wifi) {}
        
        ~MQTTclient();

        bool publishJson(String topic, JsonDocument payload);
        bool newSensor(JsonDocument sensor_config, String state_topic, String label, String name, String device_class, String sensor_id, String sensor_model, String area, String uom, String value_template);

};
#endif
