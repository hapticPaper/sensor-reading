#include <ArduinoJson.h>
#include <ArduinoJson.hpp>
#include <WiFi.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <DHT.h>

//#include secrets.h to import SSID and SSIDPASSWORD
#include "secrets.h"
#include "sensor_config.h"


    
const char* ssid = SSID;
const char* password = SSIDPASSWORD;
const char* mqtt_server = "42.0.0.77";

const int dhtpin = DHTPIN;
//const char* dhttype = DHTTYPE;

WiFiClient espClient;
PubSubClient client(espClient);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  delay(100);
  Serial.begin(115200);
  while (!Serial);

  Serial.println(DHTPIN);

  //Serial.println("Using wifi SSID: " + ssid + " and password: " + password);
  //Serial.println("DHTPIN: " + DHTPIN + " DHTTYPE: " + DHTTYPE);

  // if (!bme.begin()) {
  //  Serial.println("Could not find a valid BME680 sensor, check wiring!");
  //   while (1);
  // }


  // We start by connecting to a WiFi network
  Serial.println();
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

  client.setServer(mqtt_server, 1883);
}

void loop() {

  delay(2000);

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
    return;
  }

  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);


  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.print(F("°C "));
  Serial.print(f);
  Serial.print(F("°F  Heat index: "));
  Serial.print(hic);
  Serial.print(F("°C "));
  Serial.print(hif);
  Serial.println(F("°F"));









  if (!client.connected()) {
    reconnect();
  }

  StaticJsonDocument<80> doc;
  char output[120];
  long lastMsg = 0;
  long now = millis();
  if (now - lastMsg > 5000) {
    lastMsg = now;
    doc["Temperature (c)"] = t;
    doc["Temperature (f) "] = f;
    doc["Humidity"] = h;
    doc["Heat Index (f)"] = hif;
    doc["Heat Index (c)"] = hic;

    serializeJson(doc, output);
    Serial.println(output);
    lastMsg = millis();
    client.publish("/haptic-mq", output);
    
  }
    
}

void reconnect() {
  // Loop until we're reconnected
  int rc=0;
  while (!client.connected() && rc<3) {
    rc++;
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP32s3dev-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(rc);
      Serial.println(client.state());
      delay(3000);
    }
  }
}