//dht11.c程序

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "DHT11.h"

hi_gpio_value	DHT_DQ_IN = HI_GPIO_VALUE1;
hi_gpio_value	DHT_DQ_OUT = HI_GPIO_VALUE0;

uint8_t DHT_buf[5] = {0};

void DHT11_Init(void)
{     
	hi_gpio_init();
	hi_io_set_func(DHT11_GPIO,DHT11_GPIO_FUNC);
	hi_gpio_set_dir(DHT11_GPIO,HI_GPIO_DIR_OUT);
	hi_gpio_set_ouput_val(DHT11_GPIO,1);
	
	DHT11_Start();  //复位DHT11
	DHT11_Check();//等待DHT11的回应
	
}


//初始化为输出
void DHT11_GPIO_OUT(void)
{
	hi_gpio_set_dir(DHT11_GPIO,HI_GPIO_DIR_OUT);	
}
//初始化为输入
void DHT11_GPIO_IN(void)
{
	hi_gpio_set_dir(DHT11_GPIO,HI_GPIO_DIR_IN);
	hi_io_set_pull(DHT11_GPIO,HI_IO_PULL_NONE);			
}

uint8_t DHT11_Get_Input(hi_io_name id,hi_gpio_value *val)
{
    hi_gpio_get_input_val(id,val);
    return *val;
}


void DHT11_Start(void)
{
	
	//主机设置为输出
	DHT11_GPIO_OUT();
	
	DHT11_GPIO_DQ_OUT_L;//拉低18ms
	hi_udelay(20000);//delay_ms(18)ms
	
	DHT11_GPIO_DQ_OUT_H;//拉高20~40us
	hi_udelay(40);//delay_us(40)us
	

}


//从DHT11读取一次数据
//temp:温度值(范围:0~50°)
//humi:湿度值(范围:20%~90%)
//返回值：1,正常;0,读取失败
uint8_t DHT11_Read_Data(void)    
{        
	uint8_t i;
	DHT11_Start();
	
	if(DHT11_Check()==0)
	{
		 for(i=0;i<5;i++)//读取40位数据
		 {
				DHT_buf[i]=DHT11_Read_Byte();
		 }	
	} 
	else
		{	
			return 0;
		}
	
		return 1;
}


uint8_t DHT11_Check(void)    
{   
	uint8_t retry=0;//定义临时变量
	
	DHT11_GPIO_IN();//SET INPUT 

	
	
	while (DHT11_Get_Input(DHT11_GPIO,&DHT_DQ_IN)&&retry<100)//DHT11会拉低40~80us
	 {
	   retry++;
	   hi_udelay(1);//delay_us(1)us
	 }
	if(retry>=100){
		printf("---DHT11 TimeOut---\r\n");
		return 1;
	}
		
	else retry=0;

  	while (!DHT11_Get_Input(DHT11_GPIO,&DHT_DQ_IN)&&retry<100)//DHT11拉低后会再次拉高40~80us
	 {
	   retry++;
	   hi_udelay(1);//delay_us(1)us
	 }
	if(retry>=100){
		return 1;    
	}
	return 0;
	 
}

//从DHT11读取一个位
//返回值：1/0
uint8_t DHT11_Read_Bit(void)  
{
	 uint8_t retry=0;
	
	while(DHT11_Get_Input(DHT11_GPIO,&DHT_DQ_IN)&&retry<100)//等待变为低电平
	{
	 retry++;
	 hi_udelay(1);//delay_us(1)us
	}
	
	retry=0;
	while(!DHT11_Get_Input(DHT11_GPIO,&DHT_DQ_IN)&&retry<100)//等待变高电平
	{
	 retry++;
	 hi_udelay(1);//delay_us(1)us
	}
	hi_udelay(40);//等待40us
	
	if((DHT11_Get_Input(DHT11_GPIO,&DHT_DQ_IN)))
		return 1;
	
	else 
	return 0;   
}


//从DHT11读取一个字节
//返回值：读到的数据
uint8_t DHT11_Read_Byte(void)    
{        
    uint8_t i,dat;
    dat=0;
	
		for (i=0;i<8;i++) 
		{
				dat<<=1; 
				dat|=DHT11_Read_Bit();
		}    
		
    return dat;
}

void DHT11Task(void)
{
	DHT11_Init();
	// osDelay(10);
	
	while(1)
	{
		DHT11_Read_Data();
		
		printf("Temp:%d\tHumi:%d\r\n",DHT_buf[2],DHT_buf[0]);
		osDelay(200);
	}

}



