//
// Created by wbh on 2021/2/14.
//

#ifndef __GT9147_H__
#define __GT9147_H__

#include "main.h"
#include "fsmc.h"
#include "string.h"

#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2))
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr))
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum))


//////////////////////TOUCH/////////////////////////
#define TP_PRES_DOWN 0x80  		//触屏被按下	  
#define TP_CATH_PRES 0x40  		//有按键按下了 
#define CT_MAX_TOUCH  5    		//电容屏支持的点数,固定为5点

//触摸屏控制器
typedef struct
{
    
    uint8_t (*init)(void);
    //初始化触摸屏控制器
    uint8_t (*scan)(uint8_t);
    //扫描触摸屏.0,屏幕扫描;1,物理坐标;
    void (*adjust)(void);
    //触摸屏校准
    uint16_t x[CT_MAX_TOUCH];
    //当前坐标
    uint16_t y[CT_MAX_TOUCH];
    //电容屏有最多5组坐标,电阻屏则用x[0],y[0]代表:此次扫描时,触屏的坐标,用x[4],y[4]存储第一次按下时的坐标.
    uint8_t  sta;
    //笔的状态
    //b7:按下1/松开0; 
    //b6:0,没有按键按下;1,有按键按下. 
    //b5:保留
    //b4~b0:电容触摸屏按下的点数(0,表示未按下,1表示按下)
    //触摸屏校准参数(电容屏不需要校准)
    float xfac;
    float yfac;
    short xoff;
    short yoff;
    //新增的参数,当触摸屏的左右上下完全颠倒时需要用到.
    //b0:0,竖屏(适合左右为X坐标,上下为Y坐标的TP)
    //   1,横屏(适合左右为Y坐标,上下为X坐标的TP)
    //b1~6:保留.
    //b7:0,电阻屏
    //   1,电容屏
    uint8_t touchtype;
    uint16_t  xt;
    uint16_t  yt;
}_m_touchdev;

extern _m_touchdev touchdev;
//外部声明

//电阻/电容屏芯片连接引脚	   
// #define PEN  		PFin(10)  	//PF10 INT
// #define DOUT 		PBin(2)   	//PB2  MISO
// #define TDIN 		PFout(9)  	//PF9  MOSI
// #define TCLK 		PBout(1)  	//PB1  SCLK
// #define TCS  		PFout(11)  	//PF11  CS 

//电阻屏函数
void TP_Write_Byte(uint8_t num);						//向控制芯片写入一个数据
uint16_t TP_Read_AD(uint8_t CMD);							//读取AD转换值
uint16_t TP_Read_XOY(uint8_t xy);							//带滤波的坐标读取(X/Y)
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y);					//双方向读取(X+Y)
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y);					//带加强滤波的双方向坐标读取
void TP_Drow_Touch_Point(uint16_t x,uint16_t y,uint16_t color);//画一个坐标校准点
void TP_Draw_Big_Point(uint16_t x,uint16_t y,uint16_t color);	//画一个大点
void TP_Save_Adjdata(void);						//保存校准参数
uint8_t TP_Get_Adjdata(void);						//读取校准参数
void TP_Adjust(void);							//触摸屏校准
void TP_Adj_Info_Show(uint16_t x0,uint16_t y0,uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2,uint16_t x3,uint16_t y3,uint16_t fac);//显示校准信息
//电阻屏/电容屏 共用函数
uint8_t TP_Scan(uint8_t tp);								//扫描
uint8_t TP_Init(void);								//初始化

//////////////////////GT9147/////////////////////////

//IO操作函数	   
#define GT_RST    		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_RESET)	//GT9147复位引脚
#define GT_INT    		HAL_GPIO_WritePin(GPIOF,GPIO_PIN_10,GPIO_PIN_RESET)	//GT9147中断引脚

//I2C读写命令	
#define GT_CMD_WR 		0X28     	//写命令
#define GT_CMD_RD 		0X29		//读命令

//GT9147 部分寄存器定义 
#define GT_CTRL_REG 	0X8040   	//GT9147控制寄存器
#define GT_CFGS_REG 	0X8047   	//GT9147配置起始地址寄存器
#define GT_CHECK_REG 	0X80FF   	//GT9147校验和寄存器
#define GT_PID_REG 		0X8140   	//GT9147产品ID寄存器

#define GT_GSTID_REG 	0X814E   	//GT9147当前检测到的触摸情况
#define GT_TP1_REG 		0X8150  	//第一个触摸点数据地址
#define GT_TP2_REG 		0X8158		//第二个触摸点数据地址
#define GT_TP3_REG 		0X8160		//第三个触摸点数据地址
#define GT_TP4_REG 		0X8168		//第四个触摸点数据地址
#define GT_TP5_REG 		0X8170		//第五个触摸点数据地址  

uint8_t GT9147_Send_Cfg(uint8_t mode);
uint8_t GT9147_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
void GT9147_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len);
uint8_t GT9147_Init(void);
uint8_t GT9147_Scan(uint8_t mode);

//////////////////////I2C/////////////////////////
//IO方向设置
#define CT_SDA_IN()  {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=0<<2*11;}	//PF11输入模式
#define CT_SDA_OUT() {GPIOF->MODER&=~(3<<(2*11));GPIOF->MODER|=1<<2*11;} 	//PF11输出模式

//IIC所有操作函数
void CT_IIC_Init(void);                	//初始化IIC的IO口				 
void CT_IIC_Start(void);				//发送IIC开始信号
void CT_IIC_Stop(void);	  				//发送IIC停止信号
void CT_IIC_Send_Byte(uint8_t txd);			//IIC发送一个字节
uint8_t CT_IIC_Read_Byte(unsigned char ack);	//IIC读取一个字节
uint8_t CT_IIC_Wait_Ack(void); 				//IIC等待ACK信号
void CT_IIC_Ack(void);					//IIC发送ACK信号
void CT_IIC_NAck(void);					//IIC不发送ACK信号

#endif 
