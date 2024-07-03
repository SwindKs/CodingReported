#ifndef __SGP30_H
#define __SGP30_H

#include "cmsis_os2.h"
#include "hi_i2c.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "hi_time.h"
#include "UserI2C.h"


// 定义SGP30结构体，存储CO2和TVOC的值
typedef struct {
  uint16_t co2;
  uint16_t tvoc;
} sgp30_data_t;
extern sgp30_data_t sgp30Data;

/************************************SGP30**地址/指令*******************************************/
#define  SGP30_I2C_ADDR           (0x58)//从器件地址
#define SGP30_read      ((SGP30_I2C_ADDR<<1) | 0x01)  //SGP30读地址
#define SGP30_write     (SGP30_I2C_ADDR<<1)       //SGP30写地址
#define CRC8_POLYNOMIAL (0x31)

/************************************SGP30**函数*******************************************/

void SGP30_Init(void);
int SGP30_ReadData(void);
uint8_t CheckCrc8(uint8_t* const message, uint8_t initial_value);
void SGP30_Reset(void);
void SGP30Task(void);



#endif

