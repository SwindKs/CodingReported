//dth11.h程序
#ifndef __DHT11_H
#define __DHT11_H

#include "hi_io.h"
#include "hi_gpio.h"
#include "cmsis_os2.h"
#include "hi_time.h"



#define DHT11_GPIO            HI_GPIO_IDX_5
#define DHT11_GPIO_FUNC       HI_IO_FUNC_GPIO_5_GPIO  
#define DHT11_GPIO_DQ_OUT_H   hi_gpio_set_ouput_val(DHT11_GPIO,1);
#define DHT11_GPIO_DQ_OUT_L   hi_gpio_set_ouput_val(DHT11_GPIO,0);


extern uint8_t DHT_buf[5];


void DHT11_Init(void);
void DHT11_GPIO_OUT(void);
void DHT11_GPIO_IN(void);
uint8_t DHT11_Get_Input(hi_io_name id,hi_gpio_value *val);
void DHT11_Start(void);
uint8_t DHT11_Read_Data(void);
uint8_t DHT11_Read_Bit(void);
uint8_t DHT11_Read_Byte(void);
uint8_t DHT11_Check(void);
void DHT11Task(void);


#endif 
