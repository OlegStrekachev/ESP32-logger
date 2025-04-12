#ifndef RTC_H
#define RTC_H

#include <RTClib.h>

void initRTC();
void syncRTCWithNTP();
DateTime getTimestamp();

#endif
