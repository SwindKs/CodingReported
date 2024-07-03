#ifndef __OLEDFONT_H
#define __OLEDFONT_H


#include <stdio.h>
#include "cmsis_os2.h"
#include "stdint.h"
#include "string.h"


typedef struct Image
{
    /* data */
    uint8_t w;
    uint8_t h;
    const uint8_t * data;
}Image;



extern const uint8_t kkData[];
extern const uint8_t F6x8[][6];
extern const uint8_t F8X16[];





#endif /*__OLEDFONT_H*/