
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <PubSubClient.h>
#include <WiFi.h>

class MQTTclient {
    private:
        WiFiClient* _wc;  // Reference to a WiFiClient object
        const char* id;
        PubSubClient client;

    public:
        // Constructor that accepts a reference to a WiFiClient object
        MQTTclient(WiFiClient* wc, String clientId) : _wc(wc), client(*_wc) {            
            id = strdup(clientId.c_str());
        }
        ~MQTTclient() {
                free((void*)id);  // Free the memory allocated by strdup
            }
        void initMqtt(String server, int port, int buffer);
        void reconnect();
        int publishJson(String topic, JsonDocument payload);
        int newSensor(JsonDocument sensor_config, String state_topic, String label, String name, String device_class, String sensor_id, String sensor_model, String area, String uom, String value_template);
        };
