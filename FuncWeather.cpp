#include "FuncWeather.h"
const char *host = "api.seniverse.com"; // 将要连接的服务器地址
const int httpPort = 80;                // 将要连接的服务器端口

// 心知天气HTTP请求所需信息

String reqUnit = "c";                    // 摄氏/华氏

//初始化天气类
void FuncWeather::init()
{
}

void FuncWeather::getWeatherFromInternet()
{
  // 建立心知天气API当前天气请求资源地址
  String reqRes = "/v3/weather/daily.json?key=" + reqUserKey +
                  +"&location=" + reqLocation + "&language=en&unit=" +
                  reqUnit + "&start=0&days=3";

  // 向心知天气服务器服务器请求信息并对信息进行解析
  httpRequest(reqRes);
}

// 向心知天气服务器服务器请求信息并对信息进行解析
void FuncWeather::httpRequest(String reqRes)
{
  WiFiClient client;
  // 建立http请求信息
  String httpRequest = String("GET ") + reqRes + " HTTP/1.1\r\n" +
                       "Host: " + host + "\r\n" +
                       "Connection: close\r\n\r\n";
  // 尝试连接服务器
  if (client.connect(host, 80))
  {
    Serial.println(" Success!");
    // 向服务器发送http请求信息
    client.print(httpRequest);
    Serial.println("Sending request: ");
    Serial.println(httpRequest);
    // 获取并显示服务器响应状态行
    String status_response = client.readStringUntil('\n');
    Serial.print("status_response: ");
    Serial.println(status_response);
    // 使用find跳过HTTP响应头
    if (client.find("\r\n\r\n"))
    {
      Serial.println("Found Header End. Start Parsing.");
    }
    // 利用ArduinoJson库解析心知天气响应信息
    parseInfo(client);
  }
  else
  {
    Serial.println(" connection failed!");
  }
  //断开客户端与服务器连接工作
  client.stop();
}

// 利用ArduinoJson库解析心知天气响应信息
void FuncWeather::parseInfo(WiFiClient client)
{
  const size_t capacity = JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3) + JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(3) + JSON_OBJECT_SIZE(6) + 3 * JSON_OBJECT_SIZE(14) + 860;
  DynamicJsonDocument doc(capacity);
  deserializeJson(doc, client);
  JsonObject results_0 = doc["results"][0];
  JsonArray results_0_daily = results_0["daily"];
  JsonObject results_0_daily_0 = results_0_daily[0];
  const char *results_0_daily_0_date = results_0_daily_0["date"];
  const char *results_0_daily_0_text_day = results_0_daily_0["text_day"];
  const char *results_0_daily_0_code_day = results_0_daily_0["code_day"];
  const char *results_0_daily_0_text_night = results_0_daily_0["text_night"];
  const char *results_0_daily_0_code_night = results_0_daily_0["code_night"];
  const char *results_0_daily_0_high = results_0_daily_0["high"];
  const char *results_0_daily_0_low = results_0_daily_0["low"];
  const char *results_0_daily_0_rainfall = results_0_daily_0["rainfall"];
  const char *results_0_daily_0_precip = results_0_daily_0["precip"];
  const char *results_0_daily_0_wind_direction = results_0_daily_0["wind_direction"];
  const char *results_0_daily_0_wind_direction_degree = results_0_daily_0["wind_direction_degree"];
  const char *results_0_daily_0_wind_speed = results_0_daily_0["wind_speed"];
  const char *results_0_daily_0_wind_scale = results_0_daily_0["wind_scale"];
  const char *results_0_daily_0_humidity = results_0_daily_0["humidity"];

  // 从以上信息中摘选几个通过串口监视器显示
  String results_0_daily_0_date_str = results_0_daily_0["date"].as<String>();
  String results_0_daily_0_text_day_str = results_0_daily_0["text_day"].as<String>();
  int results_0_daily_0_code_day_int = results_0_daily_0["code_day"].as<int>();
  String results_0_daily_0_text_night_str = results_0_daily_0["text_night"].as<String>();
  int results_0_daily_0_code_night_int = results_0_daily_0["code_night"].as<int>();
  int results_0_daily_0_high_int = results_0_daily_0["high"].as<int>();
  int results_0_daily_0_low_int = results_0_daily_0["low"].as<int>();

  //当日最高温最低温
  todayHigh = results_0_daily_0_high_int;
  todayLow = results_0_daily_0_low_int;
  todayCode=results_0_daily_0_code_day_int;
  Serial.println(F("======Today Weahter ======="));
  Serial.print(F("Day Weather: "));
  Serial.print(results_0_daily_0_text_day_str);
  Serial.print(F("Code: "));
  Serial.println(results_0_daily_0_code_day_int);
  Serial.print(F("Night Weather: "));
  Serial.print(results_0_daily_0_text_night_str);
  Serial.print(F(" "));
  Serial.println(results_0_daily_0_code_night_int);
  Serial.print(F("High: "));
  Serial.println(results_0_daily_0_high_int);
  Serial.print(F("LOW: "));
  Serial.println(results_0_daily_0_low_int);
  Serial.println(F("=============================="));
}
