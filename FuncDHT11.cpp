#include "FuncDHT11.h"

//初始化传感器
void FuncDHT11::init()
{
  //创建DHT并初始化
  dht.begin();
}
//获取温度
float FuncDHT11::getTemperature()
{
  float t = dht.readTemperature();
  if (isnan(t))
  {
    Serial.println(F("Error reading temperature!"));
    return 0;
  }
  else
  {
    return t;
  }
}
//获取湿度
float FuncDHT11::getHumidity()
{
  float r = dht.readHumidity();
  if (isnan(r))
  {
    Serial.println(F("Error reading humidity!"));
    return 0;
  }
  else
  {
    return r;
  }
}