#include "HCSR04.h"

#include "ohos_init.h"
#include "cmsis_os2.h"
#include "iot_watchdog.h"
#include "iot_gpio.h"
#include "hi_time.h"
#include "hi_io.h"
#include "hi_gpio.h"
#include "iot_watchdog.h"


#define DELAY_US20    20
#define DELAY_MS10    10

#define  COUNT   10
#define  FREQ_TIME    20000

#define Trig_IO                 HI_GPIO_IDX_8
#define Trig_IO_FUNC            HI_IO_FUNC_GPIO_8_GPIO  
#define Echo_IO                 HI_GPIO_IDX_14
#define Echo_IO_FUNC            HI_IO_FUNC_GPIO_14_GPIO

#define SG90_IO                 HI_GPIO_IDX_2
#define SG90_IO_FUNC            HI_IO_FUNC_GPIO_2_GPIO

const float dis_min_limit = 10.0f;

void Hcsr04Init(void)
{
    hi_gpio_init();
    /*
     * 设置超声波Trig为输出模式
     * 设置GPIO8功能（设置为GPIO功能）
     * Set ultrasonic Trig as output mode
     * Set GPIO8 function (set as GPIO function)
     */
    hi_io_set_func(Trig_IO,Trig_IO_FUNC);
    /*
     * 设置GPIO8为输入方向
     * Set GPIO8 as the output direction
     */
    hi_gpio_set_dir(Trig_IO, IOT_GPIO_DIR_OUT);
    /*
     * 设置GPIO14功能（设置为GPIO功能）
     * Set GPIO14 function (set as GPIO function)
     */
    hi_io_set_func(Echo_IO, Echo_IO_FUNC);
    /*
     * 设置GPIO14为输出方向
     * Set GPIO14 as the Input direction
     */
    hi_gpio_set_dir(Echo_IO, IOT_GPIO_DIR_IN);
}

float GetDistance(void)
{
    static unsigned long start_time = 0, time = 0;
    float distance = 0.0;
    IotGpioValue value = IOT_GPIO_VALUE0;
    unsigned int flag = 0;
    /*
     * 设置GPIO8输出低电平
     * 给trig发送至少10us的高电平脉冲，以触发传感器测距
     * Set GPIO7 to output direction
     * Send a high level pulse of at least 10us to the trig to trigger the range measurement of the sensor
     */
    IoTGpioSetOutputVal(Trig_IO, IOT_GPIO_VALUE1);
    hi_udelay(DELAY_US20);
    IoTGpioSetOutputVal(Trig_IO, IOT_GPIO_VALUE0);
    /*
     * 计算与障碍物之间的距离
     * Calculate the distance from the obstacle
     */
    while (1) {
        /*
         * 获取GPIO14的输入电平状态
         * Get the input level status of GPIO14
         */
        IoTGpioGetInputVal(Echo_IO, &value);
        /*
         * 判断GPIO14的输入电平是否为高电平并且flag为0
         * Judge whether the input level of GPIO14 is high and the flag is 0
         */
        if (value == IOT_GPIO_VALUE1 && flag == 0) {
            /*
             * 获取系统时间
             * get SysTime
             */
            start_time = hi_get_us();
            flag = 1;
        }
        /*
         * 判断GPIO8的输入电平是否为低电平并且flag为1
         * Judge whether the input level of GPIO8 is low and the flag is 1
         */
        if (value == IOT_GPIO_VALUE0 && flag == 1) {
            /*
             * 获取高电平持续时间
             * Get high level duration
             */
            time = hi_get_us() - start_time;
            break;
        }
    }
    /* 计算距离障碍物距离（340米/秒 转换为 0.034厘米/微秒, 2代表去来，两倍距离） */
    /* Calculate the distance from the obstacle */
    /* (340 m/s is converted to 0.034 cm/microsecond 2 represents going and coming, twice the distance) */
    distance = time * 0.034 / 2;
    printf("distance is %0.2f cm\r\n", distance);
    return distance;
}

/*=====================================================舵机相关函数===========================================================================*/


//采用模拟PWM方波进行舵机转角定义

void S92RInit(void)
{
    hi_gpio_init();
    hi_io_set_func(SG90_IO, SG90_IO_FUNC);
    hi_gpio_set_dir(SG90_IO, IOT_GPIO_DIR_OUT);
    RegressMiddle();
}

void SetAngle(unsigned int duty)
{
    unsigned int time = FREQ_TIME;

    hi_gpio_set_ouput_val(SG90_IO, IOT_GPIO_VALUE1);
    hi_udelay(duty);
    hi_gpio_set_ouput_val(SG90_IO, IOT_GPIO_VALUE0);
    hi_udelay(time - duty);
}

/* The steering gear is centered
 * 1、依据角度与脉冲的关系，设置高电平时间为1500微秒
 * 2、不断地发送信号，控制舵机居中
*/
void RegressMiddle(void)
{
    unsigned int angle = 1500;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
    }
}

/* Turn 90 degrees to the right of the steering gear
 * 1、依据角度与脉冲的关系，设置高电平时间为500微秒
 * 2、不断地发送信号，控制舵机向右旋转90度
*/
/*  Steering gear turn right */
void EngineTurnRight(void)
{
    unsigned int angle = 500;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
    }
}

/* Turn 90 degrees to the left of the steering gear
 * 1、依据角度与脉冲的关系，设置高电平时间为2500微秒
 * 2、不断地发送信号，控制舵机向左旋转90度
*/
/* Steering gear turn left */
void EngineTurnLeft(void)
{
    unsigned int angle = 2500;
    for (int i = 0; i < COUNT; i++) {
        SetAngle(angle);
    }
}

void AutoControlTask(void){

    float dis_cm;

    Hcsr04Init();
    S92RInit();
    osDelay(10);
 
    while(1){

        dis_cm = GetDistance();
        hi_udelay(50000);
        //直行时，障碍距离小于10cm，进行转向判断
        if(dis_cm <= dis_min_limit){
            //车后退

            hi_udelay(50000);//500ms

            //先向左边判断是否可转向
            EngineTurnLeft();
            dis_cm = GetDistance();
            hi_udelay(50000);//10ms
            if(dis_cm > dis_min_limit){
                //车原地左转，延时

                //回正舵机
                RegressMiddle();

                //左前进

                hi_udelay(1000000);//1s

                //车原地右转，前进1s

                hi_udelay(1000000);//1s

                //车原地右转，回到轨道

                hi_udelay(1000000);//1s
            }else{
                //舵机回正，再向右转
                RegressMiddle();
                hi_udelay(10000);//10ms
                EngineTurnRight();
                //舵机向右判断
                dis_cm = GetDistance();
                hi_udelay(50000);
                if(dis_cm > dis_min_limit){
                    //车原地右转，延时

                    //回正舵机
                    RegressMiddle();

                    //右前进

                    hi_udelay(1000000);//1s

                    //原地左转，前进1s

                    hi_udelay(1000000);//1s

                    //原地左转，前进，回到轨道

                    hi_udelay(1000000);//1s

                }else{
                    //停止或后退

                }
            }
        }else{
            //循迹逻辑
            Trace_get_dir();
        }
        osDelay(10);
    }

}