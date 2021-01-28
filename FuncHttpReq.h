#ifndef __ZCZ_HTTP_FUNC
#define __ZCZ_HTTP_FUNC

#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
class FuncHttpReq
{
public:
    FuncHttpReq(){};
    String getRequest(String url,uint8 timeout=10000);
    String getSplitString(String data, char separator, int index);
};
#endif
