#ifndef EEPROM_LOGGER_H
#define EEPROM_LOGGER_H

#include <Arduino.h>

struct LogEntry {
  uint32_t timestamp;
  float temperature;
  float humidity;
};

extern const int EEPROM_SIZE;   // Correct
extern const int LOG_SIZE;      // Correct


bool initEEPROMLogger();
void saveLogEntry(const LogEntry& entry);
void trySaveSensorReading();
LogEntry readLastEntry();
int getNextAddress();   // <===== NEW

#endif
