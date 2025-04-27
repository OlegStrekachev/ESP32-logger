#ifndef RTC_H
#define RTC_H

#include <RTClib.h>

extern RTC_DS3231 rtc;   // ✅ Declare that rtc exists elsewhere

void syncRTCWithNTP();
DateTime getTimestamp();  // ✅ You NEED this if dht.cpp calls getTimestamp()

#endif
