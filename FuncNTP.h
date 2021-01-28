#ifndef __ZCZ_NTP_FUNC
#define __ZCZ_NTP_FUNC

#include <NTPClient.h>
#include <WiFiUdp.h>
#include <stdio.h>
#include <time.h>
class FuncNTP
{
public:
    WiFiUDP ntpUDP;
    NTPClient timeClient = NTPClient(ntpUDP, "cn.pool.ntp.org", 8 * 3600, 60000);
    FuncNTP(){};
    void init();
    int getDayOfWeek();
    unsigned long getNTPTimeSeconds();
};
#endif
