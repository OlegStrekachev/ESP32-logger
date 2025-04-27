#include <Arduino.h>
#include <RTClib.h>
#include "timezone.h"

// Determine if DST is active
bool isDST(int year, int month, int day, int hour) {
  if (month < 3 || month > 11) return false;
  if (month > 3 && month < 11) return true;

  int weekday;
  if (month == 3) {
    weekday = (5 + year + year/4 - year/100 + year/400) % 7;
    int secondSunday = 14 - weekday;
    return (day > secondSunday) || (day == secondSunday && hour >= 2);
  }
  if (month == 11) {
    weekday = (2 + year + year/4 - year/100 + year/400) % 7;
    int firstSunday = 7 - weekday;
    return (day < firstSunday) || (day == firstSunday && hour < 2);
  }
  return false;
}

void printLocalTimestamp(const DateTime& utcTime) {
  int year = utcTime.year();
  int month = utcTime.month();
  int day = utcTime.day();
  int hour = utcTime.hour();
  int minute = utcTime.minute();
  int second = utcTime.second();

  // UTC-7 (MST) or UTC-6 (MDT if DST active)
  int offsetHours = isDST(year, month, day, hour) ? -6 : -7;

  int localHour = hour + offsetHours;

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

  Serial.printf("[%04d-%02d-%02d %02d:%02d:%02d] ",
    year, month, day,
    localHour, minute, second);
}
