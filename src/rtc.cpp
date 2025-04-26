#include "rtc.h"
#include "wifi.h"
#include <Wire.h>
#include <WiFi.h>
#include "time.h"


// Class that allows physical connection between DS3231 over I2C wire.
RTC_DS3231 rtc;


void syncRTCWithNTP() {
  struct tm timeinfo;
  int attempts = 0;
  const int maxAttempts = 20;

  while (!getLocalTime(&timeinfo) && attempts < maxAttempts) {
    Serial.print(".");       // Visual feedback
    Serial.flush();
    delay(500);
    attempts++;
  }

  if (attempts >= maxAttempts) {
    Serial.println("\n⚠️ Failed to sync time from NTP after retries");
    return;
  }

  // Get current UNIX time after successful NTP sync
  time_t now;
  time(&now);   // now = seconds since Jan 1, 1970

  rtc.adjust(DateTime((uint32_t)now));  // ✅ Direct UNIX time sync to DS3231
  
  Serial.printf("\n✅ RTC synced to UNIX time: %ld\n", now);
}

// ✅ Completely separate, outside of syncRTCWithNTP():
DateTime getTimestamp() {
  return rtc.now();   // ✅ Fetch the current time
}