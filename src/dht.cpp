#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "dht.h"
#include "rtc.h"      // To fetch timestamp from RTC
#include "timezone.h" // To print Mountain Time timestamp

#define DHTPIN 13
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);

void initDHT() {
  dht.begin();
}

void printDHTValues() {
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  if (!isnan(temp) && !isnan(hum)) {
    DateTime now = getTimestamp();   // Get current UTC timestamp from RTC

    printLocalTimestamp(now);         // Print timestamp adjusted to Mountain Time
    Serial.printf("🌡 Temp: %.1f °C | 💧 Humidity: %.1f %%\n", temp, hum);

    // 🔥 Here you can ALSO call saveToEEPROM(now, temp, hum); later
  } else {
    Serial.println("⚠️ Failed to read from DHT sensor");
  }
}
