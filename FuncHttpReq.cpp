#include "FuncHttpReq.h"

String FuncHttpReq::getSplitString(String data, char separator, int index)
{
  int maxIndex = data.length() - 1;
  int j = 0;
  String chunkVal = "";

  for (int i = 0; i <= maxIndex && j <= index; i++)
  {
    chunkVal.concat(data[i]);

    if (data[i] == separator)
    {
      j++;

      if (j > index)
      {
        chunkVal.trim();
        return chunkVal;
      }

      chunkVal = "";
    }
    else if ((i == maxIndex) && (j < index))
    {
      chunkVal = "";
      return chunkVal;
    }
  }
}

// 发送HTTP请求并且将服务器响应通过串口输出
String FuncHttpReq::getRequest(String url,uint8 timeout)
{
	//创建 HTTPClient 对象
	HTTPClient httpClient;
	//通过begin函数配置请求地址。此处也可以不使用端口号和PATH而单纯的
	httpClient.begin(url);
	httpClient.setTimeout(timeout);
	Serial.print("url: ");
	Serial.println(url);
	//通过GET函数启动连接并发送HTTP请求
	int httpCode = httpClient.GET();
	Serial.print("Send GET request to url: ");
	Serial.println(url);
	//如果服务器响应HTTP_CODE_OK(200)则从服务器获取响应体信息并通过串口输出
	//如果服务器不响应HTTP_CODE_OK(200)则将服务器响应状态码通过串口输出
	if (httpCode == HTTP_CODE_OK)
	{
		// 使用getString函数获取服务器响应体内容
		String responsePayload = httpClient.getString();
		Serial.println("Server Response Payload: ");
		Serial.println(responsePayload);
		httpClient.end();
		return responsePayload;
	}
	else
	{
		Serial.println("Server Respose Code：");
		Serial.println(httpCode);
		httpClient.end();
		return "FAIL";
	}
}