#include <WiFi.h>          // ESP32 Wi-Fi functions
#include "wifi.h"          // Your own Wi-Fi module header
#include "secrets.h"       // Wi-Fi credentials (WIFI_SSID and WIFI_PASS)
#include "time.h"          // Time functions (configTime, getLocalTime, time)

const char* ssid = WIFI_SSID;          // SSID loaded from secrets.h
const char* password = WIFI_PASS;      // Password loaded from secrets.h

const long gmtOffset_sec = 0;           // Offset from UTC (0 = UTC time)
const int daylightOffset_sec = 0;       // Daylight saving adjustment
const char* ntpServer = "pool.ntp.org"; // NTP server to fetch time

void initWiFi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(ssid, password);

  const int maxRetries = 20;  
  int retries = 0;

  while (WiFi.status() != WL_CONNECTED && retries < maxRetries) {
    Serial.print(".");
    delay(500);
    retries++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\n❌ Failed to connect to Wi-Fi. Continuing without Wi-Fi...");
    return;  // 🚫 No Wi-Fi. Don't crash. Continue program
  }

  Serial.println("\n✅ Wi-Fi connected!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Now try to set time using NTP
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  Serial.println("🌐 Waiting for NTP time sync...");

  struct tm timeinfo;
  const int ntpMaxRetries = 20;
  int ntpRetries = 0;

  while (!getLocalTime(&timeinfo) && ntpRetries < ntpMaxRetries) {
    Serial.print(".");
    delay(500);
    ntpRetries++;
  }

  if (ntpRetries >= ntpMaxRetries) {
    Serial.println("\n⚠️ Failed to sync time from NTP after retries. Using RTC only (if available).");
    return;
  }

  // NTP sync successful
  time_t now;
  time(&now);

  Serial.println("\n✅ Current Unix time:");
  Serial.println(now);
}
