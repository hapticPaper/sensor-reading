#include "mqttClient.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>




int MQTTclient::publishJson(String topic, JsonDocument payload) {
    Serial.printf("Here to print.");
    delay(100000);
    size_t psize = 1+measureJson(payload);
    char* buf;
    serializeJson(payload, buf, psize);
    Serial.println(buf); 

    int resp = client->publish(topic.c_str(), buf);    
    Serial.println(topic.c_str());
    Serial.println(buf);
    Serial.print("mqtt publish: ");
    Serial.println(resp);

    if (resp == 0){
        Serial.print("The message was not accepted. Sizes of actual message and payload: ");
        Serial.print(measureJson(payload)); 
        Serial.print(",\t");
        Serial.println(sizeof(buf));
        Serial.println(client->getWriteError());
    }
    
    return resp;
}
void MQTTclient::initMqtt(String mqtt_clientId, String server, int port, int buffer) {    
        clientId=mqtt_clientId;
        client->setServer(server.c_str(), port);
        client->setBufferSize(buffer); 
        Serial.printf("Initializing %s to %s buffer %d bytes\n",  clientId, server, client->getBufferSize());
        return;
    }

int MQTTclient::newSensor(JsonDocument sensor_config, String state_topic, String label,
                            String name, String device_class,
                            String sensor_id, String sensor_model,
                            String area, String uom, String value_template) {
    String unique_id; 
    unique_id =  sensor_id + "_" + name;
    
    sensor_config["state_topic"] = state_topic.c_str();
    sensor_config["area"]= area;
    sensor_config["unique_id"] = unique_id;
    sensor_config["friendly_name"] = area + " " + sensor_model + " " + label;
    sensor_config["name"] = label;
    sensor_config["object_id"] = unique_id;
    sensor_config["device_class"] = device_class;
    sensor_config["unit_of_measurement"] = uom;
    sensor_config["value_template"] = value_template.c_str();


    String discovery_topic = "haptic-mq/discovery/sensor/" + unique_id + "/config";
    return publishJson(discovery_topic, sensor_config);
}


void MQTTclient::reconnect() {
  Serial.println("Loop until we're reconnected");
  int rc=0;
  while (!client->connected() && rc<3) {
    rc++;
    Serial.println("Attempting MQTT connection...");
    
    // Attempt to connect
    if (client->connect(clientId.c_str())) {
      Serial.printf("connected, client state: %s\n", client->state());
      
    } else {
      Serial.printf("failed, client state: %s rc= %s\n", client->state(), rc);
      delay(3000);
    }
  }
}