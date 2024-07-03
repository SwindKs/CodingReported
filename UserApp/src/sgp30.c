#include "sgp30.h"
#include <stdio.h>

sgp30_data_t sgp30Data = {0};
void SGP30_Init(void)
{
  int ret ;
  SGP30_Reset();
 // 初始化命令为0x2003
  uint8_t cmd[]={0x20,0x03};
  ret = User_I2C1_Write(SGP30_write,0,cmd,sizeof(cmd));
}

int SGP30_ReadData(void)
{

  uint8_t sgp30_buf[6] = {0};// 存储返回的6个字节数据


  //读取气体值指令0x2008
  uint8_t cmd[]={0x20,0x08};
  User_I2C1_Write(SGP30_write,0,cmd,sizeof(cmd));
  hi_udelay(20000);
  User_I2C1_Read(SGP30_read,0,sgp30_buf,sizeof(sgp30_buf));
 if (CheckCrc8(&sgp30_buf[0], 0xFF) != sgp30_buf[2]) {
        printf("co2 recv data crc check failrn\r\n");
        return -1;
  }
  if (CheckCrc8(&sgp30_buf[3], 0xFF) != sgp30_buf[5]) {
      printf("tvoc recv data crc check failrn\r\n");
      return -1;
  }
  /* 转换测量数据 */
  sgp30Data.co2  = sgp30_buf[0] << 8 | sgp30_buf[1];
  sgp30Data.tvoc = sgp30_buf[3] << 8 | sgp30_buf[4];
  // printf("Co2:%d [ppm]\t TVOC:%d [ppb]\r\n",sgp30Data.co2,sgp30Data.tvoc);
  return 0;
}

uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value)
{
    uint8_t  remainder;	    //余数
    uint8_t  i = 0, j = 0;  //循环变量
    /* 初始化 */
    remainder = initial_value;
    for(j = 0; j < 2;j++)
    {
        remainder ^= message[j];
        /* 从最高位开始依次计算  */
        for (i = 0; i < 8; i++)
        {
            if (remainder & 0x80)
            {
                remainder = (remainder << 1)^CRC8_POLYNOMIAL;
            }
            else
            {
                remainder = (remainder << 1);
            }
        }
    }
    /* 返回计算的CRC码 */
    return remainder;
}

void SGP30_Reset(void){
  uint8_t cmd;
//0x0006
  cmd = 0x00;
  User_I2C1_Write(SGP30_write,0,&cmd,sizeof(cmd));
  cmd = 0x06;
  User_I2C1_Write(SGP30_write,0,&cmd,sizeof(cmd));
  osDelay(1);

}

void SGP30Task(void)
{
  SGP30_Init();
  
  while(1){
    SGP30_ReadData();
    osDelay(100);
  }
}