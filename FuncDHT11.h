#ifndef __ZCZ_DHT11_FUNC
#define __ZCZ_DHT11_FUNC
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>
class FuncDHT11
{
public:
    DHT dht=DHT(D1, DHT11);
    FuncDHT11(){};
    void init();
    float getTemperature(); //获取温度
    float getHumidity();    //获取湿度
};
#endif
