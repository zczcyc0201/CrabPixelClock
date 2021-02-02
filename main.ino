#include "DisplayManager.h"
#include "FuncDHT11.h"          //DHT11温湿度模块
#include "FuncHttpReq.h"        //HTTP模块
#include "FuncNTP.h"            //NTP网络时间模块
#include "FuncNeoPixelMatrix.h" //灯阵模块
#include "FuncWeather.h"        //天气获取模块
#include "IconsPixels.h"        //图标定义
#include "IconsWeather.h"       //图标定义
#include "ShapeAnimation.h"
#include <Adafruit_GFX.h>
#include <Adafruit_NeoMatrix.h>
#include <Adafruit_NeoPixel.h>
#include <ThreeWire.h>    //RTC模块 必须在RtcDS1302之前include
#define PIN_LED D5        //定义LED数据使用的PIN
#define PIN_RTC_CE_RST D2 //DS1302 PIN
#define PIN_RTC_DAT_IO D3 //DS1302 PIN
#define PIN_RTC_CLK D4    //DS1302 PIN
#define BUTTON_PIN D8     //按钮的gpio接口
#include "WiFiManager.h"
#include <Arduino.h>
#include <ArduinoJson.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <OneButton.h>
#include <RtcDS1302.h> //RTC模块
int matrixBright = 50; //灯阵亮度
//时钟模块定义
ThreeWire myWire(PIN_RTC_DAT_IO, PIN_RTC_CLK, PIN_RTC_CE_RST); // DAT/IO, SCLK/CLK, CE/RST
RtcDS1302<ThreeWire> rtc(myWire);

//2020年1月1日时间戳 因为时区的原因要加8个小时 RTC模块计算时间要用到
const int epochTime2000 = 946684800;
//NTP功能类
FuncNTP funcNtp = FuncNTP();
//DHT11功能类
FuncDHT11 funcDHT11 = FuncDHT11();
//天气功能类
FuncWeather funcWeather = FuncWeather();
//灯阵对象
FuncNeoPixelMatrix funcMatrix = FuncNeoPixelMatrix(32, 8, 1, 1, PIN_LED,
                                                   NEO_MATRIX_TOP + NEO_MATRIX_LEFT + NEO_MATRIX_COLUMNS + NEO_MATRIX_ZIGZAG,
                                                   NEO_GRB + NEO_KHZ800);
//显示管理类
DisplayManager displayer = DisplayManager();

uint8_t lastShowTime = 10;         //时间显示每次持续时长 单位秒
bool shouldShowTemperature = true; //是否显示温度
bool shouldShowHumidity = true;    //是否显示湿度
bool shouldShowWeather = true;     //是否显示天气
bool shouldShowStock = true;       //是否显示股票价格
bool shouldShowStock2 = true;      //是否显示股票价格
bool shouldShowBiliFans = true;    //是否显示bili粉丝
//是否为设置模式
bool isSettingMode = false;
//初始化按钮控制
OneButton oneBtn = OneButton(BUTTON_PIN, true, true);

//设置网页的服务器
ESP8266WebServer esp8266_server(80);

//读取配置文件
//储存配置的文件名称
const String settingFileName = "/mysetting/setJson.txt";
const String settingBrightFileName = "/mysetting/setBrightJson.txt";

void readSettingJson()
{ //读取之前保存的亮度
    if (SPIFFS.exists(settingBrightFileName))
    {
        File dataFileRead = SPIFFS.open(settingBrightFileName, "r");
        String setJson = dataFileRead.readString();
        dataFileRead.close();
        DynamicJsonDocument doc(256);
        deserializeJson(doc, setJson);
        JsonObject jobj = doc.as<JsonObject>();
        int brightness = jobj["brightness"];
        if (brightness < 1)
        { //防止出现错误亮度为0不显示
            brightness = 30;
        }
        matrixBright = brightness;
        funcMatrix.setBrightness(brightness);
    }
    //确认闪存中是否有file_name文件
    if (SPIFFS.exists(settingFileName))
    {
        Serial.print(settingFileName);
        Serial.println("SETTING FOUND.");
        //建立File对象用于从SPIFFS中读取文件
        File dataFileRead = SPIFFS.open(settingFileName, "r");
        //读取文件内容并且通过串口监视器输出文件信息
        String setJson = dataFileRead.readString();
        Serial.print(setJson);
        //完成文件读取后关闭文件
        dataFileRead.close();
        DynamicJsonDocument doc(1024);
        deserializeJson(doc, setJson);
        JsonObject jobj = doc.as<JsonObject>();
        shouldShowWeather = jobj["showWeather"];
        shouldShowTemperature = jobj["showTemperature"];
        shouldShowHumidity = jobj["showHumidity"];
        shouldShowStock = jobj["showStock1"];
        shouldShowStock2 = jobj["showStock2"];
        shouldShowBiliFans = jobj["showBili"];
        String weatherKey = jobj["weatherKey"];
        funcWeather.reqUserKey = weatherKey;
        String weatherCity = jobj["weatherCity"];
        funcWeather.reqLocation = weatherCity;
        String stockCode1 = jobj["stockCode1"];
        displayer.stockCode1 = stockCode1;
        String stockCode2 = jobj["stockCode2"];
        displayer.stockCode2 = stockCode2;
        String biliUid = jobj["biliUid"];
        displayer.biliUID = biliUid;
    }
    else
    {
        Serial.println("SETTING NOT FOUND.");
    }
}

//双击检测 进入设置模式
void handleDoubleClick()
{
    Serial.println("handleDoubleClick");
    isSettingMode = !isSettingMode;
    if (isSettingMode)
    {
        funcMatrix.setTextColor(BLUE);
        esp8266_server.begin(); //设置模式 开启网络服务
    }
    else
    {
        esp8266_server.stop(); //显示模式 停止网络服务
    }
}
// 处理亮度设置
void handleBrightness()
{
    String brightStr = esp8266_server.arg("bright"); // 获取用户请求中的PWM数值
    int brightVal = brightStr.toInt();               // 将用户请求中的PWM数值转换为整数
    brightVal = map(brightVal, 1, 100, 1, 255);
    Serial.print("brightVal = ");
    Serial.println(brightVal);
    //设置灯阵亮度
    funcMatrix.setBrightness(brightVal);
    //生成json 保存到文件系统
    char objStr[256] = "";
    DynamicJsonDocument saveDoc(256);
    saveDoc["brightness"] = brightVal;
    serializeJson(saveDoc, objStr);
    Serial.print("objStr = ");
    Serial.println(objStr);
    saveSetting(objStr, settingBrightFileName);
    matrixBright = brightVal;
    esp8266_server.send(200, "text/plain"); //向客户端发送200响应信息
}

//设置长按回调
void setup()
{
    //初始化串行接口
    Serial.begin(9600);

    //初始化矩阵
    funcMatrix.initMatrix(matrixBright);
    //显示WIFI设置界面
    funcMatrix.setCursor(1, 0);
    funcMatrix.setTextColor(displayer.getColorObjByInt(BLUE));
    funcMatrix.print("WIFI...");
    funcMatrix.show();
    //WIFI连接
    WiFiManager wifiManager;
    if (!wifiManager.autoConnect("PIXEL-CLOCK"))
    {
        Serial.println("failed to connect and hit timeout");
        delay(3000);
        ESP.reset();
        delay(3000);
    }
    // 启动SPIFFS
    SPIFFS.begin();
    //读取配置文件
    readSettingJson();
    //没设置key 不显示天气
    if (funcWeather.reqUserKey == "")
    {
        shouldShowWeather = false;
    }
    //设置按钮长按触发时间和回调
    oneBtn.attachDoubleClick(handleDoubleClick);
    // 告知系统如何处理用户请求
    esp8266_server.onNotFound(handleUserRequet);
    esp8266_server.on("/settingSubmit", handSettingSubmit);
    esp8266_server.on("/setBrightness", handleBrightness); // 处理PWM设置请求

    //显示完成设置的动画
    funcMatrix.fillScreen(0);
    funcMatrix.setTextWrap(false);
    //paceman动画
    displayer.displayAnimationPacman(funcMatrix);
    //设置字体用于显示数字
    funcMatrix.setFont(&ApcFont35);
    //初始化NTP网络时间模块
    funcNtp.init();
    //初始化DHT11
    funcDHT11.init();
    //初始化时钟
    rtc.Begin();
    //检测RTC是否处于运行状态
    if (rtc.GetIsRunning() != true)
    {
        Serial.println("RTC IS NOT RUNNING!");
    }
    //获取当日天气预报数据
    funcWeather.getWeatherFromInternet();
    //获取网络时间设置给RTC模块
    RtcDateTime newDt(funcNtp.getNTPTimeSeconds() - epochTime2000);
    rtc.SetDateTime(newDt);
}

uint8_t showContentType = 1;                     //当前要显示的内容类型
uint8_t maxTypeCount = 7;                        //最多显示的种类
unsigned long startLoopSecond = millis() / 1000; //每个loop开始的时间
int settingCursorX = funcMatrix.width();
unsigned long weatherRefreshTime = millis() / 1000;

RtcDateTime rtcNow = rtc.GetDateTime();
char showHour = rtcNow.Hour();
char showMin = rtcNow.Minute();
char showSec = rtcNow.Second();
char showDayOfWeek = funcNtp.getDayOfWeek();
unsigned long rtcRefreshTime = millis();

void loop()
{
    delay(100);
    oneBtn.tick();
    if (isSettingMode)
    {
        //设置模式 开启AP热点 开启服务器模式 让用户进入网页进行设置
        esp8266_server.handleClient(); // 处理用户请求
        funcMatrix.fillScreen(0);
        funcMatrix.setCursor(settingCursorX, martrixOffsetY); //滚动显示IP
        funcMatrix.print(WiFi.localIP().toString());
        if (--settingCursorX < -50)
        {
            settingCursorX = funcMatrix.width();
        }
        funcMatrix.show();
        delay(60);
    }
    else
    {
        long millNum = millis();
        //天气和网络时间刷新逻辑
        long weatherRefreshGap = (millNum / 1000) - weatherRefreshTime;
        if (weatherRefreshGap > 60)
        {
            weatherRefreshTime = millNum / 1000;
            unsigned long ntpTime=funcNtp.getNTPTimeSeconds();
            RtcDateTime newDt(ntpTime - epochTime2000);
            rtc.SetDateTime(newDt);
            newDt = NULL;
            Serial.print(ntpTime);
            Serial.println("正在刷新网络时间...");
            funcWeather.getWeatherFromInternet();
            Serial.println("正在刷新天气信息...");
        }

        //切换显示内容条件：已经到达了要求显示的持续时间 或者mills已经溢出 显示下一个类型
        long gapSec = (millNum / 1000) - startLoopSecond;
        if (gapSec >= lastShowTime || gapSec < 0)
        {
            startLoopSecond = millis() / 1000;
            showContentType++;
        }
        if (showContentType > maxTypeCount)
            showContentType = 1;

        switch (showContentType)
        {
        case 1:
        { //时间必须显示
            //获取当前时间 并格式化后显示到屏幕
            long rtcGap =millis() - rtcRefreshTime;
            if (rtcGap > 900)
            {   //每超过900毫秒获取一次时间
                RtcDateTime rtcNow = rtc.GetDateTime();
                showHour = rtcNow.Hour();
                showMin = rtcNow.Minute();
                showSec = rtcNow.Second();
                showDayOfWeek = funcNtp.getDayOfWeek();
                rtcRefreshTime=millis();
                rtcNow=NULL;
            }
            displayer.displayTime(funcMatrix, showDayOfWeek, showHour, showMin, showSec);
            delay(100);
            break;
        }
        case 2:
        { //天气显示逻辑
            if (shouldShowWeather)
            {
                displayer.displayWeather(funcMatrix, funcWeather);
            }
            else
            {
                showContentType++;
            }
            break;
        }
        case 3:
        { //温度显示逻辑
            if (shouldShowTemperature)
            {
                displayer.displayTemperature(funcMatrix, funcDHT11);
            }
            else
            {
                showContentType++;
            }
            break;
        }
        case 4:
        { //湿度显示逻辑
            if (shouldShowHumidity)
            {
                displayer.displayHumidity(funcMatrix, funcDHT11);
            }
            else
            {
                showContentType++;
            }
            break;
        }
        case 5:
        { //股票显示逻辑
            if (shouldShowStock)
            {
                displayer.displayStockPrice(funcMatrix, displayer.stockCode1, stockColorArr, stockPixels);
            }
            else
            {
                showContentType++;
            }
            break;
        }
        case 6:
        { //股票显示逻辑
            if (shouldShowStock2)
            {
                displayer.displayStockPrice(funcMatrix, displayer.stockCode2, stock2ColorArr, stock2Pixels);
            }
            else
            {
                showContentType++;
            }
            break;
        }
        case 7:
        { //b站粉丝
            if (shouldShowBiliFans)
            {
                displayer.displayBili(funcMatrix);
            }
            else
            {
                showContentType++;
            }
            break;
        }
        }
    }
}
//处理用户提交的显示设置信息
void handSettingSubmit()
{
    String returnMsg = "设置成功,连续点击三次设置按钮可回到显示模式~";
    // 从浏览器发送的信息中获取PWM控制数值（字符串格式）
    String weatherKey = esp8266_server.arg("weatherKey");
    char showWeather = 0;
    char showTemperature = 0;
    char showHumidity = 0;
    char showStock1 = 0;
    char showStock2 = 0;
    char showBili = 0;
    String stockCode1 = esp8266_server.arg("stockCode1");
    String stockCode2 = esp8266_server.arg("stockCode2");
    String weatherCity = esp8266_server.arg("city");
    String biliUid = esp8266_server.arg("bilibili");
    Serial.print("biliUid:");
    Serial.println(biliUid);
    if (esp8266_server.arg("showWeather") == "1")
    {
        Serial.println("showWeather:1");
        if ((weatherKey == "" && funcWeather.reqUserKey == "") || (weatherCity == "" && funcWeather.reqLocation == ""))
        {
            returnMsg = "保存失败,天气显示需要设置城市和心知天气私钥,心知天气官网:https://www.seniverse.com/";
            esp8266_server.send(200, "text/html;charset=utf8", "<p>" + returnMsg + "</p>");
            return;
        }
        showWeather = 1;
    }
    if (esp8266_server.arg("showTemperature") == "1")
    {
        showTemperature = 1;
    }
    if (esp8266_server.arg("showHumidity") == "1")
    {
        showHumidity = 1;
    }
    if (esp8266_server.arg("showStock1") == "1")
    {
        if (stockCode1 == "" && displayer.stockCode1 == "")
        {
            returnMsg = "保存失败,显示股票价格需要设置股票代码";
            esp8266_server.send(200, "text/html;charset=utf8", "<p>" + returnMsg + "</p>");
            return;
        }
        showStock1 = 1;
    }
    if (esp8266_server.arg("showStock2") == "1")
    {
        if (stockCode2 == "" && displayer.stockCode2 == "")
        {
            returnMsg = "保存失败,显示股票价格需要设置股票代码";
            esp8266_server.send(200, "text/html;charset=utf8", "<p>" + returnMsg + "</p>");
            return;
        }
        showStock2 = 1;
    }
    if (esp8266_server.arg("showBili") == "1")
    {
        if (biliUid == "" && displayer.biliUID == "")
        {
            returnMsg = "保存失败,显示B站粉丝需要设置UID";
            esp8266_server.send(200, "text/html;charset=utf8", "<p>" + returnMsg + "</p>");
            return;
        }
        showBili = 1;
    }
    //生成json 保存到文件系统
    char objStr[1024] = "";
    DynamicJsonDocument saveDoc(1024);
    saveDoc["showWeather"] = showWeather;
    saveDoc["showTemperature"] = showTemperature;
    saveDoc["showHumidity"] = showHumidity;
    saveDoc["showStock1"] = showStock1;
    saveDoc["showStock2"] = showStock2;
    saveDoc["showBili"] = showBili;

    //防止输入的空值覆盖
    if (stockCode1 == "")
    {
        saveDoc["stockCode1"] = displayer.stockCode1;
    }
    else
    {
        saveDoc["stockCode1"] = stockCode1;
    }
    if (stockCode2 == "")
    {
        saveDoc["stockCode2"] = displayer.stockCode2;
    }
    else
    {
        saveDoc["stockCode2"] = stockCode2;
    }
    if (weatherKey == "")
    {
        saveDoc["weatherKey"] = funcWeather.reqUserKey;
    }
    else
    {
        saveDoc["weatherKey"] = weatherKey;
    }
    if (weatherCity == "")
    {
        saveDoc["weatherCity"] = funcWeather.reqLocation;
    }
    else
    {
        saveDoc["weatherCity"] = weatherCity;
    }
    if (biliUid == "")
    {
        saveDoc["biliUid"] = displayer.biliUID;
    }
    else
    {
        saveDoc["biliUid"] = biliUid;
    }
    serializeJson(saveDoc, objStr);
    saveSetting(objStr, settingFileName);
    Serial.println(objStr);
    //读取配置文件
    readSettingJson();
    // 建立基本网页信息显示当前数值以及返回链接
    esp8266_server.send(200, "text/html;charset=utf8", "<p>" + returnMsg + "</p>");
    handleDoubleClick();
}
//保存json到文件
void saveSetting(char *objStr, String fileName)
{
    File dataFile = SPIFFS.open(fileName, "w"); // 建立File对象用于向SPIFFS中的file对象（即/notes.txt）写入信息
    dataFile.println(objStr);                   // 向dataFile写入字符串信息
    dataFile.flush();
    dataFile.close(); // 完成文件写入后关闭文件
}
bool handleFileRead(String path)
{ //处理浏览器HTTP访问
    if (path.endsWith("/"))
    { // 如果访问地址以"/"为结尾
        path = "/index.html";
    }
    String contentType = getContentType(path); // 获取文件类型
    if (SPIFFS.exists(path))
    {                                                 // 如果访问的文件可以在SPIFFS中找到
        File file = SPIFFS.open(path, "r");           // 则尝试打开该文件
        esp8266_server.streamFile(file, contentType); // 并且将该文件返回给浏览器
        file.close();                                 // 并且关闭文件
        return true;                                  // 返回true
    }
    return false; // 如果文件未找到，则返回false
}
// 获取文件类型
String getContentType(String filename)
{
    if (filename.endsWith(".htm"))
        return "text/html";
    else if (filename.endsWith(".html"))
        return "text/html";
    else if (filename.endsWith(".css"))
        return "text/css";
    else if (filename.endsWith(".js"))
        return "application/javascript";
    else if (filename.endsWith(".png"))
        return "image/png";
    else if (filename.endsWith(".gif"))
        return "image/gif";
    else if (filename.endsWith(".jpg"))
        return "image/jpeg";
    else if (filename.endsWith(".ico"))
        return "image/x-icon";
    else if (filename.endsWith(".xml"))
        return "text/xml";
    else if (filename.endsWith(".pdf"))
        return "application/x-pdf";
    else if (filename.endsWith(".zip"))
        return "application/x-zip";
    else if (filename.endsWith(".gz"))
        return "application/x-gzip";
    return "text/plain";
}

// 处理用户浏览器的HTTP访问
void handleUserRequet()
{
    // 获取用户请求网址信息
    String webAddress = esp8266_server.uri();
    // 通过handleFileRead函数处处理用户访问
    bool fileReadOK = handleFileRead(webAddress);
    // 如果在SPIFFS无法找到用户访问的资源，则回复404 (Not Found)
    if (!fileReadOK)
    {
        esp8266_server.send(404, "text/plain", "404 Not Found");
    }
}
