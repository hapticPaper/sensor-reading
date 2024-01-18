
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <PubSubClient.h>
#include <WiFi.h>

class MQTTclient {
private:
    WiFiClient* _wc;  // Reference to a WiFiClient object
    String clientId;
    PubSubClient* client;

public:
    // Constructor that accepts a reference to a WiFiClient object
    MQTTclient(WiFiClient* wc) : _wc(wc), client(new PubSubClient(*_wc)){}

   
    void initMqtt(String mqtt_clientId, String server, int port, int buffer);
    void reconnect();
    int publishJson(String topic, JsonDocument payload);
    int newSensor(JsonDocument sensor_config, String state_topic, String label, String name, String device_class, String sensor_id, String sensor_model, String area, String uom, String value_template);
    };
