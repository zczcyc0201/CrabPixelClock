#ifndef __ZCZ_WEATHER_FUNC
#define __ZCZ_WEATHER_FUNC

#include <ArduinoJson.h>
#include <ESP8266WiFi.h>

class FuncWeather {
public:
    String reqUserKey = ""; // 私钥
    String reqLocation = ""; // 城市
    //当日最高温
    int todayHigh = 0;
    //当日最低温
    int todayLow = 0;
    //当日天气代码
    int todayCode = 1;
    FuncWeather() {};
    void init();
    void getWeatherFromInternet();

private:
    void httpRequest(String reqRes);
    void parseInfo(WiFiClient client);
};
#endif
//天气代码说明
// 0	晴（国内城市白天晴）	Sunny
// 1	晴（国内城市夜晚晴）	Clear
// 2	晴（国外城市白天晴）	Fair
// 3	晴（国外城市夜晚晴）	Fair
// 4	多云	Cloudy
// 5	晴间多云	Partly Cloudy
// 6	晴间多云	Partly Cloudy
// 7	大部多云	Mostly Cloudy
// 8	大部多云	Mostly Cloudy
// 9	阴	Overcast
// 10	阵雨	Shower
// 11	雷阵雨	Thundershower
// 12	雷阵雨伴有冰雹	Thundershower with Hail
// 13	小雨	Light Rain
// 14	中雨	Moderate Rain
// 15	大雨	Heavy Rain
// 16	暴雨	Storm
// 17	大暴雨	Heavy Storm
// 18	特大暴雨	Severe Storm
// 19	冻雨	Ice Rain
// 20	雨夹雪	Sleet
// 21	阵雪	Snow Flurry
// 22	小雪	Light Snow
// 23	中雪	Moderate Snow
// 24	大雪	Heavy Snow
// 25	暴雪	Snowstorm
// 26	浮尘	Dust
// 27	扬沙	Sand
// 28	沙尘暴	Duststorm
// 29	强沙尘暴	Sandstorm
// 30	雾	Foggy
// 31	霾	Haze
// 32	风	Windy
// 33	大风	Blustery
// 34	飓风	Hurricane
// 35	热带风暴	Tropical Storm
// 36	龙卷风	Tornado
// 37	冷	Cold
// 38	热	Hot
// 99	未知	Unknown
