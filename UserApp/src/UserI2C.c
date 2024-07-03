#include "UserI2C.h"


void User_I2C1_Init(void)
{
    hi_gpio_init();
    // GPIO_0 multiplexed to I2C1_SDA
    hi_io_set_func(HI_GPIO_IDX_0,HI_IO_FUNC_GPIO_0_I2C1_SDA);

    // GPIO_1 multiplexed to I2C1_SCL
    hi_io_set_func(HI_GPIO_IDX_1,HI_IO_FUNC_GPIO_1_I2C1_SCL);

    // baudrate: 100kbps
    hi_i2c_init(HI_I2C_IDX_1, 400000);
    // hi_i2c_set_baudrate(HI_I2C_IDX_1, 400000);
    printf("I2C Init Succ\r\n");
}


/**
 @brief I2C写数据函数   向从机设备 发送数据
 @param slaveAddr -[in] 从设备地址
 @param regAddr -[in] 寄存器地址
 @param pData -[in] 读出数据
 @param dataLen -[in] 读出数据长度
 @return 错误码
*/
uint32_t User_I2C1_Write(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint16_t dataLen)
{
    int ret;
    hi_i2c_data i2cData = {0};

    if(0 != regAddr)
    {
        i2cData.send_buf = &regAddr;
        i2cData.send_len = 1;
        ret =  hi_i2c_write(HI_I2C_IDX_1, slaveAddr, &i2cData);
        if(ret != 0)
        {
            printf("===== Error: I2C write status1 = 0x%x! =====\r\n", ret);
            return 0;
        }
    }

    i2cData.send_buf = pData;
    i2cData.send_len = dataLen;
    ret = hi_i2c_write(HI_I2C_IDX_1,slaveAddr, &i2cData);
    
    if(ret != 0)
    {
        printf("===== Error: I2C write status2 = 0x%x! =====\r\n", ret);
        return 0;
    }

    return 1;



}
/**
 @brief I2C读数据函数   读从机设备的数据
 @param slaveAddr -[in] 从设备地址
 @param regAddr -[in] 寄存器地址
 @param pData -[in] 读出数据
 @param dataLen -[in] 读出数据长度
 @return 错误码
*/
uint32_t User_I2C1_Read(uint8_t slaveAddr, uint8_t regAddr, uint8_t *pData, uint16_t dataLen)
{
    uint32_t ret;
    hi_i2c_data i2c_data = {0};

    if(0 != regAddr)
    {
        i2c_data.send_buf = &regAddr;
        i2c_data.send_len = 1;
        ret = hi_i2c_write(HI_I2C_IDX_1, slaveAddr, &i2c_data);
        if(ret != 0)
        {
            printf("===== Error: I2C write status3 = 0x%x! =====\r\n", ret);
            return 0;
        }
    }

    i2c_data.receive_buf = pData;
    i2c_data.receive_len = dataLen;
    ret = hi_i2c_read(HI_I2C_IDX_1, slaveAddr, &i2c_data);

    if(ret != 0)
    {
        printf("===== Error: I2C read status4 = 0x%x! =====\r\n", ret);
        return 0;
    }

    return 1;

}

