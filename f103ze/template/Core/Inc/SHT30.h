//
// Created by wbh on 2021/2/5.
//

#ifndef __SHT30_H__
#define __SHT30_H__
#include "main.h"
#include "i2c.h"
#define    SHT30_ADDR_WRITE    0x44<<1         //10001000
#define    SHT30_ADDR_READ        (0x44<<1)+1        //10001011

typedef enum
{
    /* 软件复位命令 */

    SOFT_RESET_CMD = 0x30A2,
    /*
    单次测量模式
    命名格式：Repeatability_CS_CMD
    CS： Clock stretching
    */
    HIGH_ENABLED_CMD    = 0x2C06,
    MEDIUM_ENABLED_CMD  = 0x2C0D,
    LOW_ENABLED_CMD     = 0x2C10,
    HIGH_DISABLED_CMD   = 0x2400,
    MEDIUM_DISABLED_CMD = 0x240B,
    LOW_DISABLED_CMD    = 0x2416,

    /*
    周期测量模式
    命名格式：Repeatability_MPS_CMD
    MPS：measurement per second
    */
    HIGH_0_5_CMD   = 0x2032,
    MEDIUM_0_5_CMD = 0x2024,
    LOW_0_5_CMD    = 0x202F,
    HIGH_1_CMD     = 0x2130,
    MEDIUM_1_CMD   = 0x2126,
    LOW_1_CMD      = 0x212D,
    HIGH_2_CMD     = 0x2236,
    MEDIUM_2_CMD   = 0x2220,
    LOW_2_CMD      = 0x222B,
    HIGH_4_CMD     = 0x2334,
    MEDIUM_4_CMD   = 0x2322,
    LOW_4_CMD      = 0x2329,
    HIGH_10_CMD    = 0x2737,
    MEDIUM_10_CMD  = 0x2721,
    LOW_10_CMD     = 0x272A,
    /* 周期测量模式读取数据命令 */
    READOUT_FOR_PERIODIC_MODE = 0xE000,
} SHT30_CMD;

/**
 * @brief    向SHT30发送一条指令(16bit)
 * @param    cmd —— SHT30指令（在SHT30_MODE中枚举定义）
 * @retval    成功返回HAL_OK
*/
static uint8_t    SHT30_Send_Cmd(SHT30_CMD cmd)
{
  uint8_t cmd_buffer[2];
  cmd_buffer[0] = cmd >> 8;
  cmd_buffer[1] = cmd;
  return HAL_I2C_Master_Transmit(&hi2c2, SHT30_ADDR_WRITE, (uint8_t*)cmd_buffer, 2, 0xFFFF);
}

void SHT30_Reset(void);
uint8_t SHT30_Init(void);
uint8_t SHT30_Read_Dat(uint8_t* dat);
uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value);
uint8_t SHT30_Dat_To_Float(uint8_t* const dat, float* temperature, float* humidity);




#endif //OLED_SHT30_H
