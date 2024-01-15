
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>

#include "wifiConnection.h"
#include <PubSubClient.h>

WiFiClient appClient;
PubSubClient client;

class MQTTclient{    
    public: 
        MQTTclient();
        PubSubClient client;
        MQTTclient(WiFiClient wifi) : client(wifi) {}
        

        bool publishJson(String topic, JsonDocument payload);
        bool newSensor(JsonDocument sensor_config, String state_topic, String label, String name, String device_class, String sensor_id, String sensor_model, String area, String uom, String value_template);

};

