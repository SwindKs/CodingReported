#include "BH1750.h"

/**
 @brief BH1750初始化函数
 @param 无
 @return 无
*/
void BH1750_Init(void)
{
    uint8_t data;
    data = BH1750_PWR_ON;              // 发送启动命令
    User_I2C1_Write((BH1750_SLAVE_ADDR << 1) | WRITE_BIT, 0, &data, 1);
    data = BH1750_CON_H;               // 设置连续高分辨率模式，1lx，120ms
    User_I2C1_Write((BH1750_SLAVE_ADDR << 1) | WRITE_BIT, 0, &data, 1);
}

/**
 @brief BH1750获取光强度
 @param 无
 @return 光强度
*/
float BH1750_ReadLightIntensity(void)
{
    float lux = 0.0;
    uint8_t sensorData[2] = {0};
    User_I2C1_Read((BH1750_SLAVE_ADDR << 1) | READ_BIT, 0, sensorData, 2);
    lux = (sensorData[0] << 8 | sensorData[1]) / 1.2;
    return lux;
}


void BH1750Task(void){
    float lux;

    BH1750_Init();
    hi_udelay(200000);                     // 设置完成后要有一段延迟
    while (1)
    {
        lux = BH1750_ReadLightIntensity();
        printf("light val: %.02f [Lux]\n", lux);
        osDelay(200);
    }

}