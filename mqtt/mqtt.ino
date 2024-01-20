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
#include "mqtt_sensors\br_sensor.h"

const char* ssid = SSID;
const char* password = SSIDPASSWORD;
const char* mqtt_server = MQTT_HOST;
const int mqtt_port = MQTT_PORT;
const int mqtt_buffer = MQTT_BUFFER;

DHT dht(DHTPIN, DHTTYPE);


String sensorId = SENSORID;
String clientId = CLIENTID;


String state_topic = "haptic-mq/sensor/" + sensorId + "/state";
WiFiClient espClient;
MQTTclient mq(&espClient, clientId);

void setup() {
        
    Serial.begin(115200);
    while (!Serial);
    delay(100);


    initWifi(ssid, password);
    initTime("EST5EDT,M3.2.0,M11.1.0");    
    mq.initMqtt(mqtt_server, mqtt_port, mqtt_buffer);
    mq.reconnect();


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
    mq.newSensor(ha_config, state_topic, "Temp (c)", "temp_c",  "temperature", SENSORID, SENSORMODEL, AREA, "°C", "{{ value_json.temp_c | round(2)  }}");
    mq.newSensor(ha_config, state_topic, "Heat Index (f)", "heat_index_f",  "temperature", SENSORID, SENSORMODEL, AREA, "°F", "{{ value_json.heat_index_f | round(2)  }}");
    mq.newSensor(ha_config, state_topic, "Heat Index (c)", "heat_index_c",  "temperature", SENSORID, SENSORMODEL, AREA, "°C", "{{ value_json.heat_index_c | round(2)  }}");
    mq.newSensor(ha_config, state_topic, "Humidity", "humidity",  "humidity", SENSORID, SENSORMODEL, AREA, "%", "{{ value_json.humidity | round(0)  }}");

}


float roundDec(double n, int decimals) {
    // Format the numerical value with the specified precision
    std::string formattedValue = std::to_string(n);
    size_t dotPos = formattedValue.find('.');

    // Check if there's a decimal point in the original number
    if (dotPos != std::string::npos) {
        // Adjust the precision if decimals is less than the actual decimal places
        size_t actualDecimals = formattedValue.length() - dotPos - 1;
        if (actualDecimals > static_cast<size_t>(decimals)) {
            formattedValue.erase(dotPos + decimals + 1, std::string::npos);
        }
    }

    // Convert the string to a float using std::stof()
    return std::stof(formattedValue);
}


static std::time_t lastMsg = 0;


void loop() {
  
  delay(5000);
  mq.reconnect();
  std::time_t now = std::time(nullptr);
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  // Read temperature as Fahrenheit (isFahrenheit = true)
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    delay(3000);
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);




  JsonDocument doc; 
  String output;
  
  char event_time[24];
  std::strftime(event_time, sizeof(event_time), "%c", std::localtime(&now));
  
  // YYYYmmdd
   char dts[9];
   std::strftime(dts, sizeof(dts), "%Y%m%d", std::localtime(&now));

  // YYYYmmdd
  // char dt_buf[9];   
  // std::strncpy(dt_buf, dts, 8);
  // dt_buf[8] = '\0'; // Null-terminate the string


  char hour_st[3];   
  std::strftime(hour_st, sizeof(hour_st), "%H", std::localtime(&now));
  if (now - lastMsg > 5) {
    lastMsg = now;    
    doc["temp_c"] = roundDec(t,2);
    doc["temp_f"] = roundDec(f,2);
    doc["humidity"] = roundDec(h,2);
    doc["heat_index_f"] = roundDec(hif,2);
    doc["heat_index_c"] = roundDec(hic,2);
    doc["refreshed"] = now;
    doc["hour"] = std::stoi(hour_st);
    doc["dt"] = std::stoi(dts);
    doc["dev_id"] = clientId;
    doc["sensor_id"] = sensorId;
    mq.publishJson(state_topic, doc);
    
  }
    
}
