#include "DisplayManager.h"

int DisplayManager::textCenterX(int strLength, int charWidth, int maxCharCount)
{
    if (strLength > maxCharCount)
        strLength = maxCharCount;
    return (maxCharCount - strLength) * charWidth / 2;
}

//显示温度
void DisplayManager::displayTemperature(FuncNeoPixelMatrix &funcMatrix, FuncDHT11 &funcDHT11)
{
    float temperature = funcDHT11.getTemperature();
    unsigned char iconWidth = 8;
    funcMatrix.fillScreen(0);
    funcMatrix.setCursor(0, 0);
    //温度图标
    funcMatrix.setCursor(0, 0);
    funcMatrix.drawColorMap(temperatureColorArr, iconWidth, 8, temperaturePixels);
    //显示温度数字 小于15度蓝色 大于15度红色
    if (temperature < 15)
    {
        funcMatrix.setTextColor(myColorTemperatureLow);
    }
    else
    {
        funcMatrix.setTextColor(myColorTemperatureHigh);
    }
    funcMatrix.setCursor(iconWidth + 3, martrixOffsetY);
    funcMatrix.print(temperature);
    funcMatrix.show();
}

//显示湿度
void DisplayManager::displayHumidity(FuncNeoPixelMatrix &funcMatrix, FuncDHT11 &funcDHT11)
{
    int humidity = (int)funcDHT11.getHumidity();
    unsigned char iconWidth = 8;
    funcMatrix.fillScreen(0);
    //湿度图标
    funcMatrix.setCursor(0, 0);
    funcMatrix.drawColorMap(humidityColorArr, iconWidth, 8, humidityPixels);
    //百分号图标 湿度为两个数字 占8格 右边再留2格空白 所以+10
    funcMatrix.setCursor(0, 0);
    funcMatrix.drawColorMap(percentColorArr, 5, 5, percentPixels, iconWidth + 13, 1);
    //湿度数值
    funcMatrix.setTextColor(myColorHumidity);
    funcMatrix.setCursor(iconWidth + 2, martrixOffsetY);
    funcMatrix.print(humidity);
    funcMatrix.show();
}
//显示室外天气温度
void DisplayManager::displayWeather(FuncNeoPixelMatrix &funcMatrix, FuncWeather &funcWeather)
{
    int todayHigh = funcWeather.todayHigh;
    int todayLow = funcWeather.todayLow;
    int weatherCode = funcWeather.todayCode;
    unsigned char iconWidth = 8;
    funcMatrix.fillScreen(0); //清空屏幕
    //天气图标
    if (weatherCode <= 3) //晴天
    {
        funcMatrix.drawColorMap(weatherSunnyColorArr, iconWidth, 8, weatherSunnyPixels);
    }
    else if (weatherCode >= 4 && weatherCode <= 9) //多云 阴天
    {
        funcMatrix.drawColorMap(weatherCloudyColorArr, iconWidth, 8, weatherCloudyPixels);
    }
    else if (weatherCode >= 10 && weatherCode <= 20) //各种雨
    {
        funcMatrix.drawColorMap(weatherRainColorArr, iconWidth, 8, weatherRainPixels);
    }
    else if (weatherCode >= 21 && weatherCode <= 25) //各种雪
    {
        funcMatrix.drawColorMap(weatherSnowColorArr, iconWidth, 8, weatherSnowPixels);
    }
    else if (weatherCode >= 26 && weatherCode <= 31) //各种雾霾
    {
        funcMatrix.drawColorMap(weatherDustColorArr, iconWidth, 8, weatherDustPixels);
    }
    else if (weatherCode <= 32) //各种风
    {
        funcMatrix.drawColorMap(weatherWindColorArr, iconWidth, 8, weatherWindPixels);
    }
    //最低温
    funcMatrix.setTextColor(myColorWeatherLow);
    funcMatrix.setCursor(iconWidth + 1, martrixOffsetY);
    funcMatrix.print(todayLow);
    //最高温
    funcMatrix.setTextColor(myColorWeatherHigh);
    if (todayHigh > 0 || todayLow > 0) //有一个温度值大于0 间隙可以小一点 好看
    {
        funcMatrix.setCursor(iconWidth + 14, martrixOffsetY);
    }
    else
    {
        funcMatrix.setCursor(iconWidth + 16, martrixOffsetY);
    }
    funcMatrix.print(todayHigh);
    funcMatrix.show();
}

//显示时间
void DisplayManager::displayTime(FuncNeoPixelMatrix &funcMatrix, char dayOfWeek, char hour, char min, char sec)
{
    //格式化时间
    char timeString[10];
    snprintf(timeString, 10, PSTR("%02u:%02u:%02u"), hour, min, sec);
    //清空屏幕
    char xOffset = 3; //左边留白
    funcMatrix.fillScreen(0);
    funcMatrix.setCursor(xOffset, martrixOffsetY);
    funcMatrix.setTextColor(myColorTime);
    //打印时间
    funcMatrix.print(timeString);
    //显示星期图标
    funcMatrix.setCursor(0, 0);
    switch (dayOfWeek)
    {
    case 0:
        funcMatrix.drawColorMap(weekColorArr, 20, 1, sunPixels, 6, 7);
        break;
    case 1:
        funcMatrix.drawColorMap(weekColorArr, 20, 1, monPixels, 6, 7);
        break;
    case 2:
        funcMatrix.drawColorMap(weekColorArr, 20, 1, tuePixels, 6, 7);
        break;
    case 3:
        funcMatrix.drawColorMap(weekColorArr, 20, 1, wedPixels, 6, 7);
        break;
    case 4:
        funcMatrix.drawColorMap(weekColorArr, 20, 1, thuPixels, 6, 7);
        break;
    case 5:
        funcMatrix.drawColorMap(weekColorArr, 20, 1, friPixels, 6, 7);
        break;
    case 6:
        funcMatrix.drawColorMap(weekColorArr, 20, 1, staPixels, 6, 7);
        break;
    }
    funcMatrix.show();
}

//获取股票价格
String DisplayManager::getStockPrice(FuncHttpReq &httpClient, String &stockCode)
{
    // 获取一次股票实时价格
    // var hq_str_sz002307="北新路桥,4.840,4.840,4.900,4.910,4.810,4.890,4.900,5540988,27006418.280,406740,4.890,238880,4.880,122060,4.870,158700,4.860,212900,4.850,128400,4.900,184500,4.910,168700,4.920,91760,4.930,92880,4.940,2021-01-04,14:53:51,00";
    String stockPrice = httpClient.getRequest(stockPriceApi + stockCode, 5000);
    return stockPrice;
}
//显示股票价格
void DisplayManager::displayStockPrice(FuncNeoPixelMatrix &funcMatrix, String stockCode, unsigned long *colorMap, unsigned char *pixels)
{
    FuncHttpReq httpClient = FuncHttpReq();
    currentShowStockCode = stockCode;
    funcMatrix.fillScreen(0); //清空屏幕
    funcMatrix.setTextColor(myColorStockUp);
    //当前价和开盘价
    if ((millis() - stockUpdateTime) > updateGap && currentShowStockCode == stockCode)
    { //检查是否到了更新时间
        stockUpdateTime = millis();
        String stockResult = getStockPrice(httpClient, stockCode); //网络更新
        if (stockResult != "FAIL")
        {
            String stockCurrentResult = httpClient.getSplitString(stockResult, ',', 3);
            stockCurrentPrice = stockCurrentResult.substring(0, stockCurrentResult.length() - 1);
            String stockOpenResult = httpClient.getSplitString(stockResult, ',', 2);
            stockOpenPrice = stockOpenResult.substring(0, stockOpenResult.length() - 1);
        }
    }
    if (stockCurrentPrice < stockOpenPrice)
    { // 涨了红色 跌了绿色
        funcMatrix.setTextColor(myColorStockDown);
    }
    unsigned char iconWidth = 5;
    funcMatrix.drawColorMap(colorMap, iconWidth, 5, pixels, 0, 1);
    //设置颜色文字和写入股票价格
    if (stockCurrentPrice.length() > 6)
    {
        funcMatrix.setCursor((iconWidth + 1), martrixOffsetY);
    }
    else
    {
        //.toFloat()主要是为了去除多余的后面的0
        funcMatrix.setCursor((iconWidth + 1) + this->textCenterX(String(stockCurrentPrice.toFloat()).length(), 4, 7), martrixOffsetY);
    }
    funcMatrix.print(stockCurrentPrice.toFloat());
    funcMatrix.show();
}

//获取B站粉丝数量
String DisplayManager::getBiliFans()
{
    FuncHttpReq httpClient = FuncHttpReq();

    String biliFans = httpClient.getRequest(biliAPI + biliUID, 5000);
    if (biliFans == "FAIL")
    {
        return "ERR";
    }
    else
    {
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, biliFans);
        JsonObject obj = doc.as<JsonObject>();
        biliFans = obj["data"]["follower"].as<int>();
        return String(biliFans);
    }
}

//显示B站粉丝
void DisplayManager::displayBili(FuncNeoPixelMatrix &funcMatrix)
{
    funcMatrix.fillScreen(0); //清空屏幕
    if ((millis() - biliUpdateTime) > biliUpdateGap)
    { //检查是否到了更新时间
        biliUpdateTime = millis();
        String fansCount = getBiliFans();
        if (fansCount != "ERR")
        {
            biliFansCount = fansCount;
        }
    }
    unsigned char iconWidth = 8;
    funcMatrix.setCursor(0, 0);
    funcMatrix.drawColorMap(biliColorArr, iconWidth, 8, biliPixels, 0, 0);
    funcMatrix.drawColorMap(bottomSideColorArr, 20, 1, bottomSidePixels, 10, 7);
    //设置颜色文字和写入股票价格
    funcMatrix.setTextColor(myColorBili);
    funcMatrix.setCursor(iconWidth + this->textCenterX(biliFansCount.length(), 4, 6), martrixOffsetY);
    funcMatrix.print(biliFansCount);
    funcMatrix.show();
}
//色值转color函数
uint32_t DisplayManager::getColorObjByInt(unsigned long color)
{
    byte red = (color & 0xFF0000) >> 16;
    byte green = (color & 0x00FF00) >> 8;
    byte blue = (color & 0x0000FF);
    return Adafruit_NeoPixel::Color(red, green, blue);
}

//显示内容设置画面
void DisplayManager::displaySetting(FuncNeoPixelMatrix &funcMatrix)
{
    funcMatrix.fillScreen(0);
    //湿度图标
    funcMatrix.setCursor(0, 0);
    funcMatrix.drawColorMap(settingColorArr, 8, 8, settingPixels);
    funcMatrix.show();
    funcMatrix.setTextColor(VIOLET);
    funcMatrix.setCursor(10, martrixOffsetY);
    funcMatrix.print("SET");
    funcMatrix.show();
}
//播放动画单帧
void DisplayManager::playSingleFrame(FuncNeoPixelMatrix &funcMatrix, uint8_t x, uint8_t y, const uint8_t *animationFrame, unsigned long color, int delayTime, bool fillZero)
{
    funcMatrix.drawBitmap(x, y, animationFrame, 8, 8, getColorObjByInt(color));
    funcMatrix.show();
    delay(delayTime);
    if (fillZero)
        funcMatrix.fillScreen(0);
}

//对勾动画
void DisplayManager::displayAnimationHook(FuncNeoPixelMatrix &funcMatrix, uint8_t x, uint8_t y)
{
    int frameDelayTime = 200;
    playSingleFrame(funcMatrix, x, y, animationHook1, GREENYELLOW, frameDelayTime);
    playSingleFrame(funcMatrix, x, y, animationHook2, GREENYELLOW, frameDelayTime);
    playSingleFrame(funcMatrix, x, y, animationHook3, GREENYELLOW, frameDelayTime);
    playSingleFrame(funcMatrix, x, y, animationHook4, GREENYELLOW, frameDelayTime);
    playSingleFrame(funcMatrix, x, y, animationClear, VIOLET, frameDelayTime);
}
//吃豆人动画
void DisplayManager::displayAnimationPacman(FuncNeoPixelMatrix &funcMatrix)
{
    int frameDelayTime = 120;
    for (char i = 0; i <= 32; i++)
    {
        char beanStartPosition = i;
        if (i % 2 == 0)
        {
            playSingleFrame(funcMatrix, i, 0, pacman1, PINK, 0, false);
        }
        else
        {
            beanStartPosition = i + 2;
            playSingleFrame(funcMatrix, i, 0, pacman2, PINK, 0, false);
        }
        // for (char j = i; j <=32; j += 4) {
        //     //从i所在的位置开始每隔2个像素绘制豆子
        //     playSingleFrame(funcMatrix, j, 0, pacmanBean, YELLOW, 0, false);
        // }
        delay(frameDelayTime);
        funcMatrix.fillScreen(0);
    }
}
