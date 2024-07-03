#ifndef __USERI2C_H
#define __USERI2C_H

#include <stdio.h>
#include "cmsis_os2.h"
#include "hi_i2c.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "hi_time.h"


void User_I2C1_Init(void);
uint32_t User_I2C1_Write(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint16_t dataLen);
uint32_t User_I2C1_Read(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint16_t dataLen);


#endif