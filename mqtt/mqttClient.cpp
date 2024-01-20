#include "mqttClient.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <ArduinoJson.h>
#include <ArduinoJson.hpp>




int MQTTclient::publishJson(String topic, JsonDocument data) {
    size_t psize = measureJson(data);
    psize++;
    String buf;

    serializeJson(data, buf);
    char* payload = new char[buf.length()+1];
    strcpy(payload, buf.c_str());

    Serial.printf("\n%s\n%d:\n%s\n", topic.c_str(), psize, payload);
    
    int resp = client.publish(topic.c_str(), payload, psize);  
    Serial.printf("mqtt publish: %d\n", resp);

    if (resp == 0){
        Serial.printf("The message was not accepted. Sizes of actual message and payload: %d : %d \n", psize, sizeof(payload));
        Serial.println(payload);
        Serial.printf("Error: %s", client.getWriteError());
    }
    delete payload;
    return resp;
}
void MQTTclient::initMqtt(String server, int port, int buffer) {  
    const char* s = strdup(server.c_str());
    client.setServer(s, port);
    client.setBufferSize(buffer); 
    Serial.printf("Initialized %s to %s:%d buffer %d bytes\n",  id, s, port, buffer);    
    return;
    }

int MQTTclient::newSensor(JsonDocument sensor_config, String state_topic, String label,
                            String name, String device_class,
                            String sensor_id, String sensor_model,
                            String area, String uom, String value_template) {
    String unique_id; 
    unique_id =  sensor_id + "_" + name;


    // serializeJson(sensor_config, Serial);
    // Serial.println();
    
    
    sensor_config["state_topic"]=state_topic.c_str();     
    sensor_config["area"]= area;
    sensor_config["unique_id"] = unique_id;
    sensor_config["friendly_name"] = area + " " + sensor_model + " " + label;
    sensor_config["name"] = label;
    sensor_config["object_id"] = unique_id;
    sensor_config["device_class"] = device_class;
    sensor_config["unit_of_measurement"] = uom;
    sensor_config["value_template"] = value_template.c_str();

    // serializeJson(sensor_config, Serial);
    // Serial.println();

    String discovery_topic = "haptic-mq/discovery/sensor/" + unique_id + "/config";
    return publishJson(discovery_topic, sensor_config);
}


void MQTTclient::reconnect() {
  // Serial.printf("Loop until %s is connected, \n", id);
  int rc=0;
  while (!client.connected() && rc<3) {
    rc++;
    Serial.println("Attempting MQTT connection...");
    
    // Attempt to connect
    /*
      MQTT States
      -4 : MQTT_CONNECTION_TIMEOUT - the server didn't respond within the keepalive time
      -3 : MQTT_CONNECTION_LOST - the network connection was broken
      -2 : MQTT_CONNECT_FAILED - the network connection failed
      -1 : MQTT_DISCONNECTED - the client is disconnected cleanly
      0 : MQTT_CONNECTED - the client is connected
      1 : MQTT_CONNECT_BAD_PROTOCOL - the server doesn't support the requested version of MQTT
      2 : MQTT_CONNECT_BAD_CLIENT_ID - the server rejected the client identifier
      3 : MQTT_CONNECT_UNAVAILABLE - the server was unable to accept the connection
      4 : MQTT_CONNECT_BAD_CREDENTIALS - the username/password were rejected
      5 : MQTT_CONNECT_UNAUTHORIZED - the client was not authorized to connect
    */


    if (client.state()!=MQTT_CONNECTED) {
      client.connect(id);
      delay(500);
      Serial.println("connected.");
      
    } else {
      Serial.printf("failed, client state: %d rc= %d\n", client.state(), rc);
      delay(3000);
    }
  }
}