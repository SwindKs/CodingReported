/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "ohos_init.h"
#include "cmsis_os2.h"

#include "hi_gpio.h"
#include "iot_gpio.h"
#include "hi_io.h"
#include <hi_pwm.h>
#include "adc_drv1.h"
#include "motor_control.h"
#include "HCSR04.h"


#define IOT_PWM_PORT_PWM0   0
#define IOT_PWM_PORT_PWM1   1
#define IOT_PWM_PORT_PWM2   2
#define IOT_PWM_PORT_PWM3   3
#define IOT_FREQ            65535


#define Trace_left_IO           HI_GPIO_IDX_6
#define Trace_left_IO_FUNC      HI_IO_FUNC_GPIO_6_GPIO
#define Trace_right_IO          HI_GPIO_IDX_7
#define Trace_right_IO_FUNC     HI_IO_FUNC_GPIO_7_GPIO

static volatile int g_State = 1;
int g_CarStarted = 1;
CAR_DRIVE car_drive = { 0 };

hi_gpio_value Trace_left_IN = HI_GPIO_VALUE1;
hi_gpio_value Trace_right_IN = HI_GPIO_VALUE1;





void TB6612FNGInit(void)
{

    hi_gpio_init();
    //左电机
    // 设置GPIO11的管脚复用关系为PWM2输出
    hi_io_set_func(HI_GPIO_IDX_11, HI_IO_FUNC_GPIO_11_PWM2_OUT);
    // 设置GPIO12的管脚复用关系为PWM3输出
    hi_io_set_func(HI_GPIO_IDX_12, HI_IO_FUNC_GPIO_12_PWM3_OUT);

    //右电机
    // 设置GPIO9的管脚复用关系为PWM0输出
    hi_io_set_func(HI_GPIO_IDX_9, HI_IO_FUNC_GPIO_9_PWM0_OUT);
    // 设置GPIO10的管脚复用关系为PWM1输出
    hi_io_set_func(HI_GPIO_IDX_10, HI_IO_FUNC_GPIO_10_PWM1_OUT);

    // GPIO9方向设置为输出
    hi_gpio_set_dir(HI_GPIO_IDX_9, HI_GPIO_DIR_OUT);
    // GPIO10方向设置为输出
    hi_gpio_set_dir(HI_GPIO_IDX_10, HI_GPIO_DIR_OUT);
    // GPIO11方向设置为输出
    hi_gpio_set_dir(HI_GPIO_IDX_11, HI_GPIO_DIR_OUT);
    // GPIO12方向设置为输出
    hi_gpio_set_dir(HI_GPIO_IDX_12, HI_GPIO_DIR_OUT);


    // 初始化PWM0
    hi_pwm_init(IOT_PWM_PORT_PWM0);
    // 初始化PWM1
    hi_pwm_init(IOT_PWM_PORT_PWM1);
    // 初始化PWM2
    hi_pwm_init(IOT_PWM_PORT_PWM2);
    // 初始化PWM3
    hi_pwm_init(IOT_PWM_PORT_PWM3);

    hi_pwm_set_clock(PWM_CLK_XTAL);

}

// PWM取值：[1, 65535]，占空比[0-99]
void car_backward(uint32_t left_pwm_value, uint32_t right_pwm_value)
{
    car_stop();
    hi_pwm_start(IOT_PWM_PORT_PWM0, right_pwm_value, IOT_FREQ);
    hi_pwm_start(IOT_PWM_PORT_PWM3, left_pwm_value, IOT_FREQ);
}

void car_forward(uint32_t left_pwm_value, uint32_t right_pwm_value)
{
    car_stop();
    hi_pwm_start(IOT_PWM_PORT_PWM2, left_pwm_value, IOT_FREQ);
    hi_pwm_start(IOT_PWM_PORT_PWM1, right_pwm_value, IOT_FREQ);
}

void car_left(uint32_t right_pwm_value)
{
    car_stop();
    hi_pwm_start(IOT_PWM_PORT_PWM1, right_pwm_value, IOT_FREQ);
}

void car_right(uint32_t left_pwm_value)
{
    car_stop();
    hi_pwm_start(IOT_PWM_PORT_PWM2, left_pwm_value, IOT_FREQ);
}

void car_stop(void)
{
    hi_pwm_stop(IOT_PWM_PORT_PWM0);
    hi_pwm_stop(IOT_PWM_PORT_PWM1);
    hi_pwm_stop(IOT_PWM_PORT_PWM2);
    hi_pwm_stop(IOT_PWM_PORT_PWM3);
}

/******************************************************循迹代码块**********************************************************************/
void init_ctrl_algo(void)
{
    memset(car_drive, 0, sizeof(CAR_DRIVE));
    car_drive.LeftForward = 28100; // 10 左轮前进速度
    car_drive.RightForward = 34100; // 10 右轮前进速度
    car_drive.TurnLeft = 34500; // 35 左转弯右轮速度
    car_drive.TurnRight = 34500; // 30 右转弯左轮速度
}

void trace_module_init(void)
{
    hi_gpio_init();
    // 设置GPIO04的管脚复用关系为GPIO
    hi_io_set_func(Trace_left_IO, Trace_left_IO_FUNC);
     // 设置GPIO04的管脚方向为入
    hi_gpio_set_dir(Trace_left_IO, HI_GPIO_DIR_IN);
    hi_io_set_pull(Trace_left_IO,HI_IO_PULL_UP);
    
 
    // GPIO7初始化
    // 设置GPIO7的管脚复用关系为GPIO
    hi_io_set_func(Trace_right_IO, Trace_right_IO_FUNC);
    // 设置GPIO7的管脚方向为入
    hi_gpio_set_dir(Trace_right_IO, HI_GPIO_DIR_IN);
    hi_io_set_pull(Trace_right_IO,HI_IO_PULL_UP);

}


void Trace_get_dir(void)
{

    // int ret = -1;
    hi_gpio_get_input_val(Trace_left_IO,&Trace_left_IN);
    hi_gpio_get_input_val(Trace_right_IO,&Trace_right_IN);
    if(Trace_left_IN == 0 && Trace_right_IN == 0){
        //未检测到黑线，直线运行
        car_forward(car_drive.LeftForward,car_drive.RightForward);
        printf("Forward\r\n");
    }
    if(Trace_left_IN == 1 && Trace_right_IN == 0){
        //碰到黑线输出1，右偏，向左转 
        car_left(car_drive.TurnLeft);
        printf("left\r\n");
    }
    if(Trace_left_IN == 0 && Trace_right_IN == 1){
        //左偏，向右转
        
        
        car_right(car_drive.TurnRight);
        printf("right\r\n");
    }
    if(Trace_left_IN == 1 && Trace_right_IN == 1){
        //两路都检测到黑线，停止
        car_stop();
        printf("stop\r\n");
    }

}



/************************************************************循迹代码块**********************************************************************/


void TraceTask(void)
{

    float dis_cm;

    Hcsr04Init();
    S92RInit();
    osDelay(100);


    while(1){
        
        dis_cm = GetDistance();
        osDelay(2);
        //直行时，障碍距离小于10cm，进行转向判断
        if(dis_cm <= dis_min_limit){
                //停止或后退
                car_stop();
        }else{
            //循迹逻辑
            hi_gpio_get_input_val(Trace_left_IO,&Trace_left_IN);
            hi_gpio_get_input_val(Trace_right_IO,&Trace_right_IN);
            if(Trace_left_IN == 0 && Trace_right_IN == 0){
            //未检测到黑线，直线运行
                car_forward(car_drive.LeftForward,car_drive.RightForward);
                continue;
            }
            if(Trace_left_IN == 1 && Trace_right_IN == 0){
            //碰到黑线输出1，右偏，向左转 
                car_left(car_drive.TurnLeft);
                continue;
            }
            if(Trace_left_IN == 0 && Trace_right_IN == 1){
            //左偏，向右转
                car_right(car_drive.TurnRight);
                continue;
            }
            if(Trace_left_IN == 1 && Trace_right_IN == 1){
            //两路都检测到黑线，停止
                car_stop();
            }
        }
        // osDelay(2);

    }
}