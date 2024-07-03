/*
 * Copyright (c) 2022 HiSilicon (Shanghai) Technologies CO., LIMITED.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef MOTOR_CONTROL_H
#define MOTOR_CONTROL_H


typedef enum {
    MODE_ON_OFF = 0,
    MODE_SET_LEFT_FORWARD,
    MODE_SET_RIGHT_FORWARD,
    MODE_SET_TURN_LEFT,
    MODE_SET_TURN_RIGHT,
    MODE_SET_LEFT_ADC,
    MODE_SET_RIGHT_ADC,
    MODE_END,
} ENUM_MODE;

typedef struct {
    uint32_t LeftForward;
    uint32_t RightForward;
    uint32_t TurnLeft;
    uint32_t TurnRight;
    unsigned short leftadcdata;
    unsigned short rightadcdata;
} CAR_DRIVE;


// void TraceTask(void);
void trace_module_init(void);
// IotGpioValue get_do_value(hi_adc_channel_index idx);
void Trace_get_dir(void);
void init_ctrl_algo(void);
void TraceTask(void);

void TB6612FNGInit(void);
void car_backward(uint32_t left_pwm_value, uint32_t right_pwm_value);
void car_forward(uint32_t left_pwm_value, uint32_t right_pwm_value);
void car_left(uint32_t right_pwm_value);
void car_right(uint32_t left_pwm_value);
void car_stop(void);

#endif  //MOTOR_CONTROL_H