#include "FuncNeoPixelMatrix.h"
void FuncNeoPixelMatrix::initMatrix(unsigned char brightness)
{
    begin();
    setTextWrap(false);
    setBrightness(brightness);
    setTextColor(Color(255, 255, 255));
}
//重写父类drawPixel函数
void FuncNeoPixelMatrix::drawPixel(int16_t x, int16_t y, uint16_t color)
{
    if (x < 0 || y < 0 || x >= areaWidth || y >= areaHeigh)
    {
        return;
    }
    Adafruit_NeoMatrix::drawPixel(offsetX + x, offsetY + y, color);
}

void FuncNeoPixelMatrix::drawColorMap(unsigned long *colorMap, unsigned char cols,
                                      unsigned char rows, unsigned char *pixels, char xStartPosition, char yStartPosition)
{
    for (int x = 0; x < cols; x++)
    {
        for (int y = 0; y < rows; y++)
        {
            //计算要绘制的像素位置
            int pixelIndex = y * cols + x;
            //计算该位置的颜色
            unsigned long colorValue = colorMap[pixels[pixelIndex]];
            //绘制 颜色值转换并显示
            this->drawPixel(x + xStartPosition, y + yStartPosition, this->Color(colorValue >> 16 & 0xFF, colorValue >> 8 & 0xFF, colorValue & 0xFF));
        }
    }
}
