// eeprom_logger.cpp

#include <Arduino.h>   // Core Arduino functions
#include <EEPROM.h>    // EEPROM library for reading/writing non-volatile memory
#include "rtc.h"       // RTC functions (for timestamps)
#include "dht.h"       // DHT sensor functions (temperature/humidity)

// --- Data Structure ---

// Structure to store one log entry
struct LogEntry {
  uint32_t timestamp;    // Time of recording (Unix time)
  float temperature;     // Temperature in Celsius
  float humidity;        // Humidity in %
};

// --- Global Variables ---

int EEPROM_SIZE = 4096;            // EEPROM size in bytes (modifiable if needed)
int LOG_SIZE = sizeof(LogEntry);   // Size of one LogEntry in bytes (dynamic)

int nextAddress = 0;               // Address where the next log entry will be written

unsigned long lastSaveTime = 0;    // Last time a reading was saved (in milliseconds)

// --- Helper Functions ---

// Returns the next address to write a log entry
int getNextAddress() {
  return nextAddress;
}

// --- EEPROM Initialization ---

// Initialize EEPROM for logging
void initEEPROMLogger() {
  if (!EEPROM.begin(EEPROM_SIZE)) {       // Attempt to allocate EEPROM
    Serial.println("❌ Failed to initialize EEPROM");
    while (true);                         // Stop program if initialization fails
  }
  Serial.println("✅ EEPROM ready!");      // Successfully initialized
}

// --- Save a New Log Entry ---

// Saves a given LogEntry into EEPROM at the next available address
void saveLogEntry(const LogEntry& entry) {
  // Check if there’s enough space left, else wrap around to 0
  if (nextAddress + LOG_SIZE > EEPROM_SIZE) {
    Serial.println("⚠️ EEPROM full, wrapping to 0!");
    nextAddress = 0;
  }

  // Save entry byte-by-byte into EEPROM
  const uint8_t* p = (const uint8_t*)&entry;
  for (unsigned int i = 0; i < LOG_SIZE; i++) {
    EEPROM.write(nextAddress + i, p[i]);
  }
  EEPROM.commit();     // Actually write changes to memory

  Serial.printf("✅ Log saved at address 0x%03X\n", nextAddress);

  nextAddress += LOG_SIZE;   // Move pointer to next location
}

// --- Try to Save a New Sensor Reading ---

// Checks if 1 minute has passed and saves new sensor reading
void trySaveSensorReading() {
  if (millis() - lastSaveTime >= 60000) {    // If 60,000 ms (1 minute) passed
    lastSaveTime += 60000;                   // Update next save time

    float temp = dht.readTemperature();      // Read temperature from DHT
    float hum = dht.readHumidity();           // Read humidity from DHT

    if (!isnan(temp) && !isnan(hum)) {        // Check if sensor readings are valid
      DateTime now = getTimestamp();          // Get current timestamp from RTC

      LogEntry entry;                         // Create new log entry
      entry.timestamp = now.unixtime();       // Save timestamp
      entry.temperature = temp;               // Save temperature
      entry.humidity = hum;                   // Save humidity

      saveLogEntry(entry);                    // Write to EEPROM

      Serial.printf("[%lu] 📥 Temp: %.1f°C | Hum: %.1f%%\n", entry.timestamp, temp, hum);
    } else {
      Serial.println("⚠️ Sensor read failed, skipping save.");
    }
  }
}

// --- Read Last Saved Entry ---

// Reads the most recent log entry from EEPROM
LogEntry readLastEntry() {
  LogEntry entry;
  
  int lastAddress = nextAddress - sizeof(LogEntry);    // Calculate address of last entry
  if (lastAddress < 0) {
    lastAddress = EEPROM_SIZE - sizeof(LogEntry);      // Wrap around if necessary
  }

  uint8_t* p = (uint8_t*)&entry;
  for (unsigned int i = 0; i < sizeof(LogEntry); i++) {
    p[i] = EEPROM.read(lastAddress + i);               // Read back bytes into structure
  }

  return entry;    // Return the recovered log entry
}
