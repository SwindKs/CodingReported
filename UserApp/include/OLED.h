#ifndef __OLED_H
#define __OLED_H

#include <stdio.h>
#include "string.h"
#include "cmsis_os2.h"
#include "hi_i2c.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "hi_time.h"
#include "UserI2C.h"
#include "OLEDfont.h"

#define OLED_SLAVE_ADDRESS 0x78 //器件地址

#define RESET_TIME (100 * 1000) // 100ms

#define TEXT_SIZE_8_WIDTH_6 6 // 字体大小  8：6*8
#define TEXT_SIZE_8_HIGHT_8 8 // 字体大小  8：6*8
#define TEXT_SIZE_16_WIDTH_8 8 // 字体大小  16：8*16
#define TEXT_SIZE_16_HIGHT_16 16 // 字体大小  16：8*16

#define TEXT_8_MAX_ROW_POS 126
#define TEXT_16_MAX_ROW_POS 120

#define OFFSET_2 2
#define OFFSET_8 8
#define OFFSET_16 16

#define TEXT_SIZE_8 8
#define TEXT_SIZE_16 16


void OLED_Init(void);
void OLED_WriteCommand(uint8_t byte);
void OLED_WriteData(uint8_t byte);
void OLED_Fill(uint8_t fill_Data);
void OLED_SetPos(uint8_t x, uint8_t y);
void OLED_Clear(void);
void OLED_Set_Vertical_Rol(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(uint8_t x, uint8_t y, uint8_t ch[], uint8_t TextSize);
void OLED_DrawBMP(uint8_t xMove, uint8_t yMove,
                     uint8_t width, uint8_t height,
                     uint8_t *BMP);



void OLEDTask(void);
#endif