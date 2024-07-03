#ifndef __BH1750_H
#define __BH1750_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "hi_i2c.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "hi_time.h"
#include "ohos_init.h"
#include "cmsis_os2.h"
#include "UserI2C.h"




#define WRITE_BIT           0x00
#define READ_BIT            0x01

#define BH1750_SLAVE_ADDR   0x23 // 从机地址
#define BH1750_PWR_DOWN     0x00 // 关闭模块
#define BH1750_PWR_ON       0x01 // 打开模块等待测量指令
#define BH1750_RST          0x07 // 重置数据寄存器值在PowerOn模式下有效
#define BH1750_CON_H        0x10 // 连续高分辨率模式，1lx，120ms
#define BH1750_CON_H2       0x11 // 连续高分辨率模式，0.5lx，120ms
#define BH1750_CON_L        0x13 // 连续低分辨率模式，4lx，16ms
#define BH1750_ONE_H        0x20 // 一次高分辨率模式，1lx，120ms，测量后模块转到PowerDown模式
#define BH1750_ONE_H2       0x21 // 一次高分辨率模式，0.5lx，120ms，测量后模块转到PowerDown模式
#define BH1750_ONE_L        0x23 // 一次低分辨率模式，4lx，16ms，测量后模块转到PowerDown模式

void BH1750_Init(void);
float BH1750_ReadLightIntensity(void);

void BH1750Task(void);
#endif