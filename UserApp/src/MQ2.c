#include "MQ2.h"
#include "adc_drv1.h"

void MQ2_Init(void)
{
    // hi_gpio_init();
    // hi_io_set_func(HI_GPIO_IDX_11, HI_IO_FUNC_GPIO_11_GPIO);
    // hi_gpio_set_dir(HI_GPIO_IDX_11, HI_GPIO_DIR_IN);
    hi_adc_init();
}



float GetMQ2_Value(void)
{
    hi_u16 value;

    unsigned int ret = hi_adc_read(HI_ADC_CHANNEL_6,&value,HI_ADC_EQU_MODEL_8,HI_ADC_CUR_BAIS_DEFAULT,0xff);
    if(ret != HI_ERR_SUCCESS)
    {
        printf("ADC read Fail!\r\n");
    }
    return (float)value* 1.8 * 4.0 / 4096.0;

}


/**校准传感器**/
void MQ2_PPM_Cali(void)
{
    float voltage = GetMQ2_Value();
    //计算RS
    if (voltage != 0) {
        float RS = (5.0f - voltage) / voltage * RL;
        //计算ppm
        R0 = RS / pow(CAL_PPM / 613.9f, 1 / -2.074f);

    }
}

/**获取PPM的函数**/
float GetMQ2_PPM(void)
{
    unsigned int ret;
    unsigned short data;
    float voltage,RS,ppm;

    voltage = GetMQ2_Value();

    RS = (5.0f - voltage) / voltage * RL;     // 计算RS
    ppm = 613.9f * pow(RS / R0, -2.074f); // 计算ppm
    return ppm;

}

void MQ2_Task(void)
{

    float ppm;
    MQ2_Init();
    osDelay(100);
    MQ2_PPM_Cali();

    while(1)
    {
        //获取PPM的值
        printf("PPM is value : %.3f\n",GetMQ2_PPM());

        osDelay(100);
    }


}
