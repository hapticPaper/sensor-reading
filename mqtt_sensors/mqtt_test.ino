
#include "MQTTclient.h"

#include "br_sensor.h"

//#include secrets.h to import SSID and SSIDPASSWORD
#include "secrets.h"

const char* tssid = SSID;
const char* tpassword = SSIDPASSWORD;
const char* tmqtt_server = "42.0.0.77";




int main(){
    WiFiClient espClient;
    initWifi(tssid, tpassword);

    initTime("EST5EDT,M3.2.0,M11.1.0");  


    PubSubClient client(espClient);
    client.setServer(tmqtt_server, 1883);
    client.setBufferSize(512);
    Serial.print("MQTT Buffer Size: " );
    Serial.println(client.getBufferSize());
    String test_json;
    const char* topic;
    test_json = '{"device":{"identifiers":["ESP32s3LR"],"name":"lrDHT","model":"DHT11","manufacturer":"Generic"},"state_topic":"haptic-mq/sensor/lrDHT/state","area":"Living Room","unique_id":"lrDHT_temp_f","friendly_name":"Living Room DHT11 Temp (f)","name":"Temp (f)","object_id":"lrDHT_temp_f","device_class":"temperature","unit_of_measurement":"°F","value_template":"{{ value_json.temp_f | round(2)  }}"}';
    topic = "haptic-mq/sensor/brDHT_temp_f/config";
    
    client.publish(topic, test_json.c_str());
    return 0;
    }