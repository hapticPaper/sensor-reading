#ifndef mqttClient_h
#define mqttClient_h


#include <PubSubClient.h>
#include <WiFi.h>

WiFiClient appClient;
PubSubClient client;

class mqttClient{
    public: 
        PubSubClient client();
        mqttClient(WiFiClient wifi);
        mqttClient& publishJson(String topic, JsonDocument payload);
        mqttClient& newSensor(String topic, JsonDocument payload);
};
#endif
