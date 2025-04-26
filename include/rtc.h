#ifndef RTC_H
#define RTC_H

#include <RTClib.h>

void syncRTCWithNTP();
DateTime getTimestamp();  // ✅ You NEED this if dht.cpp calls getTimestamp()

#endif
