#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
#include "dht.h"
#include "rtc.h"  // 🔁 Include RTC module to get timestamp

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
    DateTime now = getTimestamp();  // 🔁 Get current timestamp from DS3231

    Serial.printf("[%04d-%02d-%02d %02d:%02d:%02d] ",
                  now.year(), now.month(), now.day(),
                  now.hour(), now.minute(), now.second());

    Serial.print("🌡 Temp: ");
    Serial.print(temp);
    Serial.print(" °C  |  💧 Humidity: ");
    Serial.print(hum);
    Serial.println(" %");
  } else {
    Serial.println("⚠️ Failed to read from DHT sensor");
  }
}
