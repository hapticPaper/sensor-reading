
#include <WiFi.h>


void printLocalTime(){
    struct tm timeinfo;
    if(!getLocalTime(&timeinfo)){
        Serial.println("Failed to obtain time 1");
        return;
    }
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S zone %Z %z ");
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
        delay(1000);
        //reset the device if it still hasnt gotten an NTP reply after 1 minute
        if (retries>=60){
            printf("Restting... from \n%s :  \t %s", __FILE__, __LINE__);
            //static_cast<void(*)(void)>(0)();
        }
    }
    setenv("TZ",timezone.c_str(), 1); 
    tzset();
    delay(100);
    Serial.println("Got the time from NTP");
    printLocalTime();
    return true;
}

void initWifi(const char* ssid, const char* password){
        
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

    return; 

}
