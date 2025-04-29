#ifndef TIMEZONE_H
#define TIMEZONE_H

#include <RTClib.h>

bool isDST(int year, int month, int day, int hour);
String getLocalTimestampString(const DateTime& utcTime);
void printLocalTimestamp(const DateTime& utcTime);

#endif
