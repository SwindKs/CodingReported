#include "TurnLight.h"

#define LED_IO              HI_GPIO_IDX_6
#define LED_IO_FUNC         HI_IO_FUNC_GPIO_6_GPIO


void LEDLight_Init(void){


	hi_gpio_init();
	hi_io_set_func(LED_IO,LED_IO_FUNC);
	hi_gpio_set_dir(LED_IO,HI_GPIO_DIR_OUT);
	hi_gpio_set_ouput_val(LED_IO,0);

}

void LEDLightTask(void){
    LEDLight_Init();

    while(1){

        hi_gpio_set_ouput_val(LED_IO,1);
        printf("off\r\n");
        usleep(1000000);

        hi_gpio_set_ouput_val(LED_IO,0);
        printf("on\r\n");
        usleep(1000000);
    }


}