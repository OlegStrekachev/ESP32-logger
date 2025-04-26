#include <WiFi.h>          // ESP32 Wi-Fi functions
#include "wifi.h"          // Your own Wi-Fi module header
#include "secrets.h"       // Wi-Fi credentials (WIFI_SSID and WIFI_PASS)
#include "time.h"          // Time functions (configTime, getLocalTime, time)

const char* ssid = WIFI_SSID;          // SSID loaded from secrets.h
const char* password = WIFI_PASS;      // Password loaded from secrets.h

// NTP settings (pure UTC time, no timezone shift)
const long gmtOffset_sec = 0;          // Offset from UTC (0 means UTC)
const int daylightOffset_sec = 0;      // Daylight saving adjustment (0 means no DST)
const char* ntpServer = "pool.ntp.org"; // NTP server to fetch time from

void initWiFi() {
  WiFi.begin(ssid, password);          // Start connecting to Wi-Fi network
  Serial.print("Connecting to WiFi");

  int retries = 0;                     // Initialize Wi-Fi connection attempt counter
  const int maxRetries = 20;            // Maximum number of retries before giving up

  // Try connecting to Wi-Fi until connected or retries exhausted
  while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
    delay(500);                        // Wait 500ms between connection attempts
    Serial.print(".");                 // Print a dot for visual feedback
    retries++;                         // Increase retry counter
  }

  // Check if connected after loop
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println(" Connected!");      // Connected to Wi-Fi successfully
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());     // Print the device's local IP address
  } else {
    Serial.println(" Failed to connect to Wi-Fi.");
    return;                             // Abort function if Wi-Fi connection failed
  }

  // Set up NTP (Network Time Protocol) client to fetch UTC time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;                  // Structure to hold calendar time (year, month, day, etc.)
  int ntpRetries = 0;                  // Initialize NTP time fetch retry counter
  const int ntpMaxRetries = 20;         // Maximum retries for NTP sync

  // Wait until ESP32 fetches time from NTP or retries are exhausted
  Serial.println("Waiting for NTP time sync...");  // Print once before starting loop

  while (!getLocalTime(&timeinfo) && ntpRetries < ntpMaxRetries) {
      Serial.print(".");          // Print a dot each retry
      Serial.flush();             // Immediately push out the dot to Serial Monitor
      delay(500);                 // Wait 500 ms
      ntpRetries++;
  }

  // Check if NTP sync was successful
  if (ntpRetries >= ntpMaxRetries) {
    Serial.println("⚠️ Failed to sync time from NTP server!");
    return;                             // Abort function if time sync failed
  }

  // Successfully got time! Retrieve it as UNIX timestamp
  time_t now;
  time(&now);                           // Get current time as seconds since Jan 1, 1970

  Serial.println("✅ Current Unix time:");
  Serial.println(now);                  // Print the UNIX timestamp
}
