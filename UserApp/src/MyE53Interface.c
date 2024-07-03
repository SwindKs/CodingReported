#include "MyE53Interface.h"

#include "BH1750.h"
#include "DHT11.h"
#include "MQ2.h"
#include "sgp30.h"

void MyE53Interface_Init(void)
{
    BH1750_Init();
    DHT11_Init();
    SGP30_Init();
    MQ2_Init();
    usleep(10000);
    MQ2_PPM_Cali();
    usleep(500000);                     // 设置完成后要有一段延迟
}


void MyE53A1GetData(MyE53IA1Data *ReadData)
{

    SGP30_ReadData();
    DHT11_Read_Data();

    ReadData->Humidity = DHT_buf[0];
    ReadData->Temperature = DHT_buf[2];
    ReadData->CO2 = sgp30Data.co2;
    ReadData->TVOC = sgp30Data.tvoc;
    ReadData->Smoke = GetMQ2_PPM();
    ReadData->Light = BH1750_ReadLightIntensity();
}