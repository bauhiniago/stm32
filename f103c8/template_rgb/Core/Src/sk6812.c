#include <math.h>
#include "main.h"
#include "tim.h"
#include "sk6812.h"


uint16_t send_Buf[NUM];

void Delay_us(uint32_t t) {
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000000);
    HAL_Delay(t - 1);
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
}

void SK_Load(void)
{
	HAL_TIM_PWM_Start_DMA(&htim2, TIM_CHANNEL_1, (uint32_t *)send_Buf, NUM);
}


void SK_CloseAll(void)
{
	uint16_t i;
	for (i = 0; i < PIXEL_NUM * 24; i++)
		send_Buf[i] = WS0; // 写入逻辑0的占空比
	for (i = PIXEL_NUM * 24; i < NUM; i++)
		send_Buf[i] = 0; // 占空比比为0，全为低电平
	SK_Load();
}
void SK_ReturnColor(uint8_t red,uint8_t green,uint8_t blue,uint8_t * data){
	uint16_t i;
	for (i = 0; i < 8; i++)
	{
		data[i] = ((green & 0x80) ? WS1 : WS0);
		green <<= 1;
	}
    for (i = 0; i < 8; i++)
	{
		data[i+8] = ((red & 0x80) ? WS1 : WS0);
		red <<= 1;
	}
    for (i = 0; i < 8; i++)
	{
		data[i+16] = ((blue & 0x80) ? WS1 : WS0);
		blue <<= 1;
	}

}
void SK_WriteAll_RGB(uint8_t red,uint8_t green,uint8_t blue)
{
	//HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
    uint16_t i,j;
	uint8_t dat[24];
    SK_ReturnColor(red,green,blue,dat);
    for (i = 0; i < PIXEL_NUM; i++)
	{
		for (j = 0; j < 24; j++)
		{
			send_Buf[i * 24 + j] = dat[j];
		}
	}
    SK_Load();
}
void SK_WritePixel(uint8_t n,uint8_t red,uint8_t green,uint8_t blue)
{
	//HAL_TIM_PWM_Stop_DMA(&htim2, TIM_CHANNEL_1);
	uint8_t dat[24];
    SK_ReturnColor(red,green,blue,dat);
	for (int i = 0; i < 24; i++)
	{
		send_Buf[n * 24 + i] = dat[i];
	}
	
	SK_Load();
}
void SK_Breathing_RGB(void) {
	uint16_t red;
  	uint16_t green;
  	uint16_t blue;
	for (red = 0; red < 256; red++)
    {
      SK_WriteAll_RGB(red,0,0);
      HAL_Delay(5);
    }
    for (red = 255; red >0; red--)
    {
      SK_WriteAll_RGB(red,0,0);
      HAL_Delay(5);
    }
    for (green = 0; green < 256; green++)
    {
      SK_WriteAll_RGB(0,green,0);
      HAL_Delay(5);
    }
    for (green = 255; green >0; green--)
    {
      SK_WriteAll_RGB(0,green,0);
      HAL_Delay(5);
    }
    for (blue = 0; blue < 256; blue++)
    {
      SK_WriteAll_RGB(0,0,blue);
      HAL_Delay(5);
    }
    for (blue = 255; blue >0; blue--)
    {
      SK_WriteAll_RGB(0,0,blue);
      HAL_Delay(5);
    }
}
void SK_Watering_RGB(void) {
	for (int i = 0; i <18; i++)
	{
		SK_CloseAll();
		SK_WritePixel(i,15,0,0);
		SK_WritePixel(i+1,0,15,0);
		SK_WritePixel(i+2,0,0,15);
		HAL_Delay(50);
	}
}
/* 阻塞式单灯定时变色，建议配合线程使用 */
void SK_RGBtoRGB(uint8_t r1,uint8_t g1,uint8_t b1,uint8_t r2,uint8_t g2,uint8_t b2,uint32_t time,uint16_t led){
	uint16_t temp;
	if(r1==r2&&g1==g2&&b1==b2){
		return;
	}
	if (r1!=r2&&g1==g2&&b1==b2||r1==r2&&g1!=g2&&b1==b2||r1==r2&&g1==g2&&b1!=b2)
	{
		if(r1!=r2){
			for (int i = r1; (r1<r2)?(i <= r2):(i >= r2); (r1<r2)?(i++):(i--))
			{
				
				SK_WritePixel(led,i,g1,b1);
			
				HAL_Delay(time/abs(r1-r2));
			}
		}
		else if(g1!=g2){
			for (int i = g1;  (g1<g2)?(i <= g2):(i >= g2); (g1<g2)?(i++):(i--))
			{
				
				SK_WritePixel(led,r1,i,b1);
				
				HAL_Delay(time/abs(g1-g2));
			}
		}
		else if(b1!=b2){
			for (int i = b1; (b1<b2)?(i <= b2):(i >= b2); (b1<b2)?(i++):(i--))
			{
				
				SK_WritePixel(led,r1,g1,i);
				
				HAL_Delay(time/abs(b1-b2));
			}
		}
	}
	if (r1!=r2&&g1!=g2&&b1==b2||r1!=r2&&g1==g2&&b1!=b2||r1==r2&&g1!=g2&&b1!=b2)
	{
		if(r1==r2){
			for (int i = g1; (g1<g2)?(i <= g2):(i >= g2); (g1<g2)?(i++):(i--))
			{
				
				SK_WritePixel(led,r1,i,b1+(b2-b1)/abs(g1-g2)*i);
				
				HAL_Delay(time/abs(g1-g2));
			}
		}
		else if(g1==g2){
			for (int i = b1; (b1<b2)?(i <= b2):(i >= b2); (b1<b2)?(i++):(i--))
			{
				
				SK_WritePixel(led,r1+(r2-r1)/abs(b1-b2)*i,g1,i);
				
				HAL_Delay(time/abs(b1-b2));
			}
		}
		else if(b1==b2){
			for (int i = r1; (r1<r2)?(i <= r2):(i >= r2); (r1<r2)?(i++):(i--))
			{
				
				SK_WritePixel(led,i,g1+(g2-g1)/abs(r1-r2)*i,b1);
				
				HAL_Delay(time/abs(r1-r2));
			}
		}
	}
	if(r1!=r2&&g1!=g2&&b1!=b2){
		if(abs(r2-r1)>abs(g2-g1)){

		}else{

		}
		
	}
}
void SK_RainbowStep(uint32_t pos,uint16_t led){
	if (pos/256==0)
	{
		//255,0,0 -> 255,165,0 红橙
		uint16_t i=0;
		uint16_t margin=165;
		while (i<256)
		{
			if (i==pos%256)
			{
				SK_WritePixel(led,255,pos%256/256.0*margin,0);
				break;
			}
			i++;
		}
	}
	else if (pos/256==1) 
	{
		//255,165,0 -> 255,255,0 橙黄
		uint16_t i=0;
		uint16_t margin=255-165;
		while (i<256)
		{
			if (i==pos%256)
			{
				SK_WritePixel(led,255,165+pos%256/256.0*margin,0);
				break;
			}
			i++;
		}
	}
	else if (pos/256==2)
	{
		//255,255,0 -> 0,255,0 黄绿
		uint16_t i=0;
		uint16_t margin=255;
		while (i<256)
		{
			if (i==pos%256)
			{
				SK_WritePixel(led,255-pos%256,255,0);
				break;
			}
			i++;
		}
	}
	else if (pos/256==3) 
	{
		//0,255,0 -> 0,255,255 绿青
		uint16_t i=0;
		uint16_t margin=255;
		while (i<256)
		{
			if (i==pos%256)
			{
				SK_WritePixel(led,0,255,pos%256);
				break;
			}
			i++;
		}
	}
	else if (pos/256==4)
	{
		//0,255,255 -> 0,127,255 青 天蓝
		uint16_t i=0;
		uint16_t margin=255-127;
		while (i<256)
		{
			if (i==pos%256)
			{
				SK_WritePixel(led,0,255-pos%256/256.0*margin,255);
				break;
			}
			i++;
		}
	}
	else if (pos/256==5)
	{
		//0,127,255 -> 0,0,255 天蓝 深蓝
		uint16_t i=0;
		uint16_t margin=127;
		while (i<256)
		{
			if (i==pos%256)
			{
				SK_WritePixel(led,0,127-pos%256/256.0*margin,255);
				break;
			}
			i++;
		}
	}
	else if (pos/256==6)
	{
		//0,0,255 -> 135,0,255 蓝靛
		uint16_t i=0;
		uint16_t margin=135;
		while (i<256)
		{
			if (i==pos%256)
			{
				SK_WritePixel(led,pos%256/256.0*margin,0,255);
				break;
			}
			i++;
		}
	}
	else if (pos/256==7) 
	{
		//135,0,255 -> 255,0,255 靛紫
		uint16_t i=0;
		uint16_t margin=255-135;
		while (i<256)
		{
			if (i==pos%256)
			{
				SK_WritePixel(led,135+pos%256/256.0*margin,0,255);
				break;
			}
			i++;
		}
		
	}
	else if (pos/256==8) 
	{
		//255,0,255 -> 255,0,0 紫红
		uint16_t i=0;
		uint16_t margin=255;
		while (i<256)
		{
			if (i==pos%256)
			{
				SK_WritePixel(led,255,0,255-pos%256);
				break;
			}
			i++;
		}
	}
	
}
void SK_Rainbow(uint8_t n){
	uint32_t time=1024;
	for (uint32_t pos = 0; pos < 256*9; pos++)
	{
		for (int j = 0; j < n; j++)
		{
			SK_RainbowStep((pos+255*j/n/2)%(256*9),j);
			//HAL_Delay(1);
			Delay_us(100);
			
		}

	}
}
