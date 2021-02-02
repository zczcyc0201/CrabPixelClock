#include "FuncNTP.h"

//初始化RTC时间类
void FuncNTP::init()
{
	//开始获取NTP服务器时间
	timeClient.begin();
	Serial.println("<NTP SERVER> Already initialized");
}

//获取epoch时间秒数 1971年开始计算
unsigned long FuncNTP::getNTPTimeSeconds()
{
	timeClient.update();
	unsigned long epochTime = timeClient.getEpochTime();
	return epochTime;
}
//Return value: Sunday=1, Monday=2, ... Saturday=7
int FuncNTP::getDayOfWeek()
{
	uint32_t epochTime = getNTPTimeSeconds();
	time_t localtime = epochTime;
	int days_since_epoch = localtime / 86400;
	int day_of_week = (days_since_epoch + 4) % 7;
	return day_of_week+1;
}
