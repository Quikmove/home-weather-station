#include <Arduino.h>
#include <DHT.h>
#include <string>
#include <sstream>
#include <WiFi.h>
using std::string;


#define SLEEP_TIME 15*60*1000 // 15 minutes
#define DHTPIN 26
#define DHTTYPE DHT11

size_t delayMS;

DHT dht(DHTPIN, DHTTYPE);
string jsonify(float temp, float humidity);


void setup() {
  WiFi.mode(WIFI_STA);
  Serial.begin(9600);
  dht.begin();

  initWiFi();
}


void loop() {
  delay(5000);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  Serial.println(dht.read());
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F("°C"));
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println(F("%"));
  string json = jsonify(t, h);
  Serial.println("to string:");
  Serial.println(json.c_str());
}

void initWiFi() {
  WiFi.begin("ZTE_71B310", "5WE226635H");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(F("Connecting to WiFi..."));
  }
  Serial.println(F("Connected to WiFi"));
}


string jsonify(float temp, float humidity)
{
  std::stringstream ss;
  ss << "{ \"temp\": " << temp << ", \"humidity\": " << humidity << " }"; 
  return ss.str();
}