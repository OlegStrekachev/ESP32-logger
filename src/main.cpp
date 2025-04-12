
// I2C debugger - logs all I2C devices connected to ESP32
#include <Wire.h>
#include <Arduino.h>  // 🔁 Required in PlatformIO!

void setup() {
  Serial.begin(115200);
  Wire.begin(21, 22);  // ESP32 I2C pins
  Serial.println("I2C Scanner");

  for (byte addr = 1; addr < 127; addr++) {
    Wire.beginTransmission(addr);
    if (Wire.endTransmission() == 0) {
      Serial.print("✅ Found I2C device at 0x");
      Serial.println(addr, HEX);
    }
  }

  Serial.println("Done.");
}

void loop() {}




// #include "wifi.h"
// #include "dht.h"
// #include "rtc.h"
// #include "eeprom.h"
// #include <Arduino.h>

// // function declarations
// int myFunction(int, int);

// void setup() {
//   Serial.begin(115200);
//   initWiFi();      // Connects to WiFi (used by RTC sync)
//   initDHT();       // Initializes DHT sensor
//   initRTC();       // ❗ Required to init & sync DS3231 RTC

//   int result = myFunction(2, 3);
//   Serial.println(result);
// }

// void loop() {
//   printDHTValues();  // Uses getTimestamp() internally
//   delay(2000);
// }

// // simple test function
// int myFunction(int x, int y) {
//   return x + y;
// }
