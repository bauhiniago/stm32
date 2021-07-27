#include "main.h"
#include "spi.h"
#include "RC522.h"
#include <stdio.h>
#include <string.h>

// M1卡分为16个扇区，每个扇区由四个块（块0、块1、块2、块3）组成
// 将16个扇区的64个块按绝对地址编号为：0~63
// 第0个扇区的块0（即绝对地址0块），用于存放厂商代码，已经固化不可更改
// 每个扇区的块0、块1、块2为数据块，可用于存放数据
// 每个扇区的块3为控制块（绝对地址为:块3、块7、块11.....）包括密码A，存取控制、密码B等

#define RC522_DELAY()   Delay_us( 20 )

// 用户自定义GPIO，用于控制片选和重启信号
#define RC522_Reset_Disable()   HAL_GPIO_WritePin(RC522_RST_GPIO_Port,RC522_RST_Pin,GPIO_PIN_SET)
#define RC522_Reset_Enable()    HAL_GPIO_WritePin(RC522_RST_GPIO_Port,RC522_RST_Pin,GPIO_PIN_RESET)
#define RC522_CS_Disable()    HAL_GPIO_WritePin(RC522_CS_GPIO_Port,RC522_CS_Pin,GPIO_PIN_SET)
#define RC522_CS_Enable()    HAL_GPIO_WritePin(RC522_CS_GPIO_Port,RC522_CS_Pin,GPIO_PIN_RESET)
#define cardsNum 2

unsigned char CT[2];             //卡类型
unsigned char SN[4];             //卡号
unsigned char DATA[16];
unsigned char RFID[16];          //存放RFID 
unsigned char total=0;
unsigned char mycards[cardsNum][4]={
                    {0xAA,0x5B,0xD3,0x84},
                    {0xF9,0x56,0x2C,0xE5}
                };
int8_t cardNum;                
uint8_t KEY1_A[6]= {0xa0,0xa1,0xa2,0xa3,0xa4,0xa5};
uint8_t KEY1_B[6]= {0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char RFID1[16]={0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x07,0x80,0x29,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char DATA1[16]= {0x12,0x34,0x56,0x78,0x9A,0x00,0xff,0x07,0x80,0x29,0xff,0xff,0xff,0xff,0xff,0xff};
unsigned char status;
unsigned char addr=0x08;

void RC522_Handle1(void){
    status=MI_ERR;
    status = PcdRequest(PICC_REQALL, CT);// 寻卡 -> 扫描有无卡片
    if (status == MI_OK)
    {
        // 打印卡片类型代码
        // printf("Card Type:%x%x\r\n", CT[0],CT[1]);
        status=MI_ERR;
        status=PcdAnticoll(SN);// 防冲突 -> 可取得UID
        if (status==MI_OK)
        {
            // 打印16进制UID
            ShowID(SN); 
            // 判断卡所属用户
            for (uint8_t i = 0; i < cardsNum; i++)
            {
                if((SN[0]==mycards[i][0])&&(SN[1]==mycards[i][1])&&(SN[2]==mycards[i][2])&&(SN[3]==mycards[i][3]))
                {
                    cardNum=i;
                    printf("The Card is:card_%d\r\n",i);
                    break;
                }else{
                    cardNum=-1;
                }
            }
            if (cardNum==-1)
            {
                printf("Unkown card！\r\n");
            }
            status = MI_ERR;
            status = PcdSelect(SN);// 选择卡片
            if (status==MI_OK)
            {
                printf("Select Card\r\n");
                status = MI_ERR;
                // 注意：此处的块地址0x0B即2扇区3区块，此块地址只需要指向某一扇区就可以了，即2扇区为0x08-0x0B这个范围都有效，且只能对验证过的扇区进行读写操作
                status = PcdAuthState(KeyModeA, 0x04, KEY1_A, SN);
                if(status == MI_OK)//验证成功
                {
                    printf("PcdAuthState(A) success\r\n");
                }
                else
                {
                    printf("PcdAuthState(A) failed\r\n");
                }
            }
        }
        HAL_Delay(500); 
    }
    else{
        //NO Card
    }    
}
// 测试程序0，完成addr读写读
void RC522_Handle(void)
{
    uint8_t i = 0;
    uint8_t card0_bit = 0;

    
    if(status == MI_OK)//选卡成功
    {
       // printf("选卡成功\r\n");
        status = MI_ERR;
        // 验证A密钥 块地址 密码 SN 
		// 注意：此处的块地址0x0B即2扇区3区块，此块地址只需要指向某一扇区就可以了，即2扇区为0x08-0x0B这个范围都有效，且只能对验证过的扇区进行读写操作
        status = PcdAuthState(0x60, 0x0B, KEY1_A, SN);
       
    }
    if(status == MI_OK)//验证成功
    {
        //printf("扇区验证成功\r\n");
        status = MI_ERR;
        // 读取M1卡一块数据 块地址 读取的数据 注意：因为上面验证的扇区是2扇区，所以只能对2扇区的数据进行读写，即0x08-0x0B这个范围，超出范围读取失败。
        status = PcdRead(addr, DATA);
        if(status == MI_OK)//读卡成功
        {
            printf("RFID:%s\r\n", RFID);
            printf("DATA:");
            for(i = 0; i < 16; i++)
            {
                printf("%02x", DATA[i]);
            }
            printf("\r\n");
        }
        else
        {
            //printf("读卡失败\r\n");
        }
    }
    if(status == MI_OK)//读卡成功
    {
        //printf("读卡成功\r\n");
        status = MI_ERR;
		printf("一秒后开始写卡，请勿移开卡片！！\r\n");
		HAL_Delay(1000);
        // status = PcdWrite(addr, DATA0);
        // 写数据到M1卡一块
        status = PcdWrite(addr, DATA1);
        if(status == MI_OK)//写卡成功
        {
            printf("PcdWrite() success\r\n");
        }
        else
        {
            printf("PcdWrite() failed\r\n");
			HAL_Delay(3000);
        }
        
    }
    if(status == MI_OK)//写卡成功
    {
        status = MI_ERR;
        // 读取M1卡一块数据 块地址 读取的数据
        status = PcdRead(addr, DATA);
        if(status == MI_OK)//读卡成功
        {
            // printf("DATA:%s\r\n", DATA);
            printf("DATA:");
            for(i = 0; i < 16; i++)
            {
                printf("%02x", DATA[i]);
            }
            printf("\r\n");
        }
        else
        {
            printf("PcdRead() failed\r\n");
        }
    }

    if(status == MI_OK)//读卡成功
    {
        status = MI_ERR;
		printf("RC522_Handle() run finished after 1 second!\r\n");
        HAL_Delay(1000);
    }
}

/* us delay func */
void Delay_us(uint32_t t) {
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000000);
    HAL_Delay(t - 1);
    HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq() / 1000);
}

/* 
    RC522寻卡
    cReq_code，寻卡方式
        = 0x52，寻感应区内所有符合14443A标准的卡
        = 0x26，寻未进入休眠状态的卡
    pTagType，卡片类型代码
        = 0x4400，Mifare_UltraLight
        = 0x0400，Mifare_One(S50)
        = 0x0200，Mifare_One(S70)
        = 0x0800，Mifare_Pro(X))
        = 0x4403，Mifare_DESFire
    返回 MI_OK即调用成功
 */
char PcdRequest (uint8_t ucReq_code, uint8_t * pTagType)
{
    char cStatus;
    uint8_t ucComMF522Buf [MAXRLEN];
    uint32_t ulLen;

    ClearBitMask ( Status2Reg, 0x08 );	//清理指示MIFARECyptol单元接通以及所有卡的数据通信被加密的情况
    WriteRawRC ( BitFramingReg, 0x07 );	//	发送的最后一个字节的 七位
    SetBitMask ( TxControlReg, 0x03 );	//TX1,TX2管脚的输出信号传递经发送调制的13.56的能量载波信号

    ucComMF522Buf [0] = ucReq_code;		//存入 卡片命令字

    cStatus = PcdComMF522 ( PCD_TRANSCEIVE,	ucComMF522Buf, 1, ucComMF522Buf, & ulLen );	//寻卡
    
    if ( ( cStatus == MI_OK ) && ( ulLen == 0x10 ) )	//寻卡成功返回卡类型
    {
        * pTagType = ucComMF522Buf [ 0 ];
        * ( pTagType + 1 ) = ucComMF522Buf [ 1 ];
    }
    else
        cStatus = MI_ERR;

    return cStatus;
}
/*
    通过RC522和ISO14443卡通讯
    ucCommand，RC522命令字节
    pInData，通过RC522发送到卡片的数据
    ucInLenByte，发送数据的字节长度
    pOutData，接收到的卡片返回数据
    pOutLenBit，返回数据的位长度
    返回 MI_OK即调用成功
 */
char PcdComMF522 (uint8_t ucCommand, uint8_t * pInData, uint8_t ucInLenByte, uint8_t * pOutData, uint32_t * pOutLenBit)
{ 
    char cStatus = MI_ERR;
    uint8_t ucIrqEn   = 0x00;
    uint8_t ucWaitFor = 0x00;
    uint8_t ucLastBits;
    uint8_t ucN;
    uint32_t ul;
    switch ( ucCommand )
    {
    case PCD_AUTHENT:		//Mifare认证
        ucIrqEn   = 0x12;		//允许错误中断请求ErrIEn  允许空闲中断IdleIEn
        ucWaitFor = 0x10;		//认证寻卡等待时候 查询空闲中断标志位
        break;
    case PCD_TRANSCEIVE:		//接收发送 发送接收
        ucIrqEn   = 0x77;		//允许TxIEn RxIEn IdleIEn LoAlertIEn ErrIEn TimerIEn
        ucWaitFor = 0x30;		//寻卡等待时候 查询接收中断标志位与 空闲中断标志位
        break;
    default:
        break;
    }
    WriteRawRC ( ComIEnReg, ucIrqEn | 0x80 );		//IRqInv置位管脚IRQ与Status1Reg的IRq位的值相反
    ClearBitMask ( ComIrqReg, 0x80 );			//Set1该位清零时，CommIRqReg的屏蔽位清零
    WriteRawRC ( CommandReg, PCD_IDLE );		//写空闲命令
    SetBitMask ( FIFOLevelReg, 0x80 );			//置位FlushBuffer清除内部FIFO的读和写指针以及ErrReg的BufferOvfl标志位被清除
    for ( ul = 0; ul < ucInLenByte; ul ++ )
        WriteRawRC ( FIFODataReg, pInData [ ul ] );    		//写数据进FIFOdata

    WriteRawRC ( CommandReg, ucCommand );					//写命令

    if ( ucCommand == PCD_TRANSCEIVE )
        SetBitMask(BitFramingReg,0x80);  				//StartSend置位启动数据发送 该位与收发命令使用时才有效

    ul = 1000;//根据时钟频率调整，操作M1卡最大等待时间25ms

    do 														//认证 与寻卡等待时间
    {
        ucN = ReadRawRC ( ComIrqReg );							//查询事件中断
        ul --;
    } while ( ( ul != 0 ) && ( ! ( ucN & 0x01 ) ) && ( ! ( ucN & ucWaitFor ) ) );		//退出条件i=0,定时器中断，与写空闲命令

    ClearBitMask ( BitFramingReg, 0x80 );					//清理允许StartSend位

    if ( ul != 0 )
    {
        if ( ! (( ReadRawRC ( ErrorReg ) & 0x1B )) )			//读错误标志寄存器BufferOfI CollErr ParityErr ProtocolErr
        {
            cStatus = MI_OK;
            if ( ucN & ucIrqEn & 0x01 )					//是否发生定时器中断
                cStatus = MI_NOTAGERR;

            if ( ucCommand == PCD_TRANSCEIVE )
            {
                ucN = ReadRawRC ( FIFOLevelReg );			//读FIFO中保存的字节数
                ucLastBits = ReadRawRC ( ControlReg ) & 0x07;	//最后接收到得字节的有效位数
                if ( ucLastBits )
                    * pOutLenBit = ( ucN - 1 ) * 8 + ucLastBits;   	//N个字节数减去1（最后一个字节）+最后一位的位数 读取到的数据总位数
                else
                    * pOutLenBit = ucN * 8;   					//最后接收到的字节整个字节有效
                if ( ucN == 0 )
                    ucN = 1;
                if ( ucN > MAXRLEN )
                    ucN = MAXRLEN;
                for ( ul = 0; ul < ucN; ul ++ )
                    pOutData [ ul ] = ReadRawRC ( FIFODataReg );
            }
        }
        else
            cStatus = MI_ERR;
    // printf(ErrorReg);
    }
    SetBitMask ( ControlReg, 0x80 );    // stop timer now
    WriteRawRC ( CommandReg, PCD_IDLE );
    return cStatus;
}

/*
    PCD防冲撞
    pSnr，卡片序列号，4字节
    返回 MI_OK即调用成功
 */
char PcdAnticoll (uint8_t * pSnr)
{
    char cStatus;
    uint8_t uc, ucSnr_check = 0;
    uint8_t ucComMF522Buf [ MAXRLEN ];
    uint32_t ulLen;

    ClearBitMask ( Status2Reg, 0x08 );		//清MFCryptol On位 只有成功执行MFAuthent命令后，该位才能置位
    WriteRawRC ( BitFramingReg, 0x00);		//清理寄存器 停止收发
    ClearBitMask ( CollReg, 0x80 );			//清ValuesAfterColl所有接收的位在冲突后被清除

    /*
    参考ISO14443协议：https://blog.csdn.net/wowocpp/article/details/79910800
    PCD 发送 SEL = ‘93’，NVB = ‘20’两个字节
    迫使所有的在场的PICC发回完整的UID CLn作为应答。
    */

    ucComMF522Buf [ 0 ] = 0x93;	//卡片防冲突命令
    ucComMF522Buf [ 1 ] = 0x20;

    // 发送并接收数据 接收的数据存储于ucComMF522Buf
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 2, ucComMF522Buf, & ulLen);//与卡片通信

    if ( cStatus == MI_OK)		//通信成功
    {
        
        // 收到的UID 存入pSnr
        for ( uc = 0; uc < 4; uc ++ )
        {
            * ( pSnr + uc )  = ucComMF522Buf [ uc ];			//读出UID
            ucSnr_check ^= ucComMF522Buf [ uc ];
        }
        if ( ucSnr_check != ucComMF522Buf [ uc ] )
            cStatus = MI_ERR;
    }
    SetBitMask ( CollReg, 0x80 );
    return cStatus;
}

void ShowID(uint8_t *p)
{
    uint8_t num[9];
    uint8_t i;
    for(i=0; i<4; i++)
    {
        num[i*2] = p[i] / 16;
        num[i*2] > 9 ? (num[i*2] += '7') : (num[i*2] += '0');
        num[i*2+1] = p[i] % 16;
        num[i*2+1] > 9 ? (num[i*2+1] += '7') : (num[i*2+1] += '0');
    }
    num[8] = 0;
    printf("UID>>>%s\r\n", num);
}

/*
    选定卡片
    pSnr，卡片序列号，4字节
    返回 MI_OK调用成功
 */
char PcdSelect (uint8_t * pSnr)
{
    char cStatus;
    uint8_t uc;
    uint8_t ucComMF522Buf [ MAXRLEN ];
    uint32_t  ulLen;

    // 防冲撞 0x93
    ucComMF522Buf [ 0 ] = PICC_ANTICOLL1;
    // 假设没有冲突，PCD 指定NVB为70，此值表示PCD将发送完整的UID CLn，与40位UID CLn 匹配的PICC，以SAK作为应答
    ucComMF522Buf [ 1 ] = 0x70;
    ucComMF522Buf [ 6 ] = 0;

    // 3 4 5 6位存放UID，第7位一直异或。。。
    for ( uc = 0; uc < 4; uc ++ )
    {
        ucComMF522Buf [ uc + 2 ] = * ( pSnr + uc );
        ucComMF522Buf [ 6 ] ^= * ( pSnr + uc );
    }

    // CRC(循环冗余校验)
    CalulateCRC ( ucComMF522Buf, 7, & ucComMF522Buf [ 7 ] );
    ClearBitMask ( Status2Reg, 0x08 );
    // 发送并接收数据
    cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 9, ucComMF522Buf, & ulLen );
    if ( ( cStatus == MI_OK ) && ( ulLen == 0x18 ) )
        cStatus = MI_OK;
    else
        cStatus = MI_ERR;
    return cStatus;
}

/*
    验证卡片密码
    ucAuth_mode，密码验证模式
        = 0x60，验证A密钥
        = 0x61，验证B密钥
    ucAddr，块地址
    pKey，密码
    pSnr，卡片序列号，4字节
    返回MI_OK调用成功
 */
char PcdAuthState (uint8_t ucAuth_mode, uint8_t ucAddr, uint8_t * pKey, uint8_t * pSnr)
{
    char cStatus;
    uint8_t uc, ucComMF522Buf [ MAXRLEN ];
    uint32_t ulLen;

    ucComMF522Buf [ 0 ] = ucAuth_mode;
    ucComMF522Buf [ 1 ] = ucAddr;

    for ( uc = 0; uc < 6; uc ++ )
        ucComMF522Buf [ uc + 2 ] = * ( pKey + uc );

    for ( uc = 0; uc < 6; uc ++ )
        ucComMF522Buf [ uc + 8 ] = * ( pSnr + uc );

    // 验证密钥命令
    cStatus = PcdComMF522 ( PCD_AUTHENT, ucComMF522Buf, 12, ucComMF522Buf, & ulLen );

    if ( ( cStatus != MI_OK ) || ( ! ( ReadRawRC ( Status2Reg ) & 0x08 ) ) )
    {
        cStatus = MI_ERR;
    }
    return cStatus;
}


/*
    用RC522计算CRC16
    pIndata，计算CRC16的数组
    ucLen，计算CRC16的数组字节长度
    pOutData，存放计算结果存放的首地址
 */
void CalulateCRC (uint8_t * pIndata, uint8_t ucLen, uint8_t * pOutData)
{
    uint8_t uc, ucN;

    ClearBitMask(DivIrqReg,0x04);
	
    WriteRawRC(CommandReg,PCD_IDLE);
	
    SetBitMask(FIFOLevelReg,0x80);
	
    for ( uc = 0; uc < ucLen; uc ++)
	    WriteRawRC ( FIFODataReg, * ( pIndata + uc ) );   

    WriteRawRC ( CommandReg, PCD_CALCCRC );
	
    uc = 0xFF;
	
    do 
    {
        ucN = ReadRawRC ( DivIrqReg );
        uc --;
    } while ( ( uc != 0 ) && ! ( ucN & 0x04 ) );
		
    pOutData [ 0 ] = ReadRawRC ( CRCResultRegL );
    pOutData [ 1 ] = ReadRawRC ( CRCResultRegM );
	
}

/*
    读取M1卡一块数据
    ucAddr，块地址
    pData，读出的数据，16字节
    返回MI_OK用成功
 */
char PcdRead (uint8_t ucAddr, uint8_t * pData)
{
	char cStatus;
	uint8_t uc, ucComMF522Buf [ MAXRLEN ]; 
	uint32_t ulLen;
	
	ucComMF522Buf [ 0 ] = PICC_READ;
	ucComMF522Buf [ 1 ] = ucAddr;
	
	CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
	
	cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
	
	if ((cStatus == MI_OK) && (ulLen == 0x90))
	{
		for (uc = 0; uc < 16; uc ++)
			*(pData + uc) = ucComMF522Buf[uc];   
	}
	else
		cStatus = MI_ERR;   
	return cStatus;
}

/*
    写数据到M1卡一块
    ucAddr，块地址
    pData，写入的数据，16字节
    返回MI_OK调用成功
 */
char PcdWrite (uint8_t ucAddr, uint8_t * pData)
{
	char cStatus;
	uint8_t uc, ucComMF522Buf [ MAXRLEN ];
	uint32_t ulLen;
	
	ucComMF522Buf [ 0 ] = PICC_WRITE;
	ucComMF522Buf [ 1 ] = ucAddr;
	
	CalulateCRC ( ucComMF522Buf, 2, & ucComMF522Buf [ 2 ] );
	cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 4, ucComMF522Buf, & ulLen );
	
	if ( ( cStatus != MI_OK ) || ( ulLen != 4 ) || ( ( ucComMF522Buf [ 0 ] & 0x0F ) != 0x0A ) )
		cStatus = MI_ERR;   
	if ( cStatus == MI_OK )
	{
		memcpy(ucComMF522Buf, pData, 16);
		for ( uc = 0; uc < 16; uc ++ )
			ucComMF522Buf [ uc ] = * ( pData + uc );  
		CalulateCRC ( ucComMF522Buf, 16, & ucComMF522Buf [ 16 ] );
		cStatus = PcdComMF522 ( PCD_TRANSCEIVE, ucComMF522Buf, 18, ucComMF522Buf, & ulLen );
		if (( cStatus != MI_OK ) || ( ulLen != 4 ) || (( ucComMF522Buf [0] & 0x0F ) != 0x0A ))
			cStatus = MI_ERR;   
	} 
	return cStatus;
}



/* 初始化RC522 */
void RC522_Init (void)
{
	RC522_Reset_Disable();
	RC522_CS_Disable();
    PcdAntennaOff ();//关天线
    PcdAntennaOn ();//开天线
    PCD_Reset();
	M500PcdConfigISOType ( 'A' );//设置工作方式
}
/* 复位RC522 */
void PCD_Reset (void)
{
    RC522_Reset_Disable();
    Delay_us ( 1 );
    RC522_Reset_Enable();
    Delay_us ( 1 );
    RC522_Reset_Disable();
    Delay_us ( 1 );
    WriteRawRC ( CommandReg, 0x0f );

    while ( ReadRawRC ( CommandReg ) & 0x10 );
    Delay_us ( 1 );

    WriteRawRC ( ModeReg, 0x3D );            //定义发送和接收常用模式 和Mifare卡通讯，CRC初始值0x6363
    WriteRawRC ( TReloadRegL, 30 );          //16位定时器低位    
    WriteRawRC ( TReloadRegH, 0 );			 //16位定时器高位
    WriteRawRC ( TModeReg, 0x8D );			 //定义内部定时器的设置
    WriteRawRC ( TPrescalerReg, 0x3E );		 //设置定时器分频系数
    WriteRawRC ( TxAutoReg, 0x40 );			 //调制发送信号为100%ASK	
}
/* 设置RC522的工作方式 */
void M500PcdConfigISOType (uint8_t ucType)
{
	if ( ucType == 'A')                     //ISO14443_A
	{
		ClearBitMask ( Status2Reg, 0x08 );
		WriteRawRC ( ModeReg, 0x3D );//3F
		WriteRawRC ( RxSelReg, 0x86 );//84
		WriteRawRC( RFCfgReg, 0x7F );   //4F
		WriteRawRC( TReloadRegL, 30 );//tmoLength);// TReloadVal = 'h6a =tmoLength(dec) 
		WriteRawRC ( TReloadRegH, 0 );
		WriteRawRC ( TModeReg, 0x8D );
		WriteRawRC ( TPrescalerReg, 0x3E );
		Delay_us ( 2 );
	}
}
/* 开启天线 */
void PcdAntennaOn (void)
{
    uint8_t uc;
    uc = ReadRawRC ( TxControlReg );
    if ( ! ( uc & 0x03 ) )//0x7F;
        SetBitMask(TxControlReg, 0x03);
}
/* 关闭天线 */
void PcdAntennaOff (void)
{
    ClearBitMask ( TxControlReg, 0x03 );
}

/* 向RC522写入指定地址的数据 */
void WriteRawRC(uint8_t ucAddress, uint8_t ucValue)
{  
	uint8_t sentData[2];
	sentData[0] = ( ucAddress << 1 ) & 0x7E;
    sentData[1] = ucValue;
	RC522_CS_Enable();
	HAL_SPI_Transmit(&hspi1,(uint8_t*)sentData,2,0xFFFF);
	RC522_CS_Disable();	
}
/* 读取RC522指定地址的数据 */
uint8_t ReadRawRC (uint8_t ucAddress)
{
	uint8_t ucAddr[1], ucReturn[1];
	ucAddr[0] = ( ( ucAddress << 1 ) & 0x7E ) | 0x80;
	RC522_CS_Enable();
	//SPI_RC522_SendByte ( ucAddr );
    HAL_SPI_Transmit(&hspi1,(uint8_t*)ucAddr,1,0xFFFF);
	// ucReturn = SPI_RC522_ReadByte ();
    HAL_SPI_Receive(&hspi1,(uint8_t*)ucReturn,1,0xFFFF);
    //printf("%x\r\n",ucReturn[0]);
	RC522_CS_Disable();
	return ucReturn[0];
}
/* 向RCC指定寄存器置位 */
void SetBitMask (uint8_t ucReg, uint8_t ucMask)
{
    uint8_t ucTemp;
    ucTemp = ReadRawRC ( ucReg );
    WriteRawRC ( ucReg, ucTemp | ucMask );         // set bit mask
}
/* 向RCC指定寄存器清位 */
void ClearBitMask (uint8_t ucReg, uint8_t ucMask) 
{
    uint8_t ucTemp;
    ucTemp = ReadRawRC ( ucReg );
    WriteRawRC ( ucReg, ucTemp & ( ~ ucMask) );  // clear bit mask
}


