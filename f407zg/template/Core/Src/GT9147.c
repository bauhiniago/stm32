//
// Created by wbh on 2021/2/14.
// 移植自正点原子HAL库实例
//

#include "GT9147.h"
//#include "myiic.h"
#include <stdio.h>

_m_touchdev touchdev=
{
  TP_Init,
  TP_Scan,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
  0,
};
static uint32_t fac_us=0;
void delay_us(uint32_t nus) {
  uint32_t ticks;
  uint32_t told, tnow, tcnt = 0;
  uint32_t reload = SysTick->LOAD;        //LOAD的值
  ticks = nus * fac_us;            //需要的节拍数
  told = SysTick->VAL;                //刚进入时的计数器值
  while (1) {
    tnow = SysTick->VAL;
    if (tnow != told) {
      if (tnow < told)tcnt += told - tnow;  //这里注意一下SYSTICK是一个递减的计数器就可以了.
      else tcnt += reload - tnow + told;
      told = tnow;
      if (tcnt >= ticks)break;      //时间超过/等于要延迟的时间,则退出.
    }
  };
}

//默认为touchtype=0的数据.
uint8_t CMD_RDX=0XD0;
uint8_t CMD_RDY=0X90;
const uint8_t GT9147_CFG_TBL[]=
{
        0X60,0XE0,0X01,0X20,0X03,0X05,0X35,0X00,0X02,0X08,
        0X1E,0X08,0X50,0X3C,0X0F,0X05,0X00,0X00,0XFF,0X67,
        0X50,0X00,0X00,0X18,0X1A,0X1E,0X14,0X89,0X28,0X0A,
        0X30,0X2E,0XBB,0X0A,0X03,0X00,0X00,0X02,0X33,0X1D,
        0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X32,0X00,0X00,
        0X2A,0X1C,0X5A,0X94,0XC5,0X02,0X07,0X00,0X00,0X00,
        0XB5,0X1F,0X00,0X90,0X28,0X00,0X77,0X32,0X00,0X62,
        0X3F,0X00,0X52,0X50,0X00,0X52,0X00,0X00,0X00,0X00,
        0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
        0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X0F,
        0X0F,0X03,0X06,0X10,0X42,0XF8,0X0F,0X14,0X00,0X00,
        0X00,0X00,0X1A,0X18,0X16,0X14,0X12,0X10,0X0E,0X0C,
        0X0A,0X08,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
        0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
        0X00,0X00,0X29,0X28,0X24,0X22,0X20,0X1F,0X1E,0X1D,
        0X0E,0X0C,0X0A,0X08,0X06,0X05,0X04,0X02,0X00,0XFF,
        0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,0X00,
        0X00,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,0XFF,
        0XFF,0XFF,0XFF,0XFF,
};
const uint16_t GT9147_TPX_TBL[5]={GT_TP1_REG,GT_TP2_REG,GT_TP3_REG,GT_TP4_REG,GT_TP5_REG};
uint8_t GT9147_Init(void)
{

  uint8_t temp[5];

  GPIO_InitTypeDef GPIO_Initure;
	
	__HAL_RCC_GPIOB_CLK_ENABLE();			//开启GPIOB时钟
  __HAL_RCC_GPIOC_CLK_ENABLE();			//开启GPIOC时钟

  //PB1
  GPIO_Initure.Pin=GPIO_PIN_1;           	//PB1设置为上拉输入
  GPIO_Initure.Mode=GPIO_MODE_INPUT;      //输入
  GPIO_Initure.Pull=GPIO_PULLUP;          //上拉
  GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
  HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //初始化
          
  //PC13
  GPIO_Initure.Pin=GPIO_PIN_13;          	//PC13设置为推挽输出
  GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //推挽输出
  HAL_GPIO_Init(GPIOC,&GPIO_Initure);     //初始化

  CT_IIC_Init();      	//初始化电容屏的I2C总线
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET);  //复位C13
  HAL_Delay(10);
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);//释放复位
  HAL_Delay(10);

  GPIO_Initure.Pin=GPIO_PIN_1;           	//PB1设置为上拉输入
  GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  //输出模式
  GPIO_Initure.Pull=GPIO_NOPULL;          //无上下拉
  GPIO_Initure.Speed=GPIO_SPEED_HIGH;     //高速
  HAL_GPIO_Init(GPIOB,&GPIO_Initure);     //初始化

  HAL_Delay(100);
  GT9147_RD_Reg(GT_PID_REG,temp,4);	//读取产品ID
  temp[4]=0;
  HAL_Delay(100);
  printf("CTP ID:%s\r\n",temp);		//打印ID
  if(strcmp((char*)temp,"9147")==0)	//ID==9147
  {
    temp[0]=0X02;
    GT9147_WR_Reg(GT_CTRL_REG,temp,1);//软复位GT9147
    GT9147_RD_Reg(GT_CFGS_REG,temp,1);//读取GT_CFGS_REG寄存器
    if(temp[0]<0X60)//默认版本比较低,需要更新flash配置
    {
      printf("Default Ver:%d\r\n",temp[0]);
      GT9147_Send_Cfg(1);//更新并保存配置
    }
    HAL_Delay(10);
    temp[0]=0X00;
    GT9147_WR_Reg(GT_CTRL_REG,temp,1);	//结束复位
    return 0;
  }
  return 0;
}uint8_t GT9147_WR_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
  uint8_t i;
  uint8_t ret=0;
  CT_IIC_Start();
  CT_IIC_Send_Byte(GT_CMD_WR);   	//发送写命令
  CT_IIC_Wait_Ack();
  CT_IIC_Send_Byte(reg>>8);   	//发送高8位地址
  CT_IIC_Wait_Ack();
  CT_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
  CT_IIC_Wait_Ack();
  for(i=0;i<len;i++)
  {
    CT_IIC_Send_Byte(buf[i]);  	//发数据
    ret=CT_IIC_Wait_Ack();
    if(ret)break;
  }
  CT_IIC_Stop();					//产生一个停止条件
  return ret;
}
void GT9147_RD_Reg(uint16_t reg,uint8_t *buf,uint8_t len)
{
  uint8_t i;
  CT_IIC_Start();
  CT_IIC_Send_Byte(GT_CMD_WR);   //发送写命令
  CT_IIC_Wait_Ack();
  CT_IIC_Send_Byte(reg>>8);   	//发送高8位地址
  CT_IIC_Wait_Ack();
  CT_IIC_Send_Byte(reg&0XFF);   	//发送低8位地址
  CT_IIC_Wait_Ack();
  CT_IIC_Start();
  CT_IIC_Send_Byte(GT_CMD_RD);   //发送读命令
  CT_IIC_Wait_Ack();
  for(i=0;i<len;i++)
  {
    buf[i]=CT_IIC_Read_Byte(i==(len-1)?0:1); //发数据
    //printf("%d\r\n",buf[i]);
  }
  CT_IIC_Stop();//产生一个停止条件
}
uint8_t GT9147_Send_Cfg(uint8_t mode)
{
  uint8_t buf[2];
  uint8_t i=0;
  buf[0]=0;
  buf[1]=mode;	//是否写入到GT9147 FLASH?  即是否掉电保存
  for(i=0;i<sizeof(GT9147_CFG_TBL);i++)buf[0]+=GT9147_CFG_TBL[i];//计算校验和
  buf[0]=(~buf[0])+1;
  GT9147_WR_Reg(GT_CFGS_REG,(uint8_t*)GT9147_CFG_TBL,sizeof(GT9147_CFG_TBL));//发送寄存器配置
  GT9147_WR_Reg(GT_CHECK_REG,buf,2);//写入校验和,和配置更新标记
  return 0;
}
uint8_t GT9147_Scan(uint8_t mode)
{
  uint8_t buf[4];
  uint8_t i=0;
  uint8_t res=0;
  uint8_t temp;
  uint8_t tempsta;
  static uint8_t t=0;//控制查询间隔,从而降低CPU占用率
  t++;
  if((t%10)==0||t<10)//空闲时,每进入10次CTP_Scan函数才检测1次,从而节省CPU使用率
  {
    GT9147_RD_Reg(GT_GSTID_REG,&mode,1);	//读取触摸点的状态
    if(mode&0X80&&((mode&0XF)<6))
    {
      temp=0;
      GT9147_WR_Reg(GT_GSTID_REG,&temp,1);//清标志
    }
    if((mode&0XF)&&((mode&0XF)<6))
    {
      temp=0XFF<<(mode&0XF);		//将点的个数转换为1的位数,匹配touchdev.sta定义
      tempsta=touchdev.sta;			//保存当前的touchdev.sta值
      touchdev.sta=(~temp)|TP_PRES_DOWN|TP_CATH_PRES;
      touchdev.x[4]=touchdev.x[0];	//保存触点0的数据
      touchdev.y[4]=touchdev.y[0];
      for(i=0;i<5;i++)
      {
        if(touchdev.sta&(1<<i))	//触摸有效?
        {
          GT9147_RD_Reg(GT9147_TPX_TBL[i],buf,4);	//读取XY坐标值
          if(touchdev.touchtype&0X01)//横屏
          {
            touchdev.y[i]=((uint16_t)buf[1]<<8)+buf[0];
            touchdev.x[i]=800-(((uint16_t)buf[3]<<8)+buf[2]);
          }else
          {
            touchdev.x[i]=((uint16_t)buf[1]<<8)+buf[0];
            touchdev.y[i]=((uint16_t)buf[3]<<8)+buf[2];
          }
          //printf("x[%d]:%d,y[%d]:%d\r\n",i,touchdev.x[i],i,touchdev.y[i]);
        }
      }
      res=1;
      if(touchdev.x[0]>lcddev.width||touchdev.y[0]>lcddev.height)//非法数据(坐标超出了)
      {
        if((mode&0XF)>1)		//有其他点有数据,则复第二个触点的数据到第一个触点.
        {
          touchdev.x[0]=touchdev.x[1];
          touchdev.y[0]=touchdev.y[1];
          t=0;				//触发一次,则会最少连续监测10次,从而提高命中率
        }else					//非法数据,则忽略此次数据(还原原来的)
        {
          touchdev.x[0]=touchdev.x[4];
          touchdev.y[0]=touchdev.y[4];
          mode=0X80;
          touchdev.sta=tempsta;	//恢复touchdev.sta
        }
      }else t=0;					//触发一次,则会最少连续监测10次,从而提高命中率
    }
  }
  if((mode&0X8F)==0X80)//无触摸点按下
  {
    if(touchdev.sta&TP_PRES_DOWN)	//之前是被按下的
    {
      touchdev.sta&=~(1<<7);	//标记按键松开
    }else						//之前就没有被按下
    {
      touchdev.x[0]=0xffff;
      touchdev.y[0]=0xffff;
    }
  }
  if(t>240)t=10;//重新从10开始计数

  return res;
}
void TP_Write_Byte(uint8_t num)
{
  uint8_t count=0;
  for(count=0;count<8;count++)
  {
    if(num&0x80)HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_SET);
    else HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_RESET);
    num<<=1;
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
    delay_us(1);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);	//上升沿有效
  }
}
uint8_t TP_Init(void)
{
  if(lcddev.id==0X5510)				//4.3寸电容触摸屏
  {
    if(GT9147_Init()==0)			//是GT9147
    {

      touchdev.scan=GT9147_Scan;	//扫描函数指向GT9147触摸屏扫描
    }else
    {
      printf("unkonw dev!\r\n");
    }
    touchdev.touchtype|=0X80;			//电容屏
    touchdev.touchtype|=lcddev.dir&0X01;//横屏还是竖屏
    return 0;
  }else {
    printf("unkonw dev!\r\n");
    return 1;
  }
}
uint8_t TP_Scan(uint8_t tp)
{
  if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_10)==GPIO_PIN_RESET){//有按键按下
    if(tp)TP_Read_XY2(&touchdev.x[0],&touchdev.y[0]);//读取物理坐标
    else if(TP_Read_XY2(&touchdev.x[0],&touchdev.y[0]))//读取屏幕坐标
    {
      touchdev.x[0]=touchdev.xfac*touchdev.x[0]+touchdev.xoff;//将结果转换为屏幕坐标
      touchdev.y[0]=touchdev.yfac*touchdev.y[0]+touchdev.yoff;
    }
    if((touchdev.sta&TP_PRES_DOWN)==0)//之前没有被按下
    {
      touchdev.sta=TP_PRES_DOWN|TP_CATH_PRES;//按键按下
      touchdev.x[4]=touchdev.x[0];//记录第一次按下时的坐标
      touchdev.y[4]=touchdev.y[0];
    }
  }else
  {
    if(touchdev.sta&TP_PRES_DOWN)//之前是被按下的
    {
      touchdev.sta&=~(1<<7);//标记按键松开
    }else//之前就没有被按下
    {
      touchdev.x[4]=0;
      touchdev.y[4]=0;
      touchdev.x[0]=0xffff;
      touchdev.y[0]=0xffff;
    }
  }
  return touchdev.sta&TP_PRES_DOWN;//返回当前的触屏状态
}
uint16_t TP_Read_AD(uint8_t CMD)
{
  uint8_t count=0;
  uint16_t Num=0;
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);		//先拉低时钟
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_RESET); 	//拉低数据线
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_RESET); 		//选中触摸屏IC
  TP_Write_Byte(CMD);//发送命令字
  delay_us(6);//ADS7846的转换时间最长为6us
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
  delay_us(1);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);		//给1个时钟，清除BUSY
  delay_us(1);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
  for(count=0;count<16;count++)//读出16位数据,只有高12位有效
  {
    Num<<=1;
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);	//下降沿有效
    delay_us(1);
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
    if(HAL_GPIO_ReadPin(GPIOB,GPIO_PIN_2))Num++;
  }
  Num>>=4;   	//只有高12位有效.
  HAL_GPIO_WritePin(GPIOC,GPIO_PIN_13,GPIO_PIN_SET);		//释放片选
  return(Num);
}

#define READ_TIMES 5 	//读取次数
#define LOST_VAL 1	  	//丢弃值
uint16_t TP_Read_XOY(uint8_t xy)
{
  uint16_t i, j;
  uint16_t buf[READ_TIMES];
  uint16_t sum=0;
  uint16_t temp;
  for(i=0;i<READ_TIMES;i++)buf[i]=TP_Read_AD(xy);
  for(i=0;i<READ_TIMES-1; i++)//排序
  {
    for(j=i+1;j<READ_TIMES;j++)
    {
      if(buf[i]>buf[j])//升序排列
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  sum=0;
  for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
  temp=sum/(READ_TIMES-2*LOST_VAL);
  return temp;
}
uint8_t TP_Read_XY(uint16_t *x,uint16_t *y)
{
  uint16_t xtemp,ytemp;
  xtemp=TP_Read_XOY(CMD_RDX);
  ytemp=TP_Read_XOY(CMD_RDY);
  //if(xtemp<100||ytemp<100)return 0;//读数失败
  *x=xtemp;
  *y=ytemp;
  return 1;//读数成功
}
#define ERR_RANGE 50 //误差范围
uint8_t TP_Read_XY2(uint16_t *x,uint16_t *y)
{
  uint16_t x1,y1;
  uint16_t x2,y2;
  uint8_t flag;
  flag=TP_Read_XY(&x1,&y1);
  if(flag==0)return(0);
  flag=TP_Read_XY(&x2,&y2);
  if(flag==0)return(0);
  if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//前后两次采样在+-50内
     &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
  {
    *x=(x1+x2)/2;
    *y=(y1+y2)/2;
    return 1;
  }else return 0;
}


void CT_Delay(void)
{
  delay_us(5);
}
//电容触摸芯片IIC接口初始化
void CT_IIC_Init(void)
{
  GPIO_InitTypeDef GPIO_Initure;

  __HAL_RCC_GPIOB_CLK_ENABLE();           	//开启GPIOB时钟
  __HAL_RCC_GPIOF_CLK_ENABLE();           	//开启GPIOF时钟
  __HAL_RCC_GPIOC_CLK_ENABLE();           	//开启GPIOF时钟

  GPIO_Initure.Pin=GPIO_PIN_0; 				//PB0
  GPIO_Initure.Mode=GPIO_MODE_OUTPUT_PP;  	//推挽输出
  GPIO_Initure.Pull=GPIO_PULLUP;          	//上拉
  GPIO_Initure.Speed=GPIO_SPEED_FREQ_HIGH;    //高速
  HAL_GPIO_Init(GPIOB,&GPIO_Initure);

  GPIO_Initure.Pin=GPIO_PIN_11; 				//PF11
  HAL_GPIO_Init(GPIOF,&GPIO_Initure);

  GPIO_Initure.Pin=GPIO_PIN_13; 				//PC13
  HAL_GPIO_Init(GPIOC,&GPIO_Initure);
  
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
  
}
//产生IIC起始信号
void CT_IIC_Start(void)
{
  CT_SDA_OUT();     //sda线输出
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
  delay_us(30);
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_RESET);//START:when CLK is high,DATA change form high to low
  CT_Delay();
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);//钳住I2C总线，准备发送或接收数据
}
//产生IIC停止信号
void CT_IIC_Stop(void)
{
  CT_SDA_OUT();//sda线输出
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
  delay_us(30);
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_RESET);//STOP:when CLK is high DATA change form low to high
  CT_Delay();
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_SET);//发送I2C总线结束信号
}
//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
uint8_t CT_IIC_Wait_Ack(void)
{
  uint8_t ucErrTime=0;
  CT_SDA_IN();      //SDA设置为输入
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_SET);
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
  CT_Delay();
  while(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_11)==GPIO_PIN_SET)
  {
    ucErrTime++;
    if(ucErrTime>250)
    {
      CT_IIC_Stop();
      return 1;
    }
    CT_Delay();
  }
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);//时钟输出0
  return 0;
}
//产生ACK应答
void CT_IIC_Ack(void)
{
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
  CT_SDA_OUT();
  CT_Delay();
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_RESET);
  CT_Delay();
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
  CT_Delay();
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
}
//不产生ACK应答
void CT_IIC_NAck(void)
{
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
  CT_SDA_OUT();
  CT_Delay();
  HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,GPIO_PIN_SET);
  CT_Delay();
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
  CT_Delay();
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
}
//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答
void CT_IIC_Send_Byte(uint8_t txd)
{
  uint8_t t;
  CT_SDA_OUT();
  HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);//拉低时钟开始数据传输
  CT_Delay();
  for(t=0;t<8;t++)
  {
    HAL_GPIO_WritePin(GPIOF,GPIO_PIN_11,(txd&0x80)>>7);
    txd<<=1;
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
    CT_Delay();
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
    CT_Delay();
  }
}
//读1个字节，ack=1时，发送ACK，ack=0，发送nACK
uint8_t CT_IIC_Read_Byte(unsigned char ack)
{
  uint8_t i,receive=0;
  CT_SDA_IN();//SDA设置为输入
  delay_us(30);
  for(i=0;i<8;i++ )
  {
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET);
    CT_Delay();
    HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET);
    receive<<=1;
    if(HAL_GPIO_ReadPin(GPIOF,GPIO_PIN_11)==GPIO_PIN_SET)receive++;
  }
  if (!ack)CT_IIC_NAck();//发送nACK
  else CT_IIC_Ack(); //发送ACK
  return receive;
}
