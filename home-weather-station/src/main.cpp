#include <Arduino.h>
#include <DHT.h>

#define SLEEP_TIME 15*60*1000 // 15 minutes

#define DHTPIN 23
#define DHTTYPE DHT11

size_t delayMS;

DHT dht(DHTPIN, DHTTYPE);
// void printTemperatureSensorInfo();
// void printHumiditySensorInfo();

void setup() {
  Serial.begin(9600);
  dht.begin();
  // printTemperatureSensorInfo();
  // printHumiditySensorInfo();


}
void loop() {
  delay(1000);
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.println(dht.read());
  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.println(F("°C"));
  Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.println(F("%"));
  // if (isnan(event.relative_humidity)) {
  //   Serial.println(F("Error reading humidity!"));
  // }
  // else {
  // }
}

// void printTemperatureSensorInfo()
// {
//   sensor_t sensor;
//   dht.temperature().getSensor(&sensor);
//   Serial.println(F("------------------------------------"));
//   Serial.println(F("Temperature Sensor"));
//   Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
//   Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
//   Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
//   Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("°C"));
//   Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("°C"));
//   Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("°C"));
//   Serial.println(F("------------------------------------"));
// }
// void printHumiditySensorInfo()
// {
//   sensor_t sensor;
//   dht.humidity().getSensor(&sensor);
//   Serial.println(F("Humidity Sensor"));
//   Serial.print  (F("Sensor Type: ")); Serial.println(sensor.name);
//   Serial.print  (F("Driver Ver:  ")); Serial.println(sensor.version);
//   Serial.print  (F("Unique ID:   ")); Serial.println(sensor.sensor_id);
//   Serial.print  (F("Max Value:   ")); Serial.print(sensor.max_value); Serial.println(F("%"));
//   Serial.print  (F("Min Value:   ")); Serial.print(sensor.min_value); Serial.println(F("%"));
//   Serial.print  (F("Resolution:  ")); Serial.print(sensor.resolution); Serial.println(F("%"));
//   Serial.println(F("------------------------------------"));
// }