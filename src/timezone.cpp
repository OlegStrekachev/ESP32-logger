// timezone.cpp

#include "timezone.h"
#include <Arduino.h>    // Needed for Serial.print and String
#include <RTClib.h>     // Needed for DateTime class

// --- Determine if DST is active for Mountain Time (US rules) ---
bool isDST(int year, int month, int day, int hour) {
  if (month < 3 || month > 11) return false;  // January, February, December → standard time
  if (month > 3 && month < 11) return true;   // April to October → DST active

  int weekday;
  if (month == 3) {  // March
    weekday = (5 + year + year/4 - year/100 + year/400) % 7;
    int secondSunday = 14 - weekday;
    return (day > secondSunday) || (day == secondSunday && hour >= 2);
  }
  if (month == 11) { // November
    weekday = (2 + year + year/4 - year/100 + year/400) % 7;
    int firstSunday = 7 - weekday;
    return (day < firstSunday) || (day == firstSunday && hour < 2);
  }
  return false;
}

// --- Return local timestamp as a String ---
String getLocalTimestampString(const DateTime& utcTime) {
  int year = utcTime.year();
  int month = utcTime.month();
  int day = utcTime.day();
  int hour = utcTime.hour();
  int minute = utcTime.minute();
  int second = utcTime.second();

  // UTC-7 (MST) or UTC-6 (MDT if DST active)
  int offsetHours = isDST(year, month, day, hour) ? -6 : -7;

  int localHour = hour + offsetHours;

  // Handle rollover across day boundaries
  if (localHour < 0) {
    localHour += 24;
    DateTime yesterday = utcTime - TimeSpan(1, 0, 0, 0);
    year = yesterday.year();
    month = yesterday.month();
    day = yesterday.day();
  } else if (localHour >= 24) {
    localHour -= 24;
    DateTime tomorrow = utcTime + TimeSpan(1, 0, 0, 0);
    year = tomorrow.year();
    month = tomorrow.month();
    day = tomorrow.day();
  }

  char buffer[32];
  snprintf(buffer, sizeof(buffer), "%04d-%02d-%02d %02d:%02d:%02d",
    year, month, day,
    localHour, minute, second);

  return String(buffer);
}

// --- Print local timestamp directly to Serial Monitor ---
void printLocalTimestamp(const DateTime& utcTime) {
  Serial.print("[");
  Serial.print(getLocalTimestampString(utcTime));  // Reuse smart function
  Serial.print("] ");
}
