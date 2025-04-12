#include <Arduino.h>
#include "wifi.h"
#include "dht.h"
#include "rtc.h"
#include "eeprom.h"


// put function declarations here:
int myFunction(int, int);

void setup() {
  Serial.begin(115200);  // Optional, but good to have
  initWiFi();            // ✅ Call the function here
  int result = myFunction(2, 3);
  Serial.println(result);  // Just for test output
}

void loop() {
  // put your main code here, to run repeatedly:
}

// put function definitions here:
int myFunction(int x, int y) {
  return x + y;
}
