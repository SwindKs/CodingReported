#ifndef __TURNLIGHT_H
#define __TURNLIGHT_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "hi_io.h"
#include "hi_gpio.h"
#include "ohos_init.h"
#include "cmsis_os2.h"

void LEDLight_Init(void);
void LEDLightTask(void);


#endif