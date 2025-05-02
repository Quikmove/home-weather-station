#include <Arduino.h>
#include <DHT.h>
#include <string>
#include <WiFi.h>
#include <time.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
using std::string;

/*          SERVER DETAILS                     */
const char SECRET_VAlUE[]= "gicji6-wovcAp-nunbad";
#define POST_URL "https://incredible-dieffenbachia-39a8f0.netlify.app/data/weather" // Root CA certificate for server verification
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
bool syncTime();
bool alignTime();
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
  syncTime();
  #ifndef DEBUG
  if (!alignTime()) {
      Serial.println("Failed to align time. Entering sleep.");
      enterDeepSleep(); // Sleep if alignment fails
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
bool alignTime()
{
  struct tm timeinfo;
  getLocalTime(&timeinfo); 
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
bool syncTime() {
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
    return true;
}

bool readSensorData(float &temp, float &humidity) {
    Serial.println("Reading sensor data...");
    // Ensure WiFi is still connected before reading (optional, but good check)
    humidity = dht.readHumidity();
    temp = dht.readTemperature(); // Read temperature as Celsius (the default
    


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
    Serial.println("--- Preparing HTTPS Request ---");

    // Add WiFi status check right before attempting HTTPS
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("WiFi disconnected before sending data! Attempting reconnect...");
        initWiFi(); // Try to reconnect
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("Reconnect failed. Cannot send data.");
            return false; // Exit if reconnect fails
        }
        Serial.println("Reconnected to WiFi.");
    }

    // Check current time before attempting HTTPS
    struct tm timeinfo;
    if(getLocalTime(&timeinfo)){
      Serial.print("Current time before HTTPS: ");
      Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
    } else {
      Serial.println("Failed to get local time before HTTPS attempt.");
      // Time might be incorrect, potentially causing TLS errors.
    }

    // Check free heap memory
    Serial.print("Free Heap before HTTPS: ");
    Serial.println(ESP.getFreeHeap());

    Serial.print("Sending data via HTTPS to server: "); Serial.println(POST_URL);

    // Use WiFiClientSecure for HTTPS
    WiFiClientSecure client;

    client.setInsecure();

    HTTPClient http;
    bool success = false;

    // Set a timeout for the connection/response (e.g., 10 seconds)
    http.setTimeout(10000);

    // Pass the secure client to http.begin
    if (http.begin(client, POST_URL)) { // Use secure client
        http.addHeader("Content-Type", "application/json");
                Serial.print("Free Heap before POST: "); // Check memory right before sending
        Serial.println(ESP.getFreeHeap());

        int httpResponseCode = http.POST(jsonData.c_str());

        if (httpResponseCode > 0) {
            String response = http.getString();
            Serial.print(F("HTTPS Response code: ")); Serial.println(httpResponseCode);
            Serial.print(F("Response: ")); Serial.println(response);
            if (httpResponseCode >= 200 && httpResponseCode < 300) {
                success = true;
            }
        } else {
            Serial.print(F("Error on sending POST: "));
            Serial.println(httpResponseCode);
            Serial.print(F("HTTPClient error: "));
            Serial.println(http.errorToString(httpResponseCode).c_str());
        }
        http.end();
    } else {
        Serial.println("HTTPClient secure begin failed. Check DNS and WiFi connection."); // Added hint
    }

    Serial.print("Free Heap after HTTPS attempt: ");
    Serial.println(ESP.getFreeHeap());
    Serial.println("--- HTTPS Request Finished ---");

    return success;
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
  snprintf(buffer, sizeof(buffer), "{ \"temperature\": %.2f, \"humidity\": %.2f }", temp, humidity);
  return string(buffer);
}