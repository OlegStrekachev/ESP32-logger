#include "rtc.h"
#include "wifi.h"
#include <Wire.h>
#include <WiFi.h>
#include "time.h"

RTC_DS3231 rtc;

// NTP configuration
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 0;          // adjust for your timezone
const int daylightOffset_sec = 0;

void syncRTCWithNTP() {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

  struct tm timeinfo;
  int attempts = 0;
  const int maxAttempts = 20;

  while (!getLocalTime(&timeinfo) && attempts < maxAttempts) {
    delay(500);
    attempts++;
  }

  if (attempts >= maxAttempts) {
    Serial.println("⚠️ Failed to sync time from NTP after retries");
    return;
  }

  rtc.adjust(DateTime(
    timeinfo.tm_year + 1900,
    timeinfo.tm_mon + 1,
    timeinfo.tm_mday,
    timeinfo.tm_hour,
    timeinfo.tm_min,
    timeinfo.tm_sec
  ));

  Serial.printf("✅ RTC synced to: %04d-%02d-%02d %02d:%02d:%02d\n",
                timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
}

void initRTC() {
  // 🔧 Always specify pins for ESP32
  Wire.begin(21, 22);

  if (!rtc.begin()) {
    Serial.println("❌ Couldn't find DS3231 RTC");
    while (true);  // Fatal error: halt
  }

  if (rtc.lostPower()) {
    Serial.println("⚠️ RTC lost power — syncing with NTP...");

    initWiFi();          // Use external Wi-Fi module
    delay(1000);         // Allow Wi-Fi to settle
    syncRTCWithNTP();
    WiFi.disconnect(true); // Optional: turn off Wi-Fi to save power
  } else {
    Serial.println("✅ RTC is running and time appears valid.");
  }
}

DateTime getTimestamp() {
  // Note: lostPower() just tells us about historical validity
  return rtc.now();
}
