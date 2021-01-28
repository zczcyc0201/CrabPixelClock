#ifndef __ZCZ_DISPLAY_MANAGER
#define __ZCZ_DISPLAY_MANAGER
#include "FuncDHT11.h" //DHT11温湿度模块
#include "FuncHttpReq.h"
#include "FuncNTP.h" //NTP网络时间模块
#include "FuncNeoPixelMatrix.h" //灯阵模块
#include "FuncWeather.h" //天气获取模块
#include "IconsPixels.h"
#include "IconsWeather.h"
#include "ShapeAnimation.h"
#include <Adafruit_NeoMatrix.h>
//字体定义在y方向的偏移量
const char martrixOffsetY = 6;

class DisplayManager {
public:
    //网络请求类
    FuncHttpReq httpClient = FuncHttpReq();
    DisplayManager() {};
    void displayTemperature(FuncNeoPixelMatrix& funcMatrix, FuncDHT11& funcDHT11); //显示温度
    void displayHumidity(FuncNeoPixelMatrix& funcMatrix, FuncDHT11& funcDHT11); //显示湿度
    void displayWeather(FuncNeoPixelMatrix& funcMatrix, FuncWeather& funcWeather); //显示天气
    void displayTime(FuncNeoPixelMatrix& funcMatrix, FuncNTP& funcNtp, char hour, char min, char sec); //显示时间
    void displayStockPrice(FuncNeoPixelMatrix& funcMatrix, String stockCode, unsigned long* colorMap, unsigned char* pixels); //显示股票价格
    void displaySetting(FuncNeoPixelMatrix& funcMatrix);
    void displayAnimationHook(FuncNeoPixelMatrix& funcMatrix, uint8_t x, uint8_t y); //对勾动画
    void displayAnimationPacman(FuncNeoPixelMatrix& funcMatrix); //吃豆人动画

    
    void displayBili(FuncNeoPixelMatrix& funcMatrix); //显示B站粉丝
    uint32_t getColorObjByInt(unsigned long color);
    int textCenterX(int strLength, int charWidth, int maxCharCount);
    String stockCode1 = "sh000001"; //股票1 默认上证指数
    String stockCode2 = "sh600519"; //股票2 默认茅台
    String currentShowStockCode = ""; //当前显示的股票代码
    String stockOpenPrice = "0"; //股票开盘价格
    String stockCurrentPrice = "0";//股票当前价格
    String biliFansCount="0";//B站粉丝数
    String biliUID = "44061661"; //bilibili 的uid
    const String biliAPI = "http://api.bilibili.com/x/relation/stat?vmid="; //bilibili的api 获取粉丝数量
    const String stockPriceApi = "http://hq.sinajs.cn/list=";//股票价格接口

private:
    unsigned long stockUpdateTime = 0; //股票价格更新时间
    unsigned long biliUpdateTime = 0; //B站粉丝更新时间
    unsigned int biliUpdateGap = 10000; //更新间隔
    unsigned int updateGap = 5000; //更新间隔
    String getStockPrice(String& stockCode);
    void playSingleFrame(FuncNeoPixelMatrix& funcMatrix, uint8_t x, uint8_t y, const uint8_t* animationFrame, unsigned long color, int delayTime,bool fillZero=true);
    String getBiliFans();
    //定义显示用色值

    uint16_t myColorBili = Adafruit_NeoMatrix::Color(134,224,253); // bili字体颜色 浅蓝
    uint16_t myColorTime = Adafruit_NeoMatrix::Color(255, 150, 134); //时间字体颜色 
    uint16_t myColorStockUp = Adafruit_NeoMatrix::Color(250, 103, 103); //股票上涨字体颜色
    uint16_t myColorStockDown = Adafruit_NeoMatrix::Color(103,250,103); //股票下跌字体颜色
    uint16_t myColorTemperatureHigh = Adafruit_NeoMatrix::Color(250, 110, 110); //温度较高时温度颜色 
    uint16_t myColorTemperatureLow = Adafruit_NeoMatrix::Color(48, 201, 232); //温度较低时温度颜色 
    uint16_t myColorHumidity = Adafruit_NeoMatrix::Color(48, 201, 232); //湿度颜色 深蓝
    uint16_t myColorWeatherHigh = Adafruit_NeoMatrix::Color(246, 80, 60); //当日天气预报最高温 
    uint16_t myColorWeatherLow = Adafruit_NeoMatrix::Color(48, 201, 232); //当日天气预报最低温 
};
#endif
