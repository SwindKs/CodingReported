#ifndef __HCSR04_H
#define __HCSR04_H


#include <stdio.h>
#include <stdlib.h>



extern const float dis_min_limit;


void Hcsr04Init(void);
float GetDistance(void);

void S92RInit(void);
void SetAngle(unsigned int duty);
void RegressMiddle(void);
void EngineTurnRight(void);
void EngineTurnLeft(void);

void AutoControlTask(void);

#endif