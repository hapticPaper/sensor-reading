
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <PubSubClient.h>

#include <WiFi.h>

class MQTTclient {
private:
    WiFiClient* _wc;  // Reference to a WiFiClient object
    String clientId;

public:
    PubSubClient client;
    // Constructor that accepts a reference to a WiFiClient object
    MQTTclient(WiFiClient* wc) : _wc(_wc), client(*_wc) {}

    PubSubClient initMqtt(const char* server, String mqtt_clientId, int port=1883, int bufferSize=1024) {    
        clientId=mqtt_clientId;
        client.setServer(server, port);
        client.setBufferSize(bufferSize);
        return client;
    }
    int publishJson(String topic, JsonDocument& payload);
    int newSensor(JsonDocument& sensor_config, String state_topic, String label, String name, String device_class, String sensor_id, String sensor_model, String area, String uom, String value_template);
    void reconnect();
    };
