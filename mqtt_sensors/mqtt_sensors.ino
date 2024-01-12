#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DHT.h>
#include <ctime>
#include <cstring>
#include "time.h"
#include <cmath>
#include <iostream> 
#include <iomanip>  
#include <sstream>  
//#include secrets.h to import SSID and SSIDPASSWORD
#include "secrets.h"
#include "br_sensor.h"

const char* ssid = SSID;
const char* password = SSIDPASSWORD;
const char* mqtt_server = MQTT_HOST;

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);


String sensorId = SENSORID;
String clientId = CLIENTID;


String state_topic = "haptic-mq/sensor/" + sensorId + "/state";
// Function to handle special characters in a JSON string
String escapeJsonString(const String &input) {
  String output;
  for (size_t i = 0; i < input.length(); ++i) {
    char c = input.charAt(i);
    if (c == '/') {
      output += "\\/";  // Escape the forward slash
    } else {
      output += c;
    }
  }
  return output;
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



bool initTime(String timezone){

  Serial.println("Setting time");
  configTime(0, 0, "pool.ntp.org");
  struct tm timeinfo;  // First connect to NTP server, with 0 TZ offset
  int retries = 0; 
  while(!getLocalTime(&timeinfo)){
    Serial.print("  Failed to obtain time. waits: ");
    Serial.println(retries);
    retries++;
    delay(100);
    if (retries>=60){
      static_cast<void(*)(void)>(0)();
    }
  }
  setenv("TZ",timezone.c_str(), 1); 
  tzset();
  delay(100);
  Serial.println("Got the time from NTP");
  printLocalTime();
  return true;
}


void printLocalTime(){
  struct tm timeinfo;
   if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time 1");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S zone %Z %z ");
}


void setup() {
    
  Serial.begin(115200);
  while (!Serial);
  delay(100);


  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());


  
  initTime("EST5EDT,M3.2.0,M11.1.0");  
  //printLocalTime();     


  client.setServer(mqtt_server, 1883);
  client.setBufferSize(512);
  Serial.print("MQTT Buffer Size: " );
  Serial.println(client.getBufferSize());
}



static std::time_t lastMsg = 0;



void loop() {
  
  delay(5000);
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

  if (!client.connected()) {
    reconnect();
  }


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
    publish_json(state_topic, doc);
    
  }
    
}

int publish_json(String topic, JsonDocument payload ){
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
  }
  return resp;
}

int new_sensor(JsonDocument sensor_config, String label, String name, String device_class, String uom, String value_template) {
  String unique_id; 
  
  unique_id =  SENSORID "_" + name;
  sensor_config["state_topic"] = state_topic.c_str();
  // sensor_config["area"]= AREA;
  // sensor_config["unique_id"] = unique_id;
  // sensor_config["friendly_name"] = AREA " " SENSORMODEL " " + label;
  // sensor_config["name"] = label;
  // sensor_config["object_id"] = unique_id;
  // sensor_config["device_class"] = device_class;
  // sensor_config["unit_of_measurement"] = uom;
  // sensor_config["value_template"] = value_template.c_str();


  String discovery_topic = "haptic-mq/sensor/" + unique_id + "/config";
  
  return publish_json(discovery_topic, sensor_config);
}


void reconnect() {
  // Loop until we're reconnected
  int rc=0;
  while (!client.connected() && rc<3) {
    rc++;
    Serial.println("Attempting MQTT connection...");
    String discovery_topic;
    String unique_id;

    JsonDocument ha_config;

    JsonObject device = ha_config["device"].to<JsonObject>();    
    JsonArray identifiers = device["identifiers"].to<JsonArray>();

    identifiers.add(clientId);
    device["name"] = sensorId;
    device["model"] = SENSORMODEL;
    device["manufacturer"] = SENSORBRAND;    

    
    new_sensor(ha_config, "Temp (f)", "temp_f",  "temperature", "°F", "{{ value_json.temp_f | round(2)  }}");
    new_sensor(ha_config, "Temp (c)", "temp_c", "temperature", "°C", "{{ value_json.temp_c | round(2)  }}");
    new_sensor(ha_config, "Heat Index (f)", "heat_index_f", "temperature", "°F", "{{ value_json.heat_index_f | round(2)  }}");
    new_sensor(ha_config, "Heat Index (c)", "heat_index_c", "temperature", "°C", "{{ value_json.heat_index_c | round(2)  }}");
    new_sensor(ha_config, "Humidity", "humidity", "humidity", "%", "{{ value_json.humidity | round(2)  }}");

    Serial.printf("\n\n");
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      
      //<discovery_prefix>/<component>/[<node_id>/]<object_id>/config
    } else {
      Serial.print("failed, rc=");
      Serial.print(rc);
      Serial.println(client.state());
      delay(3000);
    }
  }
}