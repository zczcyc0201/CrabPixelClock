#ifndef __FUNC_NEOPIXEL_MATRIX__
#define __FUNC_NEOPIXEL_MATRIX__
//导入所需要的库
#include <Adafruit_NeoMatrix.h>
#include "Fonts/ApcFont35.h"

//自定义显示区域工具类 继承Adafruit_NeoMatrix
class FuncNeoPixelMatrix : public Adafruit_NeoMatrix
{
public:
    int offsetX = 0;
    int offsetY = 0;
    int areaWidth = 32;
    int areaHeigh = 8;
    //构造函数 需要显式调用父类构造方法 tileX代表X轴显示区域的数量 tileY代表Y轴显示区域的数量
    FuncNeoPixelMatrix(uint8_t matrixWidth, uint8_t matrixHeight,
               uint8_t tileX, uint8_t tileY, uint8_t pin,
               uint8_t matrixType, neoPixelType ledType = NEO_GRB + NEO_KHZ800)
        : Adafruit_NeoMatrix(matrixWidth, matrixHeight, tileX,
                             tileY, pin, matrixType, ledType) {}
    void initMatrix(unsigned char brightness);
    //显示像素
    void drawPixel(int16_t x, int16_t y, uint16_t color);
    //显示一批像素
    void drawColorMap(unsigned long *colorMap, unsigned char width,
                      unsigned char height, unsigned char *pixels, char xStartPosition = 0, char yStartPosition = 0);
   
};
#endif