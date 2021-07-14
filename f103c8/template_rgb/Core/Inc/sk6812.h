#ifndef __SK6812_H__
#define __SK6812_H__


/* 
pwm time 10us 
rest code >=50us
*/



#define PIXEL_NUM  20
#define NUM (24*PIXEL_NUM + 70)        // Reset 280us / 1.25us = 224
#define WS1 4
#define WS0 1




void SK_Load(void);
void SK_WriteAll_RGB(uint8_t red,uint8_t blue,uint8_t green);
void SK_WritePixel(uint8_t n,uint8_t red,uint8_t blue,uint8_t green);
void SK_ReturnColor(uint8_t red,uint8_t green,uint8_t blue,uint8_t * data);
void SK_CloseAll(void);
void SK_Breathing_RGB(void);
void SK_Watering_RGB(void);
void SK_RGBtoRGB(uint8_t r1,uint8_t g1,uint8_t b1,uint8_t r2,uint8_t g2,uint8_t b2,uint32_t time,uint16_t led);
void SK_RainbowStep(uint32_t pos,uint16_t led);
void Delay_us(uint32_t t) ;
#endif