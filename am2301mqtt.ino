#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <ArduinoJson.h>

#define DHT_TYPE DHT21
#define DHT_PIN D2
DHT dht(DHT_PIN, DHT_TYPE);
char data[100];
const char* mqttServer = "192.168.0.116";
const char *SSID = "ABS_ACT";
const char *PWD = "Abs$12355";
long lastime = 0;

//const char *data = "Test";
WiFiClient wifiClient = WiFiClient();


void callback(char* topic, byte* payload, unsigned int length) {
 Serial.println("Callback");
 Serial.println((char) payload[0]);
}

PubSubClient mqttClient(mqttServer, 1883, callback, wifiClient);
 
void connectToWiFi() {
 Serial.print("Connecting to ");
 Serial.println(SSID);
  WiFi.begin(SSID, PWD);
  while (WiFi.status() != WL_CONNECTED) {
   Serial.print(".");
   delay(500);
   // we can even make the ESP32 to sleep
 }
 
 Serial.print("Connected - ");
// Serial.print(WiFi.localIP);
}
 
void reconnect() {
 Serial.println("Connecting to MQTT Broker...");
 while (!mqttClient.connected()) {
     Serial.println("Reconnecting to MQTT Broker..");
     String clientId = "ESP8266Client-";
     clientId += String(random(0xffff), HEX);
    
     if (mqttClient.connect(clientId.c_str())) {
       Serial.println("Connected.");
       // subscribe to topic     
        mqttClient.subscribe("/ESP8266Client-1/");     
     }
 }
}
 
void setup() {
 Serial.begin(9600);
 connectToWiFi();
 pinMode(DHT_PIN, INPUT);
 dht.begin();
 Serial.println("DHT11 sensor....");
}
 
void loop() {
 
 if (!mqttClient.connected())
   reconnect();
  mqttClient.loop();
  
 StaticJsonBuffer<300> JSONbuffer;
 JsonObject& JSONencoder = JSONbuffer.createObject();
 
  
 // Publishing data to MQTT
 long now = millis();
 if(now - lastime > 10000) {
   Serial.println("Publishing data..");
   float temp = dht.readTemperature();
   float hum = dht.readHumidity();

  JSONencoder["temperature"] = temp ;
  JSONencoder["humidity"] = hum ;
 
  char JSONmessageBuffer[100];
  JSONencoder.printTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));
  Serial.println("Sending message to MQTT topic..");
  Serial.println(JSONmessageBuffer);
   mqttClient.publish("/ESP8266Client-1/", JSONmessageBuffer);
  
   lastime = now;
 }
 
}
