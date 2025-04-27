// eeprom_logger.cpp

#include <Arduino.h>
#include <EEPROM.h>
#include "rtc.h"     // For getTimestamp()
#include "dht.h"     // For dht.readTemperature(), etc.

struct LogEntry {
  uint32_t timestamp;  // Unix time
  float temperature;   // Celsius
  float humidity;      // % Humidity
};

int EEPROM_SIZE = 4096;           // <<< NOT const anymore
int LOG_SIZE = sizeof(LogEntry);  // <<< NOT const anymore

int nextAddress = 0;                    // Where to write next

int getNextAddress() {
  return nextAddress;
}


unsigned long lastSaveTime = 0;          // Time since last save (ms)

void initEEPROMLogger() {
  if (!EEPROM.begin(EEPROM_SIZE)) {
    Serial.println("❌ Failed to initialize EEPROM");
    while (true);
  }
  Serial.println("✅ EEPROM ready!");
}

void saveLogEntry(const LogEntry& entry) {
  if (nextAddress + LOG_SIZE > EEPROM_SIZE) {
    Serial.println("⚠️ EEPROM full, wrapping to 0!");
    nextAddress = 0;
  }

  const uint8_t* p = (const uint8_t*)&entry;
  for (unsigned int i = 0; i < LOG_SIZE; i++) {
    EEPROM.write(nextAddress + i, p[i]);
  }
  EEPROM.commit();
  Serial.printf("✅ Log saved at address 0x%03X\n", nextAddress);

  nextAddress += LOG_SIZE;
}

void trySaveSensorReading() {
  if (millis() - lastSaveTime >= 60000) {  // 1 minute passed
    lastSaveTime += 60000;                 // Advance time precisely

    float temp = dht.readTemperature();
    float hum = dht.readHumidity();

    if (!isnan(temp) && !isnan(hum)) {
      DateTime now = getTimestamp();

      LogEntry entry;
      entry.timestamp = now.unixtime();
      entry.temperature = temp;
      entry.humidity = hum;

      saveLogEntry(entry);

      Serial.printf("[%lu] 📥 Temp: %.1f°C | Hum: %.1f%%\n", entry.timestamp, temp, hum);
    } else {
      Serial.println("⚠️ Sensor read failed, skipping save.");
    }
  }
}

LogEntry readLastEntry() {
  LogEntry entry;
  
  int lastAddress = nextAddress - sizeof(LogEntry);
  if (lastAddress < 0) {
    lastAddress = EEPROM_SIZE - sizeof(LogEntry);  // Wrap around
  }

  uint8_t* p = (uint8_t*)&entry;
  for (unsigned int i = 0; i < sizeof(LogEntry); i++) {
    p[i] = EEPROM.read(lastAddress + i);
  }

  return entry;
}

