#ifndef __MQ2_H
#define __MQ2_H

#include "hi_io.h"
#include "hi_gpio.h"
#include "cmsis_os2.h"
#include "hi_time.h"
#include "hi_adc.h"
#include <math.h>
#include "hi_errno.h"
#include <stdio.h>
#include <unistd.h>




//RL阻值
#define RL 5

//校准环境中PPM值
#define CAL_PPM 20  

//原件在洁净空气中的阻值
static float R0;


// void MQ2_Init(void);
float GetMQ2_Value(void);
void MQ2_PPM_Cali(void);
float GetMQ2_PPM(void);
void MQ2_Task(void);
void MQ2_Init(void);


#endif /*__MQ2_H */