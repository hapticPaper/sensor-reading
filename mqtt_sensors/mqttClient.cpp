#include "mqttClient.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>


PubSubClient MQTTclient(WiFiClient wc, String server, int port){
    
    
  PubSubClient mq;
  mq(wc);
  mq.setServer(server, port);
  mq.setBufferSize(1024);
  Serial.print("MQTT Buffer Size: " );
  Serial.println(mq.getBufferSize());
  return mq
}


bool MQTTclient::publishJson(String topic, JsonDocument payload ){
    String buf;
    size_t n = serializeJson(payload, buf);
    int resp = client.publish(topic.c_str(), buf.c_str(), measureJson(payload));
    Serial.println(topic.c_str());
    Serial.println(buf);
    Serial.print("mqtt publish: ");
    Serial.println(resp);
    if (resp == 0){
        Serial.print("The message was not accepted. Sizes of actual message and payload: ");
        Serial.print(measureJson(payload)); 
        Serial.print("\t");
        Serial.println(sizeof(buf));
        Serial.println(client.getWriteError());
        return false;
    }
    return true;
}

bool MQTTclient::newSensor(JsonDocument sensor_config, String state_topic, String label,
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

