#include <Arduino.h>
#include <DHT.h>
#include <string>
#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
using std::string;
#define DEBUG 1

/*          SERVER DETAILS                     */
const char SECRET_VAlUE[]= "gicji6-wovcAp-nunbad";
#define POST_URL "https://incredible-dieffenbachia-39a8f0.netlify.app/data/weather"


/*        TIME SYNC        */
const char *ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3 * 60 * 60; // GMT+3
const long daylightOffset_sec = 0;   // 1 hour

/*           Deep Sleep value      */
#define SLEEP_TIME_US 15 * 60 * 1000000ULL // 15 minutes in microseconds

/*            Temperature and Humidity sensors values        */
#define DHTPIN 26
#define DHTTYPE DHT11


DHT dht(DHTPIN, DHTTYPE);

// Forward declarations
string jsonify(float temp, float humidity);
void initWiFi();
bool syncTimeAndAlign();
bool readSensorData(float &temp, float &humidity); // Pass by reference
bool sendDataToServer(const string& jsonData);
void enterDeepSleep();


void setup()
{
  Serial.begin(9600);
  Serial.println("\nBooting / Waking up...");

  // Initialize hardware
  WiFi.mode(WIFI_STA);
  dht.begin();

  // Connect to WiFi
  initWiFi();
  if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi connection failed. Entering sleep.");
      enterDeepSleep(); // Sleep if WiFi fails
      return; // Should not be reached, but good practice
  }

  // Sync time and wait for alignment
  #ifndef DEBUG
  if (!syncTimeAndAlign()) {
      Serial.println("Time sync failed or delay calculation issue. Entering sleep.");
      enterDeepSleep(); // Sleep if time sync fails
      return; // Should not be reached
  }
  #endif
  // Read sensor data
  float temperature, humidity;
  if (!readSensorData(temperature, humidity)) {
      Serial.println("Failed to read sensor data. Entering sleep.");
      enterDeepSleep(); // Sleep if sensor read fails
      return; // Should not be reached
  }

  // Format data
  string jsonData = jsonify(temperature, humidity);
  Serial.print("Formatted JSON: "); Serial.println(jsonData.c_str());

  // Send data
  if (!sendDataToServer(jsonData)) {
      Serial.println("Failed to send data to server.");
      // Decide if you still want to sleep or retry? For now, we proceed to sleep.
  }
  #ifdef DEBUG
  Serial.println("Data sent successfully.");
  return;
  #endif
  // Enter deep sleep
  enterDeepSleep();
}

void loop()
{
  // Empty: All logic is in setup() and helper functions for deep sleep cycle
}

// --- Helper Functions ---

bool syncTimeAndAlign() {
    Serial.println("Configuring time...");
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    struct tm timeinfo;
    // Attempt to get time, retry once if failed
    if (!getLocalTime(&timeinfo, 10000)) { // Add timeout (optional, getLocalTime might block)
        Serial.println(F("Failed to obtain time. Retrying in 5s..."));
        delay(5000);
        if (!getLocalTime(&timeinfo, 10000)) {
            Serial.println(F("Failed to obtain time again."));
            return false; // Indicate failure
        }
    }

    // If time was obtained successfully, calculate and apply alignment delay
    Serial.println("Time synchronized.");
    Serial.print("Current time: ");
    Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S"); // Print formatted time

    // Calculate delay until the next 15-minute mark (00, 15, 30, 45)
    int currentMinute = timeinfo.tm_min;
    int currentSecond = timeinfo.tm_sec;
    int minutesToNextMark = 15 - (currentMinute % 15);
    long secondsToNextMark = (minutesToNextMark * 60) - currentSecond;
    if (secondsToNextMark <= 0) { secondsToNextMark += 15 * 60; }

    unsigned long delayMillis = secondsToNextMark * 1000;
    Serial.print("Waiting for "); Serial.print(secondsToNextMark); Serial.print(" seconds (");
    Serial.print(delayMillis); Serial.println(" ms) to align with the next 15-minute mark...");
    delay(delayMillis); // Wait until the calculated time
    Serial.println("Initial alignment delay finished.");
    return true; // Indicate success
}

bool readSensorData(float &temp, float &humidity) {
    Serial.println("Reading sensor data...");
    // Ensure WiFi is still connected before reading (optional, but good check)
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected before sensor read. Reconnecting...");
        initWiFi();
        if(WiFi.status() != WL_CONNECTED) {
            Serial.println("Reconnect failed. Cannot read sensor.");
            return false; // Indicate failure
        }
    }

    humidity = dht.readHumidity();
    temp = dht.readTemperature(); // Read temperature as Celsius (the default)

    // Check if any reads failed
    if (isnan(humidity) || isnan(temp)) {
        Serial.println(F("Failed to read from DHT sensor!"));
        return false; // Indicate failure
    }

    Serial.print(F("Temperature: ")); Serial.print(temp); Serial.println(F("°C"));
    Serial.print(F("Humidity: ")); Serial.print(humidity); Serial.println(F("%"));
    return true; // Indicate success
}

bool sendDataToServer(const string& jsonData) {
    Serial.print("Sending data to server: "); Serial.println(jsonData.c_str());

    // Ensure WiFi is connected before sending
    if(WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected before sending. Reconnecting...");
        initWiFi();
        if(WiFi.status() != WL_CONNECTED) {
            Serial.println("Reconnect failed. Cannot send data.");
            return false; // Indicate failure
        }
    }

    HTTPClient http;
    bool success = false;
    if (http.begin(POST_URL)) { // Use if condition for better error handling
        http.addHeader("Content-Type", "application/json");
        http.addHeader("x-secret-header", SECRET_VAlUE);
        int httpResponseCode = http.POST(jsonData.c_str());

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.print(F("HTTP Response code: ")); Serial.println(httpResponseCode);
            Serial.print(F("Response: ")); Serial.println(response);
            // Consider 2xx codes as success
            if (httpResponseCode >= 200 && httpResponseCode < 300) {
                success = true;
            }
        } else {
            Serial.print(F("Error on sending POST: "));
            Serial.println(httpResponseCode);
            // Optional: Print detailed error: Serial.println(http.errorToString(httpResponseCode).c_str());
        }
        http.end(); // Free resources
    } else {
        Serial.println("HTTPClient begin failed.");
    }
    return success; // Return true only on successful POST (e.g., 2xx)
}

void enterDeepSleep() {
    Serial.print("Entering deep sleep for approx 15 minutes (");
    Serial.print(SLEEP_TIME_US / 1000000ULL); // Print sleep time in seconds
    Serial.println(" seconds)...");
    // Optional: Disconnect WiFi before sleep?
    // WiFi.disconnect(true);
    // delay(1); // Small delay might help ensure disconnect completes
    esp_deep_sleep(SLEEP_TIME_US); // Go to sleep
}


void initWiFi()
{
  WiFi.begin("ZTE_71B310", "5WE226635H");
  Serial.print(F("Connecting to WiFi..."));
  int retries = 0;
  while (WiFi.status() != WL_CONNECTED && retries < 20) {
      delay(500);
      Serial.print(".");
      retries++;
  }
  if(WiFi.status() == WL_CONNECTED) {
      Serial.println(F("\nConnected to WiFi"));
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
  } else {
      Serial.println(F("\nFailed to connect to WiFi after retries."));
  }
}

string jsonify(float temp, float humidity)
{
  char buffer[100];
  snprintf(buffer, sizeof(buffer), "{ \"temp\": %.2f, \"humidity\": %.2f }", temp, humidity);
  return string(buffer);
}